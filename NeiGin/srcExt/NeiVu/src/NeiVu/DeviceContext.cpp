#include "DeviceContext.h"

#include "Context.h"
#include <thread>
#include "CommandBuffer.h"
#include "SamplerManager.h"
#include "FxLoader.h"
#include "GraphicsPipeline.h"
#include "ComputePipeline.h"
#include "Buffer.h"
#include "BasicMemoryManager.h"
#include "MemoryManagerVMA.h"
#include "NeiVu/Fence.h"

using namespace Nei;
using namespace Vu;

DeviceContext::DeviceContext(CreateInfo const& createInfo) {
  context = createInfo.context;
  auto instance = context->getVkInstance();

  auto devices = instance.enumeratePhysicalDevices();
  if(devices.empty())
  nei_fatal("No vulkan devices!");

  int index = createInfo.deviceId;
  if(index >= devices.size()) {
    nei_error("Selected device does not exist! {}", index);
    index = 0;
  }

  physicalDevice = devices[index];

  std::vector<const char *> enabledExtensions;
  std::vector<const char *> enabledLayers;

  auto addExtension = [&](const char* name) {
    if(supportsExtension(name)) enabledExtensions.push_back(name);
  };

  auto addLayer = [&](const char* name) {
    if(supportsLayer(name)) enabledLayers.push_back(name);
  };

  // swapchain - mandatory
  addExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  // dedicated allocation - "better" for large textures and buffers
  addExtension(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
  addExtension(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);

  // allows negative viewport height - avoids inverting y in shader
  addExtension(VK_KHR_MAINTENANCE1_EXTENSION_NAME);

  for(auto& e : createInfo.extensions) {
    addExtension(e);
  }

  if(context->isValidationEnabled())
    addLayer("VK_LAYER_LUNARG_standard_validation");

  float priorities[] = {1.0};

  std::vector<vk::DeviceQueueCreateInfo> queues;

  auto queueProperties = physicalDevice.getQueueFamilyProperties();
  for(int i = 0; i < queueProperties.size(); i++) {
    auto& q = queueProperties[i];
    auto g = vk::QueueFlagBits::eGraphics;
    auto c = vk::QueueFlagBits::eCompute;
    auto t = vk::QueueFlagBits::eTransfer;
    auto s = vk::QueueFlagBits::eSparseBinding;
    auto flags = q.queueFlags & ~s; // dont care about sparse
    if(mainQueueIndex == -1 && flags == (g | c | t)) {
      mainQueueIndex = i;
      vk::DeviceQueueCreateInfo dqci;
      dqci.queueCount = 1;
      dqci.queueFamilyIndex = mainQueueIndex;
      dqci.pQueuePriorities = priorities;
      queues.push_back(dqci);
    }
    if(transferQueueIndex == -1 && flags == t) {
      transferQueueIndex = i;
      vk::DeviceQueueCreateInfo dqci;
      dqci.queueCount = 1;
      dqci.queueFamilyIndex = transferQueueIndex;
      dqci.pQueuePriorities = priorities;
      queues.push_back(dqci);
    }
    if(computeQueueIndex == -1 && (flags == (c) || flags == (c | t))) {
      computeQueueIndex = i;
      vk::DeviceQueueCreateInfo dqci;
      dqci.queueCount = 1;
      dqci.queueFamilyIndex = computeQueueIndex;
      dqci.pQueuePriorities = priorities;
      queues.push_back(dqci);
    }
  }

  nei_assert(mainQueueIndex >= 0);
  nei_assert(transferQueueIndex >= 0);
  //nei_assert(computeQueueIndex >= 0);

  vk::PhysicalDeviceFeatures features;
  features.tessellationShader = true;
  features.geometryShader = true;
  features.independentBlend = true;
  features.samplerAnisotropy = true;
  features.fillModeNonSolid = true;

  vk::DeviceCreateInfo dci;
  dci.queueCreateInfoCount = uint32(queues.size());
  dci.pQueueCreateInfos = queues.data();
  dci.enabledExtensionCount = uint32(enabledExtensions.size());
  dci.ppEnabledExtensionNames = enabledExtensions.data();
  dci.enabledLayerCount = uint32(enabledLayers.size());
  dci.ppEnabledLayerNames = enabledLayers.data();
  dci.pEnabledFeatures = &features;
  device = physicalDevice.createDevice(dci);

  nei_assert(device);

  dispatch.init(instance, vkGetInstanceProcAddr, device, vkGetDeviceProcAddr);

  mainQueue = device.getQueue(mainQueueIndex, 0);
  if(transferQueueIndex != -1)
    transferQueue = device.getQueue(transferQueueIndex, 0);
  if(computeQueueIndex != -1)
    computeQueue = device.getQueue(computeQueueIndex, 0);

  // @TODO separate queues?
  presentQueue = mainQueue;
  presentQueueIndex = mainQueueIndex;
  assert(mainQueue);

  //memoryManager = new MemoryManagerVMA(this);
  memoryManager = new BasicMemoryManager(this);

  pipelineCache = device.createPipelineCache(vk::PipelineCacheCreateInfo());

  singleUseCommandBuffer = new CommandBuffer(this);
  samplerManager = new SamplerManager(this);
  fxLoader = new FxLoader(this);

  tracyContext = TracyVkContext(physicalDevice, device, mainQueue, **singleUseCommandBuffer);
}

DeviceContext::~DeviceContext() {
  TracyVkDestroy(tracyContext);
  device.destroyPipelineCache(pipelineCache);

  for(auto& [id, cp] : commandPoolMap) {
    device.destroyCommandPool(cp);
  }

  if(device)
    device.destroy();
}

bool DeviceContext::supportsExtension(std::string const& name) const {
  auto extensions = physicalDevice.enumerateDeviceExtensionProperties();
  for(auto const& e : extensions)
    if(e.extensionName == name)
      return true;
  return false;
}

bool DeviceContext::supportsLayer(std::string const& name) const {
  auto layers = physicalDevice.enumerateDeviceLayerProperties();
  for(auto const& e : layers)
    if(e.layerName == name)
      return true;
  return false;
}

bool DeviceContext::supportsImageFormat(vk::Format format, vk::FormatFeatureFlags usage) {
  auto prop = physicalDevice.getFormatProperties(format);
  return !!(prop.optimalTilingFeatures & usage);
}

bool DeviceContext::supportsDepthFormat(vk::Format format) {
  return supportsImageFormat(format, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

vk::Queue DeviceContext::getQueue(int index) const {
  if(index == mainQueueIndex || index == DefaultQueue)return mainQueue;
  if(index == transferQueueIndex)return transferQueue;
  if(index == computeQueueIndex)return computeQueue;
  return nullptr;
}

vk::CommandPool DeviceContext::getCommandPool(int queueIndex) {
  if(queueIndex == DefaultQueue) queueIndex = mainQueueIndex;
  static std::hash<std::thread::id> hasher;
  auto id = hasher(std::this_thread::get_id());
  id = 0;
  auto cp = commandPoolMap[id];
  if(!cp) {

    vk::CommandPoolCreateInfo i;
    i.queueFamilyIndex = queueIndex;
    i.flags = vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    cp = device.createCommandPool(i);
    commandPoolMap[id] = cp;
  }
  return cp;
}

Ptr<MemoryManager> DeviceContext::getMemoryManager() const {
  return memoryManager;
}

Ptr<GraphicsPipeline> DeviceContext::loadFx(std::string const& fx) const {
  return fxLoader->loadFx(fx)->as<GraphicsPipeline>();
}

Ptr<GraphicsPipeline> DeviceContext::loadFx(fs::path const& fx) const {
  return fxLoader->loadFxFile(fx)->as<GraphicsPipeline>();
}

Ptr<ComputePipeline> DeviceContext::loadComp(fs::path const& fx) const {
  return fxLoader->loadFxFile(fx)->as<ComputePipeline>();
}

void DeviceContext::wait() {
  device.waitIdle();
}

bool DeviceContext::validation() const { return context->isValidationEnabled(); }

void DeviceContext::submit(int queueIndex, std::vector<Ptr<CommandBuffer>> const& buffers, Ptr<Fence> const& fence,
                           std::vector<vk::Semaphore> const& wait, std::vector<vk::Semaphore> const& signal,
                           std::vector<vk::PipelineStageFlags> const& stages) {
  assert(signal.size() == stages.size());
  thread_local std::vector<vk::CommandBuffer> vkBuffers;
  vkBuffers.clear();
  for(auto& b : buffers) {
    if(b->isExecutable()) {
      vkBuffers.push_back(*b);
      b->setFence(fence);
    }
  }

  vk::SubmitInfo si;
  si.commandBufferCount = (uint32)vkBuffers.size();
  si.pCommandBuffers = vkBuffers.data();
  si.waitSemaphoreCount = (uint32)wait.size();
  si.pWaitSemaphores = wait.data();
  si.pWaitDstStageMask = stages.data();
  si.signalSemaphoreCount = (uint32)signal.size();
  si.pSignalSemaphores = signal.data();

  auto queue = getQueue(queueIndex);
  assert(queue);
  queue.submit({si}, *fence);
}

CommandBuffer* DeviceContext::getSingleUseCommandBuffer() {
  return singleUseCommandBuffer;
}

vk::Sampler DeviceContext::getSampler(SamplerType type) { return samplerManager->getSampler(type); }

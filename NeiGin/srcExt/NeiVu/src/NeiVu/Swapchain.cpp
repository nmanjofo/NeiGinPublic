#include "Swapchain.h"

#include "Context.h"
#include "DeviceContext.h"
#include "CommandBuffer.h"

#include "Texture.h"

using namespace Nei::Vu;

Swapchain::Swapchain(SwapchainCreateInfo const& info) {
  init(info);
}

Swapchain::Swapchain(void* handle, DeviceContext* deviceContext) {
  SwapchainCreateInfo sci;
  sci.deviceContext = deviceContext;
  sci.windowHandle = handle;
  init(sci);
}

Swapchain::~Swapchain() {
  destroySwapchain();
  destroySurface();
}

void Swapchain::resize() {
  createSwapchain();
}

bool Swapchain::isValid() const { return size.x > 0 && size.y > 0; }

void Swapchain::init(SwapchainCreateInfo const& info) {
  deviceContext = info.deviceContext;
  vsync = info.vsync;
  imageCount = info.imageCount;
  windowHandle = info.windowHandle;
  createSurface();
  createSwapchain();

  for(uint i = 0; i < info.imageCount; i++)
    acquireSemaphore.push_back(deviceContext->getVkDevice().createSemaphore(vk::SemaphoreCreateInfo()));
  for(uint i = 0; i < info.imageCount; i++)
    presentSemaphores.push_back(deviceContext->getVkDevice().createSemaphore(vk::SemaphoreCreateInfo()));
}

void Swapchain::createSurface() {
#ifdef WIN32
  vk::Win32SurfaceCreateInfoKHR wsci;
  wsci.hinstance = (HINSTANCE)GetModuleHandle(NULL);
  wsci.hwnd = (HWND)windowHandle;

  surface = deviceContext->getContext()->getVkInstance().createWin32SurfaceKHR(wsci);
  assert(surface);

  auto surfaceFormats = deviceContext->getVkPhysicalDevice().getSurfaceFormatsKHR(surface);
  surfaceFormat = surfaceFormats[0];
  //surfaceFormat = surfaceFormats.back(); // should be R8G8B8A8SRGB

  auto supported = deviceContext->getVkPhysicalDevice().getSurfaceSupportKHR(deviceContext->getMainQueueIndex(),
    surface);
  assert(supported);

#else
  //@TODO
  assert("No pinguins" && 0);
  assert(surface);
#endif
}

void Swapchain::next() {
  Profile("Swapchain::next");
  aqIndex = (aqIndex+1)%imageCount;
  auto res = deviceContext->getVkDevice().acquireNextImageKHR(swapchain,UINT64_MAX, acquireSemaphore[aqIndex], vk::Fence());
  currentImage = res.value;
}

void Swapchain::copy(CommandBuffer* cmd, Texture* texture) {
  ProfileGPU(cmd, "Swapchain Copy");
  auto img = swapchainImages[currentImage];

  // change layout to TransferDst
  vk::ImageMemoryBarrier b1;
  b1.image = img;
  b1.oldLayout = vk::ImageLayout::eUndefined;
  b1.newLayout = vk::ImageLayout::eTransferDstOptimal;
  b1.srcAccessMask = vk::AccessFlags();
  b1.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
  b1.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);

  (**cmd).pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::PipelineStageFlagBits::eTransfer,
    vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &b1);

  // blit images - allows resize and format conversion
  auto srcSize = texture->getBaseSize();
  vk::ImageBlit region;
  region.srcOffsets[0] = vk::Offset3D(0, 0, 0);
  region.srcOffsets[1] = vk::Offset3D(srcSize.x, srcSize.y, 1);
  region.srcSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1);
  region.dstOffsets[0] = vk::Offset3D(0, size.y, 0);
  region.dstOffsets[1] = vk::Offset3D(size.x, 0, 1);
  region.dstSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1);

  (**cmd).blitImage(texture->getImage(), vk::ImageLayout::eTransferSrcOptimal, img,
    vk::ImageLayout::eTransferDstOptimal,
    1, &region, vk::Filter::eLinear);

  // change layout back to PresentSrc
  vk::ImageMemoryBarrier b2;
  b2.image = img;
  b2.oldLayout = vk::ImageLayout::eTransferDstOptimal;
  b2.newLayout = vk::ImageLayout::ePresentSrcKHR;
  b2.srcAccessMask = vk::AccessFlags();
  b2.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
  b2.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);

  (**cmd).pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTopOfPipe,
    vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &b2);
}

void Swapchain::present() {
  Profile("Swapchain::present");
  auto queue = deviceContext->getPresentQueue();
  vk::PresentInfoKHR pi;
  pi.swapchainCount = 1;
  pi.pSwapchains = &swapchain;
  pi.pImageIndices = &currentImage;
  pi.waitSemaphoreCount = 1;
  pi.pWaitSemaphores = &presentSemaphores[currentImage];

  try {
    auto res = queue.presentKHR(pi);
  } catch(vk::OutOfDateKHRError er) {
    // window closed
    nei_warning("Present Failed: OutOfDate");
  }
}

void Swapchain::createSwapchain() {

  auto device = deviceContext->getVkDevice();
  auto physicalDevice = deviceContext->getVkPhysicalDevice();

  for(auto& v : swapchainImageViews) {
    device.destroyImageView(v);
  }
  swapchainImageViews.clear();

  auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);

  size.x = surfaceCapabilities.currentExtent.width;
  size.y = surfaceCapabilities.currentExtent.height;

  if(surfaceCapabilities.currentExtent.width <= 0 ||
    surfaceCapabilities.currentExtent.height <= 0) {
    destroySwapchain();
    return;
  }

  auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

  vk::PresentModeKHR presentMode = vk::PresentModeKHR::eImmediate;

  auto setIfSupported = [&](vk::PresentModeKHR mode) {
    if(find(presentModes.begin(), presentModes.end(), mode) != presentModes.end()) {
      presentMode = mode;
    }
  };

  if(vsync) {
    setIfSupported(vk::PresentModeKHR::eFifo);
    setIfSupported(vk::PresentModeKHR::eFifoRelaxed);
  } else {
    setIfSupported(vk::PresentModeKHR::eMailbox);
    setIfSupported(vk::PresentModeKHR::eImmediate);
  }

  imageCount = min(imageCount, surfaceCapabilities.maxImageCount);
  imageCount = max(imageCount, surfaceCapabilities.minImageCount);

  auto transform = surfaceCapabilities.currentTransform;

  auto oldSwapchain = swapchain;
  vk::SwapchainCreateInfoKHR sci;
  sci.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
  sci.imageArrayLayers = 1;
  sci.imageColorSpace = surfaceFormat.colorSpace;
  sci.imageExtent = surfaceCapabilities.currentExtent;
  sci.imageFormat = surfaceFormat.format;
  sci.imageSharingMode = vk::SharingMode::eExclusive;
  sci.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
  sci.minImageCount = imageCount;
  sci.oldSwapchain = swapchain;
  sci.queueFamilyIndexCount = 1;
  uint32_t q = deviceContext->getMainQueueIndex();
  sci.pQueueFamilyIndices = &q;
  sci.presentMode = presentMode;
  sci.preTransform = transform;
  sci.surface = surface;

  swapchain = device.createSwapchainKHR(sci);
  assert(swapchain);

  if(oldSwapchain)
    device.destroySwapchainKHR(oldSwapchain);

  // assert for intel driver bug
  uint32_t count = 0;
  vkGetSwapchainImagesKHR(device, swapchain, &count, nullptr);
  assert(count == imageCount);

  swapchainImages = device.getSwapchainImagesKHR(swapchain);

  for(auto& img : swapchainImages) {
    vk::ImageViewCreateInfo ivci;
    ivci.format = surfaceFormat.format;
    ivci.image = img;
    ivci.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
    ivci.viewType = vk::ImageViewType::e2D;
    swapchainImageViews.emplace_back(device.createImageView(ivci));
  }
}

void Swapchain::destroySwapchain() {
  auto device = deviceContext->getVkDevice();
  for(auto& v : swapchainImageViews) {
    device.destroyImageView(v);
  }
  swapchainImageViews.clear();
  if(swapchain)
    device.destroySwapchainKHR(swapchain);
  swapchain = nullptr;
}

void Swapchain::destroySurface() {
  deviceContext->getContext()->getVkInstance().destroySurfaceKHR(surface);
  surface = nullptr;
}

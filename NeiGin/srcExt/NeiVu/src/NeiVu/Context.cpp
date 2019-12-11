#include "Context.h"
#include <iostream>

using namespace Nei::Vu;

VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallback(
  VkDebugReportFlagsEXT flags,
  VkDebugReportObjectTypeEXT objectType,
  uint64_t object,
  size_t location,
  int32_t messageCode,
  const char* pLayerPrefix,
  const char* pMessage,
  void* pUserData) {
  vk::DebugReportFlagBitsEXT f = (vk::DebugReportFlagBitsEXT)flags;
  if (f == vk::DebugReportFlagBitsEXT::eInformation) {
    nei_info(pMessage);
  }
  if (f == vk::DebugReportFlagBitsEXT::eWarning || f==vk::DebugReportFlagBitsEXT::ePerformanceWarning) {
    nei_warning(pMessage);
  }
  if (f == vk::DebugReportFlagBitsEXT::eError) {
    nei_warning(pMessage);
  }
  if (f == vk::DebugReportFlagBitsEXT::eDebug) {
    nei_debug(pMessage);
  }
  return VK_FALSE;
}

Context::Context(ContextCreateInfo const& info) {
  validationEnabled = info.validation;
  
  vk::ApplicationInfo appInfo;
  appInfo.apiVersion = VK_MAKE_VERSION(1,1,0);
  appInfo.applicationVersion = info.applicationVersion;
  appInfo.engineVersion = NEIVU_VERSION;
  appInfo.pApplicationName = info.applicationName.c_str();
  appInfo.pEngineName = "NeiGin";

  std::vector<const char*> enabledExtensions;
  std::vector<const char*> enabledLayers;

  auto addExtension = [&](const char* name) {
    if (supportsExtension(name)) enabledExtensions.push_back(name);
  };

  auto addLayer = [&](const char* name) {
    if (supportsLayer(name)) enabledLayers.push_back(name);
  };

  // surface support
  addExtension(VK_KHR_SURFACE_EXTENSION_NAME);

  // windows surface
#ifdef WIN32
  addExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

  // debug utils - object naming
  addExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  if (validationEnabled) {
    addLayer("VK_LAYER_LUNARG_standard_validation");
    addExtension("VK_EXT_debug_report");
    addExtension("VK_EXT_debug_utils");
  }

  vk::InstanceCreateInfo createInfo;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledExtensionCount = uint32(enabledExtensions.size());
  createInfo.ppEnabledExtensionNames = enabledExtensions.data();
  createInfo.enabledLayerCount = uint32(enabledLayers.size());
  createInfo.ppEnabledLayerNames = enabledLayers.data();

  instance = vk::createInstance(createInfo);
  assert(instance);

  dynamicDispatch.init(instance, vkGetInstanceProcAddr);

  if (validationEnabled) {
    auto drcci = vk::DebugReportCallbackCreateInfoEXT(
      //vk::DebugReportFlagBitsEXT::eDebug |
      vk::DebugReportFlagBitsEXT::eError |
      //vk::DebugReportFlagBitsEXT::eInformation |
      vk::DebugReportFlagBitsEXT::ePerformanceWarning |
      vk::DebugReportFlagBitsEXT::eWarning,
      &debugReportCallback, 0);
    debugCallback = instance.createDebugReportCallbackEXT(drcci,nullptr,dynamicDispatch);
  }
}

Context::~Context() {
  if(debugCallback)
    instance.destroyDebugReportCallbackEXT(debugCallback,nullptr, dynamicDispatch);
  if(instance)
    instance.destroy();
}

bool Context::supportsExtension(std::string const& name) {
  // @BUG this is crashing driver, so just return true
  return true; 

  auto extensions = vk::enumerateInstanceExtensionProperties();
  for (auto& e : extensions)
    if (e.extensionName == name)return true;
  return false;
}

bool Context::supportsLayer(std::string const& name) {
  // @BUG this is crashing driver, so just return true
  return true;
  auto layers = vk::enumerateInstanceLayerProperties();
  for (auto& l : layers)
    if (l.layerName == name)return true;
  return false;
}

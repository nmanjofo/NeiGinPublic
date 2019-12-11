#include "DeviceObject.h"

using namespace Nei;
using namespace Vu;

DeviceObject::DeviceObject(DeviceContext* dc): deviceContext(dc) {
  
}

DeviceObject::~DeviceObject() {
  
}

void DeviceObject::setObjectName(std::string const& name, vk::ObjectType type, uint64 handle) {
  debugName = name;
  if(!handle) return;
  vk::DebugUtilsObjectNameInfoEXT nameInfo;
  nameInfo.objectType = type;
  nameInfo.objectHandle = handle;
  nameInfo.pObjectName = debugName.c_str();
  deviceContext->getVkDevice().setDebugUtilsObjectNameEXT(nameInfo, deviceContext->getDispatch());
}

vk::Device DeviceObject::getDevice() {
  return deviceContext->getVkDevice();
}

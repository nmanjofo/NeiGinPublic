#include "MemoryManager.h"

using namespace Nei;
using namespace Vu;

MemoryManager::MemoryManager(DeviceContext* dc) : DeviceObject(dc) {
  memoryProperties = dc->getVkPhysicalDevice().getMemoryProperties();
}

MemoryManager::~MemoryManager() { }

uint MemoryManager::findMemoryType(uint memoryBits, vk::MemoryPropertyFlags requiredFlags) {
  for(uint i = 0; i < memoryProperties.memoryTypeCount; i++) {
    if((1u << i) & memoryBits) {
      if(memoryProperties.memoryTypes[i].propertyFlags & requiredFlags) {
        return i;
      }
    }
  }
  nei_error("Failed to find required memory type");
  return -1;
}

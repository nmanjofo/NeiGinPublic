#include "AdvancedMemoryManager.h"

using namespace Nei;
using namespace Vu;

AdvancedMemoryManager::AdvancedMemoryManager(DeviceContext* dc) :MemoryManager(dc){ }

AdvancedMemoryManager::~AdvancedMemoryManager() { }

Allocation AdvancedMemoryManager::allocate(vk::Image image, MemoryUsage usage) {
  return Allocation();
}
Allocation AdvancedMemoryManager::allocate(vk::Buffer buffer, MemoryUsage usage) {
  return Allocation();
}

void AdvancedMemoryManager::free(Allocation const& allocation) {
  
}
void* AdvancedMemoryManager::map(Allocation const& allocation) {
  return nullptr;
}
void AdvancedMemoryManager::unmap(Allocation const& allocation) {
  
}

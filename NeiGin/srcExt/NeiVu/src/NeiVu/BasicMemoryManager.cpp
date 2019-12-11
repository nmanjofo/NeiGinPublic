#include "BasicMemoryManager.h"

using namespace Nei;
using namespace Vu;

BasicMemoryManager::BasicMemoryManager(DeviceContext* dc): MemoryManager(dc) { }

BasicMemoryManager::~BasicMemoryManager() { }

Allocation BasicMemoryManager::allocate(vk::Image image, MemoryUsage usage) {
  nei_assert(usage != Default);

  vk::MemoryPropertyFlags requiredFlags;

  if(usage == GpuOnly) requiredFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
  else
    requiredFlags = vk::MemoryPropertyFlagBits::eHostVisible |
      vk::MemoryPropertyFlagBits::eHostCoherent |
      vk::MemoryPropertyFlagBits::eHostCached;

  auto device = deviceContext->getVkDevice();
  auto req = device.getImageMemoryRequirements(image);

  auto index = findMemoryType(req.memoryTypeBits,requiredFlags);
  nei_assert(index>=0);


  vk::MemoryAllocateInfo ai;
  ai.memoryTypeIndex = index;
  ai.allocationSize = req.size;

  Ptr block = new MemoryBlock;
  block->memory = device.allocateMemory(ai);
  block->size = req.size;

  memoryBlocks.push_back(block);

  Allocation allocation;
  allocation.size = uint(req.size);
  allocation.offset = 0;
  allocation.memory = block->memory;
  
  allocationMap[allocation] = block;

  device.bindImageMemory(image,block->memory,0);
  return allocation;
}

Allocation BasicMemoryManager::allocate(vk::Buffer buffer, MemoryUsage usage) {
  nei_assert(usage != Default);

  auto device = deviceContext->getVkDevice();
  auto req = device.getBufferMemoryRequirements(buffer);

  vk::MemoryPropertyFlags requiredFlags;
  if (usage == GpuOnly) requiredFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
  else
    requiredFlags = vk::MemoryPropertyFlagBits::eHostVisible |
    vk::MemoryPropertyFlagBits::eHostCoherent |
    vk::MemoryPropertyFlagBits::eHostCached;

  auto index = findMemoryType(req.memoryTypeBits, requiredFlags);

  vk::MemoryAllocateInfo ai;
  ai.memoryTypeIndex = index;
  ai.allocationSize = req.size;

  Ptr block = new MemoryBlock;
  block->memory = device.allocateMemory(ai);
  block->size = req.size;

  memoryBlocks.push_back(block);

  Allocation allocation;
  allocation.size = uint(req.size);
  allocation.offset = 0;
  allocation.memory = block->memory;

  allocationMap[allocation] = block;

  device.bindBufferMemory(buffer, block->memory, 0);
  return allocation;
}

void BasicMemoryManager::free(Allocation const& allocation) {
  auto& block = allocationMap[allocation];

  if(block->mappedCount > 0)
  nei_error("Freeing mapped memory!");
  deviceContext->getVkDevice().freeMemory(block->memory);

  auto it = std::find(memoryBlocks.begin(),memoryBlocks.end(),block);
  memoryBlocks.erase(it);
}

void* BasicMemoryManager::map(Allocation const& allocation) {
  auto& block = allocationMap[allocation];
  if(block->mappedCount == 0) {
    block->mappedPtr = deviceContext->getVkDevice().mapMemory(block->memory, 0,VK_WHOLE_SIZE, {});
    if(!block->mappedPtr) {
      nei_error("Failed to map memory");
    }
  }

  block->mappedCount++;
  return block->mappedPtr;
}

void BasicMemoryManager::unmap(Allocation const& allocation) {
  auto& block = allocationMap[allocation];
  nei_assert(block->mappedCount>0);

  block->mappedCount--;
  if(block->mappedCount == 0) {
    deviceContext->getVkDevice().unmapMemory(block->memory);
  }
}

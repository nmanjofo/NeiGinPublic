#include "MemoryManagerVMA.h"

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"
#undef  VMA_IMPLEMENTATION

using namespace Nei;
using namespace Vu;

inline VmaMemoryUsage memoryTypeToUsage(MemoryUsage mem) {
  switch (mem) {
  case GpuOnly: return VMA_MEMORY_USAGE_GPU_ONLY;
  case CpuOnly: return VMA_MEMORY_USAGE_CPU_ONLY;
  case ReadBack: return VMA_MEMORY_USAGE_GPU_TO_CPU;
  case Stream: return VMA_MEMORY_USAGE_CPU_TO_GPU;
  default: return VMA_MEMORY_USAGE_GPU_ONLY;
  }
}

struct Nei::Vu::VMAData : public Object {
  VmaAllocator allocator;
  std::map<Allocation, VmaAllocation> allocationMap;
};

MemoryManagerVMA::MemoryManagerVMA(DeviceContext* dc): MemoryManager(dc) {
  vma = new VMAData;

  VmaAllocatorCreateInfo info = {};
  info.physicalDevice = dc->getVkPhysicalDevice();
  info.device = dc->getVkDevice();
  //if(supportsExtension(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME))
  //info.flags = VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
  vmaCreateAllocator(&info, &vma->allocator);
}

MemoryManagerVMA::~MemoryManagerVMA() {
  vmaDestroyAllocator(vma->allocator);
}

Allocation MemoryManagerVMA::allocate(vk::Image image, MemoryUsage usage) {
  VmaAllocationCreateInfo aci = {};
  aci.usage = memoryTypeToUsage(usage);
  VmaAllocation vmaAllocation;
  VmaAllocationInfo allocationInfo;
  auto res = vmaAllocateMemoryForImage(vma->allocator, image, &aci, &vmaAllocation, &allocationInfo);
  if (res != VK_SUCCESS)
  nei_error("Allocation failed!");


  deviceContext->getVkDevice().bindImageMemory(image, allocationInfo.deviceMemory, allocationInfo.offset);

  Allocation allocation;
  allocation.memory = allocationInfo.deviceMemory;
  allocation.size = uint(allocationInfo.size);
  allocation.offset = uint(allocationInfo.offset);

  vma->allocationMap[allocation] = vmaAllocation;
  return allocation;
}

Allocation MemoryManagerVMA::allocate(vk::Buffer buffer, MemoryUsage usage) {
  VmaAllocationCreateInfo aci = {};
  aci.usage = memoryTypeToUsage(usage);
  VmaAllocation vmaAllocation;
  VmaAllocationInfo allocationInfo;
  auto res = vmaAllocateMemoryForBuffer(vma->allocator, buffer, &aci, &vmaAllocation, &allocationInfo);
  if (res != VK_SUCCESS)
  nei_error("Allocation failed!");


  deviceContext->getVkDevice().bindBufferMemory(buffer, allocationInfo.deviceMemory, allocationInfo.offset);

  Allocation allocation;
  allocation.memory = allocationInfo.deviceMemory;
  allocation.size = uint(allocationInfo.size);
  allocation.offset = uint(allocationInfo.offset);

  vma->allocationMap[allocation] = vmaAllocation;
  return allocation;
}

void MemoryManagerVMA::free(Allocation const& allocation) {
  VmaAllocation vmaAllocation = vma->allocationMap[allocation];
  vmaFreeMemory(vma->allocator, vmaAllocation);
  vma->allocationMap.erase(allocation);

}

void* MemoryManagerVMA::map(Allocation const& allocation) {
  VmaAllocation vmaAllocation = vma->allocationMap[allocation];
  void* ptr = nullptr;
  vmaMapMemory(vma->allocator, vmaAllocation, &ptr);
  return ptr;
}

void MemoryManagerVMA::unmap(Allocation const& allocation) {
  VmaAllocation vmaAllocation = vma->allocationMap[allocation];
  vmaUnmapMemory(vma->allocator, vmaAllocation);
}

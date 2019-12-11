#pragma once

#include "DeviceObject.h"

namespace Nei::Vu {
  enum MemoryUsage {
    GpuOnly,
    CpuOnly,
    ReadBack,
    Stream,
    Default
  };

  struct MemoryBlock : public Object {
    vk::DeviceMemory memory;
    vk::DeviceSize size = 0;
    uint allocations = 0;
    uint mappedCount = 0;
    void* mappedPtr = nullptr;
  };

  class MemoryManager : public DeviceObject {
  public:
    MemoryManager(DeviceContext* dc);
    virtual ~MemoryManager();

    virtual Allocation allocate(vk::Image image, MemoryUsage usage = GpuOnly) = 0;
    virtual Allocation allocate(vk::Buffer buffer, MemoryUsage usage = GpuOnly) = 0;

    virtual void free(Allocation const& allocation) = 0;
    virtual void* map(Allocation const& allocation) = 0;
    virtual void unmap(Allocation const& allocation) = 0;

    uint findMemoryType(uint memoryBits, vk::MemoryPropertyFlags requiredFlags);

  protected:
    vk::PhysicalDeviceMemoryProperties memoryProperties;
  };


}

#pragma once

#include "MemoryManager.h"

namespace Nei::Vu {
  struct VMAData;

  class MemoryManagerVMA : public MemoryManager {
  public:
    MemoryManagerVMA(DeviceContext* dc);
    virtual ~MemoryManagerVMA();

    Allocation allocate(vk::Image image, MemoryUsage usage) override;
    Allocation allocate(vk::Buffer buffer, MemoryUsage usage) override;
    void free(Allocation const& allocation) override;
    void* map(Allocation const& allocation) override;
    void unmap(Allocation const& allocation) override;
    Ptr<VMAData> vma;
  };
}

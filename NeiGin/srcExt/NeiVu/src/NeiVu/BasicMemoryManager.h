#pragma once

#include "MemoryManager.h"

namespace Nei::Vu {
  class BasicMemoryManager: public MemoryManager {
  public:
    BasicMemoryManager(DeviceContext* dc);
    virtual ~BasicMemoryManager();

    Allocation allocate(vk::Image image, MemoryUsage usage = Default) override;
    Allocation allocate(vk::Buffer buffer, MemoryUsage usage = Default) override;

    void free(Allocation const& allocation) override;
    void* map(Allocation const& allocation) override;
    void unmap(Allocation const& allocation) override;

  protected:
    std::vector<Ptr<MemoryBlock>> memoryBlocks;
    std::map<Allocation, Ptr<MemoryBlock>> allocationMap;
  };


}

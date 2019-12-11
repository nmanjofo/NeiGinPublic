#pragma once

#include "MemoryManager.h"

namespace Nei::Vu {
class AdvancedMemoryManager: public MemoryManager {
public:
  AdvancedMemoryManager(DeviceContext* dc);
  virtual ~AdvancedMemoryManager();

  Allocation allocate(vk::Image image, MemoryUsage usage) override;
  Allocation allocate(vk::Buffer buffer, MemoryUsage usage) override;
  void free(Allocation const& allocation) override;
  void* map(Allocation const& allocation) override;
  void unmap(Allocation const& allocation) override;
};
}

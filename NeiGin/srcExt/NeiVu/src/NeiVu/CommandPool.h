#pragma once

#include "DeviceObject.h"

namespace Nei::Vu {
  class NEIVU_EXPORT CommandPool : public DeviceObject {
  public:
    CommandPool(DeviceContext* dc, uint queueIndex = DefaultQueue);
    virtual ~CommandPool();

    void reset();

    uint getQueueIndex() const{return queueIndex;}

    vk::CommandPool operator*() const { return commandPool; }
    operator vk::CommandPool() const { return commandPool; }
  protected:
    vk::CommandPool commandPool;
    uint queueIndex;
  };
};

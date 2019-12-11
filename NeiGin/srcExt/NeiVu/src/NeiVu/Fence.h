#pragma once

#include "DeviceObject.h"

namespace Nei::Vu{
  class NEIVU_EXPORT Fence : public DeviceObject {
  public:
    Fence(DeviceContext* dc, bool signaled = false);
    virtual ~Fence();

    bool wait(uint64 timeout = UINT64_MAX);
    bool isSignaled();
    void reset();

    vk::Fence operator*() const { return fence; }
    operator vk::Fence() const { return fence; }
  protected:
    vk::Fence fence;
  };
};



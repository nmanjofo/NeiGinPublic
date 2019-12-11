#pragma once

#include "DeviceObject.h"

namespace Nei::Vu {  
  class NEIVU_EXPORT SamplerManager : public DeviceObject {
  public:

    SamplerManager(DeviceContext* dc);
    virtual ~SamplerManager();

    vk::Sampler getSampler(SamplerType type);
  protected:
    vk::Sampler createSampler(SamplerType type);

    std::unordered_map<SamplerType, vk::Sampler> samplerMap;
  };
};

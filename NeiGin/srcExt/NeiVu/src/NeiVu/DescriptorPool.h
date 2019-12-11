#pragma once

#include "DeviceObject.h"

namespace Nei::Vu {
  class NEIVU_EXPORT DescriptorPool : public DeviceObject {
  public:
    DescriptorPool(DeviceContext* dc, DescriptorSetLayout* layout);
    virtual ~DescriptorPool();

    int getRemainingSets() const { return remainingSets; }

    Ptr<DescriptorSet> allocate();
    std::vector<Ptr<DescriptorSet>> allocate(int count);

    void create(int count);

    operator vk::DescriptorPool() { return descriptorPools[0]; }
    vk::DescriptorPool operator *() { return descriptorPools[0]; }
  protected:
    std::vector<vk::DescriptorPoolSize> sizes;

    std::vector<vk::DescriptorPool> descriptorPools;

    Ptr<DescriptorSetLayout> descriptorLayout;
    int remainingSets = 0;
    int sizeIncrement = 64;
  };
};

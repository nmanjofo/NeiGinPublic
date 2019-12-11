#pragma once

#include "DeviceObject.h"
#include "UniformBuffer.h"

namespace Nei::Vu {
  const size_t WholeSize = ~0ull;

  class NEIVU_EXPORT DescriptorSet : public DeviceObject {
  public:
    DescriptorSet(DeviceContext* dc, DescriptorPool* pool, DescriptorSetLayout* layout, vk::DescriptorSet set);
    virtual ~DescriptorSet();

    template<typename T>
    void update(uint binding, UniformBuffer<T>* buffer) {
      update(binding,buffer->getBuffer());
    }

    template<typename T>
    void update(uint binding, Ptr<UniformBuffer<T>> const& buffer) {
      update(binding, buffer->getBuffer());
    }

    void update(uint binding, Buffer* buffer, uint offset = 0, size_t size = WholeSize);
    void update(uint binding, vk::ImageView view, vk::Sampler sampler={});
    void update(uint binding, AccelerationStructure* as);

    void update(uint binding, std::vector<Buffer*> const& buffers, std::vector<uint> const& offsets={}, std::vector<uint> const& sizes={});
    void update(uint binding, std::vector<vk::ImageView> const& views, vk::Sampler sampler={});

    operator vk::DescriptorSet() const { return descriptorSet; }
    vk::DescriptorSet operator*() const { return descriptorSet; }

    void setName(std::string const& name);
  protected:
    Ptr<DescriptorPool> descriptorPool;
    Ptr<DescriptorSetLayout> descriptorSetLayout;
    vk::DescriptorSet descriptorSet;
  };
};

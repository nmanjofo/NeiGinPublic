#pragma once

#include "NeiVuBase.h"

namespace Nei::Vu {
  class NEIVU_EXPORT DescriptorSetLayout : public Object {
  public:
    DescriptorSetLayout(DeviceContext* dc);
    virtual ~DescriptorSetLayout();

    void addDescriptor(int binding, vk::DescriptorType type, vk::ShaderStageFlags stage, uint count = 1, vk::Sampler* sampler=nullptr);
    void create(bool push=false);

    operator vk::DescriptorSetLayout() const { return descriptorSetLayout; }
    vk::DescriptorSetLayout operator*() const { return descriptorSetLayout; }

    auto const& getBindings() const{return bindings;}
  protected:
    Ptr<DeviceContext> deviceContext;
    vk::DescriptorSetLayout descriptorSetLayout;
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
  };
};

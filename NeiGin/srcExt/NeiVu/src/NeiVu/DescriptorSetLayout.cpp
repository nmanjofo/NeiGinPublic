#include "DescriptorSetLayout.h"

#include "DeviceContext.h"
using namespace Nei::Vu;

DescriptorSetLayout::DescriptorSetLayout(DeviceContext* dc): deviceContext(dc) {
}

DescriptorSetLayout::~DescriptorSetLayout() {
  auto device = deviceContext->getVkDevice();
  if (descriptorSetLayout)
    device.destroyDescriptorSetLayout(descriptorSetLayout);
}

void DescriptorSetLayout::addDescriptor(int binding, vk::DescriptorType type,
                                        vk::ShaderStageFlags stage, uint count,
                                        vk::Sampler* sampler) {
  auto it = std::find_if(bindings.begin(), bindings.end(), [&](vk::DescriptorSetLayoutBinding& b) {
    return b.binding == binding;
  });

  if (it != bindings.end()) {
    it->stageFlags |= stage;
    nei_assert(it->descriptorType == type);
    assert(it->descriptorCount == count);
  } else {
    bindings.emplace_back(binding, type, count, stage, sampler);
  }
}

void DescriptorSetLayout::create(bool push) {
  vk::DescriptorSetLayoutCreateInfo dslci;
  dslci.bindingCount = (uint32)bindings.size();
  dslci.pBindings = bindings.data();
  if (push) dslci.flags |= vk::DescriptorSetLayoutCreateFlagBits::ePushDescriptorKHR;

  descriptorSetLayout = deviceContext->getVkDevice().createDescriptorSetLayout(dslci);
}

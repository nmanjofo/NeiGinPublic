#include "DescriptorPool.h"

#include "DeviceContext.h"

#include "DescriptorSetLayout.h"
#include "DescriptorSet.h"

using namespace Nei::Vu;

DescriptorPool::DescriptorPool(DeviceContext* dc, DescriptorSetLayout* layout):
  DeviceObject(dc), descriptorLayout(layout) {
}

DescriptorPool::~DescriptorPool() {
  auto device = deviceContext->getVkDevice();
  for (auto& d : descriptorPools)
    device.destroyDescriptorPool(d);
}

void DescriptorPool::create(int count) {
  auto device = deviceContext->getVkDevice();
  remainingSets = count;
  for (auto& b : descriptorLayout->getBindings()) {
    sizes.emplace_back(b.descriptorType, b.descriptorCount*count);
  }

  vk::DescriptorPoolCreateInfo dpci;
  dpci.maxSets = count;
  dpci.poolSizeCount = (uint32)sizes.size();
  dpci.pPoolSizes = sizes.data();
  descriptorPools.push_back(device.createDescriptorPool(dpci));
}

Nei::Ptr<DescriptorSet> DescriptorPool::allocate() {
  return allocate(1)[0];
}

std::vector<Nei::Ptr<DescriptorSet>> DescriptorPool::allocate(int count) {
  if (remainingSets == 0) create(sizeIncrement);

  std::vector<Ptr<DescriptorSet>> ret;
  ret.reserve(count);

  if (count > remainingSets) {
    int part1 = remainingSets;
    int part2 = count - part1;
    ret = allocate(part1);
    auto ret2 = allocate(part2);
    ret.insert(ret.end(), ret2.begin(),ret2.end());
    return ret;
  }

  std::vector<vk::DescriptorSetLayout> layouts(count);
  std::fill(layouts.begin(), layouts.end(), *descriptorLayout);

  auto device = deviceContext->getVkDevice();
  vk::DescriptorSetAllocateInfo dsai;
  dsai.descriptorPool = descriptorPools.back();
  dsai.descriptorSetCount = count;
  dsai.pSetLayouts = layouts.data();
  auto sets = device.allocateDescriptorSets(dsai);
  remainingSets -= count;

  for (auto& s : sets) {
    ret.push_back(new DescriptorSet(deviceContext, this, descriptorLayout, s));
  }
  return ret;
}

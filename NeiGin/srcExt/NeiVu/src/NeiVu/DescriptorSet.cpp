#include "DescriptorSet.h"

#include "DeviceContext.h"
#include "DescriptorSetLayout.h"
#include "DescriptorPool.h"
#include "Buffer.h"
#include "Texture.h"
#include "AccelerationStructure.h"

using namespace Nei::Vu;


DescriptorSet::DescriptorSet(DeviceContext* dc, DescriptorPool* pool, DescriptorSetLayout* layout,
                             vk::DescriptorSet set):
  DeviceObject(dc), descriptorPool(pool), descriptorSetLayout(layout), descriptorSet(set) { }

DescriptorSet::~DescriptorSet() {
  auto device = deviceContext->getVkDevice();
  //device.freeDescriptorSets(*descriptorPool, {descriptorSet});
}

void DescriptorSet::update(uint binding, Buffer* buffer, uint offset, size_t size) {
  vk::DescriptorBufferInfo bufferInfo;
  bufferInfo.buffer = *buffer;
  bufferInfo.offset = offset;
  bufferInfo.range = size;

  vk::DescriptorType type;
  bool found = false;
  for(auto &b:descriptorSetLayout->getBindings()) {
    if (b.binding == binding) {
      found = true;
      type = b.descriptorType;
    }
  }
  if(!found) {
    nei_error("binding {} not in set", binding);
    return;
  }

  vk::WriteDescriptorSet write;
  write.descriptorCount = 1;
  write.descriptorType = type;
  write.dstSet = descriptorSet;
  write.dstBinding = binding;
  write.pBufferInfo = &bufferInfo;

  auto device = deviceContext->getVkDevice();
  device.updateDescriptorSets({write},{});
}

void DescriptorSet::update(uint binding, vk::ImageView view,  vk::Sampler sampler) {
  bool storage = false;  
  bool found = false;
  vk::DescriptorType type;
  for (auto &b : descriptorSetLayout->getBindings()) {
    if (b.binding == binding) {
      type = b.descriptorType;
      storage = type == vk::DescriptorType::eStorageImage;
      found = true;
      break;
    }
  }

  nei_assert(found);

  vk::DescriptorImageInfo dii;
  dii.imageView = view;
  dii.sampler = sampler;
  dii.imageLayout = storage ? vk::ImageLayout::eGeneral : vk::ImageLayout::eShaderReadOnlyOptimal;

  vk::WriteDescriptorSet write;
  write.descriptorCount = 1;
  write.descriptorType = type;
  write.dstSet = descriptorSet;
  write.dstBinding = binding;
  write.pImageInfo = &dii;

  auto device = deviceContext->getVkDevice();
  device.updateDescriptorSets({ write }, {});
}

void DescriptorSet::update(uint binding, AccelerationStructure* as) {
  auto vkas = **as;
  vk::WriteDescriptorSetAccelerationStructureNV writeAs;
  writeAs.accelerationStructureCount = 1;
  writeAs.pAccelerationStructures = &vkas;
  
  vk::DescriptorType type;
  for (auto &b : descriptorSetLayout->getBindings()) {
    if (b.binding == binding)type = b.descriptorType;
  }

  vk::WriteDescriptorSet write;
  write.pNext = &writeAs;
  write.descriptorCount = 1;
  write.descriptorType = type;
  write.dstSet = descriptorSet;
  write.dstBinding = binding;

  auto device = deviceContext->getVkDevice();
  device.updateDescriptorSets({ write }, {});
}

void DescriptorSet::update(uint binding, std::vector<Buffer*> const& buffers, std::vector<uint> const& offsets, std::vector<uint> const& sizes) {
  nei_assert(!buffers.empty());
  vk::DescriptorType type;
  bool found = false;
  for(auto &b:descriptorSetLayout->getBindings()) {
    if (b.binding == binding) {
      found = true;
      type = b.descriptorType;
    }
  }
  if(!found) {
    nei_error("binding {} not in set", binding);
    return;
  }

  std::vector<vk::DescriptorBufferInfo> bufferInfo;
  for(uint i=0;i<buffers.size();i++) {
    vk::DescriptorBufferInfo bi;
    bi.buffer = *buffers[i];
    bi.offset = i<offsets.size()?offsets[i]:0;
    bi.range = i<sizes.size()?sizes[i]:WholeSize;
    bufferInfo.push_back(bi);
  }
  
  vk::WriteDescriptorSet write;
  write.descriptorType = type;
  write.dstSet = descriptorSet;
  write.dstBinding = binding;
  write.descriptorCount = uint(bufferInfo.size());
  write.pBufferInfo = bufferInfo.data();

  auto device = deviceContext->getVkDevice();
  device.updateDescriptorSets({write},{});
}

void DescriptorSet::update(uint binding, std::vector<vk::ImageView> const& views, vk::Sampler sampler) {
  nei_assert(!views.empty());
  bool storage = false;  

  vk::DescriptorType type;
  for (auto &b : descriptorSetLayout->getBindings()) {
    if (b.binding == binding) {
      type = b.descriptorType;
      storage = type == vk::DescriptorType::eStorageImage;
    }
  }

  std::vector<vk::DescriptorImageInfo> descriptors;

  for(auto &v:views){
    vk::DescriptorImageInfo dii;
    dii.imageView = v;
    dii.sampler = sampler;
    dii.imageLayout = storage ? vk::ImageLayout::eGeneral : vk::ImageLayout::eShaderReadOnlyOptimal;
    descriptors.push_back(dii);
  }

  vk::WriteDescriptorSet write;
  write.descriptorType = type;
  write.dstSet = descriptorSet;
  write.dstBinding = binding;
  write.descriptorCount = uint(descriptors.size());
  write.pImageInfo = descriptors.data();

  auto device = deviceContext->getVkDevice();
  device.updateDescriptorSets({ write }, {});
}

void DescriptorSet::setName(std::string const& name) {
  setObjectName(name,vk::ObjectType::eDescriptorSet,uint64(VkDescriptorSet(descriptorSet)));
}

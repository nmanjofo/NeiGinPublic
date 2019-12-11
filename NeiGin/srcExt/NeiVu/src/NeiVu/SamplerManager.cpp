#include "SamplerManager.h"

#include "DeviceContext.h"

using namespace Nei::Vu;

SamplerManager::SamplerManager(DeviceContext* dc): DeviceObject(dc) { }

SamplerManager::~SamplerManager() {
  auto device = deviceContext->getVkDevice();
  for (auto& [type,sampler] : samplerMap) {
    device.destroySampler(sampler);
  }
}

vk::Sampler SamplerManager::getSampler(SamplerType type) {
  auto sampler = samplerMap[type];
  if (!sampler){
    sampler = createSampler(type);
    samplerMap[type] = sampler;
  }
  return sampler;
}

vk::Sampler SamplerManager::createSampler(SamplerType type) {
  vk::SamplerCreateInfo sci;
  sci.minLod = 0;
  sci.maxLod = 16;
  sci.anisotropyEnable = true;
  sci.maxAnisotropy = 8;
  sci.unnormalizedCoordinates = false;

  switch (type) {
  case SamplerType::nearestEdge:
  case SamplerType::nearestRepeat:
    sci.minFilter = vk::Filter::eNearest;
    sci.magFilter = vk::Filter::eNearest;
    sci.mipmapMode = vk::SamplerMipmapMode::eNearest;
    break;
  case SamplerType::linearEdge:
  case SamplerType::linearRepeat:
  case SamplerType::shadow:
    sci.minFilter = vk::Filter::eLinear;
    sci.magFilter = vk::Filter::eLinear;
    sci.mipmapMode = vk::SamplerMipmapMode::eLinear;
    break;
  }

  switch (type) {
  case SamplerType::nearestEdge:
  case SamplerType::linearEdge:
    sci.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    sci.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    sci.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    break;
  case SamplerType::nearestRepeat:
  case SamplerType::linearRepeat:
    sci.addressModeU = vk::SamplerAddressMode::eRepeat;
    sci.addressModeV = vk::SamplerAddressMode::eRepeat;
    sci.addressModeW = vk::SamplerAddressMode::eRepeat;
    break;
  case SamplerType::shadow:
    sci.addressModeU = vk::SamplerAddressMode::eClampToBorder;
    sci.addressModeV = vk::SamplerAddressMode::eClampToBorder;
    sci.addressModeW = vk::SamplerAddressMode::eClampToBorder;
    sci.borderColor = vk::BorderColor::eFloatOpaqueWhite;
    sci.compareOp = vk::CompareOp::eLess;
    sci.compareEnable = true;
    break;
  }

  return deviceContext->getVkDevice().createSampler(sci);
}

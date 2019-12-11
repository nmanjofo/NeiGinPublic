#include "ShaderBindingTable.h"

#include "DeviceContext.h"
#include "Buffer.h"
#include "RaytracingPipeline.h"
#include "Shader.h"
using namespace Nei::Vu;


ShaderBindingTable::ShaderBindingTable(DeviceContext* dc): DeviceObject(dc) { }

ShaderBindingTable::~ShaderBindingTable() {}

void ShaderBindingTable::create(RaytracingPipeline* pipeline) {
  auto device = deviceContext->getVkDevice();
  auto physicalDevice = deviceContext->getVkPhysicalDevice();

  vk::PhysicalDeviceRayTracingPropertiesNV rtxProps;
  vk::PhysicalDeviceProperties2 props;
  props.pNext = &rtxProps;
  physicalDevice.getProperties2(&props);

  auto handleSize = rtxProps.shaderGroupHandleSize;


  auto hitGroups = pipeline->getHitGroups();

  auto size = uint(handleSize * hitGroups.size());
  std::vector<uint8> handles(size);
  device.getRayTracingShaderGroupHandlesNV(*pipeline, 0, uint(hitGroups.size()), size,handles.data(), deviceContext->getDispatch());

  auto shaders = pipeline->getShaders();

  buffer = new Buffer(deviceContext, size, Buffer::Type::Staging);


  
  uint offset = 0;
  auto ptr =(uint8*) buffer->map();
  rayGenOffset = offset;
  rayGenBuffer = *buffer;
  for (int i = 0; i < hitGroups.size();i++) {
    auto &h = hitGroups[i];
    if (h.generalShader != VK_SHADER_UNUSED_NV && shaders[h.generalShader]->getStage() == vk::ShaderStageFlagBits::eRaygenNV) {
      memcpy(ptr+offset, handles.data()+i*handleSize, handleSize);
      offset += handleSize;
    }
  }

  missOffset = offset;
  missStride = handleSize;
  for (int i = 0; i < hitGroups.size(); i++) {
    auto &h = hitGroups[i];
    if (h.type == vk::RayTracingShaderGroupTypeNV::eGeneral && shaders[h.generalShader]->getStage() == vk::ShaderStageFlagBits::eMissNV) {
      memcpy(ptr + offset, handles.data() + i * handleSize, handleSize);
      offset += handleSize;
      missBuffer = *buffer;
    }
  }

  hitOffset = offset;
  hitStride = handleSize;
  for (int i = 0; i < hitGroups.size(); i++) {
    auto &h = hitGroups[i];
    if (h.type == vk::RayTracingShaderGroupTypeNV::eTrianglesHitGroup) {
      memcpy(ptr + offset, handles.data() + i * handleSize, handleSize);
      offset += handleSize;
      hitBuffer = *buffer;
    }
  }

  buffer->unmap();


}
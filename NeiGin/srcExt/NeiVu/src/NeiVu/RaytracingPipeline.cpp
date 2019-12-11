#include "RaytracingPipeline.h"
#include "DeviceContext.h"

#include "ShaderBindingTable.h"

#include "Shader.h"
#include "CommandBuffer.h"

using namespace Nei::Vu;

RaytracingPipeline::RaytracingPipeline(DeviceContext* deviceContext): Pipeline(deviceContext) {}

RaytracingPipeline::~RaytracingPipeline() {
  if(pipeline)
    deviceContext->getVkDevice().destroyPipeline(pipeline);
}

void RaytracingPipeline::bind(CommandBuffer* cmd) {
  (**cmd).bindPipeline(vk::PipelineBindPoint::eRayTracingNV, pipeline);
}

void RaytracingPipeline::create() {
  if(!pipelineLayout)createLayout();

  vk::RayTracingPipelineCreateInfoNV rpci;
  rpci.flags = {};
  rpci.maxRecursionDepth = recursionDepth;
  rpci.layout = pipelineLayout;
  rpci.groupCount = (uint)hitGroups.size();
  rpci.pGroups = hitGroups.data();
  rpci.stageCount = (uint)stages.size();
  rpci.pStages = stages.data();
  rpci.basePipelineHandle = nullptr;
  rpci.basePipelineIndex = -1;

  pipeline = deviceContext->getVkDevice().createRayTracingPipelineNV(deviceContext->getPipelineCache(), rpci,
    nullptr, deviceContext->getDispatch());
  nei_assert(pipeline);
}

void RaytracingPipeline::addRayGenShader(Shader* shader) {
  addShader(shader);

  vk::RayTracingShaderGroupCreateInfoNV group;
  group.type = vk::RayTracingShaderGroupTypeNV::eGeneral;
  group.generalShader = uint(shaders.size()-1);
  group.anyHitShader = VK_SHADER_UNUSED_NV;
  group.closestHitShader = VK_SHADER_UNUSED_NV;
  group.intersectionShader = VK_SHADER_UNUSED_NV;
  hitGroups.push_back(group);
}

void RaytracingPipeline::addMissShader(Shader* shader) {
  addShader(shader);

  vk::RayTracingShaderGroupCreateInfoNV group;
  group.type = vk::RayTracingShaderGroupTypeNV::eGeneral;
  group.generalShader = uint(shaders.size()-1);
  group.anyHitShader = VK_SHADER_UNUSED_NV;
  group.closestHitShader = VK_SHADER_UNUSED_NV;
  group.intersectionShader = VK_SHADER_UNUSED_NV;
  hitGroups.push_back(group);
}

void RaytracingPipeline::addHitShader(Shader* closest, Shader* any, Shader* intersection) {
  
  vk::RayTracingShaderGroupCreateInfoNV group;
  group.type = vk::RayTracingShaderGroupTypeNV::eTrianglesHitGroup;
  group.generalShader = VK_SHADER_UNUSED_NV;
  group.anyHitShader = VK_SHADER_UNUSED_NV;
  group.closestHitShader = VK_SHADER_UNUSED_NV;
  group.intersectionShader = VK_SHADER_UNUSED_NV;

  if(closest) {
    addShader(closest);
    group.closestHitShader = uint(shaders.size()-1);
  }

  if(any) {
    addShader(any);
    group.anyHitShader = uint(shaders.size()-1);
  }

  if(intersection) {
    addShader(intersection);
    group.type = vk::RayTracingShaderGroupTypeNV::eProceduralHitGroup;
    group.intersectionShader = uint(shaders.size()-1);
  }

  hitGroups.push_back(group);
}

Nei::Ptr<ShaderBindingTable> RaytracingPipeline::createShaderBindingTable() {
  Ptr ret = new ShaderBindingTable(deviceContext);
  ret->create(this);
  return ret;
}

#include "Pipeline.h"

#include "CommandBuffer.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "DescriptorSet.h"
#include "Shader.h"

using namespace Nei::Vu;

Pipeline::Pipeline(DeviceContext* deviceContext):
  DeviceObject(deviceContext) { }

Pipeline::~Pipeline() {
  auto device = deviceContext->getVkDevice();
  if (pipelineLayout) {
    device.destroyPipelineLayout(pipelineLayout);
    pipelineLayout = nullptr;
  }

}

void Pipeline::addShader(Shader* shader, const char* main) {
  if(!**shader)return;
  shaders.push_back(shader);
  addStage(shader->getStage(),*shader,main);
  shader->addResourcesToPipeline(this);
}

void Pipeline::addStage(vk::ShaderStageFlagBits stage, vk::ShaderModule shader, const char* main) {
  vk::PipelineShaderStageCreateInfo s;
  s.stage = stage;
  s.module = shader;
  s.pName = main;
  stages.emplace_back(s);
}
void Pipeline::addLayout(DescriptorSetLayout* layout) {
  layouts.push_back(layout);
}
void Pipeline::addPushConstantRange(uint32_t size, uint32_t offset, vk::ShaderStageFlags stage) {
  pushConstants.emplace_back(stage, offset, size);
}

Nei::Ptr<DescriptorSet> Pipeline::allocateDescriptorSet(int set) {
  return descriptorPools[set]->allocate();
}

Nei::Ptr<DescriptorSetLayout> Pipeline::getOrCreateDescriptorSetLayout(int i) {
  int dif = 1+i - int(layouts.size());
  for (int j = 0; j < dif; j++)layouts.push_back(new DescriptorSetLayout(deviceContext));
  return layouts[i];
}

void Pipeline::createLayout() {
  std::vector<vk::DescriptorSetLayout> vkLayouts;
  for(auto &l:layouts) {
    if (!**l)l->create();
    descriptorPools.push_back(new DescriptorPool(deviceContext, l));
    vkLayouts.push_back(*l);
  }
  auto device = deviceContext->getVkDevice();

  vk::PipelineLayoutCreateInfo plci;
  plci.pSetLayouts = vkLayouts.data();
  plci.setLayoutCount = (uint32_t)vkLayouts.size();
  plci.pPushConstantRanges = pushConstants.data();
  plci.pushConstantRangeCount = (uint32)pushConstants.size();

  pipelineLayout = device.createPipelineLayout(plci);
  assert(pipelineLayout);
}
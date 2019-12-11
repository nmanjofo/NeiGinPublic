#include "ComputePipeline.h"
#include "DeviceContext.h"

#include "CommandBuffer.h"

using namespace Nei::Vu;

ComputePipeline::ComputePipeline(DeviceContext* deviceContext):Pipeline(deviceContext) {
  
}

void ComputePipeline::create() {
  auto device = deviceContext->getVkDevice();
  if (!pipelineLayout)createLayout();

  vk::ComputePipelineCreateInfo cpci;
  cpci.stage = stages[0];
  cpci.layout = pipelineLayout;
  cpci.basePipelineHandle = nullptr;
  cpci.basePipelineIndex = -1;
  pipeline = device.createComputePipeline(deviceContext->getPipelineCache(), cpci);
  assert(pipeline);
}

void ComputePipeline::bind(CommandBuffer* cmd) {
  (**cmd).bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
}

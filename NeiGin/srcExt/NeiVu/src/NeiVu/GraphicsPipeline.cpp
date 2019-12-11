#include "GraphicsPipeline.h"
#include "DeviceContext.h"
#include "RenderPass.h"
#include "CommandBuffer.h"
#include "VertexLayout.h"

using namespace Nei::Vu;

GraphicsPipeline::GraphicsPipeline(DeviceContext* deviceContext):Pipeline(deviceContext){
  // default pipeline setting

  // input assembly
  primitive.primitiveRestartEnable = false;
  primitive.topology = vk::PrimitiveTopology::eTriangleList;

  //tessellation 
  tessellation.patchControlPoints = 0;

  //viewport 
  viewport.scissorCount = 1;
  viewport.viewportCount = 1;
  // no pointer - dynamic state

  //rasterisation
  rasterisation.cullMode = vk::CullModeFlagBits::eNone;
  rasterisation.depthBiasEnable = false;
  rasterisation.frontFace = vk::FrontFace::eCounterClockwise;
  rasterisation.lineWidth = 1.0f;
  rasterisation.polygonMode = vk::PolygonMode::eFill;
  rasterisation.rasterizerDiscardEnable = false;
  rasterisation.depthClampEnable = false;

  // multisample
  multisample.rasterizationSamples = vk::SampleCountFlagBits::e1;

  // stencil
  stencilBackOp.failOp = vk::StencilOp::eKeep;
  stencilBackOp.passOp = vk::StencilOp::eKeep;
  stencilBackOp.depthFailOp = vk::StencilOp::eKeep;
  stencilBackOp.compareOp = vk::CompareOp::eAlways;
  stencilFrontOp.failOp = vk::StencilOp::eKeep;
  stencilFrontOp.passOp = vk::StencilOp::eKeep;
  stencilFrontOp.depthFailOp = vk::StencilOp::eKeep;
  stencilFrontOp.compareOp = vk::CompareOp::eAlways;

  // depth
  depthStencil.back = stencilBackOp;
  depthStencil.front = stencilFrontOp;

  depthStencil.depthCompareOp = vk::CompareOp::eLessOrEqual;
  depthStencil.depthBoundsTestEnable = false;
  depthStencil.depthTestEnable = false;
  depthStencil.depthWriteEnable = true;
  depthStencil.minDepthBounds = 0;
  depthStencil.maxDepthBounds = 1;

  // attachments
  /*vk::PipelineColorBlendAttachmentState cbat;
  cbat.colorWriteMask = vk::ColorComponentFlagBits::eR |
    vk::ColorComponentFlagBits::eG |
    vk::ColorComponentFlagBits::eB |
    vk::ColorComponentFlagBits::eA;
  attachments.emplace_back(cbat);*/

  // default pipeline state
  dynamicStates.emplace_back(vk::DynamicState::eViewport);
  dynamicStates.emplace_back(vk::DynamicState::eScissor);

}

GraphicsPipeline::~GraphicsPipeline() {
  auto device = deviceContext->getVkDevice();
  for (auto &[config, pipeline] : pipelines) {
    device.destroyPipeline(pipeline);
  }
  pipelines.clear();
}

void GraphicsPipeline::bind(CommandBuffer* cmd) {
  auto renderPass = cmd->getCurrentRenderPass();
  auto& pipeline = pipelines[renderPass];
  if(!pipeline) {
    pipeline = create(renderPass);
  }
  (**cmd).bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
}

vk::Pipeline GraphicsPipeline::create(RenderPass* renderPass) {
  auto device = deviceContext->getVkDevice();

  if(!pipelineLayout)createLayout();

  vk::PipelineDynamicStateCreateInfo dsi;
  dsi.dynamicStateCount = (uint32_t)dynamicStates.size();
  dsi.pDynamicStates = dynamicStates.data();
  
  auto &renderPassAttachments = renderPass->getColorAttachments();
  auto attachemtsCopy = attachments;
  for(size_t i=attachments.size();i<renderPassAttachments.size();i++) {
    // fill extra attachments
    vk::PipelineColorBlendAttachmentState att;
    attachemtsCopy.push_back(att);
  }

  vk::PipelineColorBlendStateCreateInfo cbsci;
  cbsci.attachmentCount = (uint32_t)attachemtsCopy.size();
  cbsci.pAttachments = attachemtsCopy.data();

  vk::PipelineVertexInputStateCreateInfo vis;
  vis.vertexAttributeDescriptionCount = (uint32_t)vertexAttributes.size();
  vis.pVertexAttributeDescriptions = vertexAttributes.data();
  vis.vertexBindingDescriptionCount = (uint32_t)vertexBindings.size();
  vis.pVertexBindingDescriptions = vertexBindings.data();

  vk::GraphicsPipelineCreateInfo gpci;
  gpci.stageCount = (uint32_t)stages.size();
  gpci.pStages = stages.data();

  gpci.pVertexInputState = &vis;
  gpci.pInputAssemblyState = &primitive;
  gpci.pTessellationState = primitive.topology == vk::PrimitiveTopology::ePatchList ? &tessellation : nullptr;
  gpci.pViewportState = &viewport;
  gpci.pRasterizationState = &rasterisation;
  gpci.pMultisampleState = &multisample;
  gpci.pDepthStencilState = &depthStencil;
  gpci.pColorBlendState = &cbsci;
  gpci.pDynamicState = &dsi;

  gpci.renderPass = *renderPass;
  gpci.layout = pipelineLayout;
  gpci.subpass = 0; // subpasses not supported

  gpci.basePipelineHandle = nullptr;
  gpci.basePipelineIndex = -1;

  auto pipeline = device.createGraphicsPipeline(deviceContext->getPipelineCache(), gpci);
  assert(pipeline);
  pipelines[renderPass] = pipeline;
  return pipeline;
}

void GraphicsPipeline::addVertexLayout(VertexLayout const& layout) {
  for(int i=0;i<16;i++) {
    auto &a = layout.attributes[i];
    if( (1<<i)&attributeMask && a.size) {
      addAttribute(0,i,a.format,a.offset);
    }
  }
  addVertexBuffer(0,layout.stride);
}

void GraphicsPipeline::addAttribute(int bufferBinding, int location, vk::Format format, int offset) {
  vk::VertexInputAttributeDescription ad;
  ad.binding = bufferBinding;
  ad.format = format;
  ad.location = location;
  ad.offset = offset;
  vertexAttributes.push_back(ad);
}

void GraphicsPipeline::addVertexBuffer(int bufferBinding, int stride) {
  vk::VertexInputBindingDescription bd;
  bd.binding = bufferBinding;
  bd.inputRate = vk::VertexInputRate::eVertex;
  bd.stride = stride;
  vertexBindings.emplace_back(bd);
}
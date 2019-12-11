#pragma once

#include "Pipeline.h"

namespace Nei::Vu {
  class NEIVU_EXPORT GraphicsPipeline : public Pipeline {
  public:
    GraphicsPipeline(DeviceContext* deviceContext);
    virtual ~GraphicsPipeline();

    void bind(CommandBuffer* cmd) override;
    vk::Pipeline create(RenderPass* renderPass);

    std::vector<vk::VertexInputBindingDescription> vertexBindings;
    std::vector<vk::VertexInputAttributeDescription> vertexAttributes;

    vk::PipelineInputAssemblyStateCreateInfo primitive;
    vk::PipelineTessellationStateCreateInfo tessellation;
    vk::PipelineViewportStateCreateInfo viewport;
    vk::PipelineRasterizationStateCreateInfo rasterisation;
    vk::PipelineMultisampleStateCreateInfo multisample;
    vk::PipelineDepthStencilStateCreateInfo depthStencil;
    vk::StencilOpState stencilFrontOp;
    vk::StencilOpState stencilBackOp;
    std::vector<vk::PipelineColorBlendAttachmentState> attachments;
    std::vector<vk::DynamicState> dynamicStates;

    void addVertexLayout(VertexLayout const& layout);

    void addAttribute(int bufferBinding, int location, vk::Format format, int offset);
    void addVertexBuffer(int bufferBinding, int stride);

    vk::PipelineBindPoint getBindPoint() const override { return vk::PipelineBindPoint::eGraphics; }
  protected:
    std::unordered_map<RenderPass*, vk::Pipeline> pipelines;
    uint16 attributeMask=0;
    friend class Shader;
  };
};

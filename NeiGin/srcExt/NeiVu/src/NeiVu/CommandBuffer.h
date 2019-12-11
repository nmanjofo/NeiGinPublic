#pragma once

#include "DeviceObject.h"

namespace Nei::Vu {
  class NEIVU_EXPORT CommandBuffer : public DeviceObject {
  public:
    CommandBuffer(DeviceContext* dc, bool primary = true, int queueIndex = DefaultQueue);
    CommandBuffer(CommandPool* pool, bool primary = true);
    virtual ~CommandBuffer();

    void copy(Buffer* src, Buffer* dst, size_t size, size_t srcOffset = 0, size_t dstOffset = 0);
    void copy(Buffer* src, Texture* dst, int layer = 0);

    void viewport(ivec2 const& size, ivec2 const& origin = ivec2(0, 0));
    void scissor(ivec2 const& size, ivec2 const& origin = ivec2(0, 0));

    void bind(DescriptorSet* set, int binding = 0);
    void bind(DescriptorSet* set, int binding, vk::PipelineBindPoint bindPoint, vk::PipelineLayout layout);
    void bind(GraphicsPipeline* pipeline);
    void bind(ComputePipeline* pipeline);
    void bind(RaytracingPipeline* pipeline);
    void bind(Buffer* buffer, uint32 binding = 0, uint32 offset = 0);

    void begin(vk::CommandBufferUsageFlags flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    void begin(RenderPass* renderPass, Framebuffer* framebuffer,
               vk::CommandBufferUsageFlags flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit| vk::CommandBufferUsageFlagBits::eRenderPassContinue);
    void end();

    void draw(uint vertexCount, uint instanceCount = 1, uint firstVertex = 0, uint firstInstance = 0);
    void drawIndexed(uint indexCount, uint instanceCount = 1, uint firstIndex = 0, uint firstVertex = 0,
                     uint firstInstance = 0);
    void dispatch(ivec3 const& size);
    void raytrace(ShaderBindingTable* sbt, ivec3 const& size);
    void execute(Ptr<CommandBuffer> const& scmd);

    void submit(bool wait = true);
    void submit(std::vector<vk::Semaphore> wait, std::vector<vk::Semaphore> signal,
                std::vector<vk::PipelineStageFlags> stages);
    void submit(Ptr<Swapchain> const& swapchain);

    auto& getFence() const { return fence; }
    void reset();
    void wait();
    void setFence(Ptr<Fence> const& fence);

    vk::CommandBuffer operator*() const { return commandBuffer; }
    operator vk::CommandBuffer() const { return commandBuffer; }
    vk::CommandBuffer* vkPtr() { return &commandBuffer; }

    void setCurrentRenderPass(RenderPass* renderPass);
    RenderPass* getCurrentRenderPass() const { return currentRenderPass; }

    void setCurrentFramebuffer(Framebuffer* framebuffer);
    Framebuffer* getCurrentFramebuffer() const { return currentFramebuffer; }

    Pipeline* getLastBoundPipeline() const { return lastBoundPipeline; }

    void memoryBarrier(vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage, vk::AccessFlags srcFlags,
                       vk::AccessFlags dstFlags);
    void debugBarrier();

    bool isExecutable() const { return executable; }
  protected:
    bool executable = false;
    vk::CommandBuffer commandBuffer;
    vk::CommandPool pool;
    Ptr<Fence> fence;
    int queueIndex;

    Ptr<Pipeline> lastBoundPipeline;
    Ptr<GraphicsPipeline> boundGraphicsPipeline;
    Ptr<ComputePipeline> boundComputePipeline;
    Ptr<RaytracingPipeline> boundRaytracingPipeline;

    Ptr<RenderPass> currentRenderPass;
    Ptr<Framebuffer> currentFramebuffer;
  };
};

#include "CommandBuffer.h"

#include "Buffer.h"
#include "Framebuffer.h"
#include "RenderPass.h"
#include "GraphicsPipeline.h"
#include "ComputePipeline.h"
#include "RaytracingPipeline.h"

#include "DescriptorSetLayout.h"
#include "DescriptorSet.h"
#include "Swapchain.h"
#include "ShaderBindingTable.h"
#include "Texture.h"
#include "Fence.h"

#include "CommandPool.h"

using namespace Nei;
using namespace Vu;

CommandBuffer::CommandBuffer(DeviceContext* dc, bool primary, int queueIndex): DeviceObject(dc) {
  if(queueIndex == DefaultQueue) queueIndex = dc->getMainQueueIndex();
  this->queueIndex = queueIndex;

  pool = dc->getCommandPool(queueIndex);
  vk::CommandBufferAllocateInfo cbai;
  cbai.commandBufferCount = 1;
  cbai.commandPool = pool;
  cbai.level = primary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary;
  auto buffers = dc->getVkDevice().allocateCommandBuffers(cbai);
  assert(!buffers.empty());
  commandBuffer = buffers[0];
}

CommandBuffer::CommandBuffer(CommandPool* pool, bool primary): DeviceObject(pool->getDeviceContext()) {
  auto device = getDevice();
  queueIndex = pool->getQueueIndex();

  vk::CommandBufferAllocateInfo cbai;
  cbai.commandBufferCount = 1;
  cbai.commandPool = *pool;
  cbai.level = primary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary;
  auto buffers = device.allocateCommandBuffers(cbai);
  assert(!buffers.empty());
  commandBuffer = buffers[0];
}

CommandBuffer::~CommandBuffer() {
  deviceContext->getVkDevice().freeCommandBuffers(pool, {commandBuffer});
}

void CommandBuffer::copy(Buffer* src, Buffer* dst, size_t size, size_t srcOffset, size_t dstOffset) {
  nei_assert(srcOffset+size<=src->getSize());
  nei_assert(dstOffset+size<=dst->getSize());

  vk::BufferCopy bc;
  bc.size = size;
  bc.srcOffset = srcOffset;
  bc.dstOffset = dstOffset;
  commandBuffer.copyBuffer(*src, *dst, {bc});
}

void CommandBuffer::copy(Buffer* src, Texture* dst, int layer) {
  auto size = dst->getBaseSize();
  vk::BufferImageCopy region;
  region.imageOffset = vk::Offset3D(0, 0, 0);
  region.bufferImageHeight = 0;
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.imageExtent = vk::Extent3D(size.x, size.y, size.z);
  region.imageSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, layer, 1);
  commandBuffer.copyBufferToImage(*src, *dst, vk::ImageLayout::eTransferDstOptimal, {region});
}

void CommandBuffer::viewport(ivec2 const& size, ivec2 const& origin) {
  vk::Viewport v(float(origin.x), float(origin.y),
    float(size.x), float(size.y), 0.f, 1.f);
  commandBuffer.setViewport(0, {v});
}

void CommandBuffer::scissor(ivec2 const& size, ivec2 const& origin) {
  vk::Rect2D scissor(vk::Offset2D(origin.x, origin.y), vk::Extent2D(size.x, size.y));
  commandBuffer.setScissor(0, {scissor});
}

void CommandBuffer::bind(DescriptorSet* set, int binding) {
  auto& pipeline = lastBoundPipeline;
  commandBuffer.bindDescriptorSets(pipeline->getBindPoint(), pipeline->getLayout(), binding, {*set}, {});
}

void CommandBuffer::bind(DescriptorSet* set, int binding, vk::PipelineBindPoint bindPoint, vk::PipelineLayout layout) {
  commandBuffer.bindDescriptorSets(bindPoint, layout, binding, {*set}, {});
}

void CommandBuffer::bind(GraphicsPipeline* pipeline) {
  lastBoundPipeline = pipeline;
  if(boundGraphicsPipeline == pipeline) return;
  pipeline->bind(this);
  boundGraphicsPipeline = pipeline;
}

void CommandBuffer::bind(ComputePipeline* pipeline) {
  lastBoundPipeline = pipeline;
  if(boundComputePipeline == pipeline)return;
  pipeline->bind(this);
  boundComputePipeline = pipeline;
}

void CommandBuffer::bind(RaytracingPipeline* pipeline) {
  lastBoundPipeline = pipeline;
  if(boundRaytracingPipeline == pipeline)return;
  pipeline->bind(this);
  boundRaytracingPipeline = pipeline;
}

void CommandBuffer::bind(Buffer* buffer, uint32 binding, uint32 offset) {
  nei_assert(binding<16);
  if(!buffer) return;
  if(buffer->getType() == Buffer::Vertex || buffer->getType() == Buffer::VertexStorage)
    commandBuffer.bindVertexBuffers(binding, {*buffer}, {offset});
  if(buffer->getType() == Buffer::Index || buffer->getType() == Buffer::IndexStorage)
    commandBuffer.bindIndexBuffer(*buffer, offset, vk::IndexType::eUint32);
}

void CommandBuffer::memoryBarrier(vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage,
                                  vk::AccessFlags srcFlags,
                                  vk::AccessFlags dstFlags) {
  vk::MemoryBarrier mb;
  mb.srcAccessMask = srcFlags;
  mb.dstAccessMask = dstFlags;
  commandBuffer.pipelineBarrier(srcStage, dstStage, {}, {mb}, {}, {});
}

void CommandBuffer::begin(vk::CommandBufferUsageFlags flags) {
  reset();
  vk::CommandBufferBeginInfo cbi;
  cbi.flags = flags;
  commandBuffer.begin(cbi);
}

void CommandBuffer::begin(RenderPass* renderPass, Framebuffer* framebuffer, vk::CommandBufferUsageFlags flags) {

  reset();

  vk::CommandBufferInheritanceInfo inh;
  inh.framebuffer = framebuffer ? vk::Framebuffer(*framebuffer) : nullptr;
  inh.renderPass = renderPass ? vk::RenderPass(*renderPass) : nullptr;

  currentFramebuffer = framebuffer;
  currentRenderPass = renderPass;

  vk::CommandBufferBeginInfo cbi;
  cbi.flags = flags;
  cbi.pInheritanceInfo = &inh;
  commandBuffer.begin(cbi);
}

void CommandBuffer::end() {
  executable = true;
  commandBuffer.end();
}

void CommandBuffer::draw(uint vertexCount, uint instanceCount, uint firstVertex, uint firstInstance) {
  commandBuffer.draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::drawIndexed(uint indexCount, uint instanceCount, uint firstIndex, uint firstVertex,
                                uint firstInstance) {
  commandBuffer.drawIndexed(indexCount, instanceCount, firstIndex, firstVertex, firstInstance);
}

void CommandBuffer::dispatch(ivec3 const& size) {
  commandBuffer.dispatch(size.x, size.y, size.z);
}

void CommandBuffer::raytrace(ShaderBindingTable* sbt, ivec3 const& size) {
  commandBuffer.traceRaysNV(sbt->getRayGenBuffer(), sbt->getRayGenOffset(),
    sbt->getMissBuffer(), sbt->getMissOffset(), sbt->getMissStride(),
    sbt->getHitBuffer(), sbt->getHitOffset(), sbt->getHitStride(),
    sbt->getCallableBuffer(), sbt->getCallableOffset(), sbt->getCallableStride(),
    size.x, size.y, size.z, deviceContext->getDispatch());
}

void CommandBuffer::reset() {
  commandBuffer.reset({vk::CommandBufferResetFlagBits::eReleaseResources});

  lastBoundPipeline = nullptr;
  boundComputePipeline = nullptr;
  boundGraphicsPipeline = nullptr;
  boundRaytracingPipeline = nullptr;

  executable = false;
}

void CommandBuffer::submit(bool wait) {
  vk::SubmitInfo si;
  si.commandBufferCount = 1;
  si.pCommandBuffers = &commandBuffer;
  auto queue = deviceContext->getQueue(queueIndex);
  if(!fence) fence = new Fence(deviceContext);
  fence->reset();
  queue.submit({si}, *fence);
  if(wait)queue.waitIdle();
}

void CommandBuffer::submit(std::vector<vk::Semaphore> wait, std::vector<vk::Semaphore> signal,
                           std::vector<vk::PipelineStageFlags> stages) {
  assert(signal.size() == stages.size());
  vk::SubmitInfo si;
  si.commandBufferCount = 1;
  si.pCommandBuffers = &commandBuffer;
  si.waitSemaphoreCount = (uint32)wait.size();
  si.pWaitSemaphores = wait.data();
  si.pWaitDstStageMask = stages.data();
  si.signalSemaphoreCount = (uint32)signal.size();
  si.pSignalSemaphores = signal.data();

  auto queue = deviceContext->getQueue(queueIndex);
  assert(queue);
  if(!fence) fence = new Fence(deviceContext);
  fence->reset();
  queue.submit({si}, *fence);

}

void CommandBuffer::submit(Ptr<Swapchain> const& swapchain) {
  submit({swapchain->getAcquireSemaphore()},
    {swapchain->getPresentSemaphore()}, {vk::PipelineStageFlagBits::eColorAttachmentOutput});
}

void CommandBuffer::execute(Ptr<CommandBuffer> const& scmd) {
  commandBuffer.executeCommands({*scmd});
}

void CommandBuffer::wait() {
  if (!fence)return;
  Profile("CommandBuffer::wait");  
  fence->wait();
}

void CommandBuffer::setFence(Ptr<Fence> const& fence) {
  this->fence = fence;
}

void CommandBuffer::debugBarrier() {
  commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands,
    vk::PipelineStageFlagBits::eAllCommands,
    vk::DependencyFlags(), 0, nullptr, 0, nullptr, 0, nullptr);

}

void CommandBuffer::setCurrentRenderPass(RenderPass* renderPass) { currentRenderPass = renderPass; }

void CommandBuffer::setCurrentFramebuffer(Framebuffer* framebuffer) { currentFramebuffer = framebuffer; }

#include "TransferBuffer.h"

#include "CommandBuffer.h"
#include "Buffer.h"
using namespace Nei;
using namespace Vu;

TransferBuffer::TransferBuffer(DeviceContext* dc) :DeviceObject(dc) {
  //commandBuffer = new CommandBuffer(dc, true, deviceContext->getTransferQueueIndex());
  commandBuffer = new CommandBuffer(dc, true, deviceContext->getMainQueueIndex());
}

TransferBuffer::~TransferBuffer() {
  //nei_info("~TransferBuffer()");
  //nei_assert(isFinished());
}

Buffer* TransferBuffer::createStagingBuffer(uint size) {
  Ptr ret = new Buffer(deviceContext, size, Buffer::Type::Staging);
  buffers.push_back(ret);
  return ret;
}

void TransferBuffer::begin() {
  nei_assertm(!fence,"Transfer buffer already started!");
  fence = deviceContext->getVkDevice().createFenceUnique(vk::FenceCreateInfo());
  commandBuffer->begin();
}

void TransferBuffer::end() {
  commandBuffer->end();
  //auto q = deviceContext->getTransferQueue();
  auto q = deviceContext->getMainQueue();
  vk::SubmitInfo si;
  si.commandBufferCount = 1;
  si.pCommandBuffers = commandBuffer->vkPtr();
  q.submit({si}, *fence);
}

bool TransferBuffer::wait() {
  if (!fence) return true;
  auto device = deviceContext->getVkDevice();
  auto res = device.waitForFences({ *fence }, true, ~0u);
  return res == vk::Result::eSuccess;
}

bool TransferBuffer::isFinished() {
  if (!fence) return true;
  auto device = deviceContext->getVkDevice();
  auto status = device.getFenceStatus(*fence);
  if(status == vk::Result::eSuccess) {
    fence.reset();
  }
  return status == vk::Result::eSuccess;
}

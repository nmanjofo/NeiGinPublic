#include "Buffer.h"

#include "DeviceContext.h"
#include "CommandBuffer.h"
#include "TransferBuffer.h"
#include "MemoryManager.h"

using namespace Nei;
using namespace Vu;


Buffer::Buffer(DeviceContext* dc): DeviceObject(dc) {
}

Buffer::Buffer(DeviceContext* dc, uint size, Type type, MemoryUsage mem): DeviceObject(dc) {
  create(size, type, mem);
}

Buffer::~Buffer() {
  //nei_info("~Buffer()");
  destroy();
}

void Buffer::create(uint size, Type type, MemoryUsage mem) {
  if (buffer) destroy();

  this->type = type;
  this->size = size;
  vk::BufferCreateInfo bufferCreateInfo;
  bufferCreateInfo.size = size;
  switch (type) {
  case Vertex:
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
    memoryType = GpuOnly;
    break;
  case Index:
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst;
    memoryType = GpuOnly;
    break;
  case Indirect:
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eIndirectBuffer | vk::BufferUsageFlagBits::eTransferDst;
    memoryType = GpuOnly;
    break;
  case Storage:
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst;
    memoryType = GpuOnly;
    break;
  case Staging:
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst;
    memoryType = CpuOnly;
    break;
  case Uniform:
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst;
    memoryType = Stream;
    break;
  case Raytracing:
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eRayTracingNV;
    memoryType = GpuOnly;
    break;
  default:
    memoryType = GpuOnly;
    break;
  case VertexStorage:
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::
      BufferUsageFlagBits::eStorageBuffer;
    memoryType = GpuOnly;
    break;
  case IndexStorage:
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::
      BufferUsageFlagBits::eStorageBuffer;
    memoryType = GpuOnly;
    break;
  }

  if (mem != Default) {
    memoryType = mem;
  }

  mappable = memoryType == CpuOnly || memoryType == Stream;

  if (size == 0) return;

  auto device = deviceContext->getVkDevice();
  buffer = device.createBuffer(bufferCreateInfo);
  allocation = deviceContext->getMemoryManager()->allocate(buffer, memoryType);
}

void Buffer::destroy() {
  deviceContext->getMemoryManager()->free(allocation);
  buffer = nullptr;
  allocation = Allocation();
}

void Buffer::resize(uint size) {
  if (this->size == size) return;
  create(size, type, memoryType);
}

void Buffer::setName(std::string const& name) {
  setObjectName(name, vk::ObjectType::eBuffer, uint64(VkBuffer(buffer)));
}

void Buffer::setData(void* data, uint size, uint offset) {
  nei_assert(size + offset <= this->size);
  if (mappable) {
    char* ptr = (char*)map();
    memcpy(ptr + offset, data, size);
    unmap();
  } else {
    auto cmd = deviceContext->getSingleUseCommandBuffer();
    Ptr<Buffer> tempBuffer;
    {
      Scope s(cmd);
      if (size < inlineBufferCopySize) {
        setDataInline(cmd, data, size, offset);
      } else {
        tempBuffer = new Buffer(deviceContext, size, Staging);
        tempBuffer->setData(data, size, offset);
        cmd->copy(tempBuffer, this, size, 0, offset);
      }
    }
    cmd->submit();
  }
}

void Buffer::setDataAsync(TransferBuffer* tb, void* data, uint size, uint offset) {
  nei_assert(size + offset <= this->size);
  auto temp = tb->createStagingBuffer(size);
  temp->setData(data, size, 0);
  auto cmd = tb->getCommandBuffer();
  cmd->copy(temp, this, size, 0, offset);
}

void Buffer::setDataInline(CommandBuffer* cmd, void* data, uint size, uint offset) {
  nei_assert(size< inlineBufferCopySize);
  (**cmd).updateBuffer(buffer, offset, size, data);
}

void Buffer::transferOwnership(CommandBuffer* cmd, uint srcIndex, uint dstIndex) {
  vk::BufferMemoryBarrier bmb;
  bmb.srcQueueFamilyIndex = srcIndex;
  bmb.dstQueueFamilyIndex = dstIndex;
  bmb.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
  bmb.dstAccessMask = {};
  bmb.buffer = buffer;
  (**cmd).pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe, {}, {}, {bmb},
                          {});
}

void* Buffer::map() {
  if (!mappable) nei_error("Buffer not mappable!");
  return deviceContext->getMemoryManager()->map(allocation);
}

void Buffer::unmap() {
  deviceContext->getMemoryManager()->unmap(allocation);
}

Allocation const& Buffer::getAllocation() {
  return allocation;
}

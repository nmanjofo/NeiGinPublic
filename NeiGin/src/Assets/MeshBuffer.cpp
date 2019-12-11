#include "MeshBuffer.h"
#include "NeiVu/CommandBuffer.h"
#include "NeiVu/Buffer.h"
#include "Scene/Mesh.h"
#include "NeiVu/TransferBuffer.h"

using namespace Nei;

MeshBuffer::MeshBuffer(DeviceContext* dc): DeviceObject(dc) { }

MeshBuffer::~MeshBuffer() { }

void MeshBuffer::resizeIndices(uint size) {
  if (!size) return;
  if (!indexBuffer) indexBuffer = new Buffer(deviceContext, size, Buffer::Type::IndexStorage);
  indexBuffer->resize(size);
}

void MeshBuffer::resizeVertices(uint size) {
  if (!size)return;
  if (!vertexBuffer) vertexBuffer = new Buffer(deviceContext, size, Buffer::Type::VertexStorage);
  vertexBuffer->resize(size);
}

void MeshBuffer::updateVertexData(CommandBuffer* cmd, void* data, uint size, uint offset) {
  nei_assert(offset+size>=vertexBuffer->getSize());
  nei_nyi
}

void MeshBuffer::updateIndexData(CommandBuffer* cmd, void* data, uint size, uint offset) {
  nei_assert(offset+size>=indexBuffer->getSize());
  nei_nyi
}

void MeshBuffer::createFromMesh(Mesh* mesh, Ptr<TransferBuffer> tb) {
  std::vector<Ptr<Mesh>> meshes{mesh};
  createFromMeshes(meshes, tb);
}

void MeshBuffer::createFromMeshes(std::vector<Ptr<Mesh>>& meshes, Ptr<TransferBuffer> tb) {

  bool wait = !tb;
  if (wait) {
    tb = new TransferBuffer(deviceContext);
    tb->begin();
  }

  uint vertexSize = 0;
  uint indexSize = 0;

  // count total size
  for (auto& m : meshes) {
    uint iSize = m->getIndexCount() * sizeof(uint32);
    uint vSize = m->getVertexCount() * m->getVertexLayout().stride;

    iSize = sizeAlign(iSize, 0x20);
    vSize = sizeAlign(vSize, 0x20);

    m->indexBufferOffset = indexSize;
    m->vertexBufferOffset = vertexSize;
    m->meshBuffer = this;
    indexSize += iSize;
    vertexSize += vSize;
  }

  // resize buffers
  Ptr stage = tb->createStagingBuffer(vertexSize + indexSize);
  resizeVertices(vertexSize);
  if (indexSize > 0)
    resizeIndices(indexSize);

  uint8* stagePtr = (uint8*)stage->map();

  // copy meshes
  for (auto& m : meshes) {
    auto* vPtr = m->vertexPtr();
    auto* iPtr = m->indexPtr();

    uint iSize = m->getIndexCount() * sizeof(uint32);
    uint vSize = m->getVertexCount() * m->getVertexLayout().stride;

    memcpy(stagePtr + m->vertexBufferOffset, vPtr, vSize);
    if (iSize)
      memcpy(stagePtr + vertexSize + m->indexBufferOffset, iPtr, iSize);
  }

  stage->unmap();

  // copy to gpu
  auto cmd = tb->getCommandBuffer();
  cmd->copy(stage, vertexBuffer, vertexSize, 0, 0);
  if (indexSize)
    cmd->copy(stage, indexBuffer, indexSize, vertexSize, 0);

  //vertexBuffer->transferOwnership(cmd, deviceContext->getTransferQueueIndex(), deviceContext->getMainQueueIndex());
  //indexBuffer->transferOwnership(cmd, deviceContext->getTransferQueueIndex(), deviceContext->getMainQueueIndex());

  if (wait) {
    tb->end();
    tb->wait();
  }
}

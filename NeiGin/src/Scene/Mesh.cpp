#include "Mesh.h"
#include "NeiVu/CommandBuffer.h"
#include "NeiVu/Buffer.h"
#include "Assets/MeshBuffer.h"
#include "NeiVu/TransferBuffer.h"
#include "Material.h"

using namespace Nei;

Mesh::Mesh(DeviceContext* dc): deviceContext(dc) {
  type = staticType();
}

Mesh::~Mesh() {
  //nei_info("~Mesh()");
}

void Mesh::resizeIndices(uint count) {
  indexCount = count;
  indexData.resize(indexCount);
}

void Mesh::resizeVertices(uint count) {
  vertexCount = count;
  vertexData.resize(vertexCount * vertexLayout.stride);
}

void Mesh::bind(CommandBuffer* cmd) {
  Profile("buffer bind");
  cmd->bind(meshBuffer->getVertexBuffer(), 0,vertexBufferOffset);
  cmd->bind(meshBuffer->getIndexBuffer(), 0,indexBufferOffset);
}

void Mesh::draw(CommandBuffer* cmd, uint instanceCount) {
  bind(cmd);
  if (indexCount > 0)
    cmd->drawIndexed(indexCount, instanceCount);
  else
    cmd->draw(vertexCount, instanceCount);
}

void Mesh::upload() {
  meshBuffer = new MeshBuffer(deviceContext);
  meshBuffer->createFromMesh(this);
}

void Mesh::setVertices(void const* data, uint count) {
  resizeVertices(count);
  memcpy(vertexData.data(), data, vertexLayout.stride * count);
}

void Mesh::setIndices(void const* data, uint count) {
  resizeIndices(count);
  memcpy(indexData.data(), data, sizeof(uint32) * count);
}

void Mesh::setMeshBuffer(MeshBuffer* mb) { meshBuffer = mb; }

Material::Type Mesh::getMaterialType() const {
  if (materials.empty()) return Material::Type::Unknown;
  return materials[0]->getType();
}

void Mesh::setMaterials(std::vector<Ptr<Material>> const& materials) {
  this->materials = materials;
}

Ptr<Buffer> Mesh::getVertexBuffer() const {
  return meshBuffer->getVertexBuffer();
}

Ptr<Buffer> Mesh::getIndexBuffer() const {
  return meshBuffer->getIndexBuffer();
}

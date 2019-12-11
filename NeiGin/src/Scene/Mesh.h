#pragma once

#include "Node.h"
#include "NeiVu/VertexLayout.h"
#include "Material.h"

namespace Nei{
  class NEIGIN_EXPORT Mesh : public Node{
  public:
    Mesh(DeviceContext* dc);
    virtual ~Mesh();
    static NodeType staticType(){return NodeType::Mesh;}

    void resizeIndices(uint count);
    void resizeVertices(uint count);
       
    void bind(CommandBuffer* cmd);
    void draw(CommandBuffer* cmd, uint instanceCount = 1);

    void upload();
    
    void setVertices(void const* data, uint count);
    void setIndices(void const* data, uint count);

    void* vertexPtr(){return vertexData.data();}
    uint32* indexPtr(){return indexData.data();}

    vk::PrimitiveTopology getTopology() const { return topology; }
    void setTopology(vk::PrimitiveTopology topology) { this->topology = topology; }
    VertexLayout getVertexLayout() const { return vertexLayout; }
    void setVertexLayout(VertexLayout const& vertexLayout) { this->vertexLayout = vertexLayout; }

    void setMeshBuffer(MeshBuffer* mb);
    auto& getMeshBuffer(){return meshBuffer;}

    uint getIndexCount() const { return indexCount; }
    uint getVertexCount() const { return vertexCount; }

    Material::Type getMaterialType() const;
    void setMaterials(std::vector<Ptr<Material>> const& materials);
    auto& getMaterials() const { return materials; }

    Ptr<Buffer> getVertexBuffer()const;
    uint getVertexBufferOffset()const {return vertexBufferOffset;}

    Ptr<Buffer> getIndexBuffer()const;
    uint getIndexBufferOffset()const{return indexBufferOffset;}

  protected:
    std::vector<Ptr<Material>> materials;
    Ptr<DeviceContext> deviceContext;
    bool keepData = true;
    uint vertexCount = 0;
    uint indexCount = 0;

    vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList;

    VertexLayout vertexLayout;
    std::vector<uint8> vertexData;
    std::vector<uint32> indexData;

    Ptr<MeshBuffer> meshBuffer;
    uint indexBufferOffset = 0;
    uint vertexBufferOffset = 0;

    friend class MeshBuffer;
  };
};



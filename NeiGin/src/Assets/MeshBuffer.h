#pragma once

#include "NeiGinBase.h"
#include "NeiVu/DeviceObject.h"
#include "NeiVu/Buffer.h"
#include "NeiVu/TransferBuffer.h"

namespace Nei{
  class NEIGIN_EXPORT MeshBuffer : public DeviceObject{
  public:
    MeshBuffer(DeviceContext* dc);
    virtual ~MeshBuffer();
    
    void resizeIndices(uint size);
    void resizeVertices(uint size);
       
    auto & getVertexBuffer() const{return vertexBuffer;}
    auto & getIndexBuffer() const{return indexBuffer;}

    void updateVertexData(CommandBuffer* cmd, void* data, uint size, uint offset=0);
    void updateIndexData(CommandBuffer* cmd, void* data, uint size, uint offset=0);

    void createFromMesh(Mesh* mesh, Ptr<TransferBuffer> tb = nullptr);
    void createFromMeshes(std::vector<Ptr<Mesh>>& meshes, Ptr<TransferBuffer>tb = nullptr);

  protected:   
    Ptr<Buffer> vertexBuffer;
    Ptr<Buffer> indexBuffer;
  };
};


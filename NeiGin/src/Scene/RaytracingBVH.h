#pragma once

#include "NeiGinBase.h"
#include "NeiVu/DeviceObject.h"

#include "Material.h"

namespace Nei{
  struct RaytracingInstanceData {
    Ptr<Buffer> vertexBuffer;
    uint vertexBufferOffset=0;
    Ptr<Buffer> indexBuffer;
    uint indexBufferOffset=0;
    Ptr<Material> material;
  };

  class NEIGIN_EXPORT RaytracingBVH : public DeviceObject{
  public:
    RaytracingBVH(DeviceContext* dc);
    virtual ~RaytracingBVH();

    void create(CommandBuffer* cmd, Mesh* mesh);

    void rebuildTop(CommandBuffer* cmd);
    void rebuildBottom(CommandBuffer* cmd, Mesh* mesh);

    AccelerationStructure* getTop() const;
    auto& getInstanceData() const{return instanceData;}
  protected:
    Node* root;
    Ptr<AccelerationStructure> topLevel;
    Ptr<AccelerationStructure> bottomLevel;
    std::vector<RaytracingInstanceData> instanceData;
  };
};



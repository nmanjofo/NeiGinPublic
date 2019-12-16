#pragma once

#include "DeviceObject.h"

namespace vk {
  // missing in vulkan.h
  struct GeometryInstance {
    /// Transform matrix, containing only the top 3 rows
    float transform[12];
    /// Instance index
    uint32_t instanceId : 24;
    /// Visibility mask
    uint32_t mask : 8;
    /// Index of the hit group which will be invoked when a ray hits the instance
    uint32_t instanceOffset : 24;
    /// Instance flags, such as culling
    uint32_t flags : 8;
    /// Opaque handle of the bottom-level acceleration structure
    uint64_t accelerationStructureHandle;
  };
}


namespace Nei::Vu {
  class NEIVU_EXPORT AccelerationStructure : public DeviceObject {
  public:
    AccelerationStructure(DeviceContext* dc);
    virtual ~AccelerationStructure();

    vk::AccelerationStructureNV operator*() const { return structure; }
    operator vk::AccelerationStructureNV() const { return structure; }

    uint64 getHandle() const {return handle;}

    void setUpdatable(bool updatable) {this->updatable=updatable;}

    void create(CommandBuffer* cmd, std::vector<vk::GeometryNV> geometries);
    void update(CommandBuffer* cmd, std::vector<vk::GeometryNV> geometries);

    void create(CommandBuffer* cmd, std::vector<vk::GeometryInstance> instances);
    void update(CommandBuffer* cmd, std::vector<vk::GeometryInstance> instances);

    uint64 getCompactedSize();
  protected:
    bool updatable = false;
    vk::AccelerationStructureNV structure;
    uint64 handle=0;
    Ptr<Buffer> buffer;
    Ptr<Buffer> bufferScratch;
    Ptr<Buffer> bufferInstances;
  };
};

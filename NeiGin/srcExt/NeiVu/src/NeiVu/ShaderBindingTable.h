#pragma once

#include "DeviceObject.h"

namespace Nei::Vu {
  class NEIVU_EXPORT ShaderBindingTable : public DeviceObject {
  public:
    ShaderBindingTable(DeviceContext* dc);
    virtual ~ShaderBindingTable();

    void create(RaytracingPipeline* pipeline);

    vk::Buffer getRayGenBuffer() const { return rayGenBuffer; }
    vk::DeviceSize getRayGenOffset() const { return rayGenOffset; }
    vk::Buffer getMissBuffer() const { return missBuffer; }
    vk::DeviceSize getMissOffset() const { return missOffset; }
    vk::DeviceSize getMissStride() const { return missStride; }
    vk::Buffer getHitBuffer() const { return hitBuffer; }
    vk::DeviceSize getHitOffset() const { return hitOffset; }
    vk::DeviceSize getHitStride() const { return hitStride; }
    vk::Buffer getCallableBuffer() const { return callableBuffer; }
    vk::DeviceSize getCallableOffset() const { return callableOffset; }
    vk::DeviceSize getCallableStride() const { return callableStride; }
  protected:
    Ptr<Buffer> buffer;

    vk::Buffer rayGenBuffer;
    vk::DeviceSize rayGenOffset=0;
    vk::Buffer missBuffer;
    vk::DeviceSize missOffset=0;
    vk::DeviceSize missStride=0;
    vk::Buffer hitBuffer;
    vk::DeviceSize hitOffset=0;
    vk::DeviceSize hitStride=0;
    vk::Buffer callableBuffer;
    vk::DeviceSize callableOffset=0;
    vk::DeviceSize callableStride=0;
  };
};

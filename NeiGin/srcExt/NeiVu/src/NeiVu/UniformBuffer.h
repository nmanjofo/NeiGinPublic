#pragma once

#include "DeviceObject.h"
#include "Buffer.h"

namespace Nei::Vu {
  template<typename T>
  class UniformBuffer : public DeviceObject {
  public:
    UniformBuffer(DeviceContext* dc):DeviceObject(dc) {
      buffer = new Buffer(dc,sizeof(T),Buffer::Type::Uniform,MemoryUsage::GpuOnly);
    }
    virtual ~UniformBuffer(){}

    void upload(CommandBuffer* cmd) {
      buffer->setDataInline(cmd,&data,sizeof(T));
    }

    Buffer* getBuffer() const {return buffer;}
    
    T data;
  protected:
    Ptr<Buffer> buffer;
  };
};

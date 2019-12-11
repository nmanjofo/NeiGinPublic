#pragma once

#include "NeiVuBase.h"
#include "DeviceContext.h"

namespace Nei::Vu {
  class NEIVU_EXPORT DeviceObject : public Object {
  public:
    DeviceObject(DeviceContext* dc);
    virtual ~DeviceObject();

    auto& getDeviceContext() { return deviceContext; }
        
  protected:
    void setObjectName(std::string const& name, vk::ObjectType type, uint64 handle);

    vk::Device getDevice();
    Ptr<DeviceContext> deviceContext;
    std::string debugName; 
  };
};

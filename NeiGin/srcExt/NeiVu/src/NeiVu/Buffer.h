#pragma once

#include "DeviceObject.h"
#include "MemoryManager.h"

namespace Nei::Vu {
  const int inlineBufferCopySize = 65536;

  class NEIVU_EXPORT Buffer : public DeviceObject {
  public:
    enum Type { Vertex, Index, Indirect, Storage, Staging, Uniform, Raytracing, VertexStorage, IndexStorage};

    Buffer(DeviceContext* dc);
    Buffer(DeviceContext* dc, uint size, Type type, MemoryUsage mem = Default);
    virtual ~Buffer();

    void create(uint size, Type type, MemoryUsage mem = Default);
    void destroy();

    void resize(uint size);

    void setName(std::string const& name);

    void setData(void* data, uint size, uint offset = 0);
    void setDataAsync(TransferBuffer* tb, void* data, uint size, uint offset = 0);
    void setDataInline(CommandBuffer* cmd, void* data, uint size, uint offset = 0);

    void transferOwnership(CommandBuffer* cmd, uint srcIndex, uint dstIndex);

    void* map();
    void unmap();

    Allocation const& getAllocation();

    auto getSize() const { return size; }
    auto getVkBuffer() const { return buffer; }
    auto getType() const { return type; }
    auto getMemoryType() const { return memoryType; }
    bool isMappable() const { return mappable; }

    operator vk::Buffer() const { return buffer; }
    vk::Buffer operator*() const { return buffer; }

  protected:
    Type type;
    MemoryUsage memoryType;
    uint64 size = 0;
    vk::Buffer buffer;
    bool mappable = false;
    //VmaAllocation allocation = nullptr;
    //VmaAllocationInfo allocationInfo;
    Allocation allocation;

    std::vector<vk::Fence> transferFences;
  };
};

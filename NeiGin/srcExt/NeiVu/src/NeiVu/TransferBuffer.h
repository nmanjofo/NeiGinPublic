#pragma once

#include "DeviceObject.h"

namespace Nei::Vu {
  class NEIVU_EXPORT TransferBuffer : public DeviceObject {
  public:
    TransferBuffer(DeviceContext* dc);
    virtual ~TransferBuffer();

    Buffer* createStagingBuffer(uint size);
    CommandBuffer* getCommandBuffer() { return commandBuffer; }

    void begin();
    void end();
    bool wait();
    bool isFinished();
  protected:
    Ptr<CommandBuffer> commandBuffer;
    std::vector<Ptr<Buffer>> buffers;
    vk::UniqueFence fence;
  };
};

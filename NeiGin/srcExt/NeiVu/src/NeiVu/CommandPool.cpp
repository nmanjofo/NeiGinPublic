#include "CommandPool.h"

using namespace Nei;
using namespace Vu;

CommandPool::CommandPool(DeviceContext* dc, uint queueIndex): DeviceObject(dc) {
  auto device = getDevice();

  this->queueIndex = queueIndex==DefaultQueue? deviceContext->getMainQueueIndex():queueIndex;

  vk::CommandPoolCreateInfo cpci;
  cpci.queueFamilyIndex = this->queueIndex;
  cpci.flags = vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
  commandPool = device.createCommandPool(cpci);
}

CommandPool::~CommandPool() {
  auto device = getDevice();
  device.destroyCommandPool(commandPool);
}

void CommandPool::reset() {
  auto device = getDevice();
  device.resetCommandPool(commandPool,vk::CommandPoolResetFlagBits::eReleaseResources);
}

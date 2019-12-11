#include "Fence.h"

using namespace Nei;
using namespace Vu;

Fence::Fence(DeviceContext* dc, bool signaled):DeviceObject(dc) {
  vk::FenceCreateInfo fci{};
  if (signaled) fci.flags = vk::FenceCreateFlagBits::eSignaled;
  fence = dc->getVkDevice().createFence(fci);
}

Fence::~Fence() {
  auto device = deviceContext->getVkDevice();
  device.destroyFence(fence);
}

bool Fence::wait(uint64 timeout) {
  //Profile("Fence:wait");
  auto device = deviceContext->getVkDevice();
  auto res = device.waitForFences({ fence },true,timeout);
  return res == vk::Result::eSuccess;
}

bool Fence::isSignaled() {
  auto status = getDevice().getFenceStatus(fence);
  return status == vk::Result::eSuccess;
}

void Fence::reset() {
  auto device = getDevice();
  device.resetFences(fence);
}

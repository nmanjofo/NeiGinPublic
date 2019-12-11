#include "Framebuffer.h"

#include "DeviceContext.h"
#include "Texture.h"
#include "CommandBuffer.h"
#include "RenderPass.h"

using namespace Nei::Vu;


Framebuffer::Framebuffer(DeviceContext* dc):DeviceObject(dc) {
  
}

Framebuffer::~Framebuffer() {
  auto device = deviceContext->getVkDevice();
  for(auto &v:views) {
    device.destroyImageView(v);
  }
}

void Framebuffer::addTexture(Texture2D* texture, vk::ImageAspectFlagBits aspect) {
  views.push_back(texture->createView(aspect));
}

void Framebuffer::setSize(uvec2 const& size) {
  this->size = size;
}

void Framebuffer::create(RenderPass* renderPass) {
  if (framebuffer) {
    nei_error("Framebuffer already created");
  }
  vk::FramebufferCreateInfo fbci;
  fbci.renderPass = *renderPass;
  fbci.attachmentCount = (uint32)views.size();
  fbci.pAttachments = views.data();
  fbci.layers = 1;
  fbci.width = size.x;
  fbci.height = size.y;
  framebuffer = deviceContext->getVkDevice().createFramebufferUnique(fbci);
}
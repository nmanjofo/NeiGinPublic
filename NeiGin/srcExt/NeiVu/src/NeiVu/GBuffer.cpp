#include "GBuffer.h"

#include "DeviceContext.h"
#include "Texture.h"
#include "CommandBuffer.h"
#include "RenderPass.h"
#include "Framebuffer.h"

using namespace Nei::Vu;

GBuffer::GBuffer(DeviceContext* dc): deviceContext(dc) {
}

GBuffer::~GBuffer() {
}

void GBuffer::resize(uvec2 const& size) {
  if (size.x == 0 || size.y == 0) return;
  this->size = size;

  for (auto& l : colorLayers)
    l.texture->resize(size);

  if (depthLayer.texture) {
    depthLayer.texture->resize(size);
  }

  createRenderPass();
  createFrameBuffer();
  createClearValues();

  version++;
}

void GBuffer::addColorLayer(vk::Format format, std::string const& name, vk::ImageLayout layout) {
  GBufferLayer layer;
  layer.format = format;
  layer.name = name;
  layer.finalLayout = layout;
  layer.clear = vk::ClearColorValue(std::array<float, 4>({0, 0, 0, 0}));
  layer.texture = new Texture2D(deviceContext, size, format, Texture::Usage::GBuffer, false);
  if (!name.empty()) { layer.texture->setName(name); } else {
    auto aname = fmt::format("GBuffer layer {}", colorLayers.size());
    layer.texture->setName(aname);
  }

  colorLayers.push_back(layer);
}

void GBuffer::addDepthLayer(vk::Format format, std::string const& name, vk::ImageLayout layout) {
  depthLayer.format = format;
  depthLayer.name = name;
  depthLayer.finalLayout = layout;
  depthLayer.clear = vk::ClearDepthStencilValue({1, 0});
  depthLayer.texture = new Texture2D(deviceContext, size, depthLayer.format, Texture::Usage::ShadowMap, false);
  if (!name.empty())depthLayer.texture->setName(name);
}

void GBuffer::setClearValue(int i, vec4 c) {
  colorLayers[i].clear = vk::ClearColorValue(std::array<float, 4>{c.r, c.g, c.b, c.a});
  createClearValues();
}

void GBuffer::begin(CommandBuffer* cmd, vk::SubpassContents sub, bool clear) {
  if (clear)
    renderPass->begin(cmd, framebuffer, clearValues, sub);
  else
    renderPassContinue->begin(cmd, framebuffer, {}, sub);
  if (sub == vk::SubpassContents::eInline) {
    if (flip)cmd->viewport({size.x, -int(size.y)}, {0, size.y});
    else cmd->viewport(size);
    cmd->scissor(size);
  }
}

void GBuffer::end() {
  renderPass->end();
}

void GBuffer::createRenderPass() {
  renderPass = new RenderPass(deviceContext);

  for (auto& l : colorLayers) {
    renderPass->addAttachment(l.format, vk::ImageLayout::eUndefined, l.finalLayout);
  };
  if (depthLayer.texture) {
    renderPass->addAttachment(depthLayer.format, vk::ImageLayout::eUndefined,
                              vk::ImageLayout::eShaderReadOnlyOptimal);
  }
  renderPass->create();

  
  renderPassContinue = new RenderPass(deviceContext);
  for (int i = 0; i < colorLayers.size();i++) {
    auto& l = colorLayers[i];
    renderPassContinue->addAttachment(l.format, i==0? vk::ImageLayout::eGeneral :vk::ImageLayout::eShaderReadOnlyOptimal, l.finalLayout, vk::AttachmentLoadOp::eLoad,
                                      vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eLoad,
                                      vk::AttachmentStoreOp::eStore);
  }
  if (depthLayer.texture) {
    renderPassContinue->addAttachment(depthLayer.format, vk::ImageLayout::eShaderReadOnlyOptimal,
                                      vk::ImageLayout::eShaderReadOnlyOptimal, vk::AttachmentLoadOp::eLoad,
                                      vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eLoad,
                                      vk::AttachmentStoreOp::eStore);
  }
  renderPassContinue->create();
}

void GBuffer::createFrameBuffer() {
  framebuffer = new Framebuffer(deviceContext);

  for (auto& l : colorLayers) {
    framebuffer->addTexture(l.texture);
  }

  if (depthLayer.texture) {
    framebuffer->addTexture(depthLayer.texture, vk::ImageAspectFlagBits::eDepth);
  }

  framebuffer->setSize(size);
  framebuffer->create(renderPass);
}

void GBuffer::createClearValues() {
  clearValues.clear();
  for (auto& l : colorLayers) {
    clearValues.emplace_back(l.clear);
  }

  if (depthLayer.texture) {
    clearValues.emplace_back(depthLayer.clear);
  }
}

#pragma once

#include "NeiVuBase.h"

namespace Nei::Vu {

  struct GBufferLayer {
    std::string name;
    vk::Format format = vk::Format::eUndefined;
    Ptr<Texture2D> texture;
    vk::ImageLayout finalLayout = vk::ImageLayout::eUndefined;
    vk::ClearValue clear;
  };

  class NEIVU_EXPORT GBuffer : public Object {
  public:
    GBuffer(DeviceContext* dc);
    virtual ~GBuffer();

    uvec2 const& getSize() const { return size; }
    void resize(uvec2 const& size);
    void addColorLayer(vk::Format format, std::string const& name = "",
                       vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);
    void addDepthLayer(vk::Format format = vk::Format::eD32SfloatS8Uint, std::string const& name = "GBuffer Depth",
                       vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);

    Ptr<Texture2D> getLayer(int i) { return colorLayers[i].texture; }
    Ptr<Texture2D> getDepthLayer() { return depthLayer.texture; }

    auto& getRenderPass(bool clear = true) const { return clear ? renderPass : renderPassContinue; }
    auto& getFramebuffer() const { return framebuffer; }

    void setClearValue(int i, vec4 color);

    void setFlip(bool f) { flip = f; }

    void begin(CommandBuffer* cmd, vk::SubpassContents sub = vk::SubpassContents::eInline, bool clear = true);
    void end();

    int getVersion() const { return version; }
  protected:
    void createRenderPass();
    void createFrameBuffer();
    void createClearValues();

    uvec2 size;
    bool flip = false;

    int version = 0;

    Ptr<DeviceContext> deviceContext;
    std::vector<GBufferLayer> colorLayers;
    GBufferLayer depthLayer;

    Ptr<RenderPass> renderPass;
    Ptr<RenderPass> renderPassContinue;
    Ptr<Framebuffer> framebuffer;

    std::vector<vk::ClearValue> clearValues;
  };
};

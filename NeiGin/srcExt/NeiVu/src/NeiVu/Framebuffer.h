#pragma once

#include "DeviceObject.h"

namespace Nei::Vu {
  class NEIVU_EXPORT Framebuffer : public DeviceObject {
  public:
    Framebuffer(DeviceContext* dc);
    virtual ~Framebuffer();

    void addTexture(Texture2D* texture, vk::ImageAspectFlagBits aspect = vk::ImageAspectFlagBits::eColor);

    void setSize(uvec2 const& size);

    operator vk::Framebuffer() const { return *framebuffer; }
    vk::Framebuffer operator*() const { return *framebuffer; }

    void create(RenderPass* renderPass);
    auto& getSize() const { return size; }
  protected:
    vk::UniqueFramebuffer framebuffer;
    std::vector<vk::ImageView> views;
    uvec2 size;

  };
};

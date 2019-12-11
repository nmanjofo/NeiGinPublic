#pragma once

#include "DeviceObject.h"

namespace Nei::Vu {
  class NEIVU_EXPORT RenderPass : public DeviceObject {
  public:
    RenderPass(DeviceContext* dc);
    virtual ~RenderPass();

    void addAttachment(vk::Format format, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout,
      vk::AttachmentLoadOp loadOp = vk::AttachmentLoadOp::eClear,
      vk::AttachmentStoreOp storeOp = vk::AttachmentStoreOp::eStore, 
      vk::AttachmentLoadOp stencilLoadOp = vk::AttachmentLoadOp::eClear,
      vk::AttachmentStoreOp stencilStoreOp = vk::AttachmentStoreOp::eStore);

    auto & getColorAttachments()const { return colorAttachments; }

    void addSubpass(vk::ArrayProxy<int> colorAttachments, bool depthStencil = true);

    void setSize(ivec2 const& size);

    void create();

    void begin(CommandBuffer* cmd, Framebuffer* framebuffer, std::vector<vk::ClearValue> clearValues={}, vk::SubpassContents subpass = vk::SubpassContents::eInline);
    void end();

    operator vk::RenderPass() const { return *renderPass; }
    vk::RenderPass operator*() const { return *renderPass; }

    void setName(std::string const& name);
  protected:
    vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1;
    vk::UniqueRenderPass renderPass;

    std::vector<vk::AttachmentDescription> colorAttachments;
    std::vector<vk::AttachmentDescription> depthAttachments;
    std::vector<vk::AttachmentReference> colorReferences;
    vk::AttachmentReference depthReference;

    // Scope
    Ptr<CommandBuffer> scopeCommandBuffer;
  };
};

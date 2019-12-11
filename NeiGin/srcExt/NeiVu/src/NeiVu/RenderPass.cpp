#include "RenderPass.h"

#include "DeviceContext.h"
#include "Texture.h"
#include "CommandBuffer.h"
#include "Format.h"
#include "Framebuffer.h"

using namespace Nei::Vu;
RenderPass::RenderPass(DeviceContext* dc): DeviceObject(dc) { }

RenderPass::~RenderPass() { }

void RenderPass::addAttachment(vk::Format format, vk::ImageLayout initialLayout,
                               vk::ImageLayout finalLayout, vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp,
                               vk::AttachmentLoadOp stencilLoadOp, vk::AttachmentStoreOp stencilStoreOp) {

  bool depth = isDepthFormat(format);

  vk::AttachmentDescription desc;
  desc.format = format;
  desc.finalLayout = finalLayout;
  desc.initialLayout = initialLayout;
  desc.loadOp = loadOp;
  desc.storeOp = storeOp;
  desc.stencilLoadOp = stencilLoadOp;
  desc.stencilStoreOp = stencilStoreOp;
  desc.samples = samples;

  if (depth) {
    if (!depthAttachments.empty()) {
      nei_error("RenderPass::addAttachment Multiple Depth Attachments! - ignoring");
      return;
    }
    depthAttachments.push_back(desc);
    vk::AttachmentReference ref;
    ref.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    ref.attachment = 0; // will change in create()
    depthReference = ref;

  } else {
    colorAttachments.push_back(desc);
    vk::AttachmentReference ref;
    ref.layout = vk::ImageLayout::eColorAttachmentOptimal;
    ref.attachment = (uint)colorAttachments.size() - 1;
    colorReferences.push_back(ref);
  }
}

void RenderPass::addSubpass(vk::ArrayProxy<int> colorAttachments, bool depthStencil) {
  nei_error("Subpasses not supported!");
}

void RenderPass::create() {
  vk::SubpassDescription subpass;
  subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
  subpass.pColorAttachments = colorReferences.data();
  subpass.colorAttachmentCount = (uint32)colorReferences.size();
  
  if(!depthAttachments.empty()){
    depthReference.attachment = (uint32)colorReferences.size(); // depth attachment is last in list
    subpass.pDepthStencilAttachment = &depthReference;
  }
  
  auto allAttachments = colorAttachments;
  allAttachments.insert(allAttachments.end(), depthAttachments.begin(), depthAttachments.end());
  vk::RenderPassCreateInfo info;
  info.pAttachments = allAttachments.data();
  info.attachmentCount = (uint32)allAttachments.size();
  info.pSubpasses = &subpass;
  info.subpassCount = 1;

  renderPass = deviceContext->getVkDevice().createRenderPassUnique(info);
}

void RenderPass::begin(CommandBuffer* cmd, Framebuffer* framebuffer, std::vector<vk::ClearValue> clearValues, vk::SubpassContents subpass) {
  assert(!scopeCommandBuffer);
  scopeCommandBuffer = cmd;
  auto size = framebuffer->getSize();

  vk::RenderPassBeginInfo rpbi;
  rpbi.renderPass = *renderPass;
  rpbi.framebuffer = *framebuffer;
  rpbi.renderArea = vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(size.x, size.y));
  rpbi.clearValueCount = (uint32)clearValues.size();
  rpbi.pClearValues = clearValues.data();
  (**cmd).beginRenderPass(rpbi, subpass);

  cmd->setCurrentFramebuffer(framebuffer);
  cmd->setCurrentRenderPass(this);
}

void RenderPass::end() {
  auto cmd = scopeCommandBuffer;
  (**cmd).endRenderPass();
  cmd->setCurrentFramebuffer(nullptr);
  cmd->setCurrentRenderPass(nullptr);
  scopeCommandBuffer = nullptr;
}

void RenderPass::setName(std::string const& name) {
  setObjectName(name, vk::ObjectType::eRenderPass, uint64(VkRenderPass(*renderPass)));
}

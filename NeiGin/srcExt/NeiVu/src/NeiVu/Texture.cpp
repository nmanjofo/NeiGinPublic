#include "Texture.h"

#include "DeviceContext.h"
#include "CommandBuffer.h"
#include "Format.h"
#include "Buffer.h"
#include "TransferBuffer.h"
#include "MemoryManager.h"

using namespace Nei::Vu;

Texture::Texture(DeviceContext* dc): DeviceObject(dc) { }

Texture::~Texture() {
  //nei_info("~Texture()");
  destroy();
}

void Texture::resize(uvec3 size) {
  if(size == this->size)return;
  destroy();

  this->size = size;

  if (mipmap)
    levels = (int)ceil(glm::log2(float(max(max(size.x, size.y),size.z))));
  levels = max(levels,1u);

  create(type, format, size, layers, levels, usage);

  setName(debugName);
}

vk::ImageSubresourceRange Texture::getFullRange() {
  vk::ImageSubresourceRange range;
  range.baseArrayLayer = 0;
  range.layerCount = layers;
  range.baseMipLevel = 0;
  range.levelCount = levels;
  range.aspectMask = isDepthFormat(format)
                       ? vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil
                       : vk::ImageAspectFlagBits::eColor;
  return range;
}

void Texture::setLayout(CommandBuffer* cmd, vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
                        vk::ImageSubresourceRange range,
                        vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage) {

  vk::ImageMemoryBarrier imb;
  switch(oldLayout) {
    case vk::ImageLayout::eUndefined:
    case vk::ImageLayout::eGeneral:
      imb.srcAccessMask = {};
      break;
    case vk::ImageLayout::eColorAttachmentOptimal:
      imb.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
      break;
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
      imb.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
      break;
    case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
    case vk::ImageLayout::eShaderReadOnlyOptimal:
      imb.srcAccessMask = vk::AccessFlagBits::eShaderRead;
      break;
    case vk::ImageLayout::eTransferSrcOptimal:
      imb.srcAccessMask = vk::AccessFlagBits::eTransferRead;
      break;
    case vk::ImageLayout::eTransferDstOptimal:
      imb.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
      break;
    case vk::ImageLayout::ePreinitialized:
      imb.srcAccessMask = vk::AccessFlagBits::eHostWrite;
      break;
    case vk::ImageLayout::ePresentSrcKHR:
      imb.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
      break;
  }

  switch(newLayout) {

    case vk::ImageLayout::eUndefined:
      break;
    case vk::ImageLayout::eGeneral:
      imb.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
      break;
    case vk::ImageLayout::eColorAttachmentOptimal:
      imb.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
      break;
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
      break;
    case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
      break;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
      break;
    case vk::ImageLayout::eTransferSrcOptimal:
      imb.dstAccessMask = vk::AccessFlagBits::eTransferRead;
      break;
    case vk::ImageLayout::eTransferDstOptimal:
      imb.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
      break;
    case vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal:
      break;
    case vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal:
      break;
    case vk::ImageLayout::ePresentSrcKHR:
      break;
    case vk::ImageLayout::eSharedPresentKHR:
      break;
    default: ;
  }

  imb.image = image;
  imb.newLayout = newLayout;
  imb.oldLayout = oldLayout;

  imb.subresourceRange = range;
  (**cmd).pipelineBarrier(srcStage, dstStage, {}, {}, {}, {imb});

  layout = newLayout;
}

void Texture::setData(void* data, int layer) {
  auto bufferSize = size.x * size.y * size.z * formatSize(format);
  auto buffer = new Buffer(deviceContext, bufferSize, Buffer::Type::Staging);
  buffer->setData(data, bufferSize);

  auto range = getFullRange();
  range.layerCount = 1;
  range.baseArrayLayer = layer;
  auto cmd = deviceContext->getSingleUseCommandBuffer();
  cmd->begin();
  setLayout(cmd, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, range);
  cmd->copy(buffer, this, layer);
  setLayout(cmd, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal, range);
  cmd->end();
  cmd->submit();
}

void Texture::setDataAsync(TransferBuffer* tb, void* data, int layer) {

  auto bufferSize = size.x * size.y * size.z * formatSize(format);
  auto buffer = tb->createStagingBuffer(bufferSize);
  buffer->setData(data, bufferSize);

  auto range = getFullRange();
  range.layerCount = 1;
  range.baseArrayLayer = layer;

  auto cmd = tb->getCommandBuffer();
  setLayout(cmd, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, range);
  cmd->copy(buffer, this, layer);
  setLayout(cmd, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal, range);
}

void Texture::generateMipMaps(CommandBuffer* cmd) {
  bool wait = !cmd;
  if(wait) {
    cmd = deviceContext->getSingleUseCommandBuffer();
    cmd->begin();
  }

  auto range = getFullRange();
  range.baseMipLevel = 0;
  range.levelCount = 1;


  setLayout(cmd, layout, vk::ImageLayout::eTransferSrcOptimal, range);

  for(uint i = 1; i < levels; i++) {

    vk::ImageBlit region;
    region.srcOffsets[0] = vk::Offset3D();
    region.srcOffsets[1].x = glm::max(1u, size.x >> (i - 1));
    region.srcOffsets[1].y = glm::max(1u, size.y >> (i - 1));
    region.srcOffsets[1].z = glm::max(1u, size.z >> (i - 1));
    region.srcSubresource = vk::ImageSubresourceLayers(range.aspectMask, i - 1, 0, layers);
    region.dstOffsets[0] = vk::Offset3D();
    region.dstOffsets[1].x = glm::max(1u, size.x >> i);
    region.dstOffsets[1].y = glm::max(1u, size.y >> i);
    region.dstOffsets[1].z = glm::max(1u, size.z >> i);
    region.dstSubresource = vk::ImageSubresourceLayers(range.aspectMask, i, 0, layers);

    range.baseMipLevel = i;
    setLayout(cmd, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, range);

    (**cmd).blitImage(image, vk::ImageLayout::eTransferSrcOptimal, image, vk::ImageLayout::eTransferDstOptimal,
      {region}, vk::Filter::eLinear);

    setLayout(cmd, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, range);

  }

  range.baseMipLevel = 0;
  range.levelCount = levels;
  setLayout(cmd, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, range);

  if(wait) {
    cmd->end();
    cmd->submit();
  }
}

void Texture::create(vk::ImageType type, vk::Format format, uvec3 size, int layers, int levels,
                     vk::ImageUsageFlags usage) {
  this->format = format;
  this->size = size;
  this->layers = layers;
  this->levels = levels;
  this->usage = usage;
  this->type = type;

  if (size.x == 0 || size.y == 0 || size.z == 0) return;

  vk::ImageCreateInfo ici;
  ici.flags = flags;
  ici.imageType = type;
  ici.extent = vk::Extent3D(size.x, size.y, size.z);
  ici.format = format;
  ici.mipLevels = levels;
  ici.arrayLayers = layers;
  ici.usage = usage;

  image = deviceContext->getVkDevice().createImage(ici);
  allocation = deviceContext->getMemoryManager()->allocate(image,GpuOnly);
}

void Texture::destroy() {
  if(image) {
    auto device = deviceContext->getVkDevice();
    device.destroyImage(image);
    image = nullptr;
    deviceContext->getMemoryManager()->free(allocation);
  }
}

vk::ImageUsageFlags Texture::usageToFlags(Usage usage) {
  vk::ImageUsageFlags ret;
  switch(usage) {

    case Usage::Sampled:
      ret |= vk::ImageUsageFlagBits::eTransferDst;
      ret |= vk::ImageUsageFlagBits::eTransferSrc;
      ret |= vk::ImageUsageFlagBits::eSampled;
      break;
    case Usage::RenderBuffer:
      ret |= vk::ImageUsageFlagBits::eColorAttachment;
      break;
    case Usage::GBuffer:
      ret |= vk::ImageUsageFlagBits::eTransferSrc;
      ret |= vk::ImageUsageFlagBits::eColorAttachment;
      ret |= vk::ImageUsageFlagBits::eStorage;
      ret |= vk::ImageUsageFlagBits::eSampled;
      break;
    case Usage::DepthBuffer:
      ret |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
      break;
    case Usage::ShadowMap:
      ret |= vk::ImageUsageFlagBits::eTransferSrc;
      ret |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
      ret |= vk::ImageUsageFlagBits::eSampled;
      break;
    default: ;
  }
  return ret;
}

void Texture::setName(std::string const& name) {
  setObjectName(name,vk::ObjectType::eImage,uint64(VkImage(image)));
}

vk::ImageView Texture::createView(vk::ImageAspectFlags aspect) {
  bool isCube = bool(flags & vk::ImageCreateFlagBits::eCubeCompatible);

  vk::ImageViewType viewType = vk::ImageViewType::e2D;
  if(type == vk::ImageType::e1D && layers == 1) viewType = vk::ImageViewType::e1D;
  if(type == vk::ImageType::e1D && layers > 1) viewType = vk::ImageViewType::e1DArray;
  if(type == vk::ImageType::e2D && layers == 1) viewType = vk::ImageViewType::e2D;
  if(type == vk::ImageType::e2D && layers > 1 && !isCube) viewType = vk::ImageViewType::e2DArray;
  if(type == vk::ImageType::e2D && layers > 1 && isCube) viewType = vk::ImageViewType::eCube;
  if(type == vk::ImageType::e3D) viewType = vk::ImageViewType::e3D;

  return createView(viewType, aspect, 0, layers, 0, levels);
}

vk::ImageView Texture::createView(vk::ImageViewType viewType, vk::ImageAspectFlags aspect, int baseLayer, int layers,
                                  int baseLevel, int levels) {

  vk::ImageSubresourceRange range;
  range.aspectMask = aspect;
  range.baseArrayLayer = baseLayer;
  range.layerCount = layers;
  range.baseMipLevel = baseLevel;
  range.levelCount = levels;

  vk::ImageViewCreateInfo iwci;
  iwci.format = format;
  iwci.components = vk::ComponentMapping();
  iwci.image = image;
  iwci.viewType = viewType;
  iwci.subresourceRange = range;

  return deviceContext->getVkDevice().createImageView(iwci);
}

Texture2D::Texture2D(DeviceContext* dc, uvec2 const& size, vk::Format format, Usage usage, bool mipmap)
  : Texture(dc) {
  int levels = 1;
  this->mipmap = mipmap;
  if(mipmap) levels = (int)ceil(glm::log2(float(max(size.x, size.y))));
  levels = max(levels,1);
  flags = {};
  create(vk::ImageType::e2D, format, uvec3(size, 1), 1, levels, usageToFlags(usage));
}

void Texture2D::resize(uvec2 const& size) {
  Texture::resize(uvec3(size, 1));
}

TextureCube::TextureCube(DeviceContext* dc, uint size, vk::Format format, Usage usage, bool mipmap): Texture(dc) {
  int levels = 1;
  this->mipmap = mipmap;
  if(mipmap) levels = (int)ceil(glm::log2(float(size)));
  levels = max(levels,1);
  flags = vk::ImageCreateFlagBits::eCubeCompatible;
  create(vk::ImageType::e2D, format, uvec3(size, size, 1), 6, levels, usageToFlags(usage));
}

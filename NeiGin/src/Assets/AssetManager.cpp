#include "AssetManager.h"
#include "NeiVu/Texture.h"
#include "ImageLoader.h"
#include "NeiVu/FxLoader.h"
#include "Application/Application.h"
#include "NeiVu/Pipeline.h"

#include "NeiVu/TransferBuffer.h"
#include "NeiVu/CommandBuffer.h"

#include "Assets/ImageLoader.h"

using namespace Nei;

vk::Format imageToVkFormat(Image::Format format, bool srgb) {
  switch(format) {
    case Image::Format::Unknown:
      break;
    case Image::Format::BGRA:
      return srgb ? vk::Format::eB8G8R8A8Srgb : vk::Format::eB8G8R8A8Unorm;
    case Image::Format::RGBA:
      return srgb ? vk::Format::eR8G8B8A8Srgb : vk::Format::eR8G8B8A8Unorm;
  }
  nei_error("Unsupported image format!");
  return srgb ? vk::Format::eR8G8B8A8Srgb : vk::Format::eR8G8B8A8Unorm;
}


AssetManager::AssetManager(DeviceContext* dc): DeviceObject(dc) {
  imageLoader = new ImageLoader();
}

AssetManager::~AssetManager() { }

Ptr<Texture2D> AssetManager::loadTexture2D(fs::path const& path, Ptr<TransferBuffer> tb) {
  auto& tex = textures[path.string()];

  if(!tex) {
    auto& img = images[path.string()];
    if(!img) img = imageLoader->loadImage(path);
    if(!img) return nullptr;

    bool wait = !tb;
    if(wait) {
      tb = new TransferBuffer(deviceContext);
      tb->begin();
    }
    tex = new Texture2D(deviceContext, img->getSize(), imageToVkFormat(img->getFormat(), false));
    tex->setDataAsync(tb, img->getData());
    tex->generateMipMaps(tb->getCommandBuffer());

    if(wait) {
      tb->end();
      tb->wait();
    }
  }

  return tex;
}

Ptr<Texture2D> AssetManager::loadTexture2D(void* data, uint size, std::string const& name, Ptr<TransferBuffer> tb) {
  auto& tex = textures[name];

  if(!tex) {
    auto& img = images[name];
    if(!img) img = imageLoader->loadImage(data, size, name);
    if(!img) return nullptr;

    bool wait = !tb;
    if(wait) {
      tb = new TransferBuffer(deviceContext);
      tb->begin();
    }
    tex = new Texture2D(deviceContext, img->getSize(), imageToVkFormat(img->getFormat(), false));
    tex->setDataAsync(tb, img->getData());
    tex->generateMipMaps(tb->getCommandBuffer());

    if(wait) {
      tb->end();
      tb->wait();
    }
  }

  return tex;
}

Ptr<TextureCube> AssetManager::loadTextureCube(fs::path path[6], Ptr<TransferBuffer> tb) {
  bool wait = !tb;
  if(wait) {
    tb = new TransferBuffer(deviceContext);
    tb->begin();
  }
  Ptr<TextureCube> tex;
  for(int i = 0; i < 6; i++) {
    auto& img = images[path[i].string()];
    if(!img) img = imageLoader->loadImage(path[i]);
    if(!img) continue;
    if(!tex) tex = new TextureCube(deviceContext, img->getSize().x, imageToVkFormat(img->getFormat(), false));

    tex->setDataAsync(tb, img->getData(), i);
  }
  tex->generateMipMaps(tb->getCommandBuffer());

  if(wait) {
    tb->end();
    tb->wait();
  }
  return tex;
}

Ptr<Pipeline> AssetManager::loadFx(fs::path const& path) {
  return deviceContext->getFxLoader()->loadFxFile(NeiFS->resolve(path));
}

Ptr<Texture2D> AssetManager::createDummy(uvec2 const& size, Ptr<TransferBuffer> tb) {
  Ptr img = imageLoader->dummyImage(size);

  bool wait = !tb;
  if(wait) {
    tb = new TransferBuffer(deviceContext);
    tb->begin();
  }
  Ptr tex = new Texture2D(deviceContext, img->getSize(), imageToVkFormat(img->getFormat(), false));
  tex->setDataAsync(tb, img->getData());
  tex->generateMipMaps(tb->getCommandBuffer());

  if(wait) {
    tb->end();
    tb->wait();
  }
  return tex;
}

Ptr<Texture2D> AssetManager::createPixelTexture(vec4 const& color, Ptr<TransferBuffer> tb) {
  bool wait = !tb;
  if(wait) {
    tb = new TransferBuffer(deviceContext);
    tb->begin();
  }
  Ptr tex = new Texture2D(deviceContext, ivec2(1, 1), vk::Format::eR8G8B8A8Unorm);
  u8vec4 data = color*255.0f;
  tex->setDataAsync(tb, (void*)&data[0]);
  if(wait) {
    tb->end();
    tb->wait();
  }
  return tex;
}

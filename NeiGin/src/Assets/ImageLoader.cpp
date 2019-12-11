#include "ImageLoader.h"

#include "Application/Application.h"
#include "Image.h"
#include "NeiVu/Format.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace Nei;

Ptr<Image> ImageLoader::loadImage(void* data, uint size, std::string const& name) {
  int w, h, ch;
  stbi_set_flip_vertically_on_load(true);
  auto pixels = stbi_load_from_memory((uint8*)data, size, &w, &h, &ch, 4);

  if(!pixels) {
    nei_warning("Failed to load embedded image! {}", name);
    if(generateDummy) return dummyImage();
    return nullptr;
  }

  Ptr img = new Image();
  img->create({w, h}, Image::Format::RGBA, pixels);
  stbi_image_free(pixels);
  return img;
}

Ptr<Image> ImageLoader::loadImage(fs::path const& path) {
  auto p = NeiFS->resolve(path);

  int w, h, ch;
  stbi_set_flip_vertically_on_load(true);
  auto pixels = stbi_load(p.string().c_str(), &w, &h, &ch, 4);

  if(!pixels) {
    nei_warning("Image not found or unknown format! {}", path.string());
    if(generateDummy) return dummyImage();
    return nullptr;
  }

  Ptr img = new Image();
  img->create({w, h}, Image::Format::RGBA, pixels);
  stbi_image_free(pixels);
  return img;
}

Ptr<Image> ImageLoader::dummyImage(uvec2 const& size) {
  Ptr img = new Image;
  img->create(size);

  static auto gen = std::bind(std::uniform_real_distribution<float>(0, 1), std::mt19937(666));

  u8vec4 a = color({gen(), gen(), gen(), 1});
  u8vec4 b = color({gen(), gen(), gen(), 1});
  int tile = 8;
  auto* ptr = img->getPixels();
  for(uint y = 0; y < size.y; y++)
    for(uint x = 0; x < size.x; x++)
      ptr[y * size.x + x] = ((x / tile) & 1) ^ ((y / tile) & 1) ? a : b;
  return img;
}

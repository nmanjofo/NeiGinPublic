#include "Image.h"

using namespace Nei;

Image::Image() { }

Image::~Image() { }

void Image::create(uvec2 const& size, Format format, uint8* data) {
  if(format == Format::RGBA || format == Format::BGRA) {
    this->size = size;
    this->format = format;
    this->data.resize(size.x * size.y* sizeof(u8vec4));

    if(data)
      memcpy(this->data.data(), data, size.x * size.y * sizeof(u8vec4));

  } else {
    nei_nyi;
  }
}

u8vec4* Image::getPixels() {
  switch(format) {
    case Format::RGBA:
    case Format::BGRA:
      return reinterpret_cast<u8vec4*>(data.data());
    case Format::DXT1:
    case Format::Unknown:
    nei_error("Image::getPixels on compressed image!");
      return nullptr;
    default:
      return nullptr;
  }
}

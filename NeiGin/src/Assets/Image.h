#pragma once

#include "NeiGinBase.h"

namespace Nei {
  class NEIGIN_EXPORT Image : public Object {
  public:
    enum class Format{
      Unknown,
      RGBA,
      BGRA,
      DXT1
    };

    Image();
    virtual ~Image();

    void create(uvec2 const& size, Format format = Format::RGBA, uint8* data = nullptr);

    uvec2 getSize() const { return size; }
    uint8* getData() { return data.data(); }
    u8vec4* getPixels();
    auto getFormat() const { return format; }

  protected:
    std::vector<uint8> data;
    Format format = Format::Unknown;
    uvec2 size;
  };
};

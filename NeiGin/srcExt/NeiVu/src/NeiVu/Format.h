#pragma once

#include "NeiVuBase.h"

namespace Nei::Vu{
  bool NEIVU_EXPORT isDepthFormat(vk::Format format);
  int NEIVU_EXPORT formatSize(vk::Format format);

  vec4 NEIVU_EXPORT colorF(u8vec4 c);
  u8vec4 NEIVU_EXPORT color(vec4 c);
};



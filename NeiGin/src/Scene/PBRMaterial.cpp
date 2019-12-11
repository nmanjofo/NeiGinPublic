#include "PBRMaterial.h"
#include "NeiVu/Texture.h"

using namespace Nei;

PBRMaterial::PBRMaterial() {
  type = Type::PBR;
}

PBRMaterial::~PBRMaterial() {
  nei_warning("~PBRMaterial()");
}

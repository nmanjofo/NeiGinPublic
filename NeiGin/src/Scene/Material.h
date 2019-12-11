#pragma once

#include "NeiGinBase.h"

namespace Nei{
  class NEIGIN_EXPORT Material : public Object {
  public:
    enum Type: uint64 {
      Unknown=0,
      PBR,
      Custom
    };

    Material();
    virtual ~Material();

    Type getType() const{return type;}

    std::string name;
  protected:
    Type type = Type::Unknown;
  };
};



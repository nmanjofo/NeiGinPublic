#pragma once

#include <atomic>
#include "Export.h"
#include "Log.h"

namespace Nei {
  class Object {
  public:
    Object() noexcept = default;
    virtual ~Object() noexcept = default;

    Object(Object const& o) = delete;
    Object(Object && o) = delete;
    Object& operator=(Object const& o) = delete;

    int getRefCount() const {
      return refCount;
    }

    void ref() const {
      ++refCount;
    }

    void unref() const {
      if (--refCount == 0) {
        delete this;
      }
    }

    template <typename T>
    T* as() {
      return dynamic_cast<T*>(this);
    }

    template <typename T>
    T* sas() {
#ifdef DEBUG
      T* ret = as<T>();
      nei_assert(ret);
      return ret;
#endif
      return static_cast<T*>(this);
    }

  private:
    mutable std::atomic<int> refCount = 0;
  };
};

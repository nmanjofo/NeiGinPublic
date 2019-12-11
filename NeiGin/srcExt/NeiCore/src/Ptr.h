#pragma once

#include <atomic>
#include "Object.h"
#include "Log.h"

namespace Nei {

  template <typename T>
  class Ptr {
  public:
    constexpr Ptr(T* obj = nullptr): ptr(obj) {
      if (ptr)
        ptr->ref();
    }

    Ptr(Ptr const& o): ptr(o.ptr) {
      if (ptr)
        ptr->ref();
    }

    Ptr(Ptr&& o) noexcept {
      std::swap(ptr, o.ptr);
    }

    template <typename B>
    Ptr(Ptr<B>& o): ptr(o.ptr) {
      if (ptr)
        ptr->ref();
    }

    ~Ptr() {
      clear();
    }

    void set(T* obj) {
      if (ptr == obj)return;
      clear();
      ptr = obj;
      if (ptr)
        ptr->ref();
    }

    void clear() {
      if (ptr) {
        ptr->unref();
        reset();
      }
    }

    void reset() {
      ptr = nullptr;
    }

    Ptr& operator=(T* obj) {
      set(obj);
      return *this;
    }

    Ptr& operator=(Ptr const& obj) {
      set(obj.ptr);
      return *this;
    }

    template<typename U>
    Ptr& operator=(Ptr<U>& obj) {
      set(static_cast<T*>(obj.ptr));
      return *this;
    }


    operator T*() const {
      return ptr;
    }

    T* operator->() const {
      return ptr;
    }

    T* get() const {
      return ptr;
    }

    T& operator*() const {
      return *ptr;
    }

    template <typename U>
    bool operator<(Ptr<U> const& o) const {
      return ptr < o.ptr;
    }
    
    template <typename U>
    bool operator==(Ptr<U> const& o) const {
      return ptr == o.ptr;
    }

    template <typename U>
    bool operator!=(Ptr<U> const& o) const {
      return ptr != o.ptr;
    }

    operator bool() const {
      return ptr;
    }

    template <typename U>
    Ptr<U> as() const {
      return Ptr<U>(dynamic_cast<U*>(ptr));
    }

    template <typename U>
    Ptr<U> sas() const {
#ifdef DEBUG
      U* ret = as<U>();
      nei_assert(ret);
      return ret;
#endif
      return Ptr<U>(static_cast<U*>(ptr));
    }

  protected:
    template <typename B>
    friend class Ptr;

    T* ptr = nullptr;
  };
};

namespace std {
  template<typename T> struct hash<Nei::Ptr<T>>
  {
    std::size_t operator()(Nei::Ptr<T> const& s) const noexcept
    {
      return s.ptr;
    }
  };
}

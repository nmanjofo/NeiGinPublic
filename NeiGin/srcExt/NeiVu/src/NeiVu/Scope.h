#pragma once
#include "Ptr.h"

namespace Nei::Vu {

  template <typename T>
  class Scope {
  public:
    Scope(Ptr<T>& s): Scope(s.get()) { }
    
    Scope(T& v): Scope(&v) { }

    Scope(T* ptr): ptr(ptr) {
      if(ptr)ptr->begin();
    }
        
    template <typename... Args>
    Scope(Ptr<T>& s, Args const&... args): Scope(s.get(), args...) { }

    template <typename... Args>
    Scope(T& v, Args const&... args): Scope(&v, args...) { }

    template <typename... Args>
    Scope(T* ptr, Args const& ... args): ptr(ptr) {
      if(ptr)ptr->begin(args...);
    }

    ~Scope() {
      if(ptr)ptr->end();
    }

    T* ptr;
  };
};

#pragma once

#include "NeiGinBase.h"
#include <typeindex>

namespace Nei {
  class BaseListener : public Object {
  public:
    int getId() const { return id; }
  protected:
    int id = 0;
  };

  template <typename T>
  class Listener : public BaseListener {
  public:
    Listener(std::function<void(T const&)> callback, int id): callback(callback), id(id) { }

    void operator()(T const& msg) {
      if(callback) callback(msg);
    }

  protected:
    int id;
    std::function<void(T const&)> callback;
  };

  class Messenger : public Object {
  public:
    Messenger();

    template <typename T>
    int addListener(std::function<void(T const&)> callback) {
      int id = idGen++;
      listeners[std::type_index(typeid( T))].push_back(new Listener<T>(callback,id));
      return id;
    }

    void removeListener(int id) {
      for(auto& [k, v] : listeners) {
        v.erase(std::remove_if(v.begin(), v.end(),
          [&](Ptr<BaseListener> const& l) { return l->getId() == id; }), v.end());
      }
    }

    template <typename T>
    void send(T const& msg) {
      for(auto& l : listeners[std::type_index(typeid( T))]) {
        (*l->as<Listener<T>>())(msg);
      }
    }

  protected:
    int idGen = 0;
    std::map<std::type_index, std::vector<Ptr<BaseListener>>> listeners;
  };

}

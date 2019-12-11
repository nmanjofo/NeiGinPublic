#pragma once

#include "Export.h"
#include "spdlog/spdlog.h"

#define nei_debug(...) {Nei::Logger::getInstance().debug(__VA_ARGS__);}
#define nei_info(...) {Nei::Logger::getInstance().info(__VA_ARGS__);}
#define nei_log nei_info
#define nei_warning(...) {Nei::Logger::getInstance().warning(__VA_ARGS__);}
#define nei_error(...) {Nei::Logger::getInstance().error(__VA_ARGS__);__debugbreak();}
#define nei_fatal(...) {Nei::Logger::getInstance().error(__VA_ARGS__);__debugbreak();exit(-1);}

#define nei_warning_once(...) {static bool once=true; if(once){once = false;nei_warning(__VA_ARGS__); } }

#define nei_nyi { nei_error("Not yet implemented");}

#if defined(DEBUG) || defined(RELEASE_ASSERT)
#define nei_assertm(exp, msg) {if (!(exp)) nei_error("Assert failed: {}",msg); }
#define nei_assert(exp) nei_assertm(exp,#exp)
#else
#define nei_assert(exp)
#define nei_assertm(exp, msg)
#endif

namespace Nei {
  class NEICORE_EXPORT Logger {
  public:
    Logger();

    template <typename... Args>
    void debug(Args ...args) {
      spdlog::debug(args...);
    }

    template <typename... Args>
    void info(Args ...args) {
      spdlog::info(args...);
    }

    template <typename... Args>
    void error(Args ...args) {
      spdlog::error(args...);
    }

    template <typename... Args>
    void warning(Args ...args) {
      spdlog::warn(args...);
    }

    template <typename... Args>
    void fatal(Args ...args) {
      spdlog::error(args...);
      exit(-1);
    }

    static auto& getInstance() { return instance; }
  protected:
    static Logger instance;
  };
}

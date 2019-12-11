#include "Log.h"

Nei::Logger Nei::Logger::instance;

Nei::Logger::Logger() {
  spdlog::set_pattern("[%H:%M:%S][%t]%^[%l] %v%$");  
}

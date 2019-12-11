#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
using namespace glm;

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <iostream>
#include <algorithm>
#include <functional>
#include <thread>
#include <mutex>
#include <deque>
#include <optional>
#include <cassert>
#include <filesystem>
namespace fs = std::filesystem;
#include <random>

#include "Object.h"
#include "Ptr.h"

#include "Log.h"
#include "Profiler.h"
#pragma once

#define TRACY_ON_DEMAND
#define TRACY_ENABLE
#include "tracy/Tracy.hpp"

#define Profile(name) ZoneScopedN(name);
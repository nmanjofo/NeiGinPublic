#pragma once

namespace Nei::Vu {
  struct Allocation {
    vk::DeviceMemory memory = {};
    uint offset = 0;
    uint size = 0;

    bool operator==(Allocation const& o) const {
      return memory == o.memory && offset == o.offset && size == o.size;
    }

    bool operator<(Allocation const& o) const {
      if (memory != o.memory) return memory < o.memory;
      if (offset != o.offset) return offset < o.offset;
      return size < o.size;
    }
  };
}

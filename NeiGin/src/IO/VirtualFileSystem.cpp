#include "VirtualFileSystem.h"

#include <filesystem>

using namespace Nei;

VirtualFileSystem::VirtualFileSystem() {
}

VirtualFileSystem::~VirtualFileSystem() {
  
}

void VirtualFileSystem::mount(std::filesystem::path const& path) {
  auto resolved = resolve(path);

  if (resolved.empty()){
    nei_error("Path does not exist! {}", path.string());
    return;
  }

  if (!is_directory(resolved)) {
    nei_error("Path is not directory! {}", path.string());
    return;
  }

  mountedPaths.push_back(fs::absolute(resolved));
  nei_info("Mounted path: {}", fs::absolute(resolved).string());
}

std::filesystem::path VirtualFileSystem::resolve(std::filesystem::path const& path, bool warn) {
  std::error_code ec;
  if (fs::exists(path, ec)) return path;

  for(auto &m:mountedPaths) {
    fs::path p = m / path;
    if (fs::exists(p,ec)) return p;
    //nei_info("Resolve try {}", p.string());
  }
  if(warn)
    nei_warning("resolve failed {}", path.string());
  return "";
}

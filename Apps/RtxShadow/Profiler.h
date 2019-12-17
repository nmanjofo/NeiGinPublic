#pragma once

#include "Scene/Mesh.h"
#include "NeiVu/DeviceObject.h"
#include <fstream>

class Profiler : public Nei::DeviceObject {
public:
  Profiler(Nei::DeviceContext* dc);

  void init(int markers, int avgFrames, int maxFrames);

  void openLog(fs::path const& path);

  void beginFrame(Nei::CommandBuffer* cmd, int frameId);

  void writeMarker(Nei::CommandBuffer* cmd);
  void checkResults();
  void finish();

protected:
  struct Frame : Nei::Object {
    vk::QueryPool pool;
    int frameID;
    int querries;
    int current = 0;
  };
  
  bool wait = false;
  int maxFrames = 0;
  int avgFrames = 0;
  int markers = 0;
  int currentFrame = 0;
  std::vector<Nei::Ptr<Frame>> frames;

  std::vector<double> acc;
  std::ofstream stream;
};

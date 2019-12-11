#include "Profiler.h"

#include "NeiVu/CommandBuffer.h"

using namespace Nei;

Profiler::Profiler(DeviceContext* dc):DeviceObject(dc) {

}

void Profiler::init(int markers, int avgFrames, int maxFrames) {
  this->markers=markers;
  this->avgFrames = avgFrames;
  this->maxFrames = maxFrames;

  acc.resize(markers-1);
  std::fill(acc.begin(), acc.end(), 0);
}

void Profiler::openLog(fs::path const& path) {
  stream.open(path);
  if(!stream.is_open()) {
    nei_error("Failed to open log for writing! {}", path.string());
    return;
  }
  stream << "frameID,BVH,gBuffer,shadowMask,lighting,copy\n";
}

void Profiler::beginFrame(Nei::CommandBuffer* cmd, int frameId) {
  currentFrame = frameId;
  if (currentFrame >= maxFrames || currentFrame < 0) return;
  Ptr frame = new Frame;
  frames.push_back(frame);

  auto device = getDevice();

  vk::QueryPoolCreateInfo qpci;
  qpci.queryType = vk::QueryType::eTimestamp;
  qpci.queryCount = markers;

  frame->pool = device.createQueryPool(qpci);

  (**cmd).resetQueryPool(frame->pool, 0, markers);

  frame->frameID = frameId;
  frame->querries = markers;
}

void Profiler::writeMarker(CommandBuffer* cmd) {
  if (currentFrame >= maxFrames || currentFrame<0) return;
  auto& frame = frames.back();
  auto qIndex = frame->current++;
  (**cmd).writeTimestamp(vk::PipelineStageFlagBits::eAllCommands, frame->pool, qIndex);
}

void Profiler::checkResults() {
  if (frames.empty()) return;
  auto device = getDevice();

  auto& frame = frames.front();

  uint64 buffer[1024];
  vk::QueryResultFlags flags = vk::QueryResultFlagBits::e64;
  if (wait) flags |= vk::QueryResultFlagBits::eWait;
  auto res = device.getQueryPoolResults(frame->pool, 0, frame->querries, sizeof(buffer), buffer,
    sizeof(uint64), flags);

  if(res== vk::Result::eSuccess) {
    nei_log("***");
    for (int i = 0; i < frame->querries - 1; i++) {
      auto t = (buffer[i + 1] - buffer[i]) * 1e-6;
      nei_log("{}ms",t);
      acc[i]+=t;
    }

    if(frame->frameID%avgFrames == avgFrames-1) {
      stream << frame->frameID/avgFrames << ",";
      for(int i=0;i<acc.size();i++) {
        stream << acc[i]/avgFrames << (i == acc.size()-1 ? "\n":",");  
      }

      stream.flush();
      std::fill(acc.begin(),acc.end(),0);
    }

    frames.erase(frames.begin());
    
  }else if(res == vk::Result::eNotReady) {
    nei_log("q not rdy");
  }


}

void Profiler::finish() {
  wait = true;
  while(!frames.empty())
    checkResults();

  stream.flush();
  stream.close();
}

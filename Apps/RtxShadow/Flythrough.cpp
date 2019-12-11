#include "Flythrough.h"

void Flythrough::setSpeed(float speed)
{
  if (points.empty()) return;
  points[0].time = 0;
  for(int i=1;i<points.size();i++)
  {
    auto& a = points[i - 1];
    auto& b = points[i];
    auto d = distance(a.position, b.position);
    b.time = a.time + d / speed;
  }
}

mat4 Flythrough::sample(float time)
{
  time = fmod(time, points.back().time);

  if (time <= points.front().time) return points.front().getMat();
  if (time >= points.back().time) return points.back().getMat();

  int i = 1;
  while (time > points[i].time)i++;

  auto a = points[i-1];
  auto b = points[i];
  float t = (time - a.time) / (b.time - a.time);

  FlyPoint c;
  c.position = glm::mix(a.position, b.position, t);
  c.fwd = normalize(glm::mix(a.fwd, b.fwd, t));
  c.up = normalize(glm::mix(a.up, b.up, t));
  return c.getMat();
}

mat4 Flythrough::sample(int frame, int totalFrames) {
  float t = float(frame) / float(totalFrames) * points.back().time;
  return sample(t);
}

#include "Args.h"
#include <iostream>

const char* helpString =
  R".(
Usage:
-w 1920 - width
-h 1080 - height
-p "" - preset 0=sponza, 1=conference, 2=Closed Citadel, 3=Budha, 4=Hairball 
-m models/sponza/sponza.obj - model path
-f models/sponza/sponza_fly.txt - fly path
-s 2 - fly speed
-t 0 - total frames to record (overrides speed)
-l "" - log to file - "frameID,bvh,gbuffer,shadowmask,lighting,copy"
-r 1 - render scale (allows to 4k on 1080p display)
-a 1 - average render times from N frames
-b 0 - Build BVH 0=once, 1=top every frame, 2=top+bottom every frame
).";


void Args::init(int argc, char** argv) {
  std::string arg;
  auto next = [&]() {
    arg = *argv;
    argv++;
    argc--;
  };
  next();

  while(argc) {
    next();

    if(arg == "-w" && argc) {
      next();
      w = std::stoi(arg);
    } else if(arg == "-h" && argc) {
      next();
      h = std::stoi(arg);
    } else if(arg == "-m" && argc) {
      next();
      model = arg;
    } else if(arg == "-f" && argc) {
      next();
      flythrough = arg;
    } else if(arg == "-s" && argc) {
      next();
      speed = std::stof(arg);
    } else if(arg == "-t" && argc) {
      next();
      frames = std::stoi(arg);
    } else if(arg == "-l" && argc) {
      next();
      log = arg;
    } else if(arg == "-r" && argc) {
      next();
      renderScale = std::stof(arg);
    } else if(arg == "-a" && argc) {
      next();
      avgFrames = std::stoi(arg);
    } else if(arg == "-b" && argc) {
      next();
      bvh = std::stoi(arg);
    } else if(arg == "-b" && argc) {
      next();
      bvh = std::stoi(arg);
    } else if(arg == "-p" && argc) {
      next();
      int preset = std::stoi(arg);
      switch(preset) {
        case 0:
          model = "models/sponza/sponza.obj";
          flythrough = "models/sponza/sponza_fly.txt";
          light = glm::vec3(0,10,0);
          break;
        case 1:
          model = "models/conference/conference.obj";
          flythrough = "models/conference/conference_fly.txt";
          light = glm::vec3(1.98,6.20,-1.825);
          break;
        case 2:
          model = "models/citadel/mycitadel.obj";
          flythrough = "models/citadel/citadel_fly.txt";
          light = glm::vec3(-16.58, 18.77, 10.31);
          break;
        case 3:
          model = "models/buddha/buddha_plane.obj";
          flythrough = "models/buddha/buddha_fly.txt";
          light = glm::vec3(0 ,0.02, 0.015 );
          break;
        case 4:
          model = "models/hairball_plane.obj";
          flythrough = "models/hairball_plane/hairball_fly.txt";
          light = glm::vec3(0,0.1,0);
          break;
      }
    } else {
      std::cout << helpString;
      exit(0);
    }
  }
}
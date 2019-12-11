#version 450
#depthTestEnable true
#cull back

#vert
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTc;
layout(location = 3) in uint aBone; // unused - all bones identity
layout(location = 4) in uint aMaterial;

layout(push_constant) uniform PushConstants {
  mat4 vp;
};

layout(location = 0) out vec3 vPosition;
layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec2 vTc;
layout(location = 3) out uint vMaterial;

void main() {
  vec4 pos = vec4(aPosition,1);  
  
  vPosition = pos.xyz;  
  vNormal = aNormal;
  vTc = aTc;
  vMaterial = aMaterial;

  gl_Position = vp*pos;
}

#frag
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTc;
layout(location = 3) flat in uint vMaterial;

layout(set=0, binding = 0) uniform sampler2D textures[128];

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec4 fragDiffuse;

void main() {   
  vec4 diffuse = texture(textures[vMaterial],vTc);
  fragPosition = vPosition;
  fragNormal = vNormal;
  fragDiffuse = diffuse;  
}

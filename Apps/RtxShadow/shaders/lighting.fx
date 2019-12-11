#version 450

#comp
layout(local_size_x = 8, local_size_y = 8) in;

layout(push_constant) uniform PushConstants {
  vec3 lightPos;
  float pad;
  vec3 camPos;
  float pad1;
};

layout(set = 0, binding = 0, rgba8) uniform image2D texAcc;
layout(set = 0, binding = 1, rgba32f ) uniform image2D texPosition;
layout(set = 0, binding = 2, rgba32f ) uniform image2D texNormal;
layout(set = 0, binding = 3, rgba8) uniform image2D texDiffuse;
layout(set = 0, binding = 4, r8) uniform image2D texShadowMask;

void main() {   
  ivec2 id = ivec2(gl_GlobalInvocationID.xy);

  ivec2 size = imageSize(texAcc);
  if(any(greaterThanEqual(id,size))) return;
  
  vec3 normal = imageLoad(texNormal,id).xyz;
  if(normal==vec3(0,0,0)){
    imageStore(texAcc,id,vec4(0.2,0.2,0.2,1));
    return;
  }

  vec3 position = imageLoad(texPosition,id).xyz;  
  vec3 diffuse = imageLoad(texDiffuse,id).xyz;
  float shadowMask = imageLoad(texShadowMask,id).x;
  //shadowMask = 1;

  vec3 lightDir = normalize(lightPos-position);
  vec3 viewDir = normalize(camPos-position);

  vec3 kd = shadowMask*0.8*diffuse*max(0,dot(lightDir,normal));
  vec3 ka = 0.2*diffuse;

  vec4 acc = vec4(kd+ka,0);
  imageStore(texAcc,id,acc);
}

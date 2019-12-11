#extension GL_NV_ray_tracing : require

#depth 1

#rgen

layout(binding = 0, set = 0, r8) uniform image2D texShadowMask;
layout(binding = 1, set = 0) uniform accelerationStructureNV bvh;
layout(binding = 2, set = 0, rgba32f) uniform image2D texPosition;

layout(push_constant) uniform PushConstants {
  vec3 lightPosition;
};

layout(location = 0) rayPayloadNV float mask;

void main(){
  ivec2 id = ivec2(gl_LaunchIDNV.xy);

  vec3 position = imageLoad(texPosition,id).xyz;
  vec3 dir = normalize(lightPosition-position);
  
  // no geometry in gbuffer
  if(position==vec3(0,0,0)){
    mask = 1;
    imageStore(texShadowMask,id,vec4(mask,0,0,0));
    return;
  }

  uint flags = gl_RayFlagsOpaqueNV|gl_RayFlagsSkipClosestHitShaderNV|gl_RayFlagsTerminateOnFirstHitNV;
  uint cullMask = 0xff;
  float tmin = 0.001;
  float tmax = length(lightPosition-position);

  mask = 0;
  traceNV(bvh, flags, cullMask, 0 /*sbtRecordOffset*/, 0 /*sbtRecordStride*/,
      0 /*missIndex*/, position, tmin, dir, tmax, 0 /*payload*/);

  imageStore(texShadowMask,id,vec4(mask,0,0,0));
}

#rmiss
layout(location = 0) rayPayloadInNV float mask;

void main(){
  mask = 1;
}
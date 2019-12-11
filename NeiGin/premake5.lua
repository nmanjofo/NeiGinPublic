if not NeiGinSubmodule then
include("NeiSetup.lua")
setupWorkspace("NeiGin")
end

initDependencies("dependencies")

--NeiGinLibType = "static"
NeiGinLibType = "dynamic"

include("srcExt/NeiCore")
include("srcExt/NeiVu")

setupProject({
   name = "NeiGin",
   type = NeiGinLibType,
   src = { "src/**.h", "src/**.cpp" },
   include = {"src/"},
   --link = {"NeiCore","NeiVu","glfw","FreeImage","Assimp","sol2","lua","tracy","vulkan"},
   link = {"NeiCore","NeiVu","sol2", "tracy","vulkan"},
   exportInclude = {"src/"}
})
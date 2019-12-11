include("NeiGin/NeiSetup.lua")
setupWorkspace("NeiBox")

local resDir = os.realpath("resources")
resDir = resDir:gsub("\\","/")
defines({"RESOURCE_DIR=\""..resDir.."\""})

local shaderDir = os.realpath("NeiGin/shaders")
shaderDir = shaderDir:gsub("\\","/")
defines({"SHADERS_DIR=\""..shaderDir.."\""})

NeiGinSubmodule=true
include("NeiGin")

function addApps(name)
  group(name)
  os.chdir(name)
  local tests = os.matchdirs("*")
  defines{"XXX=7"}
  for i,t in ipairs(tests) do
      os.chdir(t)
      setupProject({
          name = t,
          path = t,
          type = "console",
          src = {"**.cpp","**.h"},
          include = {"**"},
          link = {"NeiGin","NeiVu","vulkan","NeiCore"}
      })      
      local appDir = os.realpath(".")
      appDir = appDir:gsub("\\","/")
      defines({"APP_DIR=\""..appDir.."\""})
      os.chdir("..")
  end
  os.chdir("..")
end

addApps("Apps")
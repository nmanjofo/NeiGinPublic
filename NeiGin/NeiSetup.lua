local globals = {
}

function setupWorkspace (name)
   print("Workspace: "..name)
   workspace(name)   
   language "C++"
   location "build/"
   globals.buildDir = os.realpath(".").."/build/"
   globals.binDir = os.realpath(".").."/bin/"

   if _TARGET_OS == "windows" then
      defines{"WIN32","NOMINMAX","WIN32_LEAN_AND_MEAN","IMGUI_API=NEIGIN_EXPORT"} 
   end

   if _TARGET_OS == "linux??? @TODO" then
      -- do something
   end

   if _ACTION == "something for linux @TODO" then
      -- some compiler flags or whatever
   end

   if _ACTION == "vs2017" then
      buildoptions {"/std:c++latest"}
      systemversion("latest")     
      flags {"MultiProcessorCompile"} 
   end
   if _ACTION == "vs2019" then
      buildoptions {"/std:c++latest"}
      systemversion("latest")     
      flags {"MultiProcessorCompile"} 
   end

   platforms "x64"
   filter "platforms:x64"
   architecture "x86_64"
   filter {}

   configurations { "Debug", "Release" }   
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      targetdir "bin/debug"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      targetdir "bin/release"
--   
   filter {}
--

end

typemap={}
typemap["static"] = "StaticLib"
typemap["dynamic"] = "SharedLib"
typemap["console"] = "ConsoleApp"
typemap["app"] = "WindowedApp"
typemap["none"] = "None"

modules = {}

function setupProject(args)
   print("Project: "..args.name.." - "..args.type)   

   local path = args.path
   if not path then path = os.realpath(".") end

   module = {
      type = args.type,
      path = path,
      exportInclude = {},
      exportLibs= args.exportLibs or {},
      exportLibsDebug = args.exportLibsDebug or args.exportLibs or {},
      exportLibDir = args.exportLibDir,
      exportDefines = args.exportDefines or {},
   }

   -- translate includes to abs path
   if args.exportInclude then
      for i,inc in ipairs(args.exportInclude) do
         local absInc = path.."/"..inc
         --print(inc.." -> "..absInc)
         table.insert(module.exportInclude,absInc)
      end
   end

   if module.exportLibDir then
      module.exportLibDir = path.."/"..module.exportLibDir
   end

   if args.type~="interface" then
      project(args.name)
      kind(typemap[args.type])
      files(args.src)
      includedirs(args.include)
      vpaths { ["*"] = {"src/","include/"} }

      if args.type == "static" then defines {args.name.."_STATIC"} end
      if args.type == "dynamic" then defines {args.name.."_EXPORTS"} end

      if args.link then
         for i, link in ipairs(args.link) do
            local l = modules[link]
            if not l then
               print("ERROR: module "..link.." not found!")
            else
               print(" +"..link)

               includedirs(l.exportInclude)

               for i,inc in ipairs(l.exportInclude) do
                  table.insert(module.exportInclude,inc)
               end

               libdirs(l.exportLibDir )

               if l.type == "static" then
                  local def = link.."_STATIC"
                  defines{def}
                  table.insert(module.exportDefines,def)
               end

               defines(l.exportDefines)

               filter "configurations:Release"
               links(l.exportLibs)

               filter "configurations:Debug"
               links(l.exportLibsDebug)

               filter {}

               if l.type ~="interface" then
                  links(link)
               end
            
            end
         end
      end   
   end

   modules[args.name] = module;
end

function initDependencies(depDir)

   -- extenal dependencies
   local path = depDir
   local list = os.matchfiles(path.."/*.lua")
   for i,l in ipairs(list) do
      include(l)
   end

   -- source dependencies
   local path = depDir.."/src"
   local list = os.matchfiles(path.."/**/premake5.lua")
   for i,l in ipairs(list) do
      include(l)
   end
   
   -- prebuild dependencies
   if _ACTION == "vs2017" or _ACTION == "vs2019" then      
      local path = depDir.."/prebuilt/vs2017"
      local list = os.matchfiles(path.."/**/premake5.lua")
      for i,l in ipairs(list) do
         include(l)
      end

      local binDirDebug = globals.binDir.."debug/"
      os.mkdir(binDirDebug)
      local binDirRelease = globals.binDir.."release/"      
      os.mkdir(binDirRelease)
      local list = os.matchdirs(path.."/*/bin")
      for i,l in ipairs(list) do
         for j,f in ipairs(os.matchfiles(l.."/*")) do
            os.copyfile(f,binDirDebug..f:gsub(".+/",""))
            os.copyfile(f,binDirRelease..f:gsub(".+/",""))
         end

         for j,f in ipairs(os.matchfiles(l.."/release/*")) do
            os.copyfile(f,binDirRelease..f:gsub(".+/",""))
         end

         for j,f in ipairs(os.matchfiles(l.."/debug/*")) do
            os.copyfile(f,binDirDebug..f:gsub(".+/",""))
         end
      end

   end

end
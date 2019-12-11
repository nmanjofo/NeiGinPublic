setupProject {
   name = "NeiVu",
   type = NeiGinLibType,
   src = { "src/**.h", "src/**.cpp" },
   include = {"src/"},
   exportInclude = {"src/"},
   link = {"NeiCore","vulkan","vma"}
}
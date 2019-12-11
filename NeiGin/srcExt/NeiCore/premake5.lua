setupProject {
   name = "NeiCore",
   type = NeiGinLibType,
   src = { "src/**.h", "src/**.cpp" },
   include = {"src/"},
   link = {"tracy"},
   exportInclude = {"src/"}
}
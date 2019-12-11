local sdk = os.getenv("VULKAN_SDK")
if not sdk then
   error("VULKAN_SDK not set")
end
setupProject({
   name = "vulkan",
   type = "interface",
   path = sdk,
   exportInclude = {"include"},
   exportLibDir = "lib",
   exportLibs = {"vulkan-1.lib"}
})
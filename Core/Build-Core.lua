project "Core"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files { "Source/**.h", 
   "Source/**.cpp",
    "Source/**.hpp",
    "Source/**.c",
    "../Vendor/Include/imgui/imconfig.h",
    "../Vendor/Include/imgui/imgui.h",
    "../Vendor/Include/imgui/imgui_impl_glfw.h",
    "../Vendor/Include/imgui/imgui_impl_opengl3.h",
    "../Vendor/Include/imgui/imgui_impl_opengl3_loader.h",
    "../Vendor/Include/imgui/imgui_internal.h",
    "../Vendor/Include/imgui/imstb_rectpack.h",
    "../Vendor/Include/imgui/imstb_textedit.h",
    "../Vendor/Include/imgui/imstb_truetype.h",
    "../Vendor/Include/imgui/imgui.cpp",
    "../Vendor/Include/imgui/imgui_demo.cpp",
    "../Vendor/Include/imgui/imgui_draw.cpp",
    "../Vendor/Include/imgui/imgui_impl_glfw.cpp",
    "../Vendor/Include/imgui/imgui_impl_opengl3.cpp",
    "../Vendor/Include/imgui/imgui_tables.cpp",
    "../Vendor/Include/imgui/imgui_widgets.cpp"
 }

   includedirs
   {
      "Source",
      "../Vendor/Include/imgui",
      "../Vendor/Include/glm",
      "../Vendor/Include/gl3w",
      "../Vendor/Include/"
   }

   links
   {
        "../Vendor/Libraries/glfw3.lib"
   }

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { }

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"

   filter "configurations:Dist"
       defines { "DIST" }
       runtime "Release"
       optimize "On"
       symbols "Off"
-- premake5.lua
workspace "WaterEngine"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Core"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group ""
	include "Core/Build-Core.lua"

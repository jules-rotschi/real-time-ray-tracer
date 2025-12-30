workspace "RealTimeRayTracer"
   architecture "x64"
   configurations { "Debug", "Release" }
   startproject "RealTimeApplication"

project "GUI"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   targetdir "GUI/bin/%{cfg.buildcfg}"
   files { "GUI/vendors/glad/gl.c", "GUI/vendors/imgui/**.cpp", "GUI/src/**.h", "GUI/src/**.cpp" }
   includedirs { "GUI/src", "GUI/vendors/include", "GUI/vendors/include/imgui" }
   libdirs { "GUI/vendors/libs" }
   links { "glfw3" }

   filter "system:windows"
      links { "opengl32" }

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"

project "RayTracer"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   targetdir "RayTracer/bin/%{cfg.buildcfg}"
   files { "RayTracer/src/**.h", "RayTracer/src/**.cpp" }
   includedirs { "RayTracer/src", "GUI/src", "GUI/vendors/include", "GUI/vendors/include/imgui" }
   links { "GUI" }

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"

project "RealTimeApplication"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir ("bin/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}/%{prj.name}")
   files { "RealTimeApplication/src/**.h", "RealTimeApplication/src/**.cpp" }
   includedirs { "RealTimeApplication/src", "RayTracer/src", "GUI/src", "GUI/vendors/include", "GUI/vendors/include/imgui" }
   links { "GUI", "RayTracer" }
   debugdir "."

   filter "configurations:Debug"
      defines { "DEBUG" }
      runtime "Debug"
      symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"
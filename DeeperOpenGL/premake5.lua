workspace "DeeperOpenGL"
    architecture "x64"
    configurations{
        "Debug",
        "Release",
        "Dist"
    }
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
IncludeDir = {}
IncludeDir["GLFW"] = "D:/openglEnv/GLFW/glfw/include"
IncludeDir["GLAD"] = "D:/openglEnv/GLAD/include"
IncludeDir["GLM"] = "D:/openglEnv/glm-1.0.1"
include "D:/openglEnv/GLAD"
project "DeeperOpenGL"
    location "DeeperOpenGL"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    targetdir ("bin/" ..outputdir.. "/%{prj.name}")
    objdir ("bin-int/" ..outputdir.. "/%{prj.name}")
    files{
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/shader/**.glsl"
    }
    includedirs{
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
        "%{prj.name}/src",
        "%{IncludeDir.GLM}"
    }
    links{
        "D:/openglEnv/GLFW/glfw/build/src/Debug/glfw3.lib",
        "Glad"
    }
    defines{
        "GLFW_INCLUDE_NONE"
    }
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "off"
        systemversion "latest"
    filter "configurations:Debug"
        symbols "on"
    filter "configurations:Release"
        optimize "on"
    filter "configurations:Dist"
        optimize "on"
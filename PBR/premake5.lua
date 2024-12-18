workspace "PBR"
    architecture "x64"
    configurations {"Debug", "Release"}
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "D:/openglEnv/GLFW/glfw/include"
IncludeDir["GLAD"] = "D:/openglEnv/GLAD/include"
IncludeDir["GLM"] = "D:/openglEnv/glm-1.0.1"
IncludeDir["Assimp"] = "D:/openglEnv/assimp-5.4.3/include"
IncludeDir["FreeType"] = "D:/openglEnv/FreeType/freetype-2.13.3/include"

include "D:/openglEnv/GLAD"

project "PBR"
    staticruntime "off"
    kind "ConsoleApp"
    cppdialect "C++17"
    language "C++"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin/" .. outputdir .. "/%{prj.name}")
    files 
    {
        "src/*.cpp",
        "src/*.h",
        "src/*.vert",
        "src/*.frag"
    }
    includedirs
    {
        "src",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.Assimp}",
        "%{IncludeDir.FreeType}"
    }
    links
    {
        "D:/openglEnv/GLFW/glfw/build/src/Debug/glfw3.lib",
        "Glad",
        "D:/openglEnv/assimp-5.4.3/build/lib/Debug/assimp-vc142-mtd.lib",
        "D:/openglEnv/FreeType/freetype-2.13.3/objs/x64/Debug/freetype.lib"
    }
    defines
    {
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
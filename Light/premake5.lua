workspace "Light"
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
project "Light"
    location "Light"
    kind "ConsoleApp"
    language "C++"
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
        "D:/openglEnv/GLFW/glfw/build/src/Debug/glfw3.lib"
    }
        

    filter {"system:windows", "configurations:Debug"}
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        buildoptions "/MDd"
    
    filter "configurations:Release"
        optimize "On"
    filter "configurations:Dist"
        optimize "On"
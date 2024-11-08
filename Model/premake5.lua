workspace "Model"
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
IncludeDir["Assimp"] = "D:/openglEnv/assimp-5.4.3/include"
project "Model"
    location "Model"
    kind "ConsoleApp"
    language "C++"
    targetdir ("bin/" ..outputdir.. "/%{prj.name}")
    objdir ("bin-int/" ..outputdir.. "/%{prj.name}")
    files{
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.glsl",
        "%{prj.name}/src/**.c"
    }
    includedirs{
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
        "%{prj.name}/src",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.Assimp}"
    }
    links{
        "D:/openglEnv/GLFW/glfw/build/src/Debug/glfw3.lib",
        "D:/openglEnv/assimp-5.4.3/build/lib/Debug/assimp-vc142-mtd.lib"
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
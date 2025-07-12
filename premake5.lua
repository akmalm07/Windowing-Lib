workspace "windowing-lib"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "windowing-lib"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "windowing-lib"
    location "build"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir)

    files
    {
        "src/**.cpp",
        "include/**.h",
    }

    includedirs
    {
        "include",
        "vendor/GLFW/include",
        "vendor/GLEW/include",
    }

    libdirs
    {
        "vendor/GLFW/lib",
        "vendor/GLEW/lib"
    }

    links
    {
        "glfw3_mt",
        "opengl32",
        "glew32s",
        "kernel32"
    }

    defines { "GLEW_STATIC" }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
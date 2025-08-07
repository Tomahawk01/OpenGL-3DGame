dofile("utils.lua")

configure_file("config.h.in", "GameLib/src/Game/config.h", {
    OpenGL_Game_VERSION_MAJOR = "0",
    OpenGL_Game_VERSION_MINOR = "0",
    OpenGL_Game_VERSION_PATCH = "8"
})

workspace "OpenGL_Game"
    architecture "x64"
    startproject "Game"

    configurations
    {
        "Debug",
        "Release"
    }

    flags
    {
        "MultiProcessorCompile"
    }

    filter "system:windows"
        buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
    include "vendor/ImGui/Build-ImGui.lua"
    include "vendor/JoltPhysics-5.2.0/Build-JoltPhysics.lua"
group ""

group "Tools"
    include "Tools/ResourcePacker/Build-ResourcePacker.lua"
group ""

include "GameLib/Build-GameLib.lua"
include "Game/Build-Game.lua"

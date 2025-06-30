project "Game"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++23"
    staticruntime "off"

    files
    {
        "**.h",
        "**.cpp"
    }

    defines
    {
        "NOMINMAX"
    }

    includedirs
    {
        "src",
        "%{wks.location}/GameLib/src",

        "%{wks.location}/vendor/OpenGL/include",
        "%{wks.location}/vendor/lua_5.4.2/include",
        "%{wks.location}/vendor/JoltPhysics-5.2.0/JoltPhysics"
    }

    links
    {
        "GameLib"
    }

    local inputAssets  = "%{wks.location}/Game/assets"
    local outputAssets = "%{wks.location}/Game/assets/resources"

    dependson { "ResourcePacker" }

    prebuildcommands
    {
        '"%{wks.location}/bin/' .. outputdir .. '/ResourcePacker/ResourcePacker.exe" ' .. inputAssets .. ' ' .. outputAssets
    }

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    filter "system:windows"
        systemversion "latest"
        defines { "WINDOWS" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        debugargs { "assets" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "RELEASE" }
        debugargs { "assets" }
        runtime "Release"
        optimize "On"
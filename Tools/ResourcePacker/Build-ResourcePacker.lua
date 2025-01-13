project "ResourcePacker"
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

        "%{wks.location}/vendor/OpenGL/include"
    }

    links
    {
        "GameLib"
    }

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    filter "system:windows"
        systemversion "latest"
        defines { "WINDOWS" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        debugargs { "%{wks.location}/Game/assets/textures ./resource" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "RELEASE" }
        debugargs { "%{wks.location}/Game/assets/textures ./resource" }
        runtime "Release"
        optimize "On"
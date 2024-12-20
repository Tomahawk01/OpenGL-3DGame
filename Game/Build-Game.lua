project "Game"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++23"
    staticruntime "off"

    files
    {
        "**.h",
        "**.cpp",

        "%{wks.location}/vendor/ImGuizmo/ImGuizmo.cpp"
    }

    defines
    {
        "NOMINMAX"
    }

    includedirs
    {
        "src",
        
        "%{wks.location}/vendor/OpenGL/include",
        "%{wks.location}/vendor/stb",
        "%{wks.location}/vendor/ImGui/src",
        "%{wks.location}/vendor/ImGuizmo",
        "%{wks.location}/vendor/assimp-5.4.3/include",
    }

    libdirs
    {
        "%{wks.location}/vendor/assimp-5.4.3/lib"
    }

    links
    {
        "OpenGL32.lib",
        "ImGui"
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

        links
        {
            "assimp-vc143-mtd.lib"
        }

    filter "configurations:Release"
        defines { "RELEASE" }
        debugargs { "assets" }
        runtime "Release"
        optimize "On"

        links
        {
            "assimp-vc143-mt.lib"
        }
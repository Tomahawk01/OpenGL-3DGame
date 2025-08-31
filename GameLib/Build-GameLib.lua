project "GameLib"
    kind "StaticLib"
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
        "NOMINMAX",
        -- TODO: Include only in debug build later
        "JPH_DEBUG_RENDERER"
    }

    includedirs
    {
        "src",
        
        "%{wks.location}/vendor/OpenGL/include",
        "%{wks.location}/vendor/stb",
        "%{wks.location}/vendor/ImGui/src",
        "%{wks.location}/vendor/ImGuizmo",
        "%{wks.location}/vendor/JoltPhysics-5.2.0/JoltPhysics",
        "%{wks.location}/vendor/lua_5.4.2/include",
        "%{wks.location}/vendor/zstd-v1.5.7/include"
    }

    libdirs
    {
        "%{wks.location}/vendor/lua_5.4.2/lib",
        "%{wks.location}/vendor/zstd-v1.5.7/lib"
    }

    links
    {
        "OpenGL32.lib",
        "ImGui",
        "JoltPhysics",
        "lua54.lib",
        "zstd_static.lib"
    }

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    filter "system:windows"
        systemversion "latest"
        defines { "WINDOWS" }
        links
        {
            "dwrite",
            "d2d1",
            "shcore",
            "xaudio2"
        }

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "RELEASE" }
        runtime "Release"
        optimize "On"
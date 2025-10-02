-- Gonna left some documentation here for future reference
-- https://premake.github.io/docs/5.0/ <-- Premake 5.0 documentation
-- (This stills wip)

-- Some preprocessor directives for the Lua language
---@diagnostic disable: lowercase-global
---@diagnostic disable: undefined-global

workspace "Flakpak Workspace"
    architecture "x64"
    startproject "Paker"

    configurations {
        "Release",
    }

-- Global variables that can be used across all project files
outputdir = "/bin/%{cfg.project.name}/%{cfg.system}-%{cfg.buildcfg}_%{cfg.architecture}"
wsdir = "%{wks.location}"

    -- Global settings that apply to all projects
-- Global Windows Configuration--
filter "system:windows"
    -- Use latest Windows SDK
    systemversion "latest"
    -- This defines the compiler, in case you want to use MSVC put msc
    -- https://premake.github.io/docs/toolset/ <-- Docs on premake toolset
    toolset "msc"
    
    defines {
        "_WIN32",
    }
-- This is for Visual Studio Community (which I use as main IDE on my main PC)
filter "action:vs*"
    defines {
        "_CRT_SECURE_NO_WARNINGS",
        "SODIUM_STATIC"
    }
    flags {
        "MultiProcessorCompile"
    }

    -- Global runtime settings  
filter "configurations:Release"
    staticruntime "on"

-- Reset filter
filter {}

include "paker_pfile.lua"

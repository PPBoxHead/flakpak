-- Some preprocessor directives for the Lua language
---@diagnostic disable: lowercase-global
---@diagnostic disable: undefined-global

project "Test Project"
    kind "ConsoleApp"

    location(wsdir.. "/test project")
    targetdir(wsdir.. outputdir)
    objdir(wsdir.. outputdir.. "/obj_output")

    language "C++"
    cppdialect "C++20"

    files {
        wsdir.. "/test project/src/**.cpp",
        wsdir.. "/test project/include/**.h",
        
        wsdir.. "/paker/include/**.h",
        wsdir.. "/vendor/include/microlog/ulog.c",

        wsdir.. "/paker/src/xccp20_encryptor.c",
        wsdir.. "/paker/src/zstd_compressor.c",
    }
    includedirs {
        wsdir.. "/test project/include/",
        wsdir.. "/paker/include/",
        wsdir.. "/vendor/include"
    }
    libdirs {
        wsdir.. "/vendor/lib",
    }
    links {
        "libzstd_static.lib", -- Compression Library
        "libsodium.lib" -- Encryption Library
    }

    vpaths {
        ["Source Files/*"] = { 
            wsdir.. "/test project/src/**.cpp",
            wsdir.. "/paker/src/**.c"  -- Add C files to Source Files filter
        },
        
        ["Header Files/*"] = { 
            wsdir.. "/test project/include/**.h",
            wsdir.. "/paker/include/**.h"  -- Add FLK headers
        },
    }

    filter "configurations:Debug"
        defines {
            "DEBUG",
            "_DEBUG",
            "TEST_DEBUG",

            "ULOG_BUILD_COLOR=1",
            "ULOG_BUILD_TIME=1",
            "ULOG_BUILD_SOURCE_LOCATION=0"
        }
        runtime "Debug"
        staticruntime "off"

        symbols "on"
        optimize "off"

    filter "configurations:Release"
        defines {
            "NDEBUG",
            "TEST_RELEASE",

            "ULOG_BUILD_COLOR=1",
            "ULOG_BUILD_TIME=1",
            "ULOG_BUILD_SOURCE_LOCATION=0"
        }
        runtime "Release"

        symbols "off"
        optimize "on"

        -- Important for mixing C and C++
    filter "action:vs*"
        defines {
            "_CRT_SECURE_NO_WARNINGS",
            "SODIUM_STATIC"
        }
        buildoptions {
            "/wd4819"  -- Suppress code page warning when mixing C/C++
        }
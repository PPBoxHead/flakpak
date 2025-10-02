-- (This stills wip)
-- Some preprocessor directives for the Lua language
---@diagnostic disable: lowercase-global
---@diagnostic disable: undefined-global

-- This will have also a name lol
-- Might end up having the name <<flakpak>>
project "Paker"
    -- You can change this to also ConsoleApp (.exe) or SharedLib (.dll) for game development
    -- https://premake.github.io/docs/kind <-- Docs on project kinds
    kind "ConsoleApp"

    targetname("flakpak")

    location(wsdir.. "/paker")
    targetdir(wsdir.. outputdir)
    objdir(wsdir.. outputdir.. "/obj_output")

    -- If using C++, ensure to use cppdialect
    -- If using C, ensure to use cdialect
    -- https://premake.github.io/docs/cppdialect <-- Docs on cppdialect you can choose
    -- https://premake.github.io/docs/cdialect <-- Docs on cdialect you can choose
    language "C++"
    cppdialect "C++20"

    files {
        wsdir.. "/paker/src/**.cpp",
        wsdir.. "/paker/include/**.hpp",
    }
    includedirs {
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

    -- Organize Filters for IDEs
    -- https://premake.github.io/docs/vpaths <-- Docs on Virtual Paths for IDEs
    vpaths {
        ["Source Files/*"] = { wsdir.. "/paker/src/**.cpp" },
        
        ["Header Files/*"] = { wsdir.. "/paker/include/**.hpp" },
    }
        
    filter "configurations:Release"
        defines {
            "NDEBUG",
            "PAK_RELEASE"
        }
        runtime "Release"

        symbols "off"
        optimize "on"

        postbuildcommands {
            -- Step 1: Set metadata (Company, Product, Version) with rcedit
            [[rcedit "%{cfg.buildtarget.abspath}" --set-version-string "CompanyName" "SACRAROSSA"]],
            [[rcedit "%{cfg.buildtarget.abspath}" --set-version-string "ProductName" "flakpak CLI"]],
            [[rcedit "%{cfg.buildtarget.abspath}" --set-file-version "1.0.0"]],
            [[rcedit "%{cfg.buildtarget.abspath}" --set-product-version "1.0.0"]],

            -- Step 2: Sign executable with EV certificate
            [[signtool sign /fd SHA256 /a /tr http://timestamp.digicert.com /td SHA256 "%{cfg.buildtarget.abspath}"]]
        }
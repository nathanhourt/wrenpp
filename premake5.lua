
newoption {
    trigger     = "include",
    value       = "path",
    description = "The location of wren.h"
}

newoption {
    trigger     = "link",
    value       = "path",
    description = "The location of the wren static lib"
}

workspace "wrenpp"
    if _ACTION then
        -- guard this in case the user is calling `premake5 --help`
        -- in which case there will be no action
        location( "build/" .. _ACTION )
    end
    configurations { "Debug", "Release" }
    platforms { "Win32", "x64" }

    filter "platforms:Win32"
        architecture "x86"

    filter "platforms:x64"
        architecture "x86_64"

    -- global configuration
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    filter "not action:vs*"
        buildoptions { "-std=c++14" }

    project "lib"
        kind "StaticLib"
        language "C++"
        targetdir "lib/"
        targetname "wrenpp"
        if _OPTIONS["include"] then
            includedirs { _OPTIONS["include"] }
        end
        files { "Wren++.cpp", "Wren++.h" }
        includedirs { "src" }

    project "test"
        kind "ConsoleApp"
        language "C++"
        targetdir "bin"
        targetname "test"
        files { "Wren++.cpp", "test/**.cpp", "test/***.h", "test/**.wren" }
        includedirs { "./", "test" }
        if _OPTIONS["include"] then
            includedirs { _OPTIONS["include"] }
        end
        if _OPTIONS["link"] then
                libdirs {
                    _OPTIONS["link"]
                }
            end

        filter "files:**.wren"
            buildcommands { "{COPY} ../../test/%{file.name} ../../bin" }
            buildoutputs { "../../bin/%{file.name}" }
            filter {}

        filter "configurations:Debug"
            debugdir "bin"

        filter { "action:vs*", "Debug" }
            links { "lib", "wren_static_d" }

        filter { "action:vs*", "Release"}
            links { "lib", "wren_static" }

        filter { "not action:vs*" }
            links { "lib", "wren" }

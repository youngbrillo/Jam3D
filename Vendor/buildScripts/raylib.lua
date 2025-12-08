-- Copyright (c) 2020-2024 Jeffery Myers
--
--This software is provided "as-is", without any express or implied warranty. In no event 
--will the authors be held liable for any damages arising from the use of this software.

--Permission is granted to anyone to use this software for any purpose, including commercial 
--applications, and to alter it and redistribute it freely, subject to the following restrictions:

--  1. The origin of this software must not be misrepresented; you must not claim that you 
--  wrote the original software. If you use this software in a product, an acknowledgment 
--  in the product documentation would be appreciated but is not required.
--
--  2. Altered source versions must be plainly marked as such, and must not be misrepresented
--  as being the original software.
--
--  3. This notice may not be removed or altered from any source distribution.

local library = {
    libraryName = "raylib",
    githubFolder = "raysan5/raylib",
    repoHead = "5.5",
    pathprefix = "../Vendor/dependencies/",
    installLocation = "",
    installFolder = "/Vendor/dependencies",
    debug_print = true,
}

function library:platformDefines()
     filter {"options:backend=GLFW"}
        defines{"PLATFORM_DESKTOP"}

    filter {"options:backend=RLFW"}
        defines{"PLATFORM_DESKTOP_RGFW"}

    filter {"options:backend=SDL2"}
        defines{"PLATFORM_DESKTOP_SDL"}

    filter {"options:backend=SDL3"}
        defines{"PLATFORM_DESKTOP_SDL"}

    filter {"options:graphics=opengl43"}
        defines{"GRAPHICS_API_OPENGL_43"}

    filter {"options:graphics=opengl33"}
        defines{"GRAPHICS_API_OPENGL_33"}

    filter {"options:graphics=opengl21"}
        defines{"GRAPHICS_API_OPENGL_21"}

    filter {"options:graphics=opengl11"}
        defines{"GRAPHICS_API_OPENGL_11"}

    filter {"options:graphics=openges3"}
        defines{"GRAPHICS_API_OPENGL_ES3"}

    filter {"options:graphics=openges2"}
        defines{"GRAPHICS_API_OPENGL_ES2"}

    filter {"system:macosx"}
        disablewarnings {"deprecated-declarations"}

    filter {"system:linux"}
        defines {"_GLFW_X11"}
        defines {"_GNU_SOURCE"}
    -- This is necessary, otherwise compilation will fail since
    -- there is no CLOCK_MONOTOMIC. raylib claims to have a workaround
    -- to compile under c99 without -D_GNU_SOURCE, but it didn't seem
    -- to work. raylib's Makefile also adds this flag, probably why it went
    -- unnoticed for so long.
    -- It compiles under c11 without -D_GNU_SOURCE, because c11 requires
    -- to have CLOCK_MONOTOMIC
    -- See: https://github.com/raysan5/raylib/issues/2729
    filter{}
end

function library:get_raylib_dir()
    local libFolder = self.libraryName .. "-".. self.repoHead
    local baseName = path.getbasename(os.getcwd());
    local installLocation = os.getcwd() .. self.installFolder
    local installLocationFolder = installLocation .. "/".. libFolder;
    
    return installLocationFolder;
end

function library:loadProject()


    local libFolder = self.libraryName .. "-".. self.repoHead
    local baseName = path.getbasename(os.getcwd());
    local installLocation = os.getcwd() .. self.installFolder
    local installLocationFolder = installLocation .. "/".. libFolder;
    local zipFile = installLocation.. "/".. libFolder .. ".zip"
    local directoryExists = os.isdir(installLocationFolder)

    if self.debug_print then
        print ("   1) Loading")
        print ("\t Library Folder:   \t".. libFolder)
        print ("\t Basename:         \t".. baseName)
        print ("\t Install Location: \t".. installLocation .. " | " .. installLocationFolder)
        print ("\t Zip file:         \t".. zipFile)
        print ("\t Directory Exists?  ", directoryExists)
    end
    -- local githubURL = "https://github.com/" .. self.githubFolder .. "/archive/refs/heads/" .. self.repoHead ..".zip"
    local githubURL = "https://github.com/raysan5/raylib/archive/refs/tags/5.5.zip"

    if(not directoryExists) then
       local res = download_library(self.libraryName, installLocation, zipFile,  githubURL, true);
    end

end

function library:link(pre) 
    local prepend = pre or "";

    local libFolder = self.libraryName .. "-".. self.repoHead
    -- links (self.libraryName)
    local root = os.getcwd(); -- path.getbasename(os.getcwd());
    local linkDest = self.pathprefix .. libFolder;
    local include_base_path  = prepend .. linkDest;

    if self.debug_print then
        print "\t   raylib.link"
        print ("\t\t including " .. self.libraryName .. " from directory : ".. linkDest)
        print ("\t\t root is:    " .. root)
        print ("\t\t base path:  " .. include_base_path)
    end


    links ("raylib")
    -- local raylib_dir = get_raylib_dir();
    -- includedirs {"../" .. raylib_dir .. "/src" }
    -- includedirs {"../" .. raylib_dir .."/src/external" }
    -- includedirs {"../" .. raylib_dir .."/src/external/glfw/include" }

    includedirs { include_base_path .. "/src" }
    includedirs { include_base_path .. "/src/external" }
    includedirs { include_base_path.. "/src/external/glfw/include" }

    self:platformDefines();
    
    filter "files:**.dll"
        buildaction "Copy"

    filter "action:vs*"
        defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
        dependson {"raylib"}
        links {"raylib.lib"}
        characterset ("MBCS")
        buildoptions { "/Zc:__cplusplus" }

    filter "system:windows"
        defines{"_WIN32"}
        links {"winmm", "gdi32", "opengl32"}
        -- libdirs {"../build/bin/%{cfg.buildcfg}"}
        libdirs {"../../build/bin/%{cfg.buildcfg}"} -- hardcoded uh-ohhhh!

    filter "system:linux"
        links {"pthread", "m", "dl", "rt", "X11"}

    filter "system:macosx"
        links {"OpenGL.framework", "Cocoa.framework", "IOKit.framework", "CoreFoundation.framework", "CoreAudio.framework", "CoreVideo.framework", "AudioToolbox.framework"}

    filter {"options:backend=SDL2"}
        includedirs {"../SDL2/include" }

    filter { "system:windows", "options:backend=SDL2", "platforms:x64"}
        libdirs {"../SDL2/lib/x64"}
        links {"SDL2"}
        files "../SDL2/lib/x64/SDL2.dll"

    filter { "system:windows", "options:backend=SDL2", "platforms:x32"}
        libdirs {"../SDL2/lib/x32"}
        links {"SDL2"}
        files "../SDL2/lib/x32/SDL2.dll"

    filter { "system:windows", "options:backend=SDL3", "platforms:x64", "action:vs*"}
        includedirs {"../SDL3/include/SDL3" }
        includedirs {"../SDL3/include" }
        libdirs {"../SDL3/lib/x64"}
        links {"SDL3"}
        files "../SDL3/lib/x64/SDL3.dll"
        
    filter { "system:windows", "options:backend=SDL3", "platforms:x32", "action:vs*"}
        includedirs {"../SDL3/include/SDL3" }
        includedirs {"../SDL3/include" }
        libdirs {"../SDL3/lib/x32"}
        links {"SDL3"}
        files "../SDL3/lib/x32/SDL3.dll"
 
    filter { "system:windows", "options:backend=SDL3", "platforms:x64", "action:gmake*"}
        includedirs {"../SDL3/x86_64-w64-mingw32/include/SDL3" }
        includedirs {"../SDL3/x86_64-w64-mingw32/include" }
        libdirs {"../SDL3/x86_64-w64-mingw32/lib/"}
        libdirs {"../SDL3/x86_64-w64-mingw32/bin/"}
        links {"SDL3"}
        files "../SDL3/x86_64-w64-mingw32/bin/SDL3.dll"

    filter { "system:*nix OR system:macosx", "options:backend=SDL2",  "configurations:Debug OR configurations:Release"}
        links {"SDL2"}

    filter { "system:*nix OR system:macosx", "options:backend=SDL3",  "configurations:Debug OR configurations:Release"}
        links {"SDL3"}

    filter{}
end

function library:include(pre)
    local prepend = pre or "";
    local libFolder = self.libraryName .. "-".. self.repoHead
    local root = os.getcwd(); -- path.getbasename(os.getcwd());
    local linkDest = self.pathprefix .. libFolder;
    local include_base_path  = prepend .. linkDest;
    local raylib_dir = self:get_raylib_dir();


    if self.debug_print then
        print "\t   raylib.include"
        print ("\t\t including " .. self.libraryName .. " from directory : ".. linkDest)
        print ("\t\t root is:    " .. root)
        print ("\t\t base path:  " .. include_base_path)
        print ("\t\t raylib dir:  " .. raylib_dir)
    end

    includedirs {include_base_path .."/src" }
    includedirs {include_base_path .."/src/external" }
    includedirs {include_base_path .."/src/external/glfw/include" }
    self:platformDefines()

    filter "action:vs*"
        defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}

    filter{}
end

function library:link_to(pre, args_)
    local args = args_ or {link = true, include = false};
    local can_link = args.link
    local can_include = args.include

    if self.debug_print then
        print ("\t\t linking: ", args.link)
        print ("\t\t including: ", args.include)
    end

    if can_link     then self:link(pre)         end 
    if can_include  then self:include(pre)      end 
    
   
end


function library:generatePremakeScript()
    local raylib_dir_0 = self:get_raylib_dir();
    local raylib_dir =  "raylib-5.5" --self:get_raylib_dir();
    if self.debug_print then
        print ("   2) Generating Script")
        print ("\t current directory:\t" .. os.getcwd())
        print ("\t raylib directory (0): \t" .. raylib_dir_0)
        print ("\t raylib directory (1): \t" .. raylib_dir)
    end


    project "raylib"
        kind "StaticLib"

        self:platformDefines()

        location "./build/"
        language "C"
        targetdir "./build/bin/%{cfg.buildcfg}"

        filter {"options:backend=SDL2"}
            includedirs {"SDL2/include" }

        filter {"options:backend=SDL3", "action:vs*"}
            includedirs {"SDL3/include/SDL3" }
            includedirs {"SDL3/include" }
            
        filter { "system:windows", "options:backend=SDL3", "platforms:x64", "action:gmake*"}
            includedirs {"SDL3/x86_64-w64-mingw32/include/SDL3" }
            includedirs {"SDL3/x86_64-w64-mingw32/include" }

        filter "action:vs*"
            defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
            characterset ("MBCS")
            buildoptions { "/Zc:__cplusplus" }
        filter{}

        includedirs {raylib_dir_0 .. "/src", raylib_dir_0 .. "/src/external/glfw/include" }
        vpaths
        {
            ["Header Files"] = { raylib_dir_0 .. "/src/**.h"},
            ["Source Files/*"] = { raylib_dir_0 .. "/src/**.c"},
        }
        files {raylib_dir_0 .. "/src/*.h", raylib_dir_0 .. "/src/*.c"}

        removefiles {raylib_dir_0 .. "/src/rcore_*.c"}

        -- sorry to my MACOS people...
        filter { "system:macosx", "files:" .. raylib_dir .. "/src/rglfw.c" }
            compileas "Objective-C"

        filter{}
end

return library;
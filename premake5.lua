
workspaceName = path.getbasename(os.getcwd())

include "Vendor/premake_buildUtils.lua"
include "Vendor/premkae_external_dependencies.lua"

GetExternalDependencies();

workspace (workspaceName)
    configurations { "Debug", "Release"}
    platforms { "x64", "x86", "ARM64"}

    defaultplatform ("x64")

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    filter { "platforms:x64" }
        architecture "x86_64"

    filter { "platforms:Arm64" }
        architecture "ARM64"

    filter {}

    targetdir "bin/%{cfg.buildcfg}/"

    if(os.isdir("game")) then
        startproject(workspaceName)
    end

cdialect "C17"
cppdialect "C++17"
-- check_raylib();

-- include ("raylib_premake5.lua")
topLevelFolders = os.matchdirs("*")
apps = os.matchdirs("Apps/*")

folders = {table.unpack(topLevelFolders)}
for i = 1,#apps do
    folders[#topLevelFolders+i] = apps[i]
end

group "Apps"
for _, folderName in ipairs(folders) do
    if (    string.starts(folderName, "build") == false 
            and string.starts(folderName, "build") == false 
            and string.starts(folderName, ".") == false
    ) then
        if (os.isfile(folderName .. "/premake5.lua")) then
            print(folderName)
            include (folderName)
        end
    end
end
group ""

AddExtenralDependencies();
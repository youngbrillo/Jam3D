local library = {
    libraryName = "entt",
    githubFolder = "skypjack/entt",
    repoHead = "master",
    pathprefix = "../Vendor/dependencies/",
    installLocation = "",
    installFolder = "/Vendor/dependencies",
    debug_print = false,
}

function library:link_to(pre) 
    local prepend = pre or "";

    local libFolder = self.libraryName .. "-".. self.repoHead
    -- links (self.libraryName)
    local root = os.getcwd(); -- path.getbasename(os.getcwd());
    local linkDest = self.pathprefix .. libFolder;
    local include_base_path  = prepend .. linkDest;

    if self.debug_print then
        print ("\t including " .. self.libraryName .. " from directory : ".. linkDest)
        print ("\t root is:    " .. root)
        print ("\t base path:  " .. include_base_path)
    end


    includedirs { include_base_path .. "/" }
    includedirs { include_base_path .. "/src/" }
    includedirs { include_base_path.. "/include/" }
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

    local githubURL = "https://github.com/" .. self.githubFolder .. "/archive/refs/heads/" .. self.repoHead ..".zip"

    if(not directoryExists) then
       local res = download_library(self.libraryName, installLocation, zipFile,  githubURL, true);
    end


end


function library:generatePremakeScript()
    if self.debug_print then
        print ("   2) Generating Script")
        print ("\t current directory:\t" .. os.getcwd())
    end
    project (self.libraryName)
        kind "StaticLib"
        location    "./build/"
        targetdir   "./build/bin/%{cfg.buildcfg}"

        filter "action:vs*"
            buildoptions { "/experimental:c11atomics" }

        vpaths 
        {
            ["Header Files/*"] = { "./include/**.h", "./include/**.hpp"},
            ["Source Files/*"] = { "./src/**.cpp", "./src/**.c"},
        }
        files {".src/**.hpp", ".src/**.h",".src/**.c", ".src/**.cpp"}

        -- includedirs { "./" }
        includedirs { "./src" }
        includedirs { "./include"}
end

return library;
local library = {
    libraryName = "spdlog",
    githubFolder = "gabime/spdlog",
    repoHead = "v1.x",
    pathprefix = "../Vendor/dependencies/",
    installLocation = "",
    installFolder = "/Vendor/dependencies",
    debug_print = true,
}

function library:get_dir()
    local libFolder = self.libraryName .. "-".. self.repoHead
    local baseName = path.getbasename(os.getcwd());
    local installLocation = os.getcwd() .. self.installFolder
    local installLocationFolder = installLocation .. "/".. libFolder;
    return installLocationFolder;
end

function library:link_to(pre) 
    local prepend = pre or "";

    local libFolder = self.libraryName .. "-".. self.repoHead
    -- links (self.libraryName)
    local root = os.getcwd(); -- path.getbasename(os.getcwd());
    local linkDest = self.pathprefix .. libFolder;
    local include_base_path  = prepend .. linkDest;

    if self.debug_print then
        print ("\t\t including " .. self.libraryName .. " from directory : ".. linkDest)
        print ("\t\t root is:    " .. root)
        print ("\t\t base path:  " .. include_base_path)
    end

    -- links (self.libraryName)
    -- includedirs { include_base_path .. "/" }
    -- includedirs { include_base_path .. "/src/" }
    includedirs { include_base_path.. "/include/" }
end


function library:loadProject()
    local libFolder = self.libraryName .. "-".. self.repoHead
    local baseName = path.getbasename(os.getcwd());
    local installLocation = os.getcwd() .. self.installFolder
    local installLocationFolder = installLocation .. "/".. libFolder;
    --change install location slightly because the folder name is 'slightly' different for spdlog
            installLocationFolder = installLocation .. "/spdlog-1.x";
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

    local lib_dir = self:get_dir()
    local local_dir = "."..self.installFolder .. "/".. self.libraryName .. "-".. self.repoHead

    if self.debug_print then
        print ("   2) Generating Script (sdlog)")
        print ("\t current directory:\t" .. os.getcwd())
        print ("\t lib dir:\t\t" .. lib_dir)
        print ("\t local dir:\t\t" .. local_dir)
    end
    project (self.libraryName)
        kind "StaticLib"
        location    "./build/"
        targetdir   "./build/bin/%{cfg.buildcfg}"

        filter "action:vs*"
            buildoptions { "/experimental:c11atomics" }

        vpaths 
        {
            ["Header Files/*"] = { lib_dir.. "./include/**.h", lib_dir.. "./include/**.hpp"},
            -- ["Source Files/*"] = { lib_dir.. "./src/**.cpp", lib_dir.. "./src/**.c"},
        }
        files {
            lib_dir.. "./include/**.hpp",
            lib_dir.. "./include/**.h",
            -- lib_dir.. "./src/**.hpp", 
            -- lib_dir.. "./src/**.h",
            -- lib_dir.. "./src/**.cpp",
            -- lib_dir.. "./src/**.c"
        }

        -- includedirs { lib_dir.. "/src" }
        includedirs { lib_dir.. "/include"}
end

return library;
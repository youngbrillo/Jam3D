
function string.starts(String,Start)
    return string.sub(String,1,string.len(Start))==Start
end

---link the current project with a library 
---@param lib any
---@param libPath any
function link_to(lib, libPath)
    links (lib)

    local path = libPath or "../"

    includedirs (path..lib.."/include")
    includedirs (path..lib.."/" )
end

function download_progress(total, current)
    local ratio = current / total;
    ratio = math.min(math.max(ratio, 0), 1);
    local percent = math.floor(ratio * 100);
    print("Download progress (" .. percent .. "%/100%)")
end

function use_library(libraryName, githubFolder, repoHead)
    libFolder = libraryName .. "-" .. repoHead
    zipFile = libFolder .. ".zip"

    baseName = path.getbasename(os.getcwd());

    links(libraryName);
    includedirs {"../" .. libFolder .. "/" }
    includedirs {"../" .. libFolder .."/src/" }
    includedirs {"../" .. libFolder .."/include/" }
    

    os.chdir("..")
    
    if(os.isdir(libFolder) == false) then
        if(not os.isfile(zipFile)) then
            print(libraryName .. " not found, downloading from github")
            local result_str, response_code = http.download("https://github.com/" .. githubFolder .. "/archive/refs/heads/" .. repoHead ..".zip", zipFile, {
                progress = download_progress,
                headers = { "From: Premake", "Referer: Premake" }
            })
        end
        print("Unzipping '" .. zipFile .. "' to " ..  os.getcwd())
        zip.extract(zipFile, os.getcwd())
        os.remove(zipFile)
    end

    os.chdir(libFolder)
    
    project (libraryName)
        kind "StaticLib"
        location "./"
        targetdir "../bin/%{cfg.buildcfg}"

        filter "action:vs*"
            buildoptions { "/experimental:c11atomics" }

        vpaths 
        {
            ["Header Files/*"] = { "include/**.h", "include/**.hpp",  "**.h", "**.hpp"},
            ["Source Files/*"] = { "src/**.cpp", "src/**.c", "**.cpp",  "**.c"},
        }
        files {"include/**.hpp", "include/**.h","src/**.hpp", "src/**.h", "src/**.cpp", "src/**.c"}

        includedirs { "./" }
        includedirs { "./src" }
        includedirs { "./include" }

    os.chdir(baseName)
end



function download_library( libraryName, installPath, zipFilepath, Url, debugPrint)
    if debugPrint == true then
        print("\t\t > " .. libraryName .. " not found, downloading from github.")
        print("\t\t will install to:\t" .. installPath)
        print("\t\t will unzip @\t\t" .. zipFilepath)
        print("\t\t will download from:\t" .. Url)
    end

    local result_str, response_code = http.download(Url, zipFilepath, {
        progress = download_progress,
        headers = { "From: Premake", "Referer: Premake" }
    })

    if debugPrint == true then
        print("\t\t Unziping '".. zipFilepath .. "' to\t" .. installPath)
    end

    zip.extract(zipFilepath, installPath)
    os.remove(zipFilepath)

end
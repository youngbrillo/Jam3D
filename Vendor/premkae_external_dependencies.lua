
external_dep = {};

function GetExternalDependencies()
    print "searching for external dependencies . . ."
    local files = os.matchfiles ("./Vendor/buildScripts/*.lua")

    if #files == 0 then 
        print "\tno dependencies found"
    end
    
    for _, fileName in ipairs(files) do
        local lib = include(fileName)
        external_dep[lib.libraryName] = lib;
    end
end

function AddExtenralDependencies()
    local workingDir = os.getcwd()
    print "Adding external dependencies"
    print "------------------------------\n"
    group "dependencies"
        for name, lib in pairs(external_dep) do
            print ("adding: ".. name .. "-'"..lib.libraryName .. "'");
            -- dl the project
            lib:loadProject(true);
            --nav to the library's directory
            os.chdir(lib.installLocation );
            -- generate a premake script to add it to the solution
            lib:generatePremakeScript();
            -- nav back to the working directory
            os.chdir(workingDir)
        end
    group ""
    print "------------------------------\n"
end


function link_to_ex(lib, prepend, args)

    if external_dep[lib] ~= nil then
        print ("\t is linking " .. lib .. "!")
        external_dep[lib]:link_to(prepend, args);
    else
        print ("\tOH NO\tCannot Find lib '".. lib .. "'in 'vendors' folder!" )
    end
end

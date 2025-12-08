baseName = path.getbasename(os.getcwd());

project (baseName)
    cppdialect "C++20"
    language "C++"
    kind "StaticLib"
    location "../build/"
    targetdir "../build/bin/%{cfg.buildcfg}"

    files {"**.hpp", "**.h", "**.cpp","**.c"}

    includedirs { "./" }
    includedirs { "./src" }
    includedirs { "./include" }

    link_to_ex("yaml-cpp")
    link_to_ex("entt")
    link_to_ex("raylib", "", {link=false, include=true})
    link_to_ex("lua")
    link_to_ex("sol2")
    
    defines {
        "YAML_CPP_STATIC_DEFINE" , 
    } 
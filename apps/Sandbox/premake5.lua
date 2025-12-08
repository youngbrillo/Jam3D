newoption
{
    trigger = "graphics",
    value = "OPENGL_VERSION",
    description = "version of OpenGL to build raylib against",
    allowed = {
        { "opengl11", "OpenGL 1.1"},
        { "opengl21", "OpenGL 2.1"},
        { "opengl33", "OpenGL 3.3"},
        { "opengl43", "OpenGL 4.3"},
        { "opengles2", "OpenGLES 2.0"},
        { "opengles3", "OpenGLES 3.0"}
    },
    default = "opengl43"
}

newoption
{
    trigger = "backend",
    value = "BACKEND",
    description = "backend to use",
    allowed = {
        { "GLFW", "GLFW"},
        { "SDL2", "SDL2"},
        { "SDL3", "SDL3"},
        { "RLFW", "RLFW"}
    },
    default = "GLFW"
}


baseName = path.getbasename(os.getcwd());

project (baseName)
    cppdialect "C++20"
    language "C++"
    kind "ConsoleApp"
    location "../build/"
    targetdir "../build/bin/%{cfg.buildcfg}"

    filter "action:vs*"
        debugdir "$(SolutionDir)"

    filter {"action:vs*", "configurations:Release"}
        kind "WindowedApp"
        entrypoint "mainCRTStartup"

    filter{}

    vpaths 
    {
        -- ["Source Files/*"] = {"src/**.c", "src/**.cpp","**.c", "**.cpp"},
        ["Application Resource Files/*"] = {"src/**.rc", "src/**.ico"},
    }
    files {"./src/**.c", "./src/**.cpp", "./src/**.h", "./src/**.hpp"}

    filter "system:windows"
        files {"res/**.rc", "res/**.ico"}
        resincludedirs { "src/**" }
    filter{}

    filter "files:**.ico"
        buildaction "Embed"
		
    filter{}

  
    includedirs { "./" }
    includedirs { "src" }
    includedirs { "include" }
    
    link_to("library", "../../")
    link_to("editor", "../../")
    link_to_ex("entt", "../")
    link_to_ex("raylib", "../",{link=true, include=false})
    link_to_ex("yaml-cpp", "../")
    link_to_ex("imgui", "../")
    link_to_ex("lua", "./../")
    link_to_ex("sol2", "../")
    link_to_ex("spdlog", "../")

    buildoptions {"/utf-8"} 
    
    defines {
        "SUPPORT_FILEFORMAT_HDR"
    } 
-- To link to a lib use link_to("LIB_FOLDER_NAME")
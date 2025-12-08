#pragma once
#include <string>
#include "raylib.h"
#include "../scene/iScene.hpp"

namespace jam
{
    struct WindowSettings
    {
        int width = 800;
        int height = 450;
        int monitor = 0;
        int configFlags = FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT;
        bool lockFPS = false;
        int targetFPS = 120;
    };

    struct SceneSettings
    {
        int currentTemplate = 0;
        int activeTemplate = 0;
        SceneConfig definition = {};
        bool resetScene = false;
        bool reloadScene = false;
        bool drawScene = true;
        bool overridePause = false;
    };

    struct AudioSettings
    {
        float masterVolume = 1.0f;
    };

    struct PhysicsSettings
    {
        float hertz = 1.0f / 60.0f;
        Vector2 gravity = { 0.0f, 15.0f };
        float hertzTimer = 0.0f;
    };
    struct TextSettings
    {
        std::string fontPath = "";
        float fontSize = 20.0f;
        float fontSpacing = 1.0f;
        Color textColor = WHITE;
        Color textBackground = ColorAlpha(GetColor(0x6495EDff), 0.65f);
    };
    struct Project
    {
        std::string name = "[Project-name]";
        std::string resourceDirectory = "apps/%s/res";
        std::string iconPathLocation = "logo.png";
        std::string fileLocation;
        int version_major = 0;
        int version_minor = 0;
        int version_patch = 0;
        bool reloadApp = false;
        bool running = true;
        WindowSettings window;
        SceneSettings scene;
        AudioSettings audio;
        PhysicsSettings physics;
        TextSettings text;

        /// <summary>
        /// Load the project settings from a yaml file on disk
        /// </summary>
        /// <param name="filepath">The name of the file to load from</param>
        /// <returns>True if a file was found, false is the file could not be found</returns>
        bool Load(std::string filepath);
        /// <summary>
        /// Save the project settings to a yaml file
        /// </summary>
        /// <param name="filepath"></param>
        /// <returns></returns>
        bool Save(std::string filepath);

        /// <summary>
        /// Looks for the specified resource dir in several common locations
        /// The working dir
        /// The app dir
        /// Up to 3 levels above the app dir
        /// When found the dir will be set as the working dir so that assets can be loaded relative to that.
        /// </summary>
        /// <param name="folderName">The name of the resources dir to look for</param>
        /// <returns>True if a dir with the name was found, false if no change was made to the working dir</returns>
        bool SetResourceDirectory(const char* folderName);

        void ChangeWindowIcon(std::string filepath);

        void onWindowInit();
    };
}


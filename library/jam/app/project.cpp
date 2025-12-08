#include "project.hpp"
#include <yaml-cpp/yaml.h>
#include "Jam/serialization/yaml/serializeYAML.hpp"
#include "Jam/serialization/yaml/serializeScene.hpp"
namespace yml = YAML;

bool jam::Project::Load(std::string filepath)
{
    YAML::Node root = LoadYamlFile(filepath);
    if (!root.IsNull())
    {
        name = root["name"].as<std::string>();
        resourceDirectory = root["resourceDirectory"].as<std::string>();

        if (!resourceDirectory.empty())
        {
            bool resDirChanged = SetResourceDirectory(resourceDirectory.c_str());

            if (resDirChanged)
                TraceLog(LOG_INFO, "[PROJECT]: resource directory successfully set to '%s'!", resourceDirectory.c_str());
            else
                TraceLog(LOG_ERROR, "[PROJECT]: could not set resource directory to '%s'...", resourceDirectory.c_str());
        }

        readValueEx(root["iconPathLocation"], &iconPathLocation);
        readValueEx(root["version_major"], &version_major);
        readValueEx(root["version_minor"], &version_minor);
        readValueEx(root["version_patch"], &version_patch);
        readValueEx(root["scene.overridePause"], &scene.overridePause);

        if (YAML::Node wNode = root["window"])
        {
            readValueEx(wNode["width"], &window.width);
            readValueEx(wNode["height"], &window.height);
            readValueEx(wNode["monitor"], &window.monitor);
            readValueEx(wNode["configFlags"], &window.configFlags);
            readValueEx(wNode["lockFPS"], &window.lockFPS);
            readValueEx(wNode["targetFPS"], &window.targetFPS);

        }


        readValueEx(root["activeTemplate"], &scene.activeTemplate);
        serialization::DeserializeSceneConfig(root["scene"], scene.definition);

        if (YAML::Node aNode = root["audio"])
        {
            readValueEx(aNode["masterVolume"], &audio.masterVolume);
        }

        //if (YAML::Node aNode = root["text"])
        //{
        //    readValueEx(aNode["fontPath"], &text.fontPath);
        //    readValueEx(aNode["fontSize"], &text.fontSize);
        //    readValueEx(aNode["fontSpacing"], &text.fontSpacing);
        //    readValueEx(aNode["textColor"], &text.textColor);
        //    readValueEx(aNode["textBackground"], &text.textBackground);
        //}
        return true;
    }
    return false;
}

bool jam::Project::Save(std::string filepath)
{
    if (filepath.empty())
        filepath = fileLocation;
    YAML::Emitter out;

    out << YAML::BeginMap
        << YAML::Key << "name" << YAML::Value << name
        << YAML::Key << "resourceDirectory" << YAML::Value << resourceDirectory
        << YAML::Key << "iconPathLocation" << YAML::Value << iconPathLocation
        << YAML::Key << "version_major" << YAML::Value << version_major
        << YAML::Key << "version_minor" << YAML::Value << version_minor
        << YAML::Key << "version_patch" << YAML::Value << version_patch
        << YAML::Key << "scene.overridePause" << YAML::Value << scene.overridePause
        << YAML::Key << "window" << YAML::Value
        << YAML::BeginMap
        << YAML::Key << "width" << YAML::Value << window.width
        << YAML::Key << "height" << YAML::Value << window.height
        << YAML::Key << "monitor" << YAML::Value << window.monitor
        << YAML::Key << "configFlags" << YAML::Value << window.configFlags
        << YAML::Key << "lockFPS" << YAML::Value << window.lockFPS
        << YAML::Key << "targetFPS" << YAML::Value << window.targetFPS
        << YAML::EndMap;
    out << YAML::Key << "activeTemplate" << YAML::Value << scene.activeTemplate;
    serialization::SerializeSceneConfig(out, scene.definition);

    out << YAML::Key << "audio" << YAML::Value
        << YAML::BeginMap
        << YAML::Key << "masterVolume" << YAML::Value << audio.masterVolume
        << YAML::EndMap;


    //out << YAML::Key << "text" << YAML::Value
    //    << YAML::BeginMap
    //    << YAML::Key << "fontPath" << YAML::Value << text.fontPath
    //    << YAML::Key << "fontSize" << YAML::Value << text.fontSize
    //    << YAML::Key << "fontSpacing" << YAML::Value << text.fontSpacing
    //    << YAML::Key << "textColor" << YAML::Value << text.textColor
    //    << YAML::Key << "textBackground" << YAML::Value << text.textBackground
    //    << YAML::EndMap;


    out << YAML::EndMap;
    SaveYamlFile(filepath, out);
    return true;
}

bool jam::Project::SetResourceDirectory(const char* folderName)
{
    // check the working dir
    if (DirectoryExists(folderName))
    {
        ChangeDirectory(TextFormat("%s/%s", GetWorkingDirectory(), folderName));
        return true;
    }

    const char* appDir = GetApplicationDirectory();

    // check the applicationDir
    const char* dir = TextFormat("%s%s", appDir, folderName);
    if (DirectoryExists(dir))
    {
        ChangeDirectory(dir);
        return true;
    }

    // check one up from the app dir
    dir = TextFormat("%s../%s", appDir, folderName);
    if (DirectoryExists(dir))
    {
        ChangeDirectory(dir);
        return true;
    }

    // check two up from the app dir
    dir = TextFormat("%s../../%s", appDir, folderName);
    if (DirectoryExists(dir))
    {
        ChangeDirectory(dir);
        return true;
    }

    // check three up from the app dir
    dir = TextFormat("%s../../../%s", appDir, folderName);
    if (DirectoryExists(dir))
    {
        ChangeDirectory(dir);
        return true;
    }

    return false;
}

void jam::Project::ChangeWindowIcon(std::string file)
{
    if (file.empty())
        return;

    Image icon = LoadImage(file.c_str());
    if (!icon.data)
        return;

    ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(icon);
    UnloadImage(icon);

}

void jam::Project::onWindowInit()
{
    ChangeWindowIcon(iconPathLocation);
}

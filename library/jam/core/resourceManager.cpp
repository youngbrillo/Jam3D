#include "resourceManager.hpp"
#include "jam/serialization/yaml/serializeYAML.hpp"
#include <filesystem>
namespace fs = std::filesystem;

jam::ResourceManager* jam::ResourceManager::_instance = nullptr;


jam::ResourceManager& jam::ResourceManager::Get()
{
	if (_instance == nullptr)
		_instance = new jam::ResourceManager();

	return *_instance;
}

void jam::ResourceManager::Startup(std::string file)
{
    YAML::Node root = jam::LoadYamlFile(file);
    auto& rm = Get();
    if (!root.IsNull())
    {
        for (auto resourceNode : root["resources"])
        {
            auto node = resourceNode["res"];
            Resource res;

            res.deserialize_as_header(node);
            if(res.id != 0 && res.type != ResourceType_Null)
                rm.resources.emplace(res.id, res);

            TraceLog(LOG_INFO, "[ResourceManager]\tRegistering Resource: %s [%d]\t[%s]",
                res.id.toString().c_str(),
                res.type,
                res.filepath.c_str()
            );
        }
    }



}

void jam::ResourceManager::initDefaults()
{
    Image i = GenImageColor(32, 32, WHITE);
    TextureResource dTexture;
    dTexture.id = _default_res_id;
    dTexture.name = "default texture";
    dTexture.type = ResourceType_Texture;
    dTexture.tag = ResourceTag_InMemory;
    dTexture.res = LoadTextureFromImage(i);
    textures.emplace(_default_res_id, dTexture);

    ModelResource dModel;
    dModel.id = _default_res_id;
    dModel.name = "default cube";
    dModel.type = ResourceType_mesh;
    dModel.tag = ResourceTag_InMemory;
    dModel.res = LoadModelFromMesh(GenMeshCube(1,1,1));

    meshes.emplace(_default_res_id, dModel);
    resourceLocations.emplace("default", _default_res_id);
    UnloadImage(i);
}

void jam::ResourceManager::UpdateShaders(const Camera3D& camera, float totalTime)
{
}

void jam::ResourceManager::Shutdown()
{
    if (_instance)
        delete _instance;

    _instance = nullptr;
}

jam::ResourceID jam::ResourceManager::FindResource(std::string fileOrAlias)
{
    auto it = resourceLocations.find(fileOrAlias);

    if (it == resourceLocations.end())
        return _default_res_id;

    return it->second;
}

jam::ResourceID jam::ResourceManager::Load_Texture(std::string file)
{
    return _load_texture(file, nullptr);
}

jam::TextureResource* jam::ResourceManager::Get_Texture(ResourceID id)
{
    TextureResource* res = nullptr;
    try {
        res = &textures.at(id);
    }
    catch (const std::out_of_range& e) {
        res = &textures.at(_default_res_id);
    }
    return res;
}

jam::TextureResource* jam::ResourceManager::Get_Or_LoadTexture(ResourceID existing_id, std::string fileLocation)
{
    auto itTex = textures.find(existing_id);
    if (itTex == textures.end())
    {
        if (fileLocation.empty())
        {
            //find it
            auto repoRecord = resources.find(existing_id);
            if (repoRecord != resources.end())
            {
                fileLocation = repoRecord->second.filepath;
            }
            else
            {
                TraceLog(LOG_ERROR, "[ResourceManager]\t Could not find a resource with ID: '%s' | %llu.", existing_id.toString(), existing_id.toUint64());
                return &textures.at(_default_res_id);
                //return nullptr;
            }
        }

        ResourceID tracked_id = this->_load_texture(fileLocation, &existing_id);
        if (existing_id != tracked_id) {
            TraceLog(LOG_WARNING, "[ResourceManager]\t Mismatched Texture IDs found for '%s'.\n\tWanted '%s'\n\tFound: '%s'"
                , fileLocation.c_str()
                , existing_id.toString().c_str()
                , tracked_id.toString().c_str()
            );
        }

        return Get_Texture(tracked_id);
    }

    return &itTex->second;
    
}

jam::ResourceID jam::ResourceManager::Load_Mesh(std::string file)
{
    return _load_mesh(file, nullptr);
}

jam::ModelResource* jam::ResourceManager::Get_Mesh(ResourceID id)
{
    ModelResource* res = nullptr;
    try {
        res = &meshes.at(id);
    }
    catch (const std::out_of_range& e) {
        res = &meshes.at(_default_res_id);
    }
    return res;
}

jam::ModelResource* jam::ResourceManager::Get_Or_LoadMesh(ResourceID existing_id, std::string fileLocation)
{

    auto itTex = meshes.find(existing_id);
    if (itTex == meshes.end())
    {
        if (fileLocation.empty())
        {
            //find it
            auto repoRecord = resources.find(existing_id);
            if (repoRecord != resources.end())
            {
                fileLocation = repoRecord->second.filepath;
            }
            else
            {
                TraceLog(LOG_ERROR, "[ResourceManager]\t Could not find a resource with ID: '%s' | %llu.", existing_id.toString(), existing_id.toUint64());
                return &meshes.at(_default_res_id);
                //return nullptr;
            }
        }

        ResourceID tracked_id = this->_load_mesh(fileLocation, &existing_id);
        if (existing_id != tracked_id) {
            TraceLog(LOG_WARNING, "[ResourceManager]\t Mismatched Texture IDs found for '%s'.\n\tWanted '%s'\n\tFound: '%s'"
                , fileLocation.c_str()
                , existing_id.toString().c_str()
                , tracked_id.toString().c_str()
            );
        }

        return Get_Mesh(tracked_id);
    }

    return &itTex->second;
}

jam::ResourceID jam::ResourceManager::Load_Shader(std::string file)
{
    return _load_shader(file, nullptr);
}

jam::ShaderResource* jam::ResourceManager::Get_Shader(ResourceID id)
{
    auto it = shaders.find(id);
    if (it != shaders.end())
        return &it->second;
 
    return nullptr;
}

jam::ShaderResource* jam::ResourceManager::Get_Or_Shader(ResourceID existing_id, std::string fileLocation)
{
    auto iter = shaders.find(existing_id);
    if (iter == shaders.end())
    {
        if (fileLocation.empty())
        {
            //find it
            auto repoRecord = resources.find(existing_id);
            if (repoRecord != resources.end())
            {
                fileLocation = repoRecord->second.filepath;
            }
            else
            {
                TraceLog(LOG_ERROR, "[ResourceManager]\t Could not find a resource with ID: '%s' | %llu.", existing_id.toString(), existing_id.toUint64());
                return nullptr;
            }
        }

        ResourceID tracked_id = this->_load_shader(fileLocation, &existing_id);
        if (existing_id != tracked_id) {
            TraceLog(LOG_WARNING, "[ResourceManager]\t Mismatched Shader IDs found for '%s'.\n\tWanted '%s'\n\tFound: '%s'"
                , fileLocation.c_str()
                , existing_id.toString().c_str()
                , tracked_id.toString().c_str()
            );
        }

        return Get_Shader(tracked_id);
    }

    return &iter->second;
}

jam::ResourceManager::ResourceManager()
{
}

jam::ResourceManager::~ResourceManager()
{
    for (auto&& [id, texture] : textures)
    {
        texture.Unload();
    }

    for (auto&& [id, model] : meshes)
    {
        //model.Unload();
    }
}

jam::ResourceID jam::ResourceManager::_load_resource(std::string file, ResourceID* tracked)
{
    ResourceID rid = FindResource(file);
    if (rid != _default_res_id) // the resource already exists, so return it
        return rid;

    fs::path target(file);

    if (!std::filesystem::exists(target)) //resource doesn't exist at all, quit
    {
        TraceLog(LOG_ERROR, "[ResourceManager]\t can not find file: '%s'", file.c_str());
        return 0;
    }

    if (tracked)
        rid = *tracked;
    else
        rid = ResourceID();

    return rid;
}

jam::ResourceID jam::ResourceManager::_load_texture(std::string file, ResourceID* tracked)
{
    ResourceID rid = _load_resource(file, tracked);

    TextureResource resource;

    bool success = resource.Load(file);
    if (success)
    {
        textures.emplace(rid, resource);
        resourceLocations.emplace(file, rid);
        TraceLog(LOG_INFO, "[ResourceManager]\t [%s] Texture Loaded Successfully  '%s'", rid.toString().c_str(), file.c_str());
    }
    else
    {
        TraceLog(LOG_ERROR, "[ResourceManager]\t could not validate texture '%s'", file.c_str());
    }
    return rid;
}

jam::ResourceID jam::ResourceManager::_load_mesh(std::string file, ResourceID* tracked)
{
    ResourceID rid = _load_resource(file, tracked);

    ModelResource resource;
    bool success = resource.Load(file);

    if (success)
    {
        meshes.emplace(rid, resource);
        resourceLocations.emplace(file, rid);
        TraceLog(LOG_INFO, "[ResourceManager]\t [%s] Model Loaded Successfully  '%s'", rid.toString().c_str(), file.c_str());
    }
    else
    {
        TraceLog(LOG_ERROR, "[ResourceManager]\t could not validate Model '%s'", file.c_str());
    }


    return rid;
}

jam::ResourceID jam::ResourceManager::_load_shader(std::string file, ResourceID* tracked)
{
    ResourceID rid = _load_resource(file, tracked);

    ShaderResource resource;
    bool success = resource.Load(file);

    if (success)
    {
        shaders.emplace(rid, resource);
        resourceLocations.emplace(file, rid);
        TraceLog(LOG_INFO, "[ResourceManager]\t [%s] Shader Loaded Successfully  '%s'", rid.toString().c_str(), file.c_str());
    }
    else
    {
        TraceLog(LOG_ERROR, "[ResourceManager]\t could not validate Shader '%s'", file.c_str());
    }


    return rid;
}

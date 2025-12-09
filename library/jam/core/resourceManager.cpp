#include "resourceManager.hpp"
#include "jam/serialization/yaml/serializeYAML.hpp"

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

    UnloadImage(i);
}

void jam::ResourceManager::UpdateShaders(const Camera3D& camera, float totalTime)
{
}

void jam::ResourceManager::Shutdown()
{
}

jam::ResourceID jam::ResourceManager::FindResource(std::string fileOrAlias)
{
	return ResourceID();
}

jam::ResourceID jam::ResourceManager::Load_Texture(std::string file)
{
	return ResourceID();
}

jam::TextureResource* jam::ResourceManager::Get_Texture(ResourceID id)
{
    return nullptr;
}

jam::TextureResource* jam::ResourceManager::Get_Or_LoadTexture(ResourceID rid, std::string fileLocation)
{
    return &textures.at(_default_res_id);
    return nullptr;
}

jam::ResourceID jam::ResourceManager::Load_Mesh(std::string file)
{
    return ResourceID();
}

jam::ModelResource* jam::ResourceManager::Get_Mesh(ResourceID id)
{
    return nullptr;
}

jam::ModelResource* jam::ResourceManager::Get_Or_LoadMesh(ResourceID rid, std::string fileLocation)
{
    return &meshes.at(_default_res_id);
    return nullptr;
}

jam::ResourceManager::ResourceManager()
{
}

jam::ResourceManager::~ResourceManager()
{
}

jam::ResourceID jam::ResourceManager::_load_resource(std::string file, ResourceID* tracked)
{
	return ResourceID();
}

jam::ResourceID jam::ResourceManager::_load_texture(std::string file, ResourceID* tracked)
{
	return ResourceID();
}

jam::ResourceID jam::ResourceManager::_load_mesh(std::string file, ResourceID* tracked)
{
    return ResourceID();
}

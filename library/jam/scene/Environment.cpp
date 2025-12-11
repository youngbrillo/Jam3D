#include "Environment.hpp"
#include "jam/serialization/yaml/serializeYAML.hpp"
#include "jam/core/resourceManager.hpp"
#include <rlgl.h>


namespace YAML
{
    template<>
    struct convert<jam::Skybox>
    {
        static Node encode(const jam::Skybox& rhs) { 
            Node node; 
            node.push_back(rhs.enabled); 
            node.push_back(rhs.shader_id);
            node.push_back(rhs.texture_id);
            return node; 
        }
        static bool decode(const Node& node, jam::Skybox& rhs) {
            if (!node.IsSequence() || node.size() < 3) 
                return false;

            jam::readValueEx(node[0], &rhs.enabled);
            jam::readValueEx(node[1], &rhs.shader_id);
            jam::readValueEx(node[2], &rhs.texture_id);
            return true;
        }
    };
}

inline YAML::Emitter& operator<<(YAML::Emitter& out, const jam::Skybox& v)
{
    out << YAML::Flow << YAML::BeginSeq << v.enabled << v.shader_id << v.texture_id << YAML::EndSeq;
    return out;
}

jam::Skybox::Skybox()
{
}

jam::Skybox::~Skybox()
{
}

bool jam::Skybox::valid() const
{
    return enabled && _valid;
}

bool jam::Skybox::Load()
{
    Unload();
    model = { 0 };

    auto& rm = ResourceManager::Get();
    auto* textureRef = rm.Get_Or_LoadTexture(texture_id);
    auto* shaderRef = rm.Get_Or_Shader(shader_id);

    if (shaderRef && textureRef)
    {

        model = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
        model.materials[0].shader = shaderRef->res;
        auto& shader = model.materials[0].shader;
        int val = MATERIAL_MAP_CUBEMAP;
        SetShaderValue(shader, GetShaderLocation(shader, "environmentMap"), &val, SHADER_UNIFORM_INT);
        val = 1;
        SetShaderValue(shader, GetShaderLocation(shader, "noGamma"), &val, SHADER_UNIFORM_INT);
        Material& mat = model.materials[0];
        SetMaterialTexture(&model.materials[0], MATERIAL_MAP_CUBEMAP, textureRef->res);

        _valid = true;
    }
    else
        _valid = false;

    return _valid;
}

void jam::Skybox::Unload()
{
    if (IsModelValid(model))
        UnloadModel(model);

    model = Model{ 0 };
    _valid = false;
}

void jam::Skybox::render()
{
    if (!valid())
        return;

    rlDisableBackfaceCulling();     // Disable backface culling to render inside the cube
    rlDisableDepthMask();			// Disable depth writes
    rlDisableDepthTest();			// Disable depth test for speed

    DrawModel(model, Vector3{ 0, 0, 0 }, 1.0f, WHITE);

    rlEnableBackfaceCulling();     // enable things we turned off
    rlEnableDepthMask();
    rlEnableDepthTest();
}

void jam::Skybox::serialize(YAML::Emitter& out)
{
    out << this;
}

void jam::Skybox::deserialize(const YAML::Node& node)
{
    readValueEx(node, this);
}

void jam::Environment::serialize(YAML::Emitter& out)
{
    out << YAML::BeginMap
        << YAML::Key << "skybox" << YAML::Value << skybox
        ;
}

void jam::Environment::deserialize(const YAML::Node& node)
{
    if (readValueEx(node["skybox"], &skybox))
    {
        skybox.Load();
    }

    if (auto light_group = node["lighting"])
    {
        for (auto light_node : light_group["lights"])
        {

        }
    }
}

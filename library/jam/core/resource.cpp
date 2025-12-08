#include "resource.hpp"
#include "jam/serialization/yaml/serializeYAML.hpp"

using namespace jam;

bool jam::ShaderResource::Reload()
{
    if (type == ShaderType_Null)
        return false;

    if (IsShaderValid(shader))
        UnloadShader(shader);

    if (format == ShaderFormat_File)
    {
        shader = LoadShader(
            vertexLocation.empty() ? nullptr : vertexLocation.c_str(),
            fragmentLocation.c_str()
        );


    }
    else if (format == ShaderFormat_Inline)
    {
        shader = LoadShaderFromMemory(vertexLocation.c_str(), fragmentLocation.c_str());
    }
    else
    {
        //invalid format
        TraceLog(LOG_ERROR, "[ShaderResource]\t could not validate shader (UNKNOWN FORMAT) %s | %s", vertexLocation.c_str(), fragmentLocation.c_str());
        return false;
    }

    bool valid = IsShaderValid(shader);
    if (valid)
    {
        uniforms.time_loc = GetShaderLocation(shader, "time");
        uniforms.view_loc = GetShaderLocation(shader, "camera_position");
    }

    return valid;
}

Image jam::generate_image(ImageResource res)
{
    return Image();
}

Mesh jam::generate_mesh(MeshResource res)
{
    return Mesh();
}

ImageResource jam::load_image_resource(const std::string& file, ResourceID* id)
{
    return ImageResource();
}

ImageResource jam::load_image_resource(YAML::Node node, ResourceID* id)
{
    return ImageResource();
}

MeshResource jam::load_mesh_resource(const std::string& file, ResourceID* id)
{
    return MeshResource();
}

MeshResource jam::load_mesh_resource(YAML::Node node, ResourceID* id)
{
    return MeshResource();
}

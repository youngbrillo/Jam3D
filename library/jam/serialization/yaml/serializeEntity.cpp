#include "serializeEntity.hpp"
#include "jam/modules/core3d/core3d.hpp"
#include "jam/core/resourceManager.hpp"

using namespace jam::components;

namespace YAML
{
    template<>
    struct convert<Transform3D>
    {
        static Node encode(const Transform3D& rhs) {
            Node node;
            node.push_back(rhs.position.x); node.push_back(rhs.position.y); node.push_back(rhs.position.z);
            node.push_back(rhs.size.x); node.push_back(rhs.size.y); node.push_back(rhs.size.z);
            node.push_back(rhs.orientation.x); node.push_back(rhs.orientation.y); node.push_back(rhs.orientation.z); node.push_back(rhs.orientation.w);
            node.push_back(rhs.scale);
            return node;
        }
        static bool decode(const Node& node, Transform3D& rhs) {
            if (!node.IsSequence() || node.size() < 11)
                return false;
            rhs.position.x = node[0].as<float>(); rhs.position.y = node[1].as<float>(); rhs.position.z = node[2].as<float>();
            rhs.size.x = node[3].as<float>(); rhs.size.y = node[4].as<float>(); rhs.size.z = node[5].as<float>();
            rhs.orientation.x = node[6].as<float>(); rhs.orientation.y = node[7].as<float>(); rhs.orientation.z = node[8].as<float>(); rhs.orientation.w = node[9].as<float>();
            rhs.scale = node[10].as<float>();
            return true;
        }
    };

    template<>
    struct convert<MeshInstance3D>
    {
        static Node encode(const MeshInstance3D& rhs) {
            Node node;
            node.push_back(rhs.mesh_id);
            node.push_back(rhs.material_id);
            return node;
        }
        static bool decode(const Node& node, MeshInstance3D& rhs) {
            if (!node.IsSequence() || node.size() < 2)
                return false;
            rhs.mesh_id = node[0].as<jam::UUID>();
            rhs.material_id = node[1].as<jam::UUID>();
            return true;
        }
    };
    static YAML::Emitter& operator<<(YAML::Emitter& out, const Transform3D& v)
    {
        out << YAML::Flow << YAML::BeginSeq
            << v.position.x << v.position.y << v.position.z
            << v.size.x << v.size.y << v.size.z
            << v.orientation.x << v.orientation.y << v.orientation.z << v.orientation.w
            << v.scale
            << YAML::EndSeq;
        return out;
    }

    static YAML::Emitter& operator<<(YAML::Emitter& out, const MeshInstance3D& v)
    {
        out << YAML::Flow << YAML::BeginSeq
            << v.mesh_id
            << v.material_id
            << YAML::EndSeq;
        return out;
    }
}




void jam::serialization::SerializeEntity(YAML::Emitter& out, Entity& e)
{

}

void jam::serialization::DeserializeEntity(YAML::Node in, Entity& e)
{
	auto& nameTag = e.add<components::NameTag>();
	auto& relationship = e.add<components::Relationship>();
    ResourceManager& rm = ResourceManager::Get();

	if (auto node = in["handle"])
	{
		bool visible = true;
		bool enabled = true;;
		readValueEx(node[0], &nameTag.name);
		if (readValueEx(node[1], &visible) && visible == false)
		{
			e.hide();
		}
		if (readValueEx(node[2], &enabled) && enabled == false)
		{
			e.disable();
		}

	}

    if (auto node = in["Transform3D"])
    {
        auto& component = e.add<components::Transform3D>();
        readValueEx(node, &component);
    }

    if (auto node = in["MeshInstance3D"])
    {
        auto& component = e.add<components::MeshInstance3D>();
        readValueEx(node, &component);

        auto* model_resource = rm.Get_Or_LoadMesh(component.mesh_id);
        if (model_resource)
        {
            component.mesh = &model_resource->res.meshes[0];
        }
        auto* texture_resource = rm.Get_Or_LoadTexture(component.material_id);
        if (texture_resource)
        {
            component.material.maps[MATERIAL_MAP_ALBEDO].texture = texture_resource->res;
        }
    }


}

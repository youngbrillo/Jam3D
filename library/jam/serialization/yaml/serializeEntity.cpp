#include "serializeEntity.hpp"
#include "jam/modules/core3d/core3d.hpp"
#include "jam/modules/ui/ui.hpp"
#include "jam/core/resourceManager.hpp"

using namespace jam::components;

namespace YAML
{
    template<>
    struct convert<Transform2D>
    {
        static Node encode(const Transform2D& rhs) {
            Node node;
            node.push_back(rhs.position.x); node.push_back(rhs.position.y); 
            node.push_back(rhs.size.x); node.push_back(rhs.size.y); 
            node.push_back(rhs.origin.x); node.push_back(rhs.origin.y);
            node.push_back(rhs.scale);
            node.push_back(rhs.angle);
            return node;
        }
        static bool decode(const Node& node, Transform2D& rhs) {
            if (!node.IsSequence() || node.size() < 7)
                return false;
            rhs.position.x  = node[0].as<float>(); rhs.position.y   = node[1].as<float>(); 
            rhs.size.x      = node[2].as<float>(); rhs.size.y       = node[3].as<float>();
            rhs.origin.x    = node[4].as<float>(); rhs.origin.y     = node[5].as<float>();
            rhs.scale       = node[6].as<float>(); rhs.angle        = node[7].as<float>();
            return true;
        }
    };

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
            node.push_back(rhs.material.maps[0].color);
            return node;
        }
        static bool decode(const Node& node, MeshInstance3D& rhs) {
            if (!node.IsSequence() || node.size() < 2)
                return false;
            rhs.mesh_id = node[0].as<jam::UUID>();
            rhs.material_id = node[1].as<jam::UUID>();
            jam::readValueEx(node[2], &rhs.material.maps[0].color);
            return true;
        }
    };


    template<>
    struct convert<UIPanel>
    {
        static Node encode(const UIPanel& rhs) {
            Node node;
            node.push_back(rhs.id);
            node.push_back(rhs.npatch);
            node.push_back(rhs.tint);
            node.push_back(rhs.visible);
            return node;
        }
        static bool decode(const Node& node, UIPanel& rhs) {
            if (!node.IsSequence() || node.size() < 3)
                return false;
            rhs.id      = node[0].as<jam::UUID>();
            rhs.npatch  = node[1].as<NPatchInfo>();
            rhs.tint    = node[2].as<Color>();
            jam::readValueEx(node[3], &rhs.visible);
            return true;
        }
    };

    template<>
    struct convert<UIText>
    {
        static Node encode(const UIText& rhs) {
            Node node;
            node.push_back(rhs.id);
            node.push_back(rhs.text);
            node.push_back(rhs.origin);
            node.push_back(rhs.fontSize);
            node.push_back(rhs.spacing);
            node.push_back(rhs.rotation);
            node.push_back(rhs.tint);
            node.push_back(rhs.visible);
            return node;
        }
        static bool decode(const Node& node, UIText& rhs) {
            if (!node.IsSequence())
                return false;

            jam::readValueEx(node[0], &rhs.id);
            jam::readValueEx(node[1], &rhs.text);
            jam::readValueEx(node[2], &rhs.origin);
            jam::readValueEx(node[3], &rhs.fontSize);
            jam::readValueEx(node[4], &rhs.spacing);
            jam::readValueEx(node[5], &rhs.rotation);
            jam::readValueEx(node[6], &rhs.tint);
            jam::readValueEx(node[7], &rhs.visible);

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
    static YAML::Emitter& operator<<(YAML::Emitter& out, const Transform2D& v)
    {
        out << YAML::Flow << YAML::BeginSeq
            << v.position.x << v.position.y 
            << v.size.x << v.size.y 
            << v.origin.x << v.origin.y 
            << v.scale
            << v.angle
            << YAML::EndSeq;
        return out;
    }

    static YAML::Emitter& operator<<(YAML::Emitter& out, const MeshInstance3D& v)
    {
        out << YAML::Flow << YAML::BeginSeq
            << v.mesh_id
            << v.material_id
            << v.material.maps[0].color
            << YAML::EndSeq;
        return out;
    }

    static YAML::Emitter& operator<<(YAML::Emitter& out, const UIPanel& v)
    {
        out << YAML::Flow << YAML::BeginSeq
            << v.id
            << v.npatch
            << v.tint
            << v.visible
            << YAML::EndSeq;
        return out;
    }
    static YAML::Emitter& operator<<(YAML::Emitter& out, const UIText& v)
    {
        out << YAML::Flow << YAML::BeginSeq
            << v.id
            << v.text
            << v.origin
            << v.fontSize
            << v.spacing
            << v.rotation
            << v.tint
            << v.visible
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


    if (auto node = in["Transform2D"])
    {
        auto& component = e.add<components::Transform2D>();
        readValueEx(node, &component);
    }

    if (auto node = in["UIPanel"])
    {
        auto& component = e.add<components::UIPanel>();
        readValueEx(node, &component);

        auto* texture_resource = rm.Get_Or_LoadTexture(component.id);
        if (texture_resource)
        {
            component.texture = texture_resource->res;
        }
    }

    if (auto node = in["UIText"])
    {
        auto& component = e.add<components::UIText>();
        readValueEx(node, &component);

        // todo: add Font to resource loader
        
        //auto* texture_resource = rm.Get_Or_LoadTexture(component.id);
        //if (texture_resource)
        //{
        //    component.texture = texture_resource->res;
        //}
    }
}

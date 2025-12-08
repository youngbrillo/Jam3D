#include "serializeScene.hpp"

namespace yml = YAML;
void jam::serialization::SerializeScene(YAML::Emitter& out, Scene* scene)
{
	out << yml::BeginMap;
		SerializeSceneConfig(out, scene->config);
		SerializeScene_Resources(out, scene);
		SerializeScene_Entities(out, scene);
	out << yml::EndMap;
}

void jam::serialization::SerializeSceneConfig(YAML::Emitter& out, SceneConfig& def)
{
	if (def.id == 0)
		def.id = UUID();

	out << yml::Key << "scene" << yml::BeginMap;
		out << yml::Key << "templateId" << yml::Value << def.templateId;
		out << yml::Key << "id" << yml::Value << def.id;
		out << yml::Key << "name" << yml::Value << def.name;
		out << yml::Key << "configPath" << yml::Value << def.configPath;
		out << yml::Key << "scriptPath" << yml::Value << def.scriptPath;
		out << yml::Key << "backgroundColor" << yml::Value << def.backgroundColor;
		out << yml::Key << "resolution" << yml::Value << def.resolution;
	out << yml::EndMap;
}

void jam::serialization::SerializeScene_Resources(YAML::Emitter& out, Scene* scene)
{
}

void jam::serialization::SerializeScene_Entities(YAML::Emitter& out, Scene* scene)
{
}

void jam::serialization::DeserializeScene(YAML::Node root, Scene* scene)
{
	if (!root)
	{
		TraceLog(LOG_ERROR, "[yamlScene]\t could not deserialize root node");
		return;
	}
	DeserializeSceneConfig(root["scene"], scene->config);
	DeserializeScene_Resources(root["resources"], scene);
	DeserializeScene_Entities(root["entities"], scene);
}

void jam::serialization::DeserializeSceneConfig(YAML::Node in, SceneConfig& config)
{
	if (!in)
		return;
	readValueEx(in["templateId"], &config.templateId);
	readValueEx(in["id"], &config.id);
	readValueEx(in["name"], &config.name);
	readValueEx(in["configPath"], &config.configPath);
	readValueEx(in["scriptPath"], &config.scriptPath);
	readValueEx(in["backgroundColor"], &config.backgroundColor);
	readValueEx(in["resolution"], &config.resolution);
}

void jam::serialization::DeserializeScene_Resources(YAML::Node in, Scene* scene)
{
}

void jam::serialization::DeserializeScene_Entities(YAML::Node in, Scene* scene)
{
}

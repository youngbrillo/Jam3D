#include "serializeScene.hpp"
#include "serializeEntity.hpp"
#include "jam/core/resourceManager.hpp"

namespace yml = YAML;
void jam::serialization::SerializeScene(YAML::Emitter& out, Scene* scene)
{
	out << yml::BeginMap;
		SerializeSceneConfig(out, scene->config);
		SerializeRenderTarget(out, scene->renderTarget);
		SerializeScene_Resources(out, scene);
		scene->worldEnv.serialize(out);
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

void jam::serialization::SerializeRenderTarget(YAML::Emitter& out, RenderTarget& rt)
{
	out << yml::Key << "viewport" << yml::BeginMap
		<< yml::Key << "resoulution" << rt.resolution
		<< yml::Key << "rendering" << rt.enabled
		<< yml::Key << "visible_on_screen" << rt.visible
	<< yml::EndMap;
}

void jam::serialization::SerializeScene_Entities(YAML::Emitter& out, Scene* scene)
{
}

void jam::serialization::SerializeScene_Resources(YAML::Emitter& out, Scene* scene)
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
	DeserializeRenderTarget(root["viewport"], scene->renderTarget);
	DeserializeScene_Resources(root["resources"], scene);
	scene->worldEnv.deserialize(root["environment"]);
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

void jam::serialization::DeserializeRenderTarget(YAML::Node in, RenderTarget& rt)
{
	if (!in)
		return;
	readValueEx(in["resolution"], &rt.resolution);
	readValueEx(in["rendering"], &rt.enabled);
	readValueEx(in["visible_on_screen"], &rt.visible);

}


void jam::serialization::DeserializeScene_Entities(YAML::Node in, Scene* scene)
{
	if (in.IsNull())
		return;

	for (auto child : in)
	{
		auto node = child["entity"];
		Entity e(scene->world.create(), scene->world);
		DeserializeEntity(node, e);
	}
}

void jam::serialization::DeserializeScene_Resources(YAML::Node in, Scene* scene)
{
	if (!in)
		return;

	auto& rm = ResourceManager::Get();

	for (auto resourceNode : in)
	{
		auto node = resourceNode["res"];
		Resource res;
		ResourceLite resLite;
		res.deserialize_as_header(node);

		resLite.id = rm.RegisterResource(res);
		resLite.type = res.type;

		if (resLite.id != 0) {
			scene->localResources.emplace_back(resLite);
			rm.LoadResource(resLite.id);
		}
	}

}


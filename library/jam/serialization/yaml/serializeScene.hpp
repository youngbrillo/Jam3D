#pragma once
#include "serializeYAML.hpp"
#include "jam/scene/iScene.hpp"

namespace jam::serialization
{
	void SerializeScene(YAML::Emitter& out, Scene* scene);
	void SerializeSceneConfig(YAML::Emitter& out, SceneConfig& config);
	void SerializeRenderTarget(YAML::Emitter& out, RenderTarget& rt);
	void SerializeScene_Entities(YAML::Emitter& out, Scene* scene);
	void SerializeScene_Resources(YAML::Emitter& out, Scene* scene);

	void DeserializeScene(YAML::Node in, Scene* scene);
	void DeserializeSceneConfig(YAML::Node in, SceneConfig& config);
	void DeserializeRenderTarget(YAML::Node in, RenderTarget& rt);
	void DeserializeScene_Entities(YAML::Node in, Scene* scene);
	void DeserializeScene_Resources(YAML::Node in, Scene* scene);
}
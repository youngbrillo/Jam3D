#pragma once
#include "serializeYAML.hpp"
#include "jam/scene/iScene.hpp"

namespace jam::serialization
{
	void SerializeScene(YAML::Emitter& out, Scene* scene);
	void SerializeSceneConfig(YAML::Emitter& out, SceneConfig& config);
	void SerializeScene_Entities(YAML::Emitter& out, Scene* scene);

	void DeserializeScene(YAML::Node in, Scene* scene);
	void DeserializeSceneConfig(YAML::Node in, SceneConfig& config);
	void DeserializeScene_Entities(YAML::Node in, Scene* scene);
}
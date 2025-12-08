#include "serializeYAML.hpp"
#include "jam/scene/iScene.hpp"

namespace jam::serialization
{
	void SerializeScene(YAML::Emitter& out, iScene* scene);
	void SerializeSceneConfig(YAML::Emitter& out, SceneConfig& config);
	void SerializeScene_Resources(YAML::Emitter& out, iScene* scene);
	void SerializeScene_Entities(YAML::Emitter& out, iScene* scene);

	void DeserializeScene(YAML::Node in, iScene* scene);
	void DeserializeSceneConfig(YAML::Node in, SceneConfig& config);
	void DeserializeScene_Resources(YAML::Node in, iScene* scene);
	void DeserializeScene_Entities(YAML::Node in, iScene* scene);
}
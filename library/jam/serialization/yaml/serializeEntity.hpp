#include "serializeYAML.hpp"
#include "jam/core/entity.hpp"

namespace jam::serialization
{
	void SerializeEntity(YAML::Emitter& out, Entity& e);
	void DeserializeEntity(YAML::Node in, Entity& e);
}
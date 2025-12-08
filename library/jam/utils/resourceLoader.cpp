#include "resourceLoader.hpp"

using namespace jam;
using namespace jam::components;

UUID jam::utils::AddSceneResource(entt::registry& resources, std::string filepath
	, UUID register_or_generate, bool* does_resource_exist)
{
	for (auto&& [e, id, tag] : resources.view<UUID, NameTag>().each())
	{
		if (tag.name == filepath)
		{
			*does_resource_exist = true;
			return id;
		}
	}
	UUID id = register_or_generate > 0 ? register_or_generate : UUID();
	return id;
}

Entity jam::utils::CreateSceneResourceTemplate(entt::registry& resources, std::string filepath, UUID id)
{
	Entity e(resources.create(), resources);
	e.add<NameTag>().name = filepath;
	e.add<UUID>(id);

	return e;
}


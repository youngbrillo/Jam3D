#pragma once
#include <raylib.h>
#include <string>
#include "Jam/core/entity.hpp"

namespace jam::utils
{
	UUID AddSceneResource(entt::registry& resources, std::string filepath, UUID register_or_generate, bool* does_resource_exist);
	Entity CreateSceneResourceTemplate(entt::registry& resources, std::string filepath, UUID id);

	template <typename Type>
	inline UUID AddSceneResource(
		entt::registry& resources, std::string filepath
		, std::function<Type(const char*)> onLoadResource
		, std::function<bool(Type)> isValidResource
		, UUID register_or_generate
	);

	template <typename Type>
	inline Type GetSceneResource(entt::registry& resources, UUID rid, Type defaultType = Type());
} // namespace jam::utils


namespace jam::utils
{
	template<typename Type>
	UUID AddSceneResource(
		entt::registry& resources,
		std::string filepath,
		std::function<Type(const char*)> onLoadResource,
		std::function<bool(Type)> isValidResource,
		UUID register_or_generate)
	{
		bool exists = false;
		UUID id = AddSceneResource(resources, filepath, register_or_generate, &exists);
		if (exists) return id;

		Entity e = CreateSceneResourceTemplate(resources, filepath, id);


		Type& resource = e.add<Type>();
		resource = onLoadResource(filepath.c_str());

		if (!isValidResource(resource))
		{
			resources.destroy(e);
			return 0;
		}

		return id;
	}
	template<typename Type>
	Type GetSceneResource(entt::registry& resources, UUID rid, Type defaultType)
	{
		for (auto&& [e, id, resource] : resources.view<UUID, Type>().each())
		{
			if (rid == id)
				return resource;
		}

		return defaultType;
	}
}
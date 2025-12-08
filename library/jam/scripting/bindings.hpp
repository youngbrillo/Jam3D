#pragma once
#include <sol/sol.hpp>
#include "Jam/core/uuid.hpp"
#include <functional>
typedef std::function<void(sol::state& )> LUAScriptBindingFunction;

namespace jam::scripting
{
	void BindAll(sol::state& lua);
	void BindScene(sol::state& lua);
	void BindComponents(sol::state& lua);
	void BindEntity(sol::state& lua);
	void BindRaylib(sol::state& lua);
	void BindSystems(sol::state& lua);


	UUID addCustomBinding(LUAScriptBindingFunction function);
	void removeCustomBinding(UUID id);
}


#include "bindings.hpp"
#include <raylib.h>
#include <map>

struct LuaBindingCollection {
	std::map<jam::UUID, LUAScriptBindingFunction> _functions;

	static LuaBindingCollection& Get();

	void apply(sol::state& lua);

};
static LuaBindingCollection* sLuaBindingCollection = nullptr;

LuaBindingCollection& LuaBindingCollection::Get()
{
	if (sLuaBindingCollection == nullptr)
		sLuaBindingCollection = new LuaBindingCollection();

	return *sLuaBindingCollection;
}
void LuaBindingCollection::apply(sol::state& lua)
{
	for (auto&& [id, func] : _functions)
	{
		func(lua);
	}
}
void jam::scripting::BindAll(sol::state& lua)
{
	lua.open_libraries(
		sol::lib::base,
		sol::lib::package,
		sol::lib::coroutine,
		sol::lib::string,
		sol::lib::os,
		sol::lib::math,
		sol::lib::math,
		sol::lib::table,
		sol::lib::debug,
		sol::lib::bit32,
		sol::lib::io
	);

	BindScene(lua);
	BindSystems(lua);
	BindComponents(lua);
	BindEntity(lua);
	BindRaylib(lua);

	//add custom bindings
	LuaBindingCollection::Get().apply(lua);

	//update package path to be along the working directory!
	std::string current_dir = GetWorkingDirectory();
	current_dir += "/?.lua;";
	lua["package"]["path"] = current_dir + lua["package"]["path"].get<std::string>();
}



jam::UUID jam::scripting::addCustomBinding(LUAScriptBindingFunction function)
{
	UUID id;
	auto& collection = LuaBindingCollection::Get();
		collection._functions.emplace(id, function);
	return id;
}

void jam::scripting::removeCustomBinding(UUID id)
{
	auto& collection = LuaBindingCollection::Get();
	auto iter = collection._functions.find(id);

	if (iter != collection._functions.end())
	{
		collection._functions.erase(iter);
	}
}


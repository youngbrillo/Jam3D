#include "scriptComponent.hpp"
#include "jam/scripting/binding_helper.hpp"
#include "Jam/scripting/script.hpp"

jam::components::ScriptComponent::ScriptComponent(std::string Filepath, sol::state_view state)
	: filepath(Filepath)
{
    sol::load_result executable = state.load_file(filepath);
    if (executable.valid())
    {
        try {
            self = executable.call();
        }
        catch (const sol::error& err)
        {
            TraceLog(TraceLogLevel::LOG_ERROR, "[ScriptComponent]\t%s", err.what());
        }
    }
    else
    {
        TraceLog(LOG_ERROR, "[ScriptComponent]\t %s is not a valid executable!", filepath.c_str());
    }
}

jam::components::ScriptComponent::~ScriptComponent()
{
}

void jam::components::on_init_script(entt::registry& registry, entt::entity handle)
{
    Entity e(handle, registry);
    ScriptComponent& script = e.get<ScriptComponent>();

    if (!script.self.valid())
    {
        TraceLog(LOG_ERROR, "[ScriptComponent]\t cannot initialize table 'self' is invalid!");
        return;
    }

    script.self["_id"] = sol::readonly_property([e] { return e.handle(); });
    script.self["owner"] = e;
    script.self["this"] = e;
    script.self["ScriptComponent"] = &script;


    script.hooks.initialize = script.self["initialize"];
    script.hooks.destroy = script.self["destroy"];
    script.hooks.update = script.self["update"];
    script.hooks.beginContact = script.self["beginContact"];
    script.hooks.endContact = script.self["endContact"];

    if (script.self.valid() && script.hooks.initialize.valid())
        script.hooks.initialize(script.self);

}

void jam::components::on_destroy_script(entt::registry& registry, entt::entity handle)
{
    Entity e(handle, registry);
    ScriptComponent& script = e.get<ScriptComponent>();

    if (script.self.valid() && script.hooks.destroy.valid())
        script.hooks.destroy.call(script.self);

    script.self.abandon();
}

void jam::components::script_system_update(entt::registry& registry, float dt)
{
    for (auto&& [e, script] : registry.view<ScriptComponent>(entt::exclude<components::DisabledTag>).each())
    {
        if (!script.self.valid())
            continue;
        auto result = script.hooks.update(script.self, dt);
        bool isValid = result.valid();
        if (!isValid)
        {
            jam::scripting::catch_sol_error(result);
        }
    }
}

void jam::components::scripting::register_ScriptComponent(sol::state& lua)
{
    namespace help = jam::scripting::helper;
    help::register_meta_component<ScriptComponent>();

    lua.new_usertype<ScriptComponent>("ScriptComponent"
        , "type_id", &entt::type_hash<ScriptComponent>::value
        , sol::call_constructor
        , sol::factories([](std::string fp, sol::this_state state) {return ScriptComponent(fp, state); })
        , "getTable", [](ScriptComponent& script) -> sol::table { return script.self; }
    );
}

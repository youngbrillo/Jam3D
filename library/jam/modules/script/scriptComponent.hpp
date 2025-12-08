#pragma once
#include "jam/scripting/script.hpp"
#include <entt/entt.hpp>
//#include "jam/events/contactEvents.hpp"

namespace jam::components
{
    struct ScriptComponent
    {
        sol::table self;
        std::string filepath;
        struct {
            sol::function initialize;
            sol::function destroy;
            sol::function update;
            sol::function beginContact;
            sol::function endContact;
        } hooks;


        ScriptComponent() = default;
        ScriptComponent(ScriptComponent const&) = default;
        ScriptComponent(std::string Filepath, sol::state_view state);
        ~ScriptComponent();
    };

    void on_init_script(entt::registry& registry, entt::entity e);
    void on_destroy_script(entt::registry& registry, entt::entity e);
    void script_system_update(entt::registry& registry, float dt);
    //void script_system_begin_contacts(events::BeginContactEvent event_);
    //void script_system_end_contacts(events::EndContactEvent event_);

    namespace scripting
    {
        void register_ScriptComponent(sol::state& lua);
    }
}
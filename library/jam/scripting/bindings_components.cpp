#include "bindings.hpp"
//#include "jam/components/base.hpp"
//#include "Jam/modules/core/coreBindings.hpp"
#include "Jam/modules/script/scriptComponent.hpp"
#include "binding_helper.hpp"
//#include "Jam/modules/physics/physModule.hpp"

void jam::scripting::BindComponents(sol::state& lua)
{
	using namespace jam::components;
	namespace help = jam::scripting::helper;
	namespace bind = jam::components::scripting;

	//jam::core::register_coreBindings(lua);
	//jam::physics::register_PhysicsModule(lua);
	bind::register_ScriptComponent(lua);
}

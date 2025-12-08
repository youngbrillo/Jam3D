#include "bindings.hpp"
#include "../utils/random.hpp"

void jam::scripting::BindSystems(sol::state& lua)
{
	auto lib = lua["lib"].get_or_create<sol::table>();

	lib["GetRandomFloat"] = jam::GetRandomFloat;
	lib["GetRandomInt"] = jam::GetRandomInt;
	lib["GetRandomUint32"] = jam::GetRandomUint32;

	//lua.new_usertype<systems::iSystem>("iSystem"
	//	, "type_id", &entt::type_hash<systems::iSystem>::value
	//	, "enabled", &systems::iSystem::enabled
	//);

#if false
	using namespace jam::systems;
	lua.new_usertype<b2DebugDraw>("b2DebugDraw"
		, "drawShapes", &b2DebugDraw::drawShapes
		, "drawJoints", &b2DebugDraw::drawJoints
		, "drawJointExtras", &b2DebugDraw::drawJointExtras
		, "drawBounds", &b2DebugDraw::drawBounds
		, "drawMass", &b2DebugDraw::drawMass
		, "drawBodyNames", &b2DebugDraw::drawBodyNames
		, "drawContacts", &b2DebugDraw::drawContacts
		, "drawGraphColors", &b2DebugDraw::drawGraphColors
		, "drawContactNormals", &b2DebugDraw::drawContactNormals
		, "drawContactImpulses", &b2DebugDraw::drawContactImpulses
		, "drawContactFeatures", &b2DebugDraw::drawContactFeatures
		, "drawFrictionImpulses", &b2DebugDraw::drawFrictionImpulses
		, "drawIslands", &b2DebugDraw::drawIslands
	);

	lua.new_usertype<PhysicsSystem2D>("PhysicsSystem2D"
		, sol::base_classes, sol::bases<iSystem>()
		, "type_id", &entt::type_hash<PhysicsSystem2D>::value
		, "worldId", &PhysicsSystem2D::worldId
		, "b2DebugDraw", &PhysicsSystem2D::debugDraw
	);
#endif

}

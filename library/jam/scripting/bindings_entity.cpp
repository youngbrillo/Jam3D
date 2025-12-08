#include "bindings.hpp"
#include "../core/entity.hpp"
#include "binding_helper.hpp"

void jam::scripting::BindEntity(sol::state& lua)
{
	namespace bind = jam::scripting::helper;
	lua.new_usertype<Entity>("Entity"
		, "type_id", &entt::type_hash<Entity>::value
		, "destroy", &Entity::destroy
		, "getName", &Entity::getName
		, "getUUID", &Entity::getUUID
		, "isEnabled", &Entity::isEnabled
		, "isVisible", &Entity::isVisible
		, "isSerializable", &Entity::isSerializable
		, "enable", &Entity::enable
		, "disable", &Entity::disable
		, "show", &Entity::show
		, "hide", &Entity::hide
		, "disableSerialization", &Entity::disableSerialization
		, "enableSerialization", &Entity::enableSerialization
		, "createChild", &Entity::createChild
		, "addParent", &Entity::addParent
		, "removeParent", &Entity::removeParent
		, "addChild", &Entity::addChild
		, "removeChild", &Entity::removeChild
		, "removeAllChildren", &Entity::removeAllChildren
		, "getFirstChild", &Entity::getFirstChild
		, "getNextSibling", &Entity::getNextSibling
		, "getPrevSibling", &Entity::getPrevSibling
		, "getParent", &Entity::getParent
		, "hasChild", &Entity::hasChild
		, "isDecendantOf", &Entity::isDecendantOf
		, "hasParent", &Entity::hasParent
		, "isValid", &Entity::isValid
		, "handle", &Entity::handle
		//, "getPosition", &Entity::getPosition
		//, "setPosition", &Entity::setPosition
		, "add", bind::entity_bind_emplace
		, "get", bind::entity_bind_get
		, "has", bind::entity_bind_has
		, "remove", bind::entity_bind_remove
	);
}

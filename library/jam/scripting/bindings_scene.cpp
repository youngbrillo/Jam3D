#include "bindings.hpp"
#include "Jam/scene/iScene.hpp"


void jam::scripting::BindScene(sol::state& lua)
{
	lua.new_usertype<UUID>("UUID"
		, sol::meta_function::construct
		, sol::factories(
			//UUID.new()
			[]() {return UUID(); },
			[](uint64_t i) {return UUID(i); },
			//UUID:new()
			[](sol::object) {return UUID(); },
			[](sol::object, uint64_t v) {return UUID(v); }
		)
		//UUID(x,y,z)
		, sol::call_constructor
		, sol::factories(
			[]() {return UUID(); }
	, [](uint64_t i) {return UUID(i); })
		, "toString", &UUID::toString
		, "toUint64", &UUID::toUint64
	);

	lua.new_usertype<SceneConfig>("SceneConfig"
		, "type_id", &entt::type_hash<SceneConfig>::value
		, "id", &SceneConfig::id
		, "name", &SceneConfig::name
		, "configPath", &SceneConfig::configPath
		, "scriptPath", &SceneConfig::scriptPath
		, "backgroundColor", &SceneConfig::backgroundColor
	);

	lua.new_usertype<Scene>("Scene"
		, "type_id", &entt::type_hash<Scene>::value
		//, "GetConfig", &iScene::GetConfig
		, "CreateEntity", &Scene::CreateEntity
		, "CreateCube", &Scene::CreateCube
		, "CreateSphere", &Scene::CreateSphere
		, "CreatePlane", &Scene::CreatePlane
		, "CreateDonut", &Scene::CreateDonut
		, "CreateCamera", &Scene::CreateCamera
		, "CreateEditorCamera", &Scene::CreateEditorCamera
		, "GetEntityByName", &Scene::GetEntityByName
		, "GetCamera", &Scene::GetCamera
		, "config", &Scene::config
	);

	//lua.new_usertype<DefaultScene>("DefaultScene"
	//	, sol::base_classes, sol::bases<iScene>()
	//	, "type_id", &entt::type_hash<DefaultScene>::value
	//	//, "CreateCamera", &DefaultScene::CreateCamera
	//	//, "CreateEditorCamera", &DefaultScene::CreateEditorCamera
	//	, "GetCamera", &DefaultScene::GetCamera
	//	//, "CreateSprite", &DefaultScene::CreateSprite
	//	//, "CreateActor", &DefaultScene::CreateActor
	//	//, "CreateText", &DefaultScene::CreateText
	//	//, "CreateUIPanel", &DefaultScene::CreateUIPanel
	//	//, "CreateSoundEmitter", &DefaultScene::CreateSoundEmitter
	//	//, "CreateMusicEmitter", &DefaultScene::CreateMusicEmitter
	//);
}

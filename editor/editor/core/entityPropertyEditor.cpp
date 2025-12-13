#include "entityPropertyEditor.hpp"
#include "jam/modules/core3d/core3d.hpp"
//#include "Jam/modules/physics/physModule.hpp"

#include "editor/editor.hpp"
#include "resource_inspector.hpp"
#include <unordered_map>
#include "editor/imgui/imgui_stdlib.h"
#include "Jam/app/application.hpp"
///External | Component | Property | Function | Repository Manager
struct ECPFRManager {
	std::unordered_map<jam::UUID, EntityInspectionFunction> pop_up_functions;
	std::unordered_map<jam::UUID, EntityInspectionFunction> widget_functions;

	static ECPFRManager& Get();
};
static ECPFRManager* sECPFRManager = nullptr;
ECPFRManager& ECPFRManager::Get()
{
	if (sECPFRManager == nullptr)
		sECPFRManager = new ECPFRManager();

	return *sECPFRManager;
}
jam::UUID jam::editor::register_component_popups(EntityInspectionFunction func)
{
	UUID id = UUID();
	ECPFRManager::Get().pop_up_functions.emplace(id, func);
	return id;
}

void jam::editor::unregister_component_popups(UUID id)
{
	ECPFRManager::Get().pop_up_functions.erase(id);
}

jam::UUID jam::editor::register_component_editWidget(EntityInspectionFunction func)
{
	UUID id = UUID();
	ECPFRManager::Get().widget_functions.emplace(id, func);
	return id;
}

void jam::editor::unregister_component_editWidget(UUID id)
{
	ECPFRManager::Get().widget_functions.erase(id);
}
namespace jam::editor {
	static bool InspectTransform3D(Entity& entity, jam::components::Transform3D& transform) {

		bool moved = ImGui::DragFloat3("position", &transform.position.x, 0.1f);
		ImGui::DragFloat3("size", &transform.size.x, 0.1f);

		Vector3 euler = QuaternionToEuler(transform.orientation);

		if (ImGui::DragFloat3("rotation", &euler.x, 0.1f))
		{
			transform.orientation = QuaternionFromEuler(euler.x, euler.y, euler.z);
		}
		ImGui::DragFloat("scale", &transform.scale, 0.1f);

		//if (moved && entity.has<physics::RigidBody>())
		//{
		//	entity.get<physics::RigidBody>().setPosition(transform.position);
		//}

		return moved;
	}

	static bool InspectMeshInstance3D(Entity& entity, jam::components::MeshInstance3D& component)
	{
		// mesh drag n drop
		ImGui::Text("Mesh id : %s", component.mesh_id.toString().c_str());

		// material drag n drop
		ImGui::Text("Material id : %s", component.material_id.toString().c_str());

		// texture drag n drop
		ImGui::Text("Shader id : %s", component.material_id.toString().c_str());

		InspectMaterial(&component.material);
		return true;
	}

	static bool InspectSceneCameraComponent(Entity& entity, jam::components::SceneCamera3D& component)
	{
		//ImGui::Checkbox("active", &component.active);
		ImGui::DragFloat3("position", &component.camera.position.x, 0.1f);
		ImGui::DragFloat3("target", &component.camera.target.x, 0.1f);
		ImGui::DragFloat("FOV", &component.camera.fovy, 1.0f);

		return false;
	}

	static bool InspectCameraEditorComponent(Entity& entity, jam::components::CameraEditorComponent& component)
	{
		//ImGui::Checkbox("active", &component.enabled);
		//ImGui::DragFloat2("mousePos", &component.mousePos.x);

		return false;
	}
	/*
	static void InspectCameraShakePresets(Entity& entity, jam::core::CameraShakeSettings& preset)
	{
		ImGui::Checkbox("additive Trauma", &preset.additiveTrauma);
		ImGui::DragFloat("trauma", &preset.trauma, 0.01f);
		ImGui::DragFloat("trauma Multiplyer", &preset.traumaMultiplyer, 0.01f);
		ImGui::DragFloat("frequency", &preset.frequency, 0.01f);
		ImGui::DragFloat("shake Magnitude", &preset.shakeMagnitude, 0.01f);
		ImGui::DragFloat("rotation Magnitude", &preset.rotationMagnitude, 0.01f);
		ImGui::DragFloat("duration", &preset.duration, 0.01f);


		if (entity.has<jam::core::CameraShake>()) {
			if (ImGui::Button("Shake"))
			{
				entity.get<jam::core::CameraShake>().begin(preset);
			}
		}
	}
	*/
	/*
		static void InspectCameraShake(Entity& entity, jam::core::CameraShake& shake)
		{
			ImGui::DragFloat("trauma", &shake.trauma, 0.01f);
			ImGui::DragFloat("trauma Multiplyer", &shake.traumaMultiplyer, 0.01f);
			ImGui::DragFloat("frequency", &shake.frequency, 0.01f);
			ImGui::DragFloat("shake Magnitude", &shake.shakeMagnitude, 0.01f);
			ImGui::DragFloat("rotation Magnitude", &shake.rotationMagnitude, 0.01f);
			ImGui::DragFloat("_time", &shake.time, 0.01f);
			ImGui::DragFloat("_shakeDuration", &shake.shakeDuration, 0.01f);
			ImGui::DragInt("_seed", &shake.seed);

			ImGui::Checkbox("is shaking", &shake.isShaking);
			ImGui::DragFloat2("_preShakeOffset", &shake.preShakeOffset.x);
			ImGui::DragFloat("_preShakeRotation", &shake.preShakeRotation);
			ImGui::DragFloat("_initialTrauma", &shake.initialTrauma);
			ImGui::DragFloat("_initialDuration", &shake.initialDuration);

		}

		static void InspectSprite(Entity& entity, jam::core::Sprite& c) {

			//editor::RecieveTexturePayload(&c.texture, &sprite.id);
			ImGui::DragFloat4("source", &c.source.x);
			ImGui::DragFloat2("scale", &c.scale.x, 0.01f, 100.0f);
			ImGui::DragFloat2("offset", &c.offset.x, 0.01f, 100.0f);
			ImGui::SliderInt("flipX", &c.flipX, -1, 1);
			ImGui::SliderInt("flipY", &c.flipY, -1, 1);
			jam::editor::EditColor("tint", &c.tint);
		}

		static void InspectSpriteAnimation(Entity& e, jam::core::SpriteAnimation& c) {

			ImGui::Checkbox("loop", &c.loop);
			ImGui::SliderFloat("timer", &c.timer.progress, 0, c.timer.duration);
			ImGui::DragFloat("animation speed", &c.timer.duration, 0.01f);
			ImGui::SliderInt("frame", &c.frame, 0, c.frames.size() - 1);

		}
	*/
}

void jam::editor::inspect_entity_properties(bool* visible, Entity& e)
{
	using namespace jam::components;
	using namespace jam::editor::functions;

	if (!*visible)
		return;

	if (!e.isValid())
		return;

	ImGui::Begin("Entity", visible, ImGuiWindowFlags_None);

	ImGui::AlignTextToFramePadding();
	ImGui::PushItemWidth(-150);

	NameTag& tag = e.get<NameTag>();
	char tagArr[255];
	TextCopy(tagArr, tag.name.c_str());

	ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");


	if (ImGui::BeginPopup("AddComponent"))
	{

		if (ImGui::BeginMenu("Cameras"))
		{
			AddComponentPopup<SceneCamera3D>(e, "Scene Camera");
			AddComponentPopup<CameraEditorComponent>(e, "Camera Editor Component");
			AddComponentPopup<ActiveCameraTag>(e, "Active Camera Tag");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("3D"))
		{
			AddComponentPopup<Transform3D>(e, "Transform 3D");
			AddComponentPopup<MeshInstance3D>(e, "Mesh Instance 3D");
			ImGui::EndMenu();
		}

		//if (ImGui::BeginMenu("Physics"))
		//{
		//	AddComponentPopup<physics::RigidBody>(e, "RigidBody");
		//	AddComponentPopup<physics::BoxCollider>(e, "Box Collider");
		//	AddComponentPopup<physics::BoxArea>(e, "Box Area");
		//	AddComponentPopup<physics::CircleCollider>(e, "Circle Collider");
		//	AddComponentPopup<physics::CircleArea>(e, "Circle Area");
		//	AddComponentPopup<physics::EdgeCollider>(e, "Edge Collider");
		//	AddComponentPopup<physics::ColliderList>(e, "Collider List");

		//	ImGui::EndMenu();
		//}
		//external popups
		for (auto&& [id, func] : ECPFRManager::Get().pop_up_functions) {
			func(e);
		}

		ImGui::EndPopup();
	}

	bool _visible = e.isVisible();
	bool _enabled = e.isEnabled();
	if (ImGui::Checkbox("visible", &_visible))
	{
		if (_visible)
			e.show();
		else
			e.hide();

	}
	ImGui::SameLine();
	if (ImGui::Checkbox("enabled", &_enabled))
	{
		if (_enabled)
			e.enable();
		else
			e.disable();
	}

	RenderEComponentEditWidget<Transform3D>("Transform 3D", e, InspectTransform3D);
	RenderEComponentEditWidget<SceneCamera3D>("Scene Camera 3D", e, InspectSceneCameraComponent);
	RenderEComponentEditWidget<CameraEditorComponent>("Camera Editor", e, InspectCameraEditorComponent);
	RenderEComponentEditWidget<MeshInstance3D>("Mesh Instance 3D", e, InspectMeshInstance3D);
	//external widgets
	for (auto&& [id, func] : ECPFRManager::Get().widget_functions) {
		func(e);
	}
	ImGui::End();
}

#include "entityPropertyEditor.hpp"
#include "jam/modules/core3d/core3d.hpp"
#include "jam/modules/ui/ui.hpp"


#include "editor/editor.hpp"
#include "resource_inspector.hpp"
#include <unordered_map>
#include "editor/imgui/imgui_stdlib.h"
#include "Jam/app/application.hpp"
#include "editor/tools/dragResources.hpp"


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
		if (editor::tools::DragAndDropMeshRecieve(&component.mesh, component.mesh_id, &component.material))
		{
			TraceLog(LOG_INFO, TextFormat("the mesh has changed! %s", component.mesh_id.toString().c_str()));
		}
		// material drag n drop
		ImGui::Text("Material id : %s", component.material_id.toString().c_str());

		// texture drag n drop
		ImGui::Text("Shader id : %s", component.shader_id.toString().c_str());

		InspectMaterial(&component.material, &component.material_id);
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

	static bool InspectTransform2D(Entity& entity, jam::components::Transform2D& component)
	{
		bool moved = ImGui::DragFloat2("position", &component.position.x, 0.1f);
		ImGui::DragFloat2("size", &component.size.x, 0.1f);
		ImGui::DragFloat2("origin", &component.origin.x, 0.1f);
		ImGui::DragFloat("scale", &component.scale, 0.1f);
		ImGui::SliderFloat("angle", &component.angle, -360, 360.0f);

		return moved;
	}
	static void InspectUIText(Entity& entity, jam::components::UIText& component)
	{
		ImGui::Text("Font id : %s", component.id.toString().c_str());
		ImGui::Checkbox("visible", &component.visible);
		ImGui::InputTextMultiline("text", &component.text);
		ImGui::DragFloat2("origin", &component.origin.x, 1.0f);
		ImGui::DragFloat("font size", &component.fontSize, 0.5f);
		ImGui::DragFloat("font spacing", &component.spacing, 0.5f);
		ImGui::SliderFloat("rotation", &component.rotation, -360.0f, 360.0f);
		editor::EditColor("tint", &component.tint);
	}
	
	static const char* npatch_layout_strings[] = { "9 patch", "3 patch vertical", "3 patch horizontal" };

	static void InspectUIPanel(Entity& entity, jam::components::UIPanel& component)
	{
		ImGui::Checkbox("visible", &component.visible);
		ImGui::Text("Texture id : %s", component.id.toString().c_str());
		
		if (ImGui::TreeNodeEx("Npatch", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat4("source", &component.npatch.source.x);
			ImGui::DragInt("left", &component.npatch.left);
			ImGui::DragInt("top", &component.npatch.top);
			ImGui::DragInt("right", &component.npatch.right);
			ImGui::DragInt("bottom", &component.npatch.bottom);
			ImGui::SliderInt("layout", &component.npatch.layout, 0, NPATCH_THREE_PATCH_HORIZONTAL, npatch_layout_strings[component.npatch.layout]);

			ImGui::TreePop();
		}

		editor::EditColor("tint", &component.tint);
	}
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

		if (ImGui::BeginMenu("UI"))
		{
			AddComponentPopup<Transform2D>(e, "Transform 2D");
			AddComponentPopup<UIPanel>(e, "Panel");
			AddComponentPopup<UIText>(e, "Text");
			ImGui::EndMenu();
		}

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
	RenderEComponentEditWidget<Transform2D>("Transform 2D", e, InspectTransform2D);
	RenderEComponentEditWidget<UIPanel>("UI Panel", e, InspectUIPanel);
	RenderEComponentEditWidget<UIText>("UI Text", e, InspectUIText);
	//external widgets
	for (auto&& [id, func] : ECPFRManager::Get().widget_functions) {
		func(e);
	}
	ImGui::End();
}

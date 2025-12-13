#include "modules.hpp"
#include "jam/scene/iScene.hpp"
#include "editor/imgui/imgui_stdlib.h"
#include "editor/editor.hpp"

namespace jam::editor {
	static void Inspect_SceneConfig(SceneConfig* config);
	static void Inspect_Environment(Environment* env);
	static void Inspect_RenderTarget(RenderTarget* renderTarget);

	static bool Inspect_EntityOnTree(Entity e, Entity& ref, Scene* scene);
	static bool Scene_create_entity_type_menu(Scene* scene, Entity& ref);
	static bool Scene_config_menu(Scene* scene);
}

bool jam::editor::renderSceneHierarchy(bool* isVisible, Scene* scene, Entity& selected)
{
	if (!*isVisible)
		return false;
	bool is_selected = false;
	ImGui::Begin("Scene", isVisible);
	ImGui::SeparatorText("config");
	//if (ImGui::TreeNode("config")){
		Inspect_SceneConfig(&scene->config);
		//ImGui::TreePop();}
	ImGui::SeparatorText("viewport");
		Inspect_RenderTarget(&scene->renderTarget);
	ImGui::SeparatorText("environment");
		Inspect_Environment(&scene->worldEnv);

	ImGui::SeparatorText("entities");

	//draw menu when not over entity
	if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_::ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
	{
		if (scene && ImGui::BeginMenu("Entity"))
		{
			is_selected = Scene_create_entity_type_menu(scene, selected);
			ImGui::EndMenu();
		}
		if (scene && ImGui::BeginMenu("Scene"))
		{
			is_selected = Scene_config_menu(scene);

			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}
	//render context menu
	for (auto handle : scene->world.view<entt::entity>())
	{
		Entity entity(handle, scene->world);
		if (entity.hasParent())
			continue;

		is_selected = is_selected || Inspect_EntityOnTree(entity, selected, scene);
	}

	ImGui::End();
	return is_selected;
}



namespace jam::editor {
	void Inspect_SceneConfig(SceneConfig* config)
	{
		ImGui::InputText("Name", &config->name);;
		if (ImGui::Button("Browse Config"))
		{

		}
		ImGui::SameLine();
		ImGui::Text("Resource File: %s", config->configPath.c_str());
		if (ImGui::Button("Browse Script"))
		{

		}
		ImGui::SameLine();
		ImGui::Text("Script File: %s", config->scriptPath.c_str());
		editor::EditColor("background color", &config->backgroundColor);

	}

	void Inspect_Environment(Environment* env)
	{
		if (ImGui::TreeNode("Skybox"))
		{
			ImGui::Checkbox("enabled", &env->skybox.enabled);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("lighting"))
		{
			static Color ambient = WHITE;
			static Vector3 direction = { 1,1,1 };
			editor::EditColor("label##ambient", &ambient);
			ImGui::DragFloat3("label##direction", &direction.x, 0.01f);
			ImGui::TreePop();
		}
	}

	void Inspect_RenderTarget(RenderTarget* t)
	{
		ImGui::Checkbox("enabled", &t->enabled);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("check if the scene should render to the render texture");
		ImGui::Checkbox("visible", &t->visible);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("check if the render texture should render to the screen");

		if (ImGui::DragFloat2("resolution", &t->resolution.x, 1.0f))
			t->Resize(t->resolution);

	}


	bool Inspect_EntityOnTree(Entity entity, Entity& ref, Scene* scene)
	{
		Entity child = entity.getFirstChild();
		bool childValid = child;
		int flags = ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_SpanAvailWidth
			| (ref == entity ? ImGuiTreeNodeFlags_Selected : 0);

		if (childValid == false)
			flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, entity.getName().c_str());
		bool _destroyed = false, _switched = false, _closed = false;

		if (ImGui::IsItemClicked())
		{
			if (ref == entity) {
				_closed = true;
				ref = Entity();
			}
			else {
				_switched = true;
				ref = entity;
			}
		}

		//perhaps... methinks... we should break this oout into a method ?

		// Our buttons are both drag sources and drag targets here!
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			// Set payload to carry the index of our item (could be anything)
			uint32_t e = entity;
			ImGui::SetDragDropPayload("ENTITY", &e, sizeof(uint32_t));

			{ ImGui::Text("Parent %s", entity.getName().c_str()); }
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
			{
				IM_ASSERT(payload->DataSize == sizeof(uint32_t));
				uint32_t payload_entity = *(uint32_t*)payload->Data;

				Entity pChild(entt::entity{ payload_entity }, scene->world);
				//entity.AddChild(pChild);

				pChild.addParent(entity);
			}
			ImGui::EndDragDropTarget();
		}


		if (ImGui::BeginPopupContextItem())
		{
			if (entity.hasParent() && ImGui::MenuItem("Unparent"))
			{
				entity.removeParent();
			}
			if (ImGui::MenuItem("Delete Entity"))
			{
				entity.destroy();
				_destroyed = true;

				if (ref == entity)
				{
					entity = Entity();
					ref = Entity();
				}

			}

			if (ImGui::MenuItem("Save as Prefab"))
			{
				//std::filesystem::path path = file::SaveFile(".yml \0*.yml\0");
				//if (!path.empty())
				//{
				//	glib::SerializeEntityAsPrefabToFile(path.string(), entity, scene);
				//}
			}

			if (ImGui::MenuItem("Duplicate"))
			{
				//ref = entity.createDuplicate(scene);

				//TraceLog(TraceLogLevel::LOG_WARNING, "Setup 'Duplicate' Function ^ _ ^ ");
			}
			ImGui::EndPopup();
		}

		if (opened)
		{
			while (child && !_destroyed)
			{
				bool r = Inspect_EntityOnTree(child, ref, scene);
				if (child) //sometimes if r is true, the child may be destroyed (user selected destroy ddoption)
					child = child.getNextSibling();
			}
			ImGui::TreePop();
		}
		return _destroyed || _switched || _closed;
	}

	bool Scene_create_entity_type_menu(Scene* scene, Entity& e)
	{
		bool res = false;
		if (ImGui::MenuItem("add Entity"))
		{
			e = scene->CreateEntity("Entity", Vector3{0, 0,0 });
			res = true;
		}
		if (ImGui::BeginMenu("Meshes"))
		{
			if (ImGui::MenuItem("add Cube")) { e = scene->CreateCube("Cube", Vector3{ 0,0,0 }); res = true; }
			if (ImGui::MenuItem("add Sphere")) { e = scene->CreateSphere("Sphere", Vector3{ 0,0,0 }); res = true; }
			if (ImGui::MenuItem("add Donut")) { e = scene->CreateDonut("Donut", Vector3{ 0,0,0 }); res = true; }
			if (ImGui::MenuItem("add Plane")) { e = scene->CreatePlane("Plane", Vector3{ 0,0,0 }); res = true; }

			ImGui::EndMenu();
		}

		return res;
	}

	bool Scene_config_menu(Scene* scene)
	{
		return false;
	}
}

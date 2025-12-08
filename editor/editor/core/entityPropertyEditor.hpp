#pragma once
#include <imgui.h>
#include <Jam/core/entity.hpp>
#include <functional>
typedef std::function<void(jam::Entity& e)>   EntityInspectionFunction;

namespace jam::strawberry::editor
{
	namespace functions {
		template<typename T>
		static void AddComponentPopup(jam::Entity& entity, const char* title)
		{
			if (entity.has<T>())
				return;
			if (ImGui::MenuItem(title))
			{
				entity.add<T>();
				ImGui::CloseCurrentPopup();
			}
		}

		template<typename T, typename UIFunction>
		inline void RenderEComponentEditWidget(const char* title, jam::Entity& entity, UIFunction uifunction)
		{
			if (!entity.has<T>())
				return;
			bool removeComponent = false;
			ImVec2 contentRegionAvaliable = ImGui::GetContentRegionAvail();
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
			float lineheight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(),
				ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth,
				title);

			ImGui::SameLine(contentRegionAvaliable.x - lineheight * 0.5f);

			if (ImGui::Button("+", ImVec2(lineheight, lineheight)))
			{
				ImGui::OpenPopup("component Settings");
			}
			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("component Settings"))
			{
				if (ImGui::MenuItem("Remove Component"))
				{
					removeComponent = true;;
				}

				ImGui::EndPopup();
			}

			if (open)
			{

				ImGui::AlignTextToFramePadding();
				ImGui::PushItemWidth(-150);
				uifunction(entity, entity.get<T>());

				ImGui::TreePop();
			}
			if (removeComponent)
			{
				entity.remove<T>();
			}
		}

	}

	/*
		register a method with the internal editor.
		the added function should use the 'AddComponentPopup' function so that the editor will 
		be able to allow the user to add custom components
		should be in this format:

		if (ImGui::BeginMenu("Game"))
		{
			AddComponentPopup<HealthComponent>(e, "Health");
			AddComponentPopup<Hitbox>(e, "Hit box");
			AddComponentPopup<Hurtbox>(e, "Hurt box");
			ImGui::EndMenu();
		}
	*/
	UUID register_component_popups(EntityInspectionFunction func);
	//unregister/remove a popup menu
	void unregister_component_popups(UUID id);


	UUID register_component_editWidget(EntityInspectionFunction func);
	void unregister_component_editWidget(UUID id);


	void inspect_entity_properties(bool* visible, Entity& entity);
}
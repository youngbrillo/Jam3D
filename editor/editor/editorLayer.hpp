#pragma once
#include "editor.hpp"
#include "jam/app/iApplicationLayer.hpp"

namespace jam::editor
{
	struct EditorSettings
	{
		bool visible = true;
		bool demoVisible = true;
		bool toolbarVisible = true;
		bool newFileDialogVisible = true;
		bool exitToWindowsDialogVisible = true;
		bool sceneHeirarchyVisible = true;
		bool entityInspectorVisible = true;
		bool viewPortVisible = true;
		bool consoleVisible = true;
		bool resourceInspectorVisible = true;
		bool assetWindowVisible = true;
	};

	class EditorLayer : public iApplicationLayer
	{
	public:
		EditorLayer();
		~EditorLayer();

		// Inherited via iApplicationLayer
		void onSceneStart() override;
		void onSceneEnd() override;
		void poll() override;
		void update(float dt) override;
		void render() override;
		void serialize(std::string configFilepath);
		void deserialize(std::string configFilepath);

		void AddExternalFunction(ExternalFunction function, void* userdata);
		static EditorLayer* getInstance();

		EditorSettings settings;
		FolderInfo sceneLookup;
	private:
		ExternalFunctionWrapper externals;
		Entity _selected_entity;
	private:
		//void renderToolbar();
		//void renderImguiDemo();
		//void renderAddNewFileDialog();
		//void renderExitToWindowsDialog();
		//void renderSceneHierachy();

		//void renderExternalFunction(ExternalFunctionWrapper* ef);

		//void setViewport(bool isVisible);
		//void renderViewport();
		void on_entity_destroyed(entt::registry& registry, entt::entity handle);
	};
}
#include "editorLayer.hpp"
#include <jam/app/application.hpp>
#include "rlImgui/rlImGui.h"
#include "core/modules.hpp"
#include "jam/serialization/yaml/serializeYAML.hpp"

using namespace jam::editor;

jam::editor::EditorLayer* s_instance = nullptr;

EditorLayer* jam::editor::EditorLayer::getInstance()
{
	return s_instance;
}

jam::editor::EditorLayer::EditorLayer()
	: iApplicationLayer(jam::Application::Instance())
{
	rlImGuiSetup(true);
#ifdef IMGUI_HAS_DOCK
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable;
#endif // 
	deserialize("editor.res.yml");
	sceneLookup.path = "scenes/";
	sceneLookup.refresh(0);
	s_instance = this;

	TraceLog(LOG_WARNING, "[EDITOR LAYER] START ON HEIRARCHY DEBUG VIEW FOR THE ACTIVE SCENE!");

}


jam::editor::EditorLayer::~EditorLayer()
{
	serialize("editor.res.yml");
	rlImGuiShutdown();
	s_instance = nullptr;
}

void jam::editor::EditorLayer::onSceneStart()
{
	_selected_entity = Entity();
	//setViewport(_viewPortVisible);
	sceneRef->world.on_destroy<entt::entity>().connect<&EditorLayer::on_entity_destroyed>(this);
}

void jam::editor::EditorLayer::onSceneEnd()
{
}

void jam::editor::EditorLayer::poll()
{
	auto& app = *jam::Application::Instance();
	auto& project = jam::Application::Instance()->GetProject();
	auto& io = ImGui::GetIO();

	if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyReleased(KEY_R))
	{
		if (IsKeyDown(KEY_LEFT_SHIFT))
			app.SetScene(project.scene.definition);
		else
			app.RestartScene();
	}

	if (IsKeyReleased(KEY_F5))
	{
		app.SetScene(project.scene.definition);
	}

	if (IsKeyReleased(KEY_F1))
	{
		settings.visible = !settings.visible;
	}

	if (IsKeyReleased(KEY_F2))
	{
		settings.toolbarVisible = !settings.toolbarVisible;
	}

	if (settings.visible)
	{
		if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_N))
			settings.newFileDialogVisible = true;
	}

	if (!io.WantCaptureKeyboard  /*IsKeyDown(KEY_LEFT_CONTROL)*/ && IsKeyReleased(KEY_P))
		app.isPaused() ? app.Resume() : app.Pause();
	if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyReleased(KEY_LEFT_BRACKET))
		app.SlowDown();

	if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyReleased(KEY_RIGHT_BRACKET))
	{
		if (IsKeyDown(KEY_LEFT_SHIFT))
			app.FastForwardx4();
		else
			app.FastForwardx2();
	}

}

void jam::editor::EditorLayer::update(float dt)
{
}

void jam::editor::EditorLayer::render()
{
	if (!settings.visible)
		return;

	rlImGuiBegin();

#ifdef IMGUI_HAS_DOCK
	ImGui::DockSpaceOverViewport(0, NULL, ImGuiDockNodeFlags_PassthruCentralNode); // set ImGuiDockNodeFlags_PassthruCentralNode so that we can see the raylib contents behind the dockspace
#endif
	if (editor::renderToolBar(*this, settings, sceneRef)){

	}

	if (editor::renderResourceInspector(*this, settings, sceneRef)){

	}


	if (settings.demoVisible)
		ImGui::ShowDemoWindow(&settings.demoVisible);

	rlImGuiEnd();
}

void jam::editor::EditorLayer::serialize(std::string configFilepath)
{
	YAML::Emitter out;
	out
		<< YAML::BeginMap
		<< YAML::Key << "visible" << YAML::Value << this->settings.visible
		<< YAML::Key << "toolbarVisible" << YAML::Value << this->settings.toolbarVisible
		<< YAML::Key << "demoVisible" << YAML::Value << this->settings.demoVisible
		<< YAML::Key << "sceneHeirarchyVisible" << YAML::Value << this->settings.sceneHeirarchyVisible
		<< YAML::Key << "entityInspectorVisible" << YAML::Value << this->settings.entityInspectorVisible
		<< YAML::Key << "viewPortVisible" << YAML::Value << this->settings.viewPortVisible
		<< YAML::Key << "consoleVisible" << YAML::Value << this->settings.consoleVisible
		<< YAML::EndMap;
	SaveYamlFile(configFilepath, out);
}

void jam::editor::EditorLayer::deserialize(std::string configFilepath)
{
	YAML::Node root = LoadYamlFile(configFilepath);
	if (root)
	{
		readValueEx(root["visible"], &settings.visible);
		readValueEx(root["toolbarVisible"], &settings.toolbarVisible);
		readValueEx(root["demoVisible"], &settings.demoVisible);
		readValueEx(root["sceneHeirarchyVisible"], &settings.sceneHeirarchyVisible);
		readValueEx(root["entityInspectorVisible"], &settings.entityInspectorVisible);
		readValueEx(root["viewPortVisible"], &settings.viewPortVisible);
		readValueEx(root["consoleVisible"], &settings.consoleVisible);
	}
}

void jam::editor::EditorLayer::AddExternalFunction(ExternalFunction function, void* userdata)
{
}

void jam::editor::EditorLayer::on_entity_destroyed(entt::registry& registry, entt::entity handle)
{
}

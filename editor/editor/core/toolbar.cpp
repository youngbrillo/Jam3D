#include "modules.hpp"
#include "jam/app/application.hpp"
#include "jam/serialization/yaml/serializeScene.hpp"
#include "editor/tools/fileTools.hpp"

namespace fs = std::filesystem;

#define NAV_TOGGLE_OPTION_FUNCTION(name, value) if (ImGui::MenuItem(name, nullptr, value)) { value= !value; }

static const char* time_state_strings[] = {
	"Paused",
	"Play",
	"Slow-mode",
	"FFD x2",
	"FFD x4",
	"Custom playback speed"
};
static void render_scene_menu_files(jam::editor::FolderInfo & info, jam::Application & app, jam::Project & project);

static std::vector<StringPair> sceneFileExtensions =
{
	{"Resource", "yaml"},
	{"Res", "yml"},
	{"Scenes", "scene"},
};

bool jam::editor::renderToolBar(jam::editor::EditorLayer& layer, jam::editor::EditorSettings& s, Scene * scene)
{
	if (!s.toolbarVisible)
		return false;

	auto& app = *jam::Application::Instance();
	auto& project = jam::Application::Instance()->GetProject();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Quit", "ALT+F4"))
			{
				project.running = false;
			}
			if (ImGui::MenuItem("Reload App", nullptr, project.reloadApp))
			{
				project.reloadApp = !project.reloadApp;
			}
			if (ImGui::MenuItem("Refresh", "CTRL+R")) {
				app.RestartScene();
			}
			if (ImGui::MenuItem("Restart", "F5")) {
				app.SetScene(project.scene.definition);
			}

			if (ImGui::MenuItem("Save Scene", "CTRL+LSHIFT+S"))
			{
				//std::string saveLocation = tools::ShowSaveFileDialog(".yaml");
				//scene->Serialize();
			}
			if (ImGui::MenuItem("Open Scene", "CTRL+LSHIFT+O"))
			{
				std::string saveLocation = tools::ShowOpenFileDialog(".yaml");
				if (!saveLocation.empty())
				{
					scene->config.configPath = saveLocation;
					app.SetScene(scene->config);
				}

			}

			if (ImGui::MenuItem("New Scene", "CTRL+N"))
			{
				s.newFileDialogVisible = true;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::BeginMenu("Application"))
			{

				if (ImGui::BeginMenu("Time"))
				{
					if (ImGui::MenuItem("Pause", "CTRL+P", app.GetTime().state == TIME_STATE::TIME_STATE_PAUSED))
						app.Pause();

					if (ImGui::MenuItem("Resume", "CTRL+P", app.GetTime().state == TIME_STATE::TIME_STATE_PLAY))
						app.Pause();

					if (ImGui::MenuItem("Slow down", "CTRL+[", app.GetTime().state == TIME_STATE::TIME_STATE_HALF))
						app.SlowDown();

					if (ImGui::MenuItem("Fast Forward x2", "CTRL+]", app.GetTime().state == TIME_STATE::TIME_STATE_FAST_FORWARD_X2))
						app.FastForwardx2();

					if (ImGui::MenuItem("Fast Forward x4", "CTRL+SHIFT+]", app.GetTime().state == TIME_STATE::TIME_STATE_FAST_FORWARD_X4))
						app.FastForwardx4();

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem("Set Monitor to current monitor", nullptr, project.window.monitor == GetCurrentMonitor()))
				{
					project.window.monitor = GetCurrentMonitor();
				}
				if (ImGui::MenuItem("Set Window Size"))
				{
					project.window.width = GetScreenWidth();
					project.window.height = GetScreenHeight();
				}
				if (ImGui::MenuItem("Lock FPS", "", project.window.lockFPS))
				{
					project.window.lockFPS = !project.window.lockFPS;
				}
				if (ImGui::MenuItem("Override Pause", nullptr, project.scene.overridePause)) {
					project.scene.overridePause = !project.scene.overridePause;
				}

				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::BeginMenu("Editor"))
			{
				NAV_TOGGLE_OPTION_FUNCTION("visible", s.visible)
				NAV_TOGGLE_OPTION_FUNCTION("toolbar", s.toolbarVisible)
				NAV_TOGGLE_OPTION_FUNCTION("Demo", s.demoVisible)
				NAV_TOGGLE_OPTION_FUNCTION("Scene Heirarchy", s.sceneHeirarchyVisible)
				NAV_TOGGLE_OPTION_FUNCTION("Viewport", s.viewPortVisible)
				NAV_TOGGLE_OPTION_FUNCTION("Entity Properties", s.entityInspectorVisible)
				NAV_TOGGLE_OPTION_FUNCTION("Console", s.consoleVisible)
				NAV_TOGGLE_OPTION_FUNCTION("Resources", s.resourceInspectorVisible)
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Scenes"))
		{
			if (ImGui::BeginMenu("Templates"))
			{
				auto& templates = app.GetTemplates();
				for (int i = 0; i < templates.size(); i++)
				{
					if (ImGui::MenuItem(templates[i].name.c_str(), nullptr, i == project.scene.activeTemplate))
					{
						app.SetDefaultTemplate(i);
						app.ReloadScene();
					}
				}
				ImGui::EndMenu();
			}
			ImGui::Separator();

			render_scene_menu_files(layer.sceneLookup, app, project);


			ImGui::EndMenu();
		}
		if (app.GetTime().state != TIME_STATE_PLAY) {
			ImGui::SameLine(ImGui::GetWindowWidth() / 2 - (ImGui::CalcTextSize(time_state_strings[app.GetTime().state]).x * 0.5f));
			ImGui::Text(time_state_strings[app.GetTime().state]);
		}
		ImGui::SameLine(ImGui::GetWindowWidth() - 200);
		ImGui::Text("%.3f ms/frame (%04.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::EndMainMenuBar();
	}

	return false;
}

static void render_scene_menu_files(jam::editor::FolderInfo& info, jam::Application& app, jam::Project& project)
{
	for (fs::path& file : info.files)
	{
		std::string file_name = file.stem().string();
		std::string file_path = file.string();
		if (ImGui::MenuItem(file_name.c_str(), nullptr, file_path == project.scene.definition.configPath))
		{
			jam::SceneConfig settings;
			settings.name = file_name;
			settings.configPath = file_path;
			auto root = jam::LoadYamlFile(settings.configPath);
			jam::serialization::DeserializeSceneConfig(root["scene"], settings);
			if (settings.id != 0)
			{
				app.SetScene(settings);
			}
		}
	}
}
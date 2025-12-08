#include "application.hpp"

using namespace jam;
static jam::Application* s_instance = nullptr;

jam::Application::Application(Project& projectSettings)
	: project(projectSettings)
	, activeScene(nullptr)
    , defaultLayer(nullptr)
{
    assert(s_instance == nullptr && "ONLY ONE APPLICATION ALLOWED AT A TIME");

    project.reloadApp = false;
    project.running = true;

    SetConfigFlags(project.window.configFlags);
    InitWindow(
        project.window.width,
        project.window.height,
        TextFormat("%s v.%d.%d.%d", project.name.c_str(), project.version_major, project.version_minor, project.version_patch)
    );
    SetWindowMonitor(project.window.monitor);
    SetWindowSize(project.window.width, project.window.height);
    if (project.window.lockFPS)
    {
        SetTargetFPS(project.window.targetFPS);
    }

    InitAudioDevice();
    SetMasterVolume(project.audio.masterVolume);
    project.onWindowInit();
    s_instance = this;
    time.fixedTimeStep = project.physics.hertz;
}

jam::Application::~Application()
{
    s_instance = nullptr;

    if (activeScene) {
        activeScene->End();
        delete activeScene;
        activeScene = nullptr;
    }
    if (defaultLayer)
    {
        delete defaultLayer;
        defaultLayer = nullptr;
    }
    CloseAudioDevice();
    CloseWindow();
}

Application* jam::Application::Instance()
{
	return s_instance;
}

void jam::Application::Run()
{
    while (project.running)
    {
        time.deltaTime = GetFrameTime();
        time.time += time.deltaTime;
        time.frameCount++;
        time.deltaTime *= time.playbackSpeed;

        poll();
        update();
        render();
    }
}

void jam::Application::SetDefaultTemplate(int id)
{
    if (id >= 0 && id < templates.size())
        this->project.scene.activeTemplate = id;
}

int jam::Application::AddTemplate(SceneTemplate temp)
{
    templates.emplace_back(temp);

    return templates.size() - 1;
}

void jam::Application::SetDefaultLayer(iApplicationLayer* layer)
{
    if (defaultLayer)
    {
        defaultLayer->FreeScene();
        delete defaultLayer;
        defaultLayer = nullptr;
    }

    defaultLayer = layer;
    if (activeScene)
        defaultLayer->GetScene(activeScene);
}

void jam::Application::SetScene(jam::SceneConfig def)
{
    project.scene.definition = def;
    project.scene.reloadScene = true;
}
void jam::Application::RestartScene()
{
    project.scene.resetScene = true;
}
void jam::Application::ReloadScene()
{
    project.scene.reloadScene = true;
}
const Clock& jam::Application::GetTime()
{
    return time;
}
bool jam::Application::isPaused() const
{
    return time.state == TIME_STATE_PAUSED;
}
void jam::Application::Pause()
{
    time.SetState(TIME_STATE_PAUSED);
}
void jam::Application::Resume()
{
    time.SetState(TIME_STATE_PLAY);
}
void jam::Application::FastForwardx2()
{
    time.SetState(TIME_STATE_FAST_FORWARD_X2);
}
void jam::Application::FastForwardx4()
{
    time.SetState(TIME_STATE_FAST_FORWARD_X4);
}
void jam::Application::SlowDown()
{
    time.SetState(TIME_STATE_HALF);
}
void jam::Application::SetPlaybackSpeed(float value)
{
    value = value < 0 ? 0 : value;
    time.SetState(TIME_STATE_CUSTOM, value);
}
iScene* jam::Application::getSceneTemplate(int slot)
{
    if (slot < 0 || slot >= templates.size())
        slot = project.scene.activeTemplate;
    //return templates[project.scene.activeTemplate].generator(project.scene.definition);;

    project.scene.currentTemplate = slot;
    return templates[slot].generator(project.scene.definition);
}

void jam::Application::refreshScene()
{
    project.scene.resetScene = false;
    TraceLog(TraceLogLevel::LOG_INFO, "[App]\tReloading Scene!");
    if (activeScene)
    {
        if (defaultLayer) defaultLayer->FreeScene();
        activeScene->End();
        activeScene->Begin();
        project.scene.definition = activeScene->config;
        if (defaultLayer) defaultLayer->GetScene(activeScene);
    }
}

void jam::Application::reloadScene()
{
    project.scene.reloadScene = false;
    if (activeScene)
    {
        TraceLog(TraceLogLevel::LOG_INFO, "[App]\tDestroying Scene!\t %s", activeScene->config.name.c_str());
        if (defaultLayer) defaultLayer->FreeScene();
        activeScene->End();
        delete activeScene;
        activeScene = nullptr;
    }
    TraceLog(TraceLogLevel::LOG_INFO, "[App]\tCreating Scene!\t %s", project.scene.definition.name.c_str());
    assert(templates.size() > 0 && "There must be atleast one template for the application!");
    activeScene = getSceneTemplate(project.scene.definition.templateId);
    activeScene->Begin();
    project.scene.definition = activeScene->config;
    if (defaultLayer) defaultLayer->GetScene(activeScene);
}

void jam::Application::poll()
{
    if (WindowShouldClose())
    {
        project.running = false;
    }
    if (defaultLayer) defaultLayer->poll();

    if (project.scene.resetScene)
        refreshScene();
    if (project.scene.reloadScene)
        reloadScene();

    if (activeScene) activeScene->Poll();
}

void jam::Application::update()
{
    if (defaultLayer) defaultLayer->update(time.deltaTime);

    //perhaps let the user override this with project settings?
    if (isPaused() && !project.scene.overridePause)
        return;

    if (activeScene) activeScene->Update(time);

    time.fixedTimer += time.deltaTime;
    if (time.fixedTimer>= time.fixedTimeStep)
    {
        time.fixedTimer = 0.0f;
        if (activeScene) activeScene->FixedUpdate(time);

    }
}

void jam::Application::render()
{
    BeginDrawing();
    ClearBackground(activeScene ? project.scene.drawScene == false ? BLACK : activeScene->config.backgroundColor : BLACK);
    if (activeScene && project.scene.drawScene)
    {
        activeScene->Render();
    }
    if (defaultLayer) defaultLayer->render();
    EndDrawing();
}

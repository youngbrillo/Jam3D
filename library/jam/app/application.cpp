#include "application.hpp"

using namespace jam;
static jam::Application* s_instance = nullptr;

jam::Application::Application(Project& projectSettings)
	: _project(projectSettings)
	, _active_scene(nullptr)
    , _default_layer(nullptr)
{
    assert(s_instance == nullptr && "ONLY ONE APPLICATION ALLOWED AT A TIME");

    _project.reloadApp = false;
    _project.running = true;

    SetConfigFlags(_project.window.configFlags);
    InitWindow(
        _project.window.width,
        _project.window.height,
        TextFormat("%s v.%d.%d.%d", _project.name.c_str(), _project.version_major, _project.version_minor, _project.version_patch)
    );
    SetWindowMonitor(_project.window.monitor);
    SetWindowSize(_project.window.width, _project.window.height);
    if (_project.window.lockFPS)
    {
        SetTargetFPS(_project.window.targetFPS);
    }

    InitAudioDevice();
    SetMasterVolume(_project.audio.masterVolume);
    _project.onWindowInit();
    s_instance = this;
    _time.fixedTimeStep = _project.physics.hertz;
}

jam::Application::~Application()
{
    s_instance = nullptr;

    if (_active_scene) {
        _active_scene->End();
        delete _active_scene;
        _active_scene = nullptr;
    }
    if (_default_layer)
    {
        delete _default_layer;
        _default_layer = nullptr;
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
    while (_project.running)
    {
        _time.deltaTime = GetFrameTime();
        _time.time += _time.deltaTime;
        _time.frameCount++;
        _time.deltaTime *= _time.playbackSpeed;

        poll();
        update();
        render();
    }
}

void jam::Application::SetDefaultTemplate(int id)
{
    if (id >= 0 && id < _scene_templates.size())
        this->_project.scene.activeTemplate = id;
}

int jam::Application::AddTemplate(SceneTemplate temp)
{
    _scene_templates.emplace_back(temp);

    return _scene_templates.size() - 1;
}

void jam::Application::SetDefaultLayer(iApplicationLayer* layer)
{
    if (_default_layer)
    {
        _default_layer->FreeScene();
        delete _default_layer;
        _default_layer = nullptr;
    }

    _default_layer = layer;
    if (_active_scene)
        _default_layer->GetScene(_active_scene);
}

void jam::Application::SetScene(jam::SceneConfig def)
{
    _project.scene.definition = def;
    _project.scene.reloadScene = true;
}
void jam::Application::RestartScene()
{
    _project.scene.resetScene = true;
}
void jam::Application::ReloadScene()
{
    _project.scene.reloadScene = true;
}
const Clock& jam::Application::GetTime()
{
    return _time;
}
bool jam::Application::isPaused() const
{
    return _time.state == TIME_STATE_PAUSED;
}
void jam::Application::Pause()
{
    _time.SetState(TIME_STATE_PAUSED);
}
void jam::Application::Resume()
{
    _time.SetState(TIME_STATE_PLAY);
}
void jam::Application::FastForwardx2()
{
    _time.SetState(TIME_STATE_FAST_FORWARD_X2);
}
void jam::Application::FastForwardx4()
{
    _time.SetState(TIME_STATE_FAST_FORWARD_X4);
}
void jam::Application::SlowDown()
{
    _time.SetState(TIME_STATE_HALF);
}
void jam::Application::SetPlaybackSpeed(float value)
{
    value = value < 0 ? 0 : value;
    _time.SetState(TIME_STATE_CUSTOM, value);
}
Scene* jam::Application::getSceneTemplate(int slot)
{
    if (slot < 0 || slot >= _scene_templates.size())
        slot = _project.scene.activeTemplate;
    //return templates[project.scene.activeTemplate].generator(project.scene.definition);;

    _project.scene.currentTemplate = slot;
    return _scene_templates[slot].generator(_project.scene.definition);
}

void jam::Application::refreshScene()
{
    _project.scene.resetScene = false;
    TraceLog(TraceLogLevel::LOG_INFO, "[App]\tReloading Scene!");
    if (_active_scene)
    {
        if (_default_layer) _default_layer->FreeScene();
        _active_scene->End();
        _active_scene->Begin();
        _project.scene.definition = _active_scene->config;
        if (_default_layer) _default_layer->GetScene(_active_scene);
    }
}

void jam::Application::reloadScene()
{
    _project.scene.reloadScene = false;
    if (_active_scene)
    {
        TraceLog(TraceLogLevel::LOG_INFO, "[App]\tDestroying Scene!\t %s", _active_scene->config.name.c_str());
        if (_default_layer) _default_layer->FreeScene();
        _active_scene->End();
        delete _active_scene;
        _active_scene = nullptr;
    }
    TraceLog(TraceLogLevel::LOG_INFO, "[App]\tCreating Scene!\t %s", _project.scene.definition.name.c_str());
    assert(_scene_templates.size() > 0 && "There must be atleast one template for the application!");
    _active_scene = getSceneTemplate(_project.scene.definition.templateId);
    _active_scene->Begin();
    _project.scene.definition = _active_scene->config;
    if (_default_layer) _default_layer->GetScene(_active_scene);
}

void jam::Application::poll()
{
    if (WindowShouldClose())
    {
        _project.running = false;
    }
    if (_default_layer) _default_layer->poll();

    if (_project.scene.resetScene)
        refreshScene();
    if (_project.scene.reloadScene)
        reloadScene();

    if (_active_scene) _active_scene->Poll();
}

void jam::Application::update()
{
    if (_default_layer) _default_layer->update(_time.deltaTime);

    //perhaps let the user override this with project settings?
    if (isPaused() && !_project.scene.overridePause)
        return;

    if (_active_scene) _active_scene->Update(_time);

    _time.fixedTimer += _time.deltaTime;
    if (_time.fixedTimer>= _time.fixedTimeStep)
    {
        _time.fixedTimer = 0.0f;
        if (_active_scene) _active_scene->FixedUpdate(_time);

    }
}

void jam::Application::render()
{
    BeginDrawing();
    ClearBackground(_active_scene ? _project.scene.drawScene == false ? BLACK : _active_scene->config.backgroundColor : BLACK);
    if (_active_scene && _project.scene.drawScene)
    {
        _active_scene->Render();
    }
    if (_default_layer) _default_layer->render();
    EndDrawing();
}

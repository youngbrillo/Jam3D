#include "iScene.hpp"
#include "jam/modules/core3d/core3d.hpp"
#include "jam/modules/ui/ui.hpp"
#include "jam/serialization/yaml/serializeScene.hpp"
#include "jam/core/resourceManager.hpp"

#define _MAX(a, b) ((a)>(b)? (a) : (b))
#define _MIN(a, b) ((a)<(b)? (a) : (b))

using namespace jam;
using namespace jam::components;

jam::Scene::Scene(SceneConfig config_)
    : config(config_)
    , renderTarget(config_.resolution)
{
}

jam::Scene::~Scene()
{
}

void jam::Scene::Begin()
{
    YAML::Node root = LoadYamlFile(config.configPath);
    if (root)
    {
        serialization::DeserializeScene(root, this);
        this->onDeserialize(root);
    }

    this->onBegin();
}

void jam::Scene::End()
{
    this->onEnd();

    //recycle editor camera if editor camera is used...
    components::EditorCamera temp;
    bool useTemp = false;
    if (world.ctx().contains<components::EditorCamera>())
    {
        temp = world.ctx().get<components::EditorCamera>();
        useTemp = true;
    }
    world.clear();
    world = entt::registry();

    if (useTemp)
    {
        world.ctx().emplace<components::EditorCamera>(temp);
    }
}

void jam::Scene::Poll()
{
    this->onPoll();
}

void jam::Scene::Update(const jam::Clock& time)
{

    this->onUpdate(time);

    auto culled = world.view<DeleteTag>();
    world.destroy(culled.begin(), culled.end());
}

void jam::Scene::FixedUpdate(const jam::Clock& time)
{
    this->onFixedUpdate(time);
}

void jam::Scene::Render()
{
    if (renderTarget.enabled)
    {
        this->RenderToCanvas();
        renderTarget.Render();
    }
    else
    {
        this->RenderContent();
    }
}

void jam::Scene::RenderContent()
{
    using namespace jam::components;

    Camera3D& cam = GetCamera();
    BeginMode3D(cam);
    this->onRender3DStart(cam);

    DrawGrid(50, 1.0f);

    auto mesh_view = world.view<Transform3D, MeshInstance3D>(entt::exclude<HiddenTag>);

    for (auto&& [id, transform, mesh] : mesh_view.each())
    {
        mesh.Render(transform.toMatrix());
    }

    this->onRender3DEnd(cam);
    EndMode3D();

    auto panel_view = world.view<Transform2D, UIPanel>();
    auto text_view = world.view<Transform2D, UIText>();

    for (auto&& [id, transform, drawable] : panel_view.each())
    {
        drawable.Render(transform);
    }

    for (auto&& [id, transform, drawable] : text_view.each())
    {
        drawable.Render(transform.position, transform.angle);
    }

    this->onRenderUI(cam);
}


void jam::Scene::RenderToCanvas()
{
    renderTarget.Begin(this->config.backgroundColor);
    RenderContent();
    renderTarget.End();
}

void jam::Scene::Serialize()
{
}

void jam::Scene::Deserialize()
{
}

void jam::Scene::onSerialize(YAML::Emitter& out)
{
}

void jam::Scene::onDeserialize(const YAML::Node& node)
{
}

Entity jam::Scene::CreateEntity(std::string name, Vector3 position)
{
    Entity e(world.create(), world);

    e.add<NameTag>().name = name;
    e.add<IDTag>().tag = UUID();
    e.add<Relationship>();
    e.add<Transform3D>(position);


    return e;
}

Entity jam::Scene::CreateCube(std::string name, Vector3)
{
    return Entity();
}

Entity jam::Scene::CreateSphere(std::string name, Vector3)
{
    return Entity();
}

Entity jam::Scene::CreatePlane(std::string name, Vector3)
{
    return Entity();
}

Entity jam::Scene::CreateDonut(std::string name, Vector3)
{
    return Entity();
}


Entity jam::Scene::CreateCamera(std::string name, Vector3 position, float fov)
{
    Entity e = CreateEntity(name, position);
    e.add<SceneCamera3D>().camera.fovy = fov;
    return e;
}

Entity jam::Scene::CreateEditorCamera(std::string name, Vector3 position, float fov)
{
    Entity e = CreateCamera(name, position, fov);
    e.add<ActiveCameraTag>();
    e.add<CameraEditorComponent>();

    return e;
}

Camera3D& jam::Scene::GetCamera()
{
    for (auto&& [e, cam, tag] : world.view<SceneCamera3D, ActiveCameraTag>(entt::exclude<DisabledTag>).each())
    {
        return cam.camera;
    }

    if (!world.ctx().contains<EditorCamera>())
    {
        world.ctx().emplace<EditorCamera>();
    }
    EditorCamera& cam = world.ctx().emplace<EditorCamera>();
    cam.Update();
    return cam.camera.camera;
}

void jam::Scene::SetResolution(Vector2 resoulution)
{
    config.resolution = resoulution;
    renderTarget.Resize(resoulution);
}

Vector2 jam::Scene::GetRenderDimensions() const
{
    if (renderTarget.enabled)
        return renderTarget.resolution;
    return Vector2{(float)GetScreenWidth(), (float)GetScreenHeight()};
}

Entity jam::Scene::GetEntityByName(std::string name)
{
    for (auto&& [id, tag] : world.view<components::NameTag>().each())
    {
        if (tag.name == name)
            return Entity(id, world);
    }
    return Entity();
}

jam::RenderTarget::RenderTarget()
{
    target = LoadRenderTexture((int)resolution.x, (int)resolution.y);
    HandleScreenResize();

}

jam::RenderTarget::RenderTarget(Vector2 Resolution)
    : resolution(Resolution)
{
    target = LoadRenderTexture((int)resolution.x, (int)resolution.y);
    HandleScreenResize();

}

jam::RenderTarget::~RenderTarget()
{
    UnloadRenderTexture(target);
}


void jam::RenderTarget::Resize(Vector2 resolution)
{
    UnloadRenderTexture(target);
    target = LoadRenderTexture((int)resolution.x, (int)resolution.y);
    HandleScreenResize();
}

void jam::RenderTarget::HandleScreenResize()
{
    scale = _MIN((float)GetScreenWidth() / resolution.x, (float)GetScreenHeight() / resolution.y);
}

void jam::RenderTarget::Update()
{
    if (!enabled)
        return;

    Vector2 mouse = GetMousePosition();
    virtualMousePosition.x  = (mouse.x - (GetScreenWidth()   - (resolution.x * scale)) * 0.5f) / scale;
    virtualMousePosition.y  = (mouse.y - (GetScreenHeight()  - (resolution.y * scale)) * 0.5f) / scale;
    virtualMousePosition    = Vector2Clamp(virtualMousePosition, Vector2{ 0, 0 }, Vector2{ resolution.x, resolution.y });

}

void jam::RenderTarget::Render() const
{
    if (!visible || !enabled)
        return;
    // Draw render texture to screen, properly scaled
    DrawTexturePro(
        target.texture,
        Rectangle{ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
        Rectangle{
            (GetScreenWidth() - (resolution.x * scale)) * 0.5f,
            (GetScreenHeight() - (resolution.y * scale)) * 0.5f,
            resolution.x * scale,
            resolution.y * scale,
        },
        Vector2{ 0, 0 },
        0.0f,
        WHITE
        );
}

void jam::RenderTarget::Begin(Color backgroundColor) const
{
    if (enabled) {
        BeginTextureMode(target);
        ClearBackground(backgroundColor);  // Clear render texture background color
    }
}

void jam::RenderTarget::End() const
{
    if(enabled)
        EndTextureMode();
}

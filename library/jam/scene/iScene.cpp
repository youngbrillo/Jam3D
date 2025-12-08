#include "iScene.hpp"
//#include "Jam/components/base.hpp"
#include "jam/modules/core3d/core3d.hpp"

#include "Jam/utils/resourceLoader.hpp"
using namespace jam;
using namespace jam::components;
using namespace jam::utils;

#define _MAX(a, b) ((a)>(b)? (a) : (b))
#define _MIN(a, b) ((a)<(b)? (a) : (b))

static void on_texture_resource_removed(entt::registry& registry, entt::entity e)
{
    Texture2D texture = registry.get<Texture2D>(e);
    UnloadTexture(texture);
}

static void on_font_resource_removed(entt::registry& registry, entt::entity e)
{
    Font res = registry.get<Font>(e);
    UnloadFont(res);
}

static void on_sound_resource_removed(entt::registry& registry, entt::entity e)
{
    Sound res = registry.get<Sound>(e);
    UnloadSound(res);
}

static void on_music_resource_removed(entt::registry& registry, entt::entity e)
{
    Music res = registry.get<Music>(e);
    UnloadMusicStream(res);
}

jam::iScene::iScene(SceneConfig config_)
    : config(config_)
    , renderTarget(config_.resolution)
{
    resources.on_destroy<Texture2D>().connect<&on_texture_resource_removed>();
    resources.on_destroy<Font>().connect<&on_font_resource_removed>();
    resources.on_destroy<Sound>().connect<&on_sound_resource_removed>();
    resources.on_destroy<Music>().connect<&on_music_resource_removed>();

}

jam::iScene::~iScene()
{
    resources.clear();
}

Entity jam::iScene::CreateEntity(std::string name, Vector3 position)
{
    Entity e(world.create(), world);

    e.add<NameTag>().name = name;
    e.add<IDTag>().tag = UUID();
    e.add<Relationship>();
    e.add<Transform3D>(position);


    return e;
}

Entity jam::iScene::CreateCube(std::string name, Vector3)
{
    return Entity();
}

Entity jam::iScene::CreateSphere(std::string name, Vector3)
{
    return Entity();
}

Entity jam::iScene::CreatePlane(std::string name, Vector3)
{
    return Entity();
}

Entity jam::iScene::CreateDonut(std::string name, Vector3)
{
    return Entity();
}


Entity jam::iScene::CreateCamera(std::string name, Vector3 position, float fov)
{
    Entity e = CreateEntity(name, position);
    e.add<SceneCamera3D>().camera.fovy = fov;
    return e;
}

Entity jam::iScene::CreateEditorCamera(std::string name, Vector3 position, float fov)
{
    Entity e = CreateCamera(name, position, fov);
    e.add<ActiveCameraTag>();
    e.add<CameraEditorComponent>();

    return e;
}

UUID jam::iScene::AddTexture(std::string filepath, UUID id)
{
    return AddSceneResource<Texture2D>(resources, filepath, LoadTexture, IsTextureValid, id);
}

Texture2D jam::iScene::GetTexture(UUID id)
{
    return GetSceneResource<Texture2D>(resources, id, Texture2D{ 1,1,1 });

}

UUID jam::iScene::AddFont(std::string filepath, UUID id)
{
    return AddSceneResource<Font>(resources, filepath, LoadFont, IsFontValid, id);
}

Font jam::iScene::GetFont(UUID id)
{
    return GetSceneResource<Font>(resources, id, GetFontDefault());
}

UUID jam::iScene::AddSound(std::string filepath, UUID id)
{
    return AddSceneResource<Sound>(resources, filepath, LoadSound, IsSoundValid, id);
}

Sound jam::iScene::GetSound(UUID id)
{
    return GetSceneResource<Sound>(resources, id, Sound());
}

UUID jam::iScene::AddMusic(std::string filepath, UUID id)
{
    return AddSceneResource<Music>(resources, filepath, LoadMusicStream, IsMusicValid, id);
}

Music jam::iScene::GetMusic(UUID id)
{
    return GetSceneResource<Music>(resources, id, Music());
}
Camera3D& jam::iScene::GetCamera()
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

void jam::iScene::SetResolution(Vector2 resoulution)
{
    config.resolution = resoulution;
    renderTarget.Resize(resoulution);
}

Vector2 jam::iScene::GetRenderDimensions() const
{
    if (renderTarget.enabled)
        return renderTarget.resolution;
    return Vector2{(float)GetScreenWidth(), (float)GetScreenHeight()};
}

Entity jam::iScene::GetEntityByName(std::string name)
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

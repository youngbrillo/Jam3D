#pragma once
#include "Jam/core/entity.hpp"
#include "Jam/core/system.hpp"
#include "jam/core/clock.hpp"
#include <raylib.h>
#include <functional>

namespace jam
{
struct SceneConfig
{
 	int templateId = 0;
 	UUID id = 0;
 	std::string name;
 	std::string configPath;
 	std::string scriptPath;
 	Color backgroundColor = BLACK;
 	Vector2 resolution = { 1280, 720 };
};

struct RenderTarget {
    Vector2 resolution = { 1280, 720 };
    Vector2 virtualMousePosition = { 0,0 };
    bool enabled = false;
    bool visible = false;
    float scale = 1.0f;
    RenderTexture target;

    RenderTarget();
    RenderTarget(Vector2 Resolution);
    ~RenderTarget();
    void Resize(Vector2 Resolution);
    void HandleScreenResize();
    void Update();
    void Render() const;
    bool Enabled() const { return enabled; }
    void Begin(Color backgroundColor = BLACK) const;
    void End() const;
};

class iScene
{
public:
 	SceneConfig config;
 	//Canvas canvas;
    SystemManager systemManager;
    entt::registry world;
 	entt::registry resources;
 	entt::dispatcher eventDispatch;
    RenderTarget renderTarget;
public:
 	iScene(SceneConfig config);
 	virtual ~iScene();

 	virtual void Begin() = 0;
 	virtual void End() = 0;
 	virtual void Poll() = 0;
 	virtual void Update(const jam::Clock& time) = 0;
 	virtual void FixedUpdate(const jam::Clock& time) = 0;
 	virtual void Render() = 0;
 	//virtual void RenderContent() = 0;
 	//virtual void RenderToCanvas();
 	virtual void Serialize() = 0;		//save to file or stream
 	virtual void Deserialize() = 0;		//load from file or stream
public: //entity functions (short cuts)
 	virtual Entity CreateEntity(std::string name, Vector3 position);
        virtual Entity CreateCube(std::string name, Vector3);
        virtual Entity CreateSphere(std::string name, Vector3);
        virtual Entity CreatePlane(std::string name, Vector3);
        virtual Entity CreateDonut(std::string name, Vector3);
        virtual Entity CreateCamera(std::string name, Vector3, float fov = 45);
        virtual Entity CreateEditorCamera(std::string name, Vector3, float fov = 45);
public: // resource functions
 	virtual UUID AddTexture(std::string filepath, UUID id = 0);
 	virtual Texture2D GetTexture(UUID id);

 	virtual UUID AddFont(std::string filepath, UUID id = 0);
 	virtual Font GetFont(UUID id);

 	virtual UUID AddSound(std::string filepath, UUID id = 0);
 	virtual Sound GetSound(UUID id);

 	virtual UUID AddMusic(std::string filepath, UUID id = 0);
 	virtual Music GetMusic(UUID id);
public: //helper functions
 	virtual Camera3D& GetCamera();
 	virtual void SetResolution(Vector2 resoulution);
    Vector2 GetRenderDimensions() const;

    virtual Entity GetEntityByName(std::string name);
};

}


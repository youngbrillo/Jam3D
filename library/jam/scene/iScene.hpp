#pragma once
#include "Jam/core/entity.hpp"
#include "Jam/core/system.hpp"
#include "jam/core/clock.hpp"
#include "jam/core/resource.hpp"
#include "Environment.hpp"
#include <raylib.h>
#include <functional>
#include <vector>

namespace YAML {
    class Emitter;
    class Node;
}

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

    class Scene
    {
    public:
        SceneConfig config;
        //Canvas canvas;
        SystemManager systemManager;
        entt::registry world;
        entt::dispatcher eventDispatch;
        RenderTarget renderTarget;
        Environment worldEnv;
        std::vector<ResourceLite> localResources;
    public:
        Scene(SceneConfig config);
        virtual ~Scene();

        void Begin();
        void End();
        void Poll();
        void Update(const jam::Clock& time);
        void FixedUpdate(const jam::Clock& time);
        void Render();
        void RenderContent();
        void RenderToCanvas();
        void Serialize();		//save to file or stream
        void Deserialize();		//load from file or stream
    protected:
        virtual void onBegin() {}
        virtual void onEnd() {}
        virtual void onPoll() {}
        virtual void onUpdate(const jam::Clock& time) {}
        virtual void onFixedUpdate(const jam::Clock& time) {}
        virtual void onRender3DStart(const Camera3D& camera) {}
        virtual void onRender3DEnd(const Camera3D& camera) {}
        virtual void onRenderUI(const Camera3D& camera) {}
        virtual void onRenderToCanvas() {}
        virtual void onSerialize(YAML::Emitter& out);		//save to file or stream
        virtual void onDeserialize(const YAML::Node& node);		//load from file or stream
    public: //entity functions (short cuts)
        virtual Entity CreateEntity(std::string name, Vector3 position);
        virtual Entity CreateCube(std::string name, Vector3);
        virtual Entity CreateSphere(std::string name, Vector3);
        virtual Entity CreatePlane(std::string name, Vector3);
        virtual Entity CreateDonut(std::string name, Vector3);
        virtual Entity CreateCamera(std::string name, Vector3, float fov = 45);
        virtual Entity CreateEditorCamera(std::string name, Vector3, float fov = 45);
    public: //helper functions
        virtual Camera3D& GetCamera();
        virtual void SetResolution(Vector2 resoulution);
        Vector2 GetRenderDimensions() const;

        virtual Entity GetEntityByName(std::string name);
    };
}

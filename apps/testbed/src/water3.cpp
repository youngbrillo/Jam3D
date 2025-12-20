#include "waters.hpp"
#include <jam/modules/core3d/components/transform3d.hpp>
#include <vector>
#include <functional>

enum NodeDrawLayer
{
    NodeDrawLayer_None = 0,
    NodeDrawLayer_World = 1 << 0,
    NodeDrawLayer_Billboard = 1 << 1,
    NodeDrawLayer_Water = 1 << 2,

    NodeDrawLayer_All = NodeDrawLayer_World | NodeDrawLayer_Billboard | NodeDrawLayer_Water
};
struct Node3D
{
    int layer = NodeDrawLayer_World;
    jam::components::Transform3D transform;
    Mesh* meshInstance = nullptr;
    Material material = LoadMaterialDefault();

    void Render() const
    {
        DrawMesh(*meshInstance, material, transform.toMatrix());
    }
};

struct NodeManager
{
    std::vector<Node3D> nodes;

    Node3D& emplace()
    {
        return nodes.emplace_back();
    }

    void Render(int layer) const
    {
        for (auto&& node : nodes)
        {
            if ((node.layer & layer) != 0)
                node.Render();
        }
    }
};

struct RenderBuffer
{
    RenderBuffer(Vector2 size)
    {
        Load((int)size.x, (int)size.y);
        SetTextureFilter(buffer.texture, TEXTURE_FILTER_BILINEAR);

    }
    ~RenderBuffer()
    {
        Unload();
    }
    RenderBuffer(const RenderBuffer& o) = delete;
    void operator=(RenderBuffer const&) = delete;

    void Load(int width, int height)
    {
        buffer.id = rlLoadFramebuffer(); // Load an empty framebuffer

        if (buffer.id > 0)
        {
            rlEnableFramebuffer(buffer.id);

            // Create color texture (default to RGBA)
            buffer.texture.id = rlLoadTexture(0, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
            buffer.texture.width = width;
            buffer.texture.height = height;
            buffer.texture.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
            buffer.texture.mipmaps = 1;

            // Create depth texture buffer (instead of raylib default renderbuffer)
            buffer.depth.id = rlLoadTextureDepth(width, height, false);
            buffer.depth.width = width;
            buffer.depth.height = height;
            buffer.depth.format = 19;       // DEPTH_COMPONENT_24BIT: Not defined in raylib
            buffer.depth.mipmaps = 1;

            // Attach color texture and depth texture to FBO
            rlFramebufferAttach(buffer.id, buffer.texture.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
            rlFramebufferAttach(buffer.id, buffer.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

            // Check if fbo is complete with attachments (valid)
            if (rlFramebufferComplete(buffer.id)) TRACELOG(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", buffer.id);

            rlDisableFramebuffer();
        }
        else TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created");
    }
    void Unload()
    {
        if (buffer.id > 0)
        {
            // Color texture attached to FBO is deleted
            rlUnloadTexture(buffer.texture.id);
            rlUnloadTexture(buffer.depth.id);

            // NOTE: Depth texture is automatically
            // queried and deleted before deleting framebuffer
            rlUnloadFramebuffer(buffer.id);
        }
    }

    void handleScreenResize(Vector2 newSize)
    {
        Unload();
        Load((int)newSize.x, (int)newSize.y);
        SetTextureFilter(buffer.texture, TEXTURE_FILTER_BILINEAR);
    }

    template<typename RenderFunction>
    void Render(Color clearColor, RenderFunction onRender)
    {
        BeginTextureMode(buffer);
        ClearBackground(clearColor);
        onRender();
        EndTextureMode();
    }

    void DrawAsTexture(Rectangle destination) const
    {
        DrawTexturePro(
            buffer.texture,
            Rectangle{ 0, 0,buffer.texture.width * 1.0f,buffer.texture.height * -1.0f },
            destination,
            Vector2{ 0.0f,0.0f }, 0.0f, WHITE
        );
    }

    RenderTexture2D buffer;
};

void water3Main(int argc, const char** argv)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 1280;
    int screenHeight = 720;
    const char* windowTitle = GetFileNameWithoutExt(argv[0]);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, windowTitle);
    if (GetMonitorCount() >= 2) {
        SetWindowMonitor(1);
        SetWindowSize(screenWidth, screenHeight);
    }

    const char* currentDirectory = GetWorkingDirectory();
    ChangeDirectory(TextFormat("%s/%s", currentDirectory, "apps\\testbed\\res"));

    // Load Resources
    //--------------------------------------------------------------------------------------
    //textures
    Image heightmap = LoadImage("heightmap.png");
    Texture2D textureTerrain = LoadTextureFromImage(heightmap);

    //meshes
    Mesh meshTerrain = GenMeshHeightmap(heightmap, Vector3{ 1, 1,1 });
    Mesh meshWaterSurface = GenMeshPlane(100, 100, 100, 100);

    //shaders
    Shader shaderBase   = LoadShader("shaders3/base.vert.glsl", "shaders3/base.frag.glsl");
    Shader shaderWater  = LoadShader("shaders3/water.vert.glsl", "shaders3/water.frag.glsl");

    //buffers
    RenderBuffer reflectBuffer = RenderBuffer(Vector2{ (float)screenWidth, (float)screenHeight });
    RenderBuffer refractBuffer = RenderBuffer(Vector2{ (float)screenWidth, (float)screenHeight });

    UnloadImage(heightmap);
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    // variables
    //--------------------------------------------------------------------------------------
    bool isPaused = false;
    bool isShowControls = false;
    bool isWireFrameMode = false;
    float elapsedTime = 0.0f;

    // game objects
    //--------------------------------------------------------------------------------------
    Camera3D camera;

    camera.position = Vector3{ 30, 10, 30 };
    camera.target = Vector3{ 0,0,0 };
    camera.up = Vector3{ 0,1,0 };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    NodeManager nodes;

    //add terrain node
    {
        Node3D& node = nodes.emplace();
        node.transform.scale = 55.0f;
        node.transform.position = { -node.transform.scale/2 , -3.75f, -node.transform.scale/2};
        node.transform.size.y = 0.5f;
        node.meshInstance = &meshTerrain;
        node.material.maps[MATERIAL_MAP_DIFFUSE].texture = textureTerrain;
        node.material.shader = shaderBase;
    }

    //add water node
    {
        Node3D& node = nodes.emplace();
        node.layer = NodeDrawLayer_Water;
        node.meshInstance = &meshWaterSurface;
        node.material.maps[MATERIAL_MAP_DIFFUSE].color = GetColor(0xD1E5F4FF);
        node.material.shader = shaderWater;
    }

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        isShowControls = IsKeyDown(KEY_LEFT_CONTROL);

        if (isShowControls && IsKeyPressed(KEY_R))
        {
            TraceLog(LOG_INFO, "Reload Resource Here!");
        }
        if (isShowControls && IsKeyPressed(KEY_P))
            isPaused = !isPaused;

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            UpdateCamera(&camera, CAMERA_THIRD_PERSON);

        if (!isPaused)
            elapsedTime += GetFrameTime();
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(DARKBLUE);

        reflectBuffer.Render(RED, [=]() mutable {
            //camera.position.y *= -1;
            BeginMode3D(camera);
                nodes.Render(NodeDrawLayer_World);
            EndMode3D();
            //camera.position.y *= -1;
            });

            //draw scene
        BeginMode3D(camera);
            nodes.Render(NodeDrawLayer_All);
        EndMode3D();

        if (isPaused)
        {
            static const char* pauseMessage = "Paused";
            DrawRectangle(0, screenHeight / 2 - 30, screenWidth, 30, ColorAlpha(BLACK, 0.65f));
            DrawText(pauseMessage, screenWidth / 2 - MeasureText(pauseMessage, 20) / 2, screenHeight / 2 - 25, 20, WHITE);
        }

        if (isShowControls)
        {
            float width = screenWidth / 3.0f;
            float ratio = (float)screenWidth / (float)screenHeight;
            float height = (width * screenHeight) / screenWidth * 1.0f;

            reflectBuffer.DrawAsTexture(Rectangle{ 0, 0, width, height });


            static const float messageHeight = 100;
            DrawRectangle(0, screenHeight - messageHeight, screenWidth, messageHeight, ColorAlpha(BLACK, 0.65f));
            DrawText("Put Instructions Here", 10, screenHeight - messageHeight + 10, 20, WHITE);
        }
        EndDrawing();
    }

    UnloadTexture(textureTerrain);
    UnloadMesh(meshTerrain);

    UnloadShader(shaderWater);
    CloseWindow();        // Close window and OpenGL context
}
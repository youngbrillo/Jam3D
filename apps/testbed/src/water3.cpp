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
        buffer = LoadRenderTexture((int)size.x, (int)size.y);
        SetTextureFilter(buffer.texture, TEXTURE_FILTER_BILINEAR);
    }
    ~RenderBuffer()
    {
        UnloadRenderTexture(buffer);
    }
    RenderBuffer(const RenderBuffer& o) = delete;
    void operator=(RenderBuffer const&) = delete;


    void handleScreenResize(Vector2 newSize)
    {
        UnloadRenderTexture(buffer);
        buffer = LoadRenderTexture((int)newSize.x, (int)newSize.y);
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
    Shader waterShader = LoadShader("shaders/waterTile.vert.glsl", "shaders/waterTile.frag.glsl");

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
    }

    //add water node
    {
        Node3D& node = nodes.emplace();
        node.layer = NodeDrawLayer_Water;
        node.meshInstance = &meshWaterSurface;
        node.material.shader = waterShader;
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
            static const float messageHeight = 100;
            DrawRectangle(0, screenHeight - messageHeight, screenWidth, messageHeight, ColorAlpha(BLACK, 0.65f));
            DrawText("Put Instructions Here", 10, screenHeight - messageHeight + 10, 20, WHITE);
        }
        EndDrawing();
    }

    UnloadTexture(textureTerrain);
    UnloadMesh(meshTerrain);

    UnloadShader(waterShader);
    CloseWindow();        // Close window and OpenGL context
}
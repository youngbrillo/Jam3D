#include "waters.hpp"
#include <jam/modules/core3d/components/transform3d.hpp>
#include <vector>

struct Node3D
{
    jam::components::Transform3D transform;
    Mesh* meshInstance = nullptr;
    Material material = LoadMaterialDefault();

    void Render() const
    {
        DrawMesh(*meshInstance, material, transform.toMatrix());
    }
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
    Image heightmap = LoadImage("heightmap.png");
    Mesh meshTerrain = GenMeshHeightmap(heightmap, Vector3{ 1, 1,1 });
    Texture2D textureTerrain = LoadTextureFromImage(heightmap);
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

    std::vector<Node3D> nodes;

    //add terrain node
    {
        Node3D& node = nodes.emplace_back();
        node.transform.scale = 55.0f;
        node.transform.position = { -node.transform.scale/2 , -3.75f, -node.transform.scale/2};
        node.transform.size.y = 0.5f;
        node.meshInstance = &meshTerrain;
        node.material.maps[MATERIAL_MAP_DIFFUSE].texture = textureTerrain;
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
            DrawGrid(50, 1.0f);
            for (auto& node : nodes) node.Render();
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

    CloseWindow();        // Close window and OpenGL context
}
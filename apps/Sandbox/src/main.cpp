
#include "raylib.h"
#include <jam/jam3D.hpp>
#include <jam/modules/core3d/core3d.hpp>

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    Mesh mesh = GenMeshCube(1, 1, 1);

    jam::components::SceneCamera3D camera;
    jam::components::CameraEditorComponent camera_editor;

    jam::components::Transform3D transform;
    jam::components::MeshInstance3D mesh_instance;
    mesh_instance.mesh = &mesh;

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------
        if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            UpdateCamera(&camera.camera, camera_editor.mode);
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(DARKGRAY);
        BeginMode3D(camera.camera);
            DrawGrid(30, 1.0f);
            mesh_instance.Render(transform.toMatrix());
        EndMode3D();

        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
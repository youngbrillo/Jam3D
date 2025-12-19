#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"



struct UberShaderUniforms
{
    int timeLocation = -1;
    int viewPositionLocation = -1;
};

void UberShaderUniformsUpdate(UberShaderUniforms* uniforms, Shader shader, float time, Vector3 viewPosition)
{
    SetShaderValue(shader, uniforms->timeLocation, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, uniforms->viewPositionLocation, &viewPosition.x, SHADER_UNIFORM_VEC3);
}


static Shader LoadShaderWater(const char* vs, const char* fs, UberShaderUniforms* uniforms)
{
    Shader shader = LoadShader(vs, fs);
    if (IsShaderValid(shader))
    {
        uniforms->timeLocation = GetShaderLocation(shader, "time");
        uniforms->viewPositionLocation = GetShaderLocation(shader, "viewPosition");
    }

    return shader;
}



//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(int argc, const char** argv)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 1280;
    int screenHeight = 720;
    const char* windowTitle = GetFileNameWithoutExt(argv[0]);
    const char* shaderBaseVertexFilepath = "shaders/baseLight.vert.glsl";
    const char* shaderBaseFragmentFilepath = "shaders/baseLight.frag.glsl";

    const char* shaderWaterVertexFilepath = "shaders/water.vert.glsl";
    const char* shaderWaterFragmentFilepath = "shaders/water.frag.glsl";

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, windowTitle);
    if (GetMonitorCount() >= 2) {
        SetWindowMonitor(1);
        SetWindowSize(screenWidth, screenHeight);
    }

    const char* currentDirectory = GetWorkingDirectory();
    ChangeDirectory(TextFormat("%s/%s", currentDirectory, "apps\\testbed\\res"));

    // variables
    //--------------------------------------------------------------------------------------
    bool isPaused = false;
    bool isShowControls = false;
    bool isWireFrameMode = false;

    float elapsedTime = 0.0f;
    float waterSize = 10.0f;
    UberShaderUniforms baseUniforms;
    UberShaderUniforms waterUniforms;
    // resources
    //--------------------------------------------------------------------------------------
    Shader shaderBase = LoadShaderWater(shaderBaseVertexFilepath, shaderBaseFragmentFilepath, &baseUniforms);
    Shader shaderWater = LoadShaderWater(shaderWaterVertexFilepath, shaderWaterFragmentFilepath, &waterUniforms);

    Texture diffuseTexture = LoadTexture("randomC.png");
    Texture normalTexture = LoadTexture("randomD.png");
    Texture waveTexture = LoadTexture("dudv.png");

    SetTextureFilter(diffuseTexture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(normalTexture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(waveTexture, TEXTURE_FILTER_BILINEAR);
    SetTextureWrap(diffuseTexture, TEXTURE_WRAP_MIRROR_REPEAT);
    SetTextureWrap(normalTexture, TEXTURE_WRAP_MIRROR_REPEAT);
    SetTextureWrap(waveTexture, TEXTURE_WRAP_MIRROR_REPEAT);


    Material matNormal = LoadMaterialDefault();
    matNormal.maps[MATERIAL_MAP_ALBEDO].texture = LoadTextureFromImage(GenImageChecked(32, 32, 8, 8, WHITE, LIGHTGRAY));
    matNormal.shader = shaderBase;

    Material matGround = LoadMaterialDefault();
    matGround.maps[MATERIAL_MAP_ALBEDO].texture = LoadTextureFromImage(GenImageWhiteNoise(128, 128, 0.5));
    matGround.maps[MATERIAL_MAP_ALBEDO].color = GREEN;

    Material matWater = LoadMaterialDefault();
    matWater.maps[MATERIAL_MAP_ALBEDO].texture = diffuseTexture;
    matWater.maps[MATERIAL_MAP_ALBEDO].color = GetColor(0x99c0e3ff);
    matWater.maps[MATERIAL_MAP_NORMAL].texture = normalTexture;
    matWater.maps[MATERIAL_MAP_NORMAL].color = GetColor(0x416bdfff);
    matWater.maps[MATERIAL_MAP_METALNESS].texture = waveTexture;
    matWater.shader = shaderWater;

    Mesh meshCube = GenMeshCube(1, 1, 1);
    Mesh meshPlane = GenMeshPlane(20, 20, 100, 100);
    Mesh meshWaterPlane = GenMeshPlane(1, 1, 200, 200);
    // game objects
    //--------------------------------------------------------------------------------------

    Camera3D camera;

    camera.position     = Vector3{ 30, 10, 30 };
    camera.target       = Vector3{ 0,0,0 };
    camera.up           = Vector3{ 0,1,0};
    camera.fovy         = 45.0f;
    camera.projection   = CAMERA_PERSPECTIVE;


    Matrix matrixWater = MatrixMultiply(MatrixScale(waterSize, 1, waterSize), MatrixTranslate(0.0f, 0.25f, 0.0f));
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------
        if (!isPaused)
            elapsedTime += GetFrameTime();

        isShowControls = IsKeyDown(KEY_LEFT_CONTROL);

        if (isShowControls && IsKeyPressed(KEY_R))
        {
            if (IsShaderValid(shaderWater))
            {
                UnloadShader(shaderWater);
                shaderWater = Shader{ 0 };
            }
            shaderWater = LoadShaderWater(shaderWaterVertexFilepath, shaderWaterFragmentFilepath, &waterUniforms);
            matWater.shader = shaderWater;
        }

        if (isShowControls && IsKeyPressed(KEY_P))
            isPaused = !isPaused;

        if (isShowControls && IsKeyPressed(KEY_W))
            isWireFrameMode = !isWireFrameMode;

        if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            UpdateCamera(&camera, CAMERA_THIRD_PERSON);

        UberShaderUniformsUpdate(&baseUniforms, shaderBase, elapsedTime, camera.position);
        UberShaderUniformsUpdate(&waterUniforms, shaderWater, elapsedTime, camera.position);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(DARKGRAY);

        BeginMode3D(camera);
            DrawMesh(meshPlane, matGround, MatrixTranslate(0.0f, 0.0f, 0.0f));
            DrawMesh(meshCube, matNormal, MatrixTranslate(0.0f, 0.5f, 0.0f));
            if (isWireFrameMode)
            {
                rlEnableWireMode();
                    DrawMesh(meshWaterPlane, matWater, matrixWater);
                rlDisableWireMode();
            }
            else
                DrawMesh(meshWaterPlane, matWater, matrixWater);
            EndMode3D();

        if (isPaused)
        {
            static const char* pauseMessage = "Paused";
            DrawRectangle(0, screenHeight/2 - 30, screenWidth, 30, ColorAlpha(BLACK, 0.65f));
            DrawText(pauseMessage, screenWidth/2 - MeasureText(pauseMessage, 20)/2, screenHeight/2 - 25, 20, WHITE);
        }

        if (isShowControls)
        {
            static const float messageHeight = 100;
            DrawRectangle(0, screenHeight - messageHeight, screenWidth, messageHeight, ColorAlpha(BLACK, 0.65f));
            DrawText(TextFormat("[P] to %s", isPaused ? "unpause" : "pause"), 10, screenHeight - messageHeight + 10, 20, WHITE);
            DrawText("[R] to Reload shader", 10, screenHeight - messageHeight + 40, 20, WHITE);
            DrawText("[W] to toggle Wire Frame", 10, screenHeight - messageHeight + 70, 20, WHITE);
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadMesh(meshCube);
    UnloadMesh(meshPlane);
    UnloadMesh(meshWaterPlane);
    
    UnloadMaterial(matNormal);
    UnloadMaterial(matGround);
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------


}

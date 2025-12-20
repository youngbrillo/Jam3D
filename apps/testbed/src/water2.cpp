#include "waters.hpp"
#include <string>
#include <vector>
#include <functional>
#define RLIGHTS_IMPLEMENTATION
#include "rlights.hpp"

#define CLIP_SHADERS_COUNT		1 // number of shaders that use a clipPlane
// data used to store shaders that make use of clipPlanes
Shader clipShaders[CLIP_SHADERS_COUNT];
int clipShaderHeightLocs[CLIP_SHADERS_COUNT];
int clipShaderTypeLocs[CLIP_SHADERS_COUNT];
int AddClipShader(Shader shader)
{
    static int clipShadersCount = 0;
    clipShaders[clipShadersCount] = shader;
    clipShaderHeightLocs[clipShadersCount] = GetShaderLocation(shader, "cullHeight");
    clipShaderTypeLocs[clipShadersCount] = GetShaderLocation(shader, "cullType");
    clipShadersCount++;
    return clipShadersCount - 1;
}


Matrix TransformToMatrix(const Transform& transform)
{
    return MatrixMultiply(MatrixMultiply(MatrixScale(transform.scale.x, transform.scale.y, transform.scale.z ),
        QuaternionToMatrix(transform.rotation)),
        MatrixTranslate(transform.translation.x, transform.translation.y, transform.translation.z));
}

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

struct Ocean
{
    Mesh mesh;
    Material material;
    Shader shader = { 0 };
    std::string vertexShaderFilepath, fragmentShaderFilepath;

    struct {
        int moveFactorLoc = -1;
    } uniforms;

    float moveFactor = 0.0f;
    float moveSpeed = 0.025f;

    void Init(const char* vs, const char* fs, Texture2D dudv, Texture2D reflection, Texture2D refraction)
    {
        mesh = GenMeshPlane(5120, 5120, 10, 10);
        material = LoadMaterialDefault();
        vertexShaderFilepath = vs;
        fragmentShaderFilepath = fs;

        BindTextures(dudv, reflection, refraction);

        this->loadShader();
    }

    void BindTextures(Texture2D dudv, Texture2D reflection, Texture2D refraction)
    {
        material.maps[MATERIAL_MAP_DIFFUSE].texture = reflection;
        material.maps[MATERIAL_MAP_SPECULAR].texture = refraction;
        material.maps[MATERIAL_MAP_NORMAL].texture = dudv;
    }
    void unloadShader()
    {
        if (IsShaderValid(shader))
            UnloadShader(shader);

        shader = Shader{ 0 };
    }
    void loadShader()
    {
        this->unloadShader();
        shader = LoadShader(vertexShaderFilepath.c_str(), fragmentShaderFilepath.c_str());

        if (IsShaderValid(shader))
        {
            uniforms.moveFactorLoc = GetShaderLocation(shader, "moveFactor");
            shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
            shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
            material.shader = shader;
        }
        else
        {
            // Using rlgl default shader
            material.shader.id = rlGetShaderIdDefault();
            material.shader.locs = rlGetShaderLocsDefault();
        }

    }

    void update(float deltaTime, Vector3 view)
    {
        moveFactor += moveSpeed * deltaTime;
        if (moveFactor > 1.0f)
            moveFactor -= 1.0f;

        SetShaderValue(shader, uniforms.moveFactorLoc, &moveFactor, SHADER_UNIFORM_FLOAT);
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], &view.x, SHADER_UNIFORM_VEC3);

    }


    void render() const
    {
        DrawMesh(mesh, material, MatrixIdentity());
    }

};

struct Environment
{
    std::vector<Transform> transforms;

    virtual void render(const Mesh& mesh, const Material& material) const
    {
        for (auto& transform : transforms)
        {
            DrawMesh(mesh, material, TransformToMatrix(transform));
        }
    }
};

struct MeshEnvironment : public Environment
{
    Mesh mesh;
    Material material;
};

void water2Main(int argc, const char** argv)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 1280;
    int screenHeight = 720;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, GetFileNameWithoutExt(argv[0]));
    if (GetMonitorCount() >= 2) {
        SetWindowMonitor(1);
        SetWindowSize(screenWidth, screenHeight);
    }

    const char* currentDirectory = GetWorkingDirectory();
    ChangeDirectory(TextFormat("%s/%s", currentDirectory, "apps\\testbed\\res"));

    Camera3D camera;

    camera.position = Vector3{ 25, 10, 25};
    camera.target = Vector3{ 0,0,0 };
    camera.up = Vector3{ 0,1,0 };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Light lights[MAX_LIGHTS] = { 0 };


    Texture2D textureCheckered = LoadTextureFromImage(GenImageChecked(32, 32, 8, 8, WHITE, LIGHTGRAY));


    Texture2D textureDUDV = LoadTexture("dudv.png");
    SetTextureFilter(textureDUDV, TEXTURE_FILTER_BILINEAR);
    GenTextureMipmaps(&textureDUDV);

    RenderTexture2D applicationBuffer   = LoadRenderTexture(screenWidth, screenHeight); // main FBO used for postprocessing
    RenderBuffer reflectBuffer = RenderBuffer(Vector2{ (float)screenWidth,(float)screenHeight });
    RenderBuffer refractBuffer = RenderBuffer(Vector2{ (float)screenWidth,(float)screenHeight });

    Ocean ocean;
    ocean.Init("shaders/waterRealistic.vert.glsl", "shaders/waterRealistic.frag.glsl", 
        textureDUDV, reflectBuffer.buffer.texture, refractBuffer.buffer.texture);

    MeshEnvironment cubes;
    cubes.mesh = GenMeshCube(1, 1, 1);
    cubes.material = LoadMaterialDefault();
    cubes.material.maps[MATERIAL_MAP_DIFFUSE].texture = textureCheckered;
    std::vector<Vector3> locs = {
        Vector3{7.0f, 2.5f, -4.5},
    };
    for (auto p : locs)
        cubes.transforms.emplace_back(Transform{ .translation = p, .rotation = QuaternionIdentity(), .scale = Vector3{1,1,1}});
    for (int i = -10; i <= 10; i += 2)
    {
        Vector3 p = { i * 1.0f, 0.0, 0.0f };
        cubes.transforms.emplace_back(Transform{ .translation = p, .rotation = QuaternionIdentity(), .scale = Vector3{1,1,1}});
    }

    Shader terrainShader = LoadShader("shaders/terrain.vert.glsl", "shaders/terrain.frag.glsl");
    Vector4 ambc = { 0.22f, 0.17f, 0.41f, 0.2f }; // current ambient color & intensity

    if (IsShaderValid(terrainShader))
    {
        terrainShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(terrainShader, "matModel");
        terrainShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(terrainShader, "viewPos");
        int terrainDaytimeLoc = GetShaderLocation(terrainShader, "daytime");
        int cs = AddClipShader(terrainShader); // register as clip shader for automatization of clipPlanes
        float param10 = 0.0f;
        int param11 = 2;
        SetShaderValue(terrainShader, clipShaderHeightLocs[cs], &param10, SHADER_UNIFORM_FLOAT);
        SetShaderValue(terrainShader, clipShaderTypeLocs[cs], &param11, SHADER_UNIFORM_INT);
        // ambient light level
        int terrainAmbientLoc = GetShaderLocation(terrainShader, "ambient");
        SetShaderValue(terrainShader, terrainAmbientLoc, &ambc.x, SHADER_UNIFORM_VEC4);
        //Texture2D rockNormalMap = LoadTexture("resources/rockNormalMap.png"); // normal map
        //SetTextureFilter(rockNormalMap, TEXTURE_FILTER_BILINEAR);
        //GenTextureMipmaps(&rockNormalMap);
        //terrainShader.locs[SHADER_LOC_MAP_ROUGHNESS] = GetShaderLocation(terrainShader, "rockNormalMap");
    }
    MeshEnvironment oceanFloor;
    oceanFloor.mesh = GenMeshPlane(5120, 5120, 10, 10);
    oceanFloor.material = LoadMaterialDefault();
    oceanFloor.material.maps[0].texture = LoadTexture("terrainGradient.png");
    oceanFloor.material.maps[2].texture = LoadTextureFromImage(GenImageColor(8, 8, BLACK));
    oceanFloor.material.shader = terrainShader;
    oceanFloor.transforms.emplace_back(Transform{
        .translation = {0.0f,-1.2f,0.0f}, .rotation = QuaternionIdentity(), .scale = Vector3{1,1,1} });

    
    lights[0] = CreateLight(LIGHT_DIRECTIONAL, { 20, 10, 0 }, Vector3Zero(), WHITE, {ocean.shader, terrainShader});


    bool isPaused = false;
    bool isShowControls = false;


    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            UpdateCamera(&camera, CAMERA_THIRD_PERSON);

        isShowControls = IsKeyDown(KEY_LEFT_CONTROL);

        if (isShowControls && IsKeyPressed(KEY_P))
            isPaused = !isPaused;
            

        float dt = isPaused ? 0 : GetFrameTime();

        ocean.update(dt, camera.position);
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            reflectBuffer.Render(RED, [=]()mutable {
                camera.position.y *= -1;
                BeginMode3D(camera);
                cubes.render(cubes.mesh, cubes.material);
                EndMode3D();
                camera.position.y *= -1;
            });

            refractBuffer.Render(GREEN, [=]()mutable {
                BeginMode3D(camera);
                cubes.render(cubes.mesh, cubes.material);
                oceanFloor.render(oceanFloor.mesh, oceanFloor.material);
                EndMode3D();
                });

            ClearBackground(DARKBLUE);
            BeginMode3D(camera);
                cubes.render(cubes.mesh, cubes.material);
                oceanFloor.render(oceanFloor.mesh, oceanFloor.material);
                ocean.render();
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
                float height = screenHeight / ratio;
                DrawTexturePro(
                    reflectBuffer.buffer.texture,
                    Rectangle{ 0, 0,reflectBuffer.buffer.texture.width * 1.0f,reflectBuffer.buffer.texture.height * -1.0f },
                    Rectangle{ 0, 0, width, height },
                    Vector2{ 0.0f,0.0f }, 0.0f, WHITE
                );

                DrawTexturePro(
                    refractBuffer.buffer.texture,
                    Rectangle{ 0, 0,refractBuffer.buffer.texture.width * 1.0f,refractBuffer.buffer.texture.height * -1.0f },
                    Rectangle{ width, 0, width, height },
                    Vector2{ 0.0f,0.0f }, 0.0f, WHITE
                );



                static const float messageHeight = 100;
                DrawRectangle(0, screenHeight - messageHeight, screenWidth, messageHeight, ColorAlpha(BLACK, 0.65f));
                DrawText(TextFormat("[P] to %s", isPaused ? "unpause" : "pause"), 10, screenHeight - messageHeight + 10, 20, WHITE);
                DrawText("[R] to Reload shader", 10, screenHeight - messageHeight + 40, 20, WHITE);
                DrawText("[W] to toggle Wire Frame", 10, screenHeight - messageHeight + 70, 20, WHITE);
            }
        EndDrawing();
    }
    CloseWindow();        // Close window and OpenGL context
}


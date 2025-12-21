#include "waters.hpp"
#include <jam/modules/core3d/components/transform3d.hpp>
#include <vector>
#include <functional>
#include <assert.h>

#define USE_DEFAULT_DEPTH_BUFFER_TECH

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

    Node3D& getLast()
    {
        assert(nodes.size() != 0 && "nodes vector is empty!");
        return nodes.at(nodes.size() - 1);
    }
};

// Load custom render texture with depth texture attached
static RenderTexture2D LoadRenderTextureDepthTex(int width, int height);
// Unload render texture from GPU memory (VRAM)
static void UnloadRenderTextureDepthTex(RenderTexture2D target);



struct RenderBufferPro
{
    RenderBufferPro(Vector2 size)
    {
        buffer = LoadRenderTextureDepthTex((int)size.x, (int)size.y);
        SetTextureFilter(buffer.texture, TEXTURE_FILTER_BILINEAR);

    }
    ~RenderBufferPro()
    {
        UnloadRenderTextureDepthTex(buffer);
    }
    RenderBufferPro(const RenderBufferPro& o) = delete;
    void operator=(RenderBufferPro const&) = delete;


    void handleScreenResize(Vector2 newSize)
    {
        UnloadRenderTextureDepthTex(buffer);
        buffer = LoadRenderTextureDepthTex((int)newSize.x, (int)newSize.y);
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

    void DrawAsTexture(Rectangle destination, bool drawDepth) const
    {
        DrawTexturePro(
            drawDepth ? buffer.depth : buffer.texture ,
            Rectangle{ 0, 0,buffer.texture.width * 1.0f,buffer.texture.height * -1.0f },
            destination,
            Vector2{ 0.0f,0.0f }, 0.0f, WHITE
        );
    }

    RenderTexture2D buffer;
};



static void RenderTexture2DDraw(RenderTexture2D buffer, Rectangle destination, bool drawDepth)
{
    DrawTexturePro(
        drawDepth ? buffer.depth : buffer.texture,
        Rectangle{ 0, 0,buffer.texture.width * 1.0f,buffer.texture.height * -1.0f },
        destination,
        Vector2{ 0.0f,0.0f }, 0.0f, WHITE
    );
}


struct UniformsWater
{
    struct {
        int locTime = -1;
        int locMoveFactor = -1;
        int locViewPosition= -1;
        int locNormalMap = -1;
        int locDepthBuffer = -1;
    } uniforms;

    struct {
        float moveFactor = 0.0f;
        float moveSpeed = 0.025f;
    } values;

    void Set(Shader shader, Camera3D camera, Texture2D normalTexture, float time, float dt)
    {
        uniforms.locTime = GetShaderLocation(shader, "time");
        uniforms.locMoveFactor = GetShaderLocation(shader, "waveMoveSpeed");
        uniforms.locViewPosition = GetShaderLocation(shader, "viewPosition");
        uniforms.locNormalMap = GetShaderLocation(shader, "texture3");
        uniforms.locDepthBuffer = GetShaderLocation(shader, "texture4");

        int roughnessid = shader.locs[SHADER_LOC_MAP_ROUGHNESS];
        int locDepthBuffer = shader.locs[SHADER_LOC_MAP_OCCLUSION];

        shader.locs[SHADER_LOC_MAP_ROUGHNESS] = uniforms.locNormalMap;
        shader.locs[SHADER_LOC_MAP_OCCLUSION] = uniforms.locDepthBuffer;
        //SetShaderValueTexture(shader, uniforms.locNormalMap, normalTexture);

        values.moveFactor = 0.0f;

        Update(shader, camera, time, dt);
    }

    void Update(Shader shader, Camera3D camera, float time, float dt)
    {
        values.moveFactor += dt * values.moveSpeed;
        if (values.moveFactor > 1.0f)
            values.moveFactor -= 1.0f;

        SetShaderValue(shader, uniforms.locTime, &time, SHADER_UNIFORM_FLOAT);
        SetShaderValue(shader, uniforms.locMoveFactor, &values.moveFactor, SHADER_UNIFORM_FLOAT);
        SetShaderValue(shader, uniforms.locViewPosition, &camera.position.x, SHADER_UNIFORM_VEC3);
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

    SetRandomSeed(1773);
    // Load Resources
    //--------------------------------------------------------------------------------------
    //textures
    Image heightmap = LoadImage("heightmap.png");
    Image checkedSM = GenImageGradientLinear(96, 96, 180, WHITE, DARKGREEN);
    Image checkedLG = GenImageChecked(512, 512, 8, 8, WHITE, LIGHTGRAY);
    Texture2D textureTerrain = LoadTextureFromImage(heightmap);
    Texture2D textureDUDV = LoadTexture("dudv.png");
    Texture2D textureNormalmap = LoadTexture("normal.png");
    Texture2D textureCheckedSM = LoadTextureFromImage(checkedSM);
    Texture2D textureCheckedLG = LoadTextureFromImage(checkedLG);

    SetTextureFilter(textureTerrain, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(textureDUDV, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(textureNormalmap, TEXTURE_FILTER_BILINEAR);

    //meshes
    Mesh meshTerrain = GenMeshHeightmap(heightmap, Vector3{ 1, 1,1 });
    Mesh meshWaterSurface = GenMeshPlane(100, 100, 100, 100);
    Mesh meshCube = GenMeshCube(1, 1, 1);
    //shaders
    Shader shaderBase   = LoadShader("shaders3/base.vert.glsl", "shaders3/base.frag.glsl");
    Shader shaderWater = LoadShader("shaders3/water.vert.glsl", "shaders3/water.frag.glsl");
    Shader shaderDepth = LoadShader(0, "shaders/depth.frag.glsl");

    assert(IsShaderValid(shaderBase) && "Base Shader is invalid!");
    assert(IsShaderValid(shaderWater) && "Water Shader is invalid!");
    assert(IsShaderValid(shaderDepth) && "Depth Shader is invalid!");
    //buffers
    RenderBufferPro reflectBuffer = RenderBufferPro(Vector2{ (float)screenWidth, (float)screenHeight });
    RenderBufferPro refractBuffer = RenderBufferPro(Vector2{ (float)screenWidth, (float)screenHeight });
    //RenderBuffer depthBuffer   = RenderBuffer(Vector2{ (float)screenWidth, (float)screenHeight });
    RenderTexture2D depthBuffer = LoadRenderTexture(screenWidth, screenHeight);


    int depthLoc = GetShaderLocation(shaderDepth, "depthTexture");
    int flipTextureLoc = GetShaderLocation(shaderDepth, "flipY");
    int flipVal = 1;
    SetShaderValue(shaderDepth, flipTextureLoc, &flipVal, SHADER_UNIFORM_INT); // Flip Y texture


    UnloadImage(heightmap);
    UnloadImage(checkedSM);
    UnloadImage(checkedLG);
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    // variables
    //--------------------------------------------------------------------------------------
    bool isPaused = false;
    bool isShowControls = false;
    bool isWireFrameMode = false;
    float elapsedTime = 0.0f;
    float deltaTime = 0.0f;

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
        node.transform.position = { -node.transform.scale/2 , -5.75f, -node.transform.scale/2};
        node.transform.size.y = 0.5f;
        node.meshInstance = &meshTerrain;
        node.material.maps[MATERIAL_MAP_DIFFUSE].texture = textureCheckedLG;
        node.material.maps[MATERIAL_MAP_DIFFUSE].color = ORANGE;
        node.material.shader = shaderBase;
    }

    //add random that stick out of the water
    int boundMin = 13, boundMax = 45;
    for(int i = 0; i < 40; i++)
    {
        Node3D& node = nodes.emplace();

        int quad1 = GetRandomValue(-1, 1);
        if (quad1 == 0) quad1 = 1;
        int quad2 = GetRandomValue(-1, 1);
        if (quad2 == 0) quad2 = 1;

        float x = quad1 * GetRandomValue(boundMin, boundMax) + GetRandomValue(-100, 100) / 100.0f;
        float z = quad2 * GetRandomValue(boundMin, boundMax) + GetRandomValue(-100, 100) / 100.0f;

        node.transform.position = { x, 0.0f, z};
        node.transform.size.y = GetRandomValue(10, 20);

        float angle = GetRandomValue(0, 90) + GetRandomValue(-100, 100) / 100.0f;
        bool rotateAroundXAxis = GetRandomValue(0, 1);
        node.transform.orientation = QuaternionFromAxisAngle(rotateAroundXAxis ? Vector3{ 1, 0, 0 } : Vector3{0, 0, 1}, angle * DEG2RAD);
        node.meshInstance = &meshCube;


        node.material.maps[MATERIAL_MAP_DIFFUSE].texture = textureCheckedSM;
        //node.material.maps[MATERIAL_MAP_DIFFUSE].color = PINK;
    }

    //add water node
    {
        Node3D& node = nodes.emplace();
        node.transform.scale = 10;
        node.layer = NodeDrawLayer_Water;
        node.meshInstance = &meshWaterSurface;
        node.material.maps[MATERIAL_MAP_DIFFUSE].color = GetColor(0xD1E5F4FF);
        node.material.maps[MATERIAL_MAP_DIFFUSE].texture = textureDUDV;
        node.material.maps[MATERIAL_MAP_SPECULAR].texture = reflectBuffer.buffer.texture;
        node.material.maps[MATERIAL_MAP_NORMAL].texture = refractBuffer.buffer.texture;
        node.material.maps[MATERIAL_MAP_ROUGHNESS].texture = textureNormalmap;
        node.material.maps[MATERIAL_MAP_OCCLUSION].texture = depthBuffer.texture;
        node.material.shader = shaderWater;
    }



    UniformsWater waterUniforms;
    waterUniforms.Set(shaderWater, camera, textureNormalmap, elapsedTime, 0);

    rlSetClipPlanes(0.001, 1000.0);

    float near = rlGetCullDistanceNear();
    float far = rlGetCullDistanceFar();
    //bool isDepthTestEnabled = glis
    rlEnableDepthTest();

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
            TraceLog(LOG_INFO, "Reloading  Water shader!");
            UnloadShader(shaderWater);
            shaderWater = LoadShader("shaders3/water.vert.glsl", "shaders3/water.frag.glsl");

            if (IsShaderValid(shaderWater))
            {
                Node3D& node = nodes.getLast();
                node.material.shader = shaderWater;
                node.material.maps[MATERIAL_MAP_SPECULAR].texture = reflectBuffer.buffer.texture;
                node.material.maps[MATERIAL_MAP_NORMAL].texture = refractBuffer.buffer.texture;
                node.material.maps[MATERIAL_MAP_ROUGHNESS].texture = textureNormalmap;
                //int reflectTextureLoc = GetShaderLocation(shaderWater, "reflectTexture");
                //int refractTextureLoc = GetShaderLocation(shaderWater, "refractTexture");
                //SetShaderValueTexture(shaderWater, reflectTextureLoc, reflectBuffer.buffer.texture);
                //SetShaderValueTexture(shaderWater, refractTextureLoc, refractBuffer.buffer.texture);
                waterUniforms.Set(shaderWater, camera, textureNormalmap, elapsedTime, 0);
            }

        }
        if (isShowControls && IsKeyPressed(KEY_P))
            isPaused = !isPaused;
        if (isShowControls && IsKeyPressed(KEY_ZERO))
        {
            TakeScreenshot("screenshot.png");
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            UpdateCamera(&camera, CAMERA_THIRD_PERSON);

        if (isPaused)
        {
            deltaTime = 0.0f;
        }
        else
        {
            deltaTime = GetFrameTime();
            elapsedTime += deltaTime;
        }

    waterUniforms.Update(shaderWater, camera, elapsedTime, deltaTime);
    ClearBackground(DARKBLUE);
        //draw reflect buffer
        reflectBuffer.Render(BLUE, [=]() mutable {
            camera.position.y *= -1;
            BeginMode3D(camera);
                nodes.Render(NodeDrawLayer_World);
            EndMode3D();
            camera.position.y *= -1;
            });

        //draw refract buffer
        refractBuffer.Render(RED, [=]() mutable {
            BeginMode3D(camera);
            nodes.Render(NodeDrawLayer_World);
            EndMode3D();
            });
        //drawing the depth buffer
            //I'm going to cheat here since the refreactBuffer has everything we want in it already..
            //or not

        BeginTextureMode(depthBuffer);
            ClearBackground(YELLOW);
            BeginShaderMode(shaderDepth);
                SetShaderValueTexture(shaderDepth, depthLoc, refractBuffer.buffer.depth);
                DrawTexture(refractBuffer.buffer.depth, 0, 0, WHITE);
            EndShaderMode();
        EndTextureMode();

        // Draw
        //----------------------------------------------------------------------------------
        
        BeginDrawing();
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
            float width = screenWidth / 4.0f;
            float ratio = (float)screenWidth / (float)screenHeight;
            float height = (width * screenHeight) / screenWidth * 1.0f;

            reflectBuffer.DrawAsTexture(Rectangle{ width*0, 0, width, height },false);
            refractBuffer.DrawAsTexture(Rectangle{ width*1, 0, width, height }, false);
            refractBuffer.DrawAsTexture(Rectangle{ width*2, 0, width, height }, true);
            //depthBuffer.DrawAsTexture(  Rectangle{ width*3, 0, width, height }, false);
            RenderTexture2DDraw(depthBuffer, Rectangle{ width * 3, 0, width, height }, false);

            static const float messageHeight = 100;
            DrawRectangle(0, screenHeight - messageHeight, screenWidth, messageHeight, ColorAlpha(BLACK, 0.65f));
            DrawText("Put Instructions Here", 10, screenHeight - messageHeight + 10, 20, WHITE);
        }
        EndDrawing();
    }

    UnloadTexture(textureTerrain);
    UnloadTexture(textureDUDV);
    UnloadTexture(textureNormalmap);
    UnloadTexture(textureCheckedSM);
    UnloadTexture(textureCheckedLG);

    UnloadMesh(meshTerrain);
    UnloadMesh(meshWaterSurface);
    UnloadMesh(meshCube);

    UnloadShader(shaderWater);
    UnloadShader(shaderBase);
    UnloadShader(shaderDepth);

    CloseWindow();        // Close window and OpenGL context
}


//--------------------------------------------------------------------------------------
// Module Functions Definition
//--------------------------------------------------------------------------------------
// Load custom render texture, create a writable depth texture buffer
static RenderTexture2D LoadRenderTextureDepthTex(int width, int height)
{
    RenderTexture2D target = { 0 };

    target.id = rlLoadFramebuffer(); // Load an empty framebuffer

    if (target.id > 0)
    {
        rlEnableFramebuffer(target.id);

        // Create color texture (default to RGBA)
        target.texture.id = rlLoadTexture(0, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
        target.texture.width = width;
        target.texture.height = height;
        target.texture.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        target.texture.mipmaps = 1;

        // Create depth texture buffer (instead of raylib default renderbuffer)
        target.depth.id = rlLoadTextureDepth(width, height, false);
        target.depth.width = width;
        target.depth.height = height;
        target.depth.format = 19;       // DEPTH_COMPONENT_24BIT: Not defined in raylib
        target.depth.mipmaps = 1;

        // Attach color texture and depth texture to FBO
        rlFramebufferAttach(target.id, target.texture.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
        rlFramebufferAttach(target.id, target.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

        // Check if fbo is complete with attachments (valid)
        if (rlFramebufferComplete(target.id)) 
            TRACELOG(LOG_INFO, "FBO: [ID %i][%i | %i] Framebuffer object created successfully", target.id, target.texture.id, target.depth.id);

        rlDisableFramebuffer();
    }
    else TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created");

    return target;
}

// Unload render texture from GPU memory (VRAM)
void UnloadRenderTextureDepthTex(RenderTexture2D target)
{
    if (target.id > 0)
    {
        // Color texture attached to FBO is deleted
        rlUnloadTexture(target.texture.id);
        rlUnloadTexture(target.depth.id);

        // NOTE: Depth texture is automatically
        // queried and deleted before deleting framebuffer
        rlUnloadFramebuffer(target.id);
    }
}
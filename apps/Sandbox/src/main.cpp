
#include "raylib.h"
#include <jam/jam3D.hpp>
#include <jam/modules/core3d/core3d.hpp>
#include "logger/customLogger.hpp"
#include <jam/core/resourceManager.hpp>
#include "editor/editorLayer.hpp"

static jam::Scene* GenBaseScene(jam::SceneConfig config)
{
    return new jam::Scene(config);
}


int main(int argv, char** args)
{
    SetTraceLogCallback(CustomLogger);
    using namespace jam;
    Project project;
    project.Load("apps/sandbox/res/project.res.yaml");
    
    ResourceManager::Startup("resource/resource.db.yaml");
    do {
        Application app(project);
        ResourceManager::Get().initDefaults();
        app.SetDefaultLayer(new editor::EditorLayer);
        app.SetDefaultTemplate(app.AddTemplate(SceneTemplate{ .name = "scene", .generator = GenBaseScene }));
        app.SetScene(project.scene.definition);
        app.Run();

    } while (project.reloadApp);

    ResourceManager::Shutdown();
    project.Save("project.res.yaml");


    return 0;
}

#include "raylib.h"
#include <jam/jam3D.hpp>
#include <jam/modules/core3d/core3d.hpp>
#include "logger/customLogger.hpp"


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

    do {
        Application app(project);
        app.SetDefaultTemplate(app.AddTemplate(SceneTemplate{ .name = "scene", .generator = GenBaseScene }));
        app.SetScene(project.scene.definition);

        app.Run();
    } while (project.reloadApp);

    project.Save("project.res.yaml");


    return 0;
}
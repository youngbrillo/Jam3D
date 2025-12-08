#pragma once
#include "project.hpp"
#include "iApplicationLayer.hpp"
#include "Jam/core/clock.hpp"
#include <vector>

 namespace jam
 {
     typedef std::function<jam::Scene* (jam::SceneConfig def)> SceneGenFunction;

     struct SceneTemplate
     {
         std::string name;
         SceneGenFunction generator;
     };
     class Application
     {
     public:
         Application(Project& projectSettings);
         ~Application();
         static Application* Instance();
         void Run();

         void SetDefaultTemplate(int id);
         int AddTemplate(SceneTemplate temp);    //returns the template index
         void SetDefaultLayer(iApplicationLayer* layer);
         void SetScene(jam::SceneConfig scene);
         void RestartScene();
         void ReloadScene();
         Project& GetProject() { return _project; }
         const std::vector<SceneTemplate>& GetTemplates() const { return _scene_templates; }
         Scene* GetScene() { return _active_scene; }

         //time functions
         const Clock& GetTime();

         bool isPaused() const;
         void Pause(); //pause the clock
         void Resume();
         void FastForwardx2();
         void FastForwardx4();
         void SlowDown();
         void SetPlaybackSpeed(float value);
     private:
         Scene* getSceneTemplate(int slot);
         void refreshScene();        //stops and starts the active cene
         void reloadScene();         //destroys the active scene and creates a new one
         void poll();
         void update();
         void render();
     private:
         Project& _project;
         Scene* _active_scene;
         iApplicationLayer* _default_layer;
         std::vector<SceneTemplate> _scene_templates;
         Clock _time;
     };
 } // namespace jam::app


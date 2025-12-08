#pragma once
#include "project.hpp"
#include "iApplicationLayer.hpp"
#include "Jam/core/clock.hpp"
#include <vector>

 namespace jam
 {
     typedef std::function<jam::iScene* (jam::SceneConfig def)> SceneGenFunction;

     struct SceneTemplate
     {
         std::string name;
         SceneGenFunction generator;
     };
     class Application
     {
         Project& project;
         iScene* activeScene;
         iApplicationLayer* defaultLayer;
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
         Project& GetProject() { return project; }
         const std::vector<SceneTemplate>& GetTemplates() const { return templates; }
         iScene* GetScene() { return activeScene; }

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
         iScene* getSceneTemplate(int slot);
         void refreshScene();        //stops and starts the active cene
         void reloadScene();         //destroys the active scene and creates a new one
         void poll();
         void update();
         void render();
         std::vector<SceneTemplate> templates;
         Clock time;
     };
 } // namespace jam::app


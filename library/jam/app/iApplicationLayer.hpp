 #pragma once
 #ifndef ILAYER_HPP
 #define ILAYER_HPP

 #include <memory>
 #include "../scene/iScene.hpp"

 namespace jam
 {
 	class Application;

 	class iApplicationLayer
 	{
 	public:
 		iApplicationLayer(Application* app) : appRef(app) {}
 		virtual ~iApplicationLayer() { appRef = nullptr; }

 		inline void GetScene(Scene* scene)
 		{
 			sceneRef = scene;
 			onSceneStart();
 		}
 		inline void FreeScene()
 		{
 			onSceneEnd();
 			sceneRef = nullptr;
 		}
 		virtual void onSceneStart() = 0;
 		virtual void onSceneEnd() = 0;
 		virtual void poll() = 0;
 		virtual void update(float dt) = 0;
 		virtual void render() = 0;
 		virtual void serialize(std::string configFilepath) = 0;
 		virtual void deserialize(std::string configFilepath) = 0;
 	protected:
 		Scene* sceneRef = nullptr;
 		Application* appRef = nullptr;
 	};
 }

 #endif
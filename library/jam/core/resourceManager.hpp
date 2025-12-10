#pragma once
#include "resource.hpp"
#include <unordered_map>

namespace jam
{
	class ResourceManager
	{
	public:
		static ResourceManager& Get();
		//populate the resource repo
		static void Startup(std::string cachefile);

		//initialize default resources
		void initDefaults();

		static void UpdateShaders(const Camera3D& camera, float totalTime);

		//unload all resources
		static void Shutdown();


		ResourceID FindResource(std::string fileOrAlias);

		ResourceID			Load_Texture(std::string file);
		TextureResource*		Get_Texture(ResourceID id);
		TextureResource*		Get_Or_LoadTexture(ResourceID rid, std::string fileLocation = "");


		ResourceID			Load_Mesh(std::string file);
		ModelResource*		Get_Mesh(ResourceID id);
		ModelResource*		Get_Or_LoadMesh(ResourceID rid, std::string fileLocation = "");
	private:
		ResourceManager();
		~ResourceManager();
		ResourceManager(const ResourceManager&) = delete;
		ResourceID _load_resource(std::string file, ResourceID* tracked);
		ResourceID _load_texture(std::string file, ResourceID* tracked);
		ResourceID _load_mesh(std::string file, ResourceID* tracked);

		static ResourceManager* _instance;
		std::unordered_map<ResourceID, TextureResource> textures;
		std::unordered_map<ResourceID, ModelResource> meshes;
		std::unordered_map<ResourceID, Resource> resources;
		std::unordered_map<std::string, ResourceID> resourceLocations;

		ResourceID _default_res_id = 0;
		friend class Editor;
	};

}
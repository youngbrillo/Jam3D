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

		//registers a resource in the resources table, does not load the resource into memory
		ResourceID RegisterResource(Resource resource);

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

		ResourceID			Load_Shader(std::string file);
		ShaderResource*		Get_Shader(ResourceID id);
		ShaderResource*		Get_Or_Shader(ResourceID rid, std::string fileLocation = "");

		Resource GetResource(ResourceID id);
		//loads a resource into memory (GPU), returns true if the operation is successful and the resource is valid
		bool LoadResource(ResourceID id); 
		void UnloadResource(ResourceID id);
	private:
		ResourceManager();
		~ResourceManager();
		ResourceManager(const ResourceManager&) = delete;

		ResourceID _load_resource(std::string file, ResourceID* tracked);
		ResourceID _load_texture(std::string file, ResourceID* tracked);
		ResourceID _load_mesh(std::string file, ResourceID* tracked);
		ResourceID _load_shader(std::string file, ResourceID* tracked);

		static ResourceManager* _instance;
		std::unordered_map<ResourceID, TextureResource> textures;
		std::unordered_map<ResourceID, ModelResource> meshes;
		std::unordered_map<ResourceID, ShaderResource> shaders;

		std::unordered_map<ResourceID, Resource> resources;
		std::unordered_map<std::string, ResourceID> resourceLocations;

		ResourceID _default_res_id = 0;
		friend class Editor;
	};

}
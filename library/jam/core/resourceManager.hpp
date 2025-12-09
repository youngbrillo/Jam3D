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

		static void UpdateShaders(const Camera3D& camera, float totalTime);

		//unload all resources
		static void Shutdown();


		ResourceID FindResource(std::string fileOrAlias);

		ResourceID Load_Texture(std::string file);
		ResourceID Load_Model(std::string file);
		ResourceID Load_Resource(std::string file);

		Texture2D	Get_Texture(ResourceID id);
		Model* Get_Model(ResourceID id);

		Texture2D   Get_Or_LoadTexture(ResourceID rid, std::string fileLocation = "");
		Model* Get_Or_LoadModel(ResourceID rid, std::string fileLocation = "");

		ResourceID			Load_Shader(std::string file);
		ShaderResource& Get_Shader(ResourceID id);
		ShaderResource& Get_Or_LoadShader(ResourceID id, std::string fileLocation = "");

		ImageResource* Get_ImageResource(ResourceID id);
		MeshResource* Get_MeshResource(ResourceID id);
		ShaderResource* Get_ShaderResource(ResourceID id);
	private:
		ResourceManager();
		~ResourceManager();
		ResourceManager(const ResourceManager&) = delete;
		ResourceID _load_resource(std::string file, ResourceID* tracked);
		ResourceID _load_texture(std::string file, ResourceID* tracked);
		ResourceID _load_model(std::string file, ResourceID* tracked);
		ResourceID _load_shader(std::string file, ResourceID* tracked);

		static ResourceManager* _instance;
		std::unordered_map<ResourceID, Texture2D> textures;
		std::unordered_map<ResourceID, Model> models;
		std::unordered_map<ResourceID, ShaderResource> shaders;
		std::unordered_map<ResourceID, ImageResource> generatedImages;
		std::unordered_map<ResourceID, MeshResource> generatedMeshes;
		std::unordered_map<std::string, ResourceID> resourceLocations;
		std::unordered_map<ResourceID, std::string> resourceRepo;

		friend class EditorLite;
	};

}
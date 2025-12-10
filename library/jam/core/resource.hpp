#pragma once
#include <raylib.h>
#include <string>
#include "uuid.hpp"
#include "iVec2.hpp"

namespace YAML {
	class Emitter;
	class Node;
}

namespace jam
{
	using ResourceID = jam::UUID;

	enum ResourceType
	{
		ResourceType_Null = 0,
		ResourceType_Texture = 1 << 1,
		ResourceType_mesh = 1 << 2,
		ResourceType_Shader = 1 << 3,
		ResourceType_Font = 1 << 4,
		ResourceType_Sound = 1 << 5,
		ResourceType_Music = 1 << 6,
		ResourceType_RenderBuffer = 1 << 7,
	};
	enum ResourceTag {
		ResourceTag_Invalid = 0,	//this is an invalid resource (corrupted, can't be found, invalid, etc)
		ResourceTag_Unloaded = 1,	//this is a valid resource, but it has not been loaded into the gpu
		ResourceTag_InMemory = 2,	//this is a valid resource, and it has been loaded unto the gpu
	};
	enum MeshPrimative
	{
		MeshPrimative_null = -1,
		MeshPrimative_Poly = 0,
		MeshPrimative_Plane = 1,
		MeshPrimative_Cube = 2,
		MeshPrimative_Sphere = 3,
		MeshPrimative_HemiSphere = 4,
		MeshPrimative_Cylinder = 5,
		MeshPrimative_Cone = 6,
		MeshPrimative_Torus = 7,
		MeshPrimative_Knot = 8,
		MeshPrimative_Heightmap = 9,
		MeshPrimative_Cubicmap = 10,
		MeshPrimative_Import = 11,
	};

	enum ImagePrimative
	{
		ImagePrimative_null = -1,
		ImagePrimative_Color = 0,
		ImagePrimative_Linear = 1,
		ImagePrimative_Radial = 2,
		ImagePrimative_Square = 3,
		ImagePrimative_Checked = 4,
		ImagePrimative_WhiteNoise = 5,
		ImagePrimative_PerlinNoise = 6,
		ImagePrimative_Cellular = 7,
		ImagePrimative_Text = 8,
		ImagePrimative_Import = 9,
	};


	struct ImageGenParam
	{
		int type;
		int width, height;
		Color primary, secondary;
		int direction;			//gradient: linear
		float density;			//gradient: radial/square
		int checksX, checksY;
		float factor;
		int offsetX, offsetY;
		float scale;
		int tileSize;		//cellular
		std::string text;	//image text
		std::string filepath; //imported image
		int filter = TEXTURE_FILTER_POINT;
		int wrap = TEXTURE_WRAP_REPEAT;
	};

	Image ImageGenParam_Generate(const ImageGenParam& generator);
	struct MeshGenParam
	{
		int type;
		int sides;
		float width, height, length;
		int resX, resY;
		float radius;
		int rings, slices;
		int radSeg;
		float size;
		std::string filepath; //imported mesh from a model
		ImageGenParam imageGenParameters;
	};
	Mesh MeshGenParam_Generate(const MeshGenParam& generator);

	struct Resource
	{
		ResourceID id = 0;
		std::string filepath;
		std::string name;
		int type = ResourceType::ResourceType_Null;
		int tag = ResourceTag::ResourceTag_Invalid;

		void serialize_as_header(YAML::Emitter& out);
		void deserialize_as_header(YAML::Node& root);

		inline bool is_valid() const {
			return type != ResourceType_Null && tag == ResourceTag_InMemory;
		}

		inline operator bool() const { return is_valid(); }
	};

	struct TextureResource : public Resource
	{
		Texture2D res = { 0 };
		ImageGenParam parameters;
		operator const Texture2D& () const { return res; }

		bool Load(std::string filepath);
		bool Load(Image image);
		bool isValid() const;
		void Unload();
	};

	struct ModelResource : public Resource
	{
		Model res = { 0 };
		MeshGenParam parameters;
		operator const Model& () const { return res; }

		bool Load(std::string filepath);
		bool Load(Mesh mesh);
		bool isValid() const;
		void Unload();
	};

	struct ShaderResource : public Resource
	{
		Shader res = { 0 };
		std::string vertex, fragment;
		bool is_external = true;
		operator const Shader& () const { return res; }

		bool Load(std::string filepath);
		bool Load(Mesh mesh);
		bool isValid() const;
		void Unload();
		void Reload();
	};
}
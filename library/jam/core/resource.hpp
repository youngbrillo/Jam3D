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

	struct ImageResource
	{
		ImagePrimative type;
		std::string name;
		jam::iVec2 size;
		Color primary, secondary;
		int direction;		//gradient: linear
		float density;		//gradient: radial/square
		jam::iVec2 check;
		struct {
			float factor;	//white
			int offsetX;	//perline
			int offsetY;	//perlin
		} noise;			//white/perlin

		int tileSize;		//cellular
		std::string text;	//image text
		std::string importPath; //imported image
	};

	struct MeshResource
	{
		MeshPrimative type;
		std::string name;
		Vector3 dimensions;
		jam::iVec2 res;
		struct
		{
			float radius;
			int rings;		// Sphere| Hemisphere 
			int slices;		// Sphere| Hemisphere | Cylinder
			float size;		// Torus | Knot	
			int radSeg;		// Torus | Knot	
			int sides;		// Poly | Torus | Knot	
		} round;			//Sphere | Hemisphere | Cylinder | Cone

		struct {
			ImageResource image;
			Vector3 size;
		} map;				//Heightmap | Cubicmap


		ResourceID AlbedoTextureID = 0;
		ResourceID SpecularTextureID = 0;
		ResourceID NormalTextureID = 0;
		std::string importPath;
	};
	enum ShaderType
	{
		ShaderType_Null = -1,
		ShaderType_Screenspace = 0,
		ShaderType_Object = 1,
	};

	enum ShaderFormat
	{
		ShaderFormat_File = 0,
		ShaderFormat_Inline = 1,
	};

	struct ShaderResource
	{
		int type = ShaderType_Null;
		int format = ShaderFormat_File;
		std::string vertexLocation;
		std::string fragmentLocation;
		std::string name;
		Shader shader;
		struct {
			int time_loc = -1;
			int view_loc = -1;
		} uniforms;
		bool Reload();
	};


	Image generate_image(ImageResource res);
	Mesh generate_mesh(MeshResource res);

	ImageResource load_image_resource(const std::string& file, ResourceID* id);
	ImageResource load_image_resource(YAML::Node node, ResourceID* id);

	MeshResource load_mesh_resource(const std::string& file, ResourceID* id);
	MeshResource load_mesh_resource(YAML::Node node, ResourceID* id);


	ShaderResource load_shader_resource(const std::string& file, ResourceID* id);
	ShaderResource load_shader_resource(YAML::Node node, ResourceID* id);

}
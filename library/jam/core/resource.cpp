#include "resource.hpp"
#include "jam/serialization/yaml/serializeYAML.hpp"

const char* ResourceType_ToString(int type)
{
	switch (type)
	{
	case jam::ResourceType_Texture:
		return "texture";
	case jam::ResourceType_mesh:
		return "mesh";
	case jam::ResourceType_Shader:
		return "shader";
	case jam::ResourceType_Font:
		return "font";
	case jam::ResourceType_Sound:
		return "sound";
	case jam::ResourceType_Music:
		return "music";
	case jam::ResourceType_RenderBuffer:
		return "buffer";
	case jam::ResourceType_Null:
	default:
		return "invalid";
	}
}

int ResourceType_FromString(const std::string& string)
{
	if (string == "texture") return jam::ResourceType_Texture;
	if (string == "mesh") return jam::ResourceType_mesh;
	if (string == "shader") return jam::ResourceType_Shader;
	if (string == "font") return jam::ResourceType_Font;
	if (string == "sound") return jam::ResourceType_Sound;
	if (string == "music") return jam::ResourceType_Music;
	if (string == "buffer") return jam::ResourceType_RenderBuffer;

	return jam::ResourceType_Null;
}



namespace YAML
{
	//resource 'bindings'
	template<>
	struct convert<jam::Resource> {
		static Node encode(const jam::Resource& v){
			Node node;
			node.push_back(v.id);
			node.push_back(ResourceType_ToString(v.type));
			node.push_back(v.filepath);
			return node;
		}

		static bool decode(const Node& node, jam::Resource& v){
			if (!node.IsSequence() || node.size() < 3)
				return false;

			v.id = node[0].as<jam::UUID>();
			v.type = ResourceType_FromString(node[1].as<std::string>());
			v.filepath = node[2].as<std::string>();
		}
	};


	//ImageGenParam 'bindings'
	template<>
	struct convert<jam::ImageGenParam> {
		static Node encode(const jam::ImageGenParam& v) {
			Node node;
			node.push_back(v.type);

			if (v.type == jam::ImagePrimative_Import)
			{
				node.push_back(v.filepath);
			}
			else
			{
				node.push_back(v.width);
				node.push_back(v.height);
				switch (v.type)
				{
				case jam::ImagePrimative_Color:
					node.push_back(v.primary);
					break;
				case jam::ImagePrimative_Linear:
					node.push_back(v.direction);
					node.push_back(v.primary);
					node.push_back(v.secondary);
					break;
				case jam::ImagePrimative_Radial:
					node.push_back(v.density);
					node.push_back(v.primary);
					node.push_back(v.secondary);
					break;
				case jam::ImagePrimative_Square:
					node.push_back(v.density);
					node.push_back(v.primary);
					node.push_back(v.secondary);
					break;
				case jam::ImagePrimative_Checked:
					node.push_back(v.checksX);
					node.push_back(v.checksY);
					node.push_back(v.primary);
					node.push_back(v.secondary);
					break;
				case jam::ImagePrimative_WhiteNoise:
					node.push_back(v.factor);
					break;
				case jam::ImagePrimative_PerlinNoise:
					node.push_back(v.offsetX);
					node.push_back(v.offsetY);
					node.push_back(v.factor);
					break;
				case jam::ImagePrimative_Cellular:
					node.push_back(v.tileSize);
					break;
				case jam::ImagePrimative_Text:
					node.push_back(v.text);
					break;
				case jam::ImagePrimative_null:
				default:
					break;
				}
			}
			return node;
		}

		static bool decode(const Node& node, jam::ImageGenParam& v) {
			if (!node.IsMap())
				return false;

			jam::readValueEx(node["type"], &v.type);
			jam::readValueEx(node["width"], &v.width);
			jam::readValueEx(node["height"], &v.height);
			jam::readValueEx(node["primary"], &v.primary);
			jam::readValueEx(node["secondary"], &v.secondary);
			jam::readValueEx(node["direction"], &v.direction);
			jam::readValueEx(node["density"], &v.density);
			jam::readValueEx(node["checksX"], &v.checksX);
			jam::readValueEx(node["checksY"], &v.checksY);
			jam::readValueEx(node["factor"], &v.factor);
			jam::readValueEx(node["offsetX"], &v.offsetX);
			jam::readValueEx(node["offsetY"], &v.offsetY);
			jam::readValueEx(node["scale"], &v.scale);
			jam::readValueEx(node["tileSize"], &v.tileSize);
			jam::readValueEx(node["text"], &v.text);
			jam::readValueEx(node["filepath"], &v.filepath);
			jam::readValueEx(node["filter"], &v.filter);
			jam::readValueEx(node["wrap"], &v.wrap);

			return true;
		}
	};


	//MeshGenParam 'bindings'
	template<>
	struct convert<jam::MeshGenParam> {
		static Node encode(const jam::MeshGenParam& v) {
			Node node;
			node.push_back(v.type);

			if (v.type == jam::ImagePrimative_Import)
			{
				node.push_back(v.filepath);
			}
			else
			{
				node.push_back(v.height);
				switch (v.type)
				{
					break;
				case jam::MeshPrimative_Poly:
					node.push_back(v.sides); node.push_back(v.radius);
					break;
				case jam::MeshPrimative_Plane:
					node.push_back(v.width); node.push_back(v.length);
					node.push_back(v.resX); node.push_back(v.resY);
					break;
				case jam::MeshPrimative_Cube:
					node.push_back(v.width); node.push_back(v.height);  node.push_back(v.length);
					break;
				case jam::MeshPrimative_Sphere:
				case jam::MeshPrimative_HemiSphere:
					node.push_back(v.radius);
					node.push_back(v.rings);
					node.push_back(v.slices);
					break;
				case jam::MeshPrimative_Cylinder:
				case jam::MeshPrimative_Cone:
					node.push_back(v.radius);
					node.push_back(v.height);
					node.push_back(v.slices);
					break;
				case jam::MeshPrimative_Torus:
				case jam::MeshPrimative_Knot:
					node.push_back(v.radius);
					node.push_back(v.size);
					node.push_back(v.radSeg);
					node.push_back(v.sides);
					break;
				case jam::MeshPrimative_Heightmap:
				case jam::MeshPrimative_Cubicmap:
					node.push_back(v.imageGenParameters);
					break;
				case jam::MeshPrimative_null:
				case jam::MeshPrimative_Import:
				default:
					break;
				}
			}
			return node;
		}

		static bool decode(const Node& node, jam::MeshGenParam& v) {
			if (!node.IsMap())
				return false;

			jam::readValueEx(node["type"], &v.type);
			jam::readValueEx(node["sides"], &v.sides);
			jam::readValueEx(node["width"], &v.width);
			jam::readValueEx(node["height"], &v.height);
			jam::readValueEx(node["length"], &v.length);
			jam::readValueEx(node["resX"], &v.resX);
			jam::readValueEx(node["resY"], &v.resY);
			jam::readValueEx(node["radius"], &v.radius);
			jam::readValueEx(node["rings"], &v.rings);
			jam::readValueEx(node["slices"], &v.slices);
			jam::readValueEx(node["radSeg"], &v.radSeg);
			jam::readValueEx(node["size"], &v.size);
			jam::readValueEx(node["filepath"], &v.filepath);
			jam::readValueEx(node["image"], &v.imageGenParameters);

			return true;
		}
	};


	YAML::Emitter& operator << (YAML::Emitter& out, const jam::Resource& v)
	{
		out << YAML::Flow << YAML::BeginSeq
			<< v.id.toString()
			<< ResourceType_ToString(v.type)
			<< v.filepath
			<< YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator << (YAML::Emitter& out, const jam::ImageGenParam& v)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "type" << YAML::Value << v.type;

		if (v.type == jam::ImagePrimative_Import)
		{
			out << YAML::Key << "filepath" << YAML::Value << v.filepath;
		}
		else
		{
			out << YAML::Key << "width" << YAML::Value << v.width;
			out << YAML::Key << "height" << YAML::Value << v.height;
			switch (v.type)
			{
			case jam::ImagePrimative_Color:
				out << YAML::Key << "primary" << YAML::Value << v.primary;
				break;
			case jam::ImagePrimative_Linear:
				out << YAML::Key << "direction" << YAML::Value << v.direction;
				out << YAML::Key << "primary" << YAML::Value << v.primary;
				out << YAML::Key << "secondary" << YAML::Value << v.secondary;
				break;
			case jam::ImagePrimative_Radial:
				out << YAML::Key << "density" << YAML::Value << v.density;
				out << YAML::Key << "primary" << YAML::Value << v.primary;
				out << YAML::Key << "secondary" << YAML::Value << v.secondary;
				break;
			case jam::ImagePrimative_Square:
				out << YAML::Key << "density" << YAML::Value << v.density;
				out << YAML::Key << "primary" << YAML::Value << v.primary;
				out << YAML::Key << "secondary" << YAML::Value << v.secondary;
				break;
			case jam::ImagePrimative_Checked:
				out << YAML::Key << "checksX" << YAML::Value << v.checksX;
				out << YAML::Key << "checksY" << YAML::Value << v.checksY;
				out << YAML::Key << "primary" << YAML::Value << v.primary;
				out << YAML::Key << "secondary" << YAML::Value << v.secondary;
				break;
			case jam::ImagePrimative_WhiteNoise:
				out << YAML::Key << "factor" << YAML::Value << v.factor;
				break;
			case jam::ImagePrimative_PerlinNoise:
				out << YAML::Key << "offsetX" << YAML::Value << v.offsetX;
				out << YAML::Key << "offsetY" << YAML::Value << v.offsetY;
				out << YAML::Key << "factor" << YAML::Value << v.factor;
				break;
			case jam::ImagePrimative_Cellular:
				out << YAML::Key << "tileSize" << YAML::Value << v.tileSize;
				break;
			case jam::ImagePrimative_Text:
				out << YAML::Key << "text" << YAML::Value << v.text;
				break;
			case jam::ImagePrimative_null:
			default:
				break;
			}
		}
		if(v.filter != 0)
			out << YAML::Key << "filter" << YAML::Value << v.filter;
		if (v.wrap != 0)
			out << YAML::Key << "wrap" << YAML::Value << v.wrap;
		out << YAML::EndMap;
		return out;
	}
}



void jam::Resource::serialize_as_header(YAML::Emitter& out)
{
	out << YAML::BeginSeq << this;
}

void jam::Resource::deserialize_as_header(YAML::Node& root)
{
	if (readValueEx(root, this) && type != ResourceType_Null)
	{
		tag = ResourceTag_Unloaded;
	}
}

bool jam::TextureResource::Load(std::string Filepath)
{
	Unload();
	filepath = Filepath;
	std::string ext = TextToLower(GetFileExtension(filepath.c_str()));
	bool result = false;
	if (ext == ".yml" || ext == ".yaml" || ext == ".txtr")
	{
		YAML::Node root = LoadYamlFile(filepath);

		if(auto node = root["resource"])
		{
			readValueEx(node["id"], &id);
			readValueEx(node["name"], &name);
			readValueEx(node["texture"], &parameters);
		}
		
		if (id == 0)
			id = UUID();


		Image img = ImageGenParam_Generate(this->parameters);
		result = Load(img);
		if (result)
		{
			UnloadImage(img);
			SetTextureFilter(res, parameters.filter);
			SetTextureWrap(res, parameters.wrap);
		}
	}
	else
	{
		res = LoadTexture(filepath.c_str());
		result = isValid();

	}
	if (result) {
		tag = ResourceTag_InMemory;
		SetTextureFilter(res, parameters.filter);
		SetTextureWrap(res, parameters.wrap);
	}
	return result;
}

bool jam::TextureResource::Load(Image image)
{
	res = LoadTextureFromImage(image);
	if (isValid())
	{
		return true;
	}

	return false;
}

bool jam::TextureResource::isValid() const
{
	return IsTextureValid(res);
}

void jam::TextureResource::Unload()
{
	if (isValid())
		UnloadTexture(res);

	tag = ResourceTag_Unloaded;
}

Image jam::ImageGenParam_Generate(const ImageGenParam& res)
{
	Image img;
	switch (res.type)
	{
	case ImagePrimative_Color:
		img = GenImageColor(res.width, res.height, res.primary);
		break;
	case ImagePrimative_Linear:
		img = GenImageGradientLinear(res.width, res.height, res.direction, res.primary, res.secondary);
		break;
	case ImagePrimative_Radial:
		img = GenImageGradientRadial(res.width, res.height, res.density, res.primary, res.secondary);
		break;
	case ImagePrimative_Square:
		img = GenImageGradientSquare(res.width, res.height, res.density, res.primary, res.secondary);
		break;
	case ImagePrimative_Checked:
		img = GenImageChecked(res.width, res.height, res.checksX, res.checksY, res.primary, res.secondary);
		break;
	case ImagePrimative_WhiteNoise:
		img = GenImageWhiteNoise(res.width, res.height, res.factor);
		break;
	case ImagePrimative_PerlinNoise:
		img = GenImagePerlinNoise(res.width, res.height, res.offsetX, res.offsetY, res.factor);
		break;
	case ImagePrimative_Cellular:
		img = GenImageCellular(res.width, res.height, res.tileSize);
		break;
	case ImagePrimative_Text:
		img = GenImageText(res.width, res.height, res.text.c_str());
		break;
	case ImagePrimative_Import:
		img = LoadImage(res.filepath.c_str());
		break;
	default:
		img = GenImageColor(32, 32, PURPLE);
		break;
	}
	return img;
}

bool jam::ModelResource::Load(std::string Filepath)
{
	Unload();
	filepath = Filepath;
	std::string ext = TextToLower(GetFileExtension(filepath.c_str()));
	bool result = false;
	if (ext == ".yml" || ext == ".yaml" || ext == ".txtr")
	{
		YAML::Node root = LoadYamlFile(filepath);

		if (auto node = root["resource"])
		{
			readValueEx(node["id"], &id);
			readValueEx(node["name"], &name);
			readValueEx(node["mesh"], &parameters);
		}

		if (id == 0)
			id = UUID();

		if (type == MeshPrimative_Import)
		{
			res = LoadModel(parameters.filepath.c_str());
			result = isValid();
		}
		else
		{
			Mesh mesh = MeshGenParam_Generate(this->parameters);
			result = Load(mesh);
		}
	}
	else
	{
		res = LoadModel(filepath.c_str());
		result = isValid();
	}
	if (result) {
		tag = ResourceTag_InMemory;
	}
	return result;
}

bool jam::ModelResource::Load(Mesh mesh)
{
	res = LoadModelFromMesh(mesh);
	if (isValid())
	{
		return true;
	}
	return false;
}

bool jam::ModelResource::isValid() const
{
	return IsModelValid(res);
}

void jam::ModelResource::Unload()
{
	if (isValid())
		UnloadModel(res);

	tag = ResourceTag_Unloaded;
}


Mesh jam::MeshGenParam_Generate(const MeshGenParam& res)
{
	Mesh mesh = { 0 };
	switch (res.type)
	{
	case jam::MeshPrimative_Poly:
		mesh = GenMeshPoly(res.sides, res.radius);
		break;
	case jam::MeshPrimative_Plane:
		mesh = GenMeshPlane(res.width, res.length, res.resX, res.resY);
		break;
	case jam::MeshPrimative_Cube:
		mesh = GenMeshCube(res.width, res.height, res.length);
		break;
	case jam::MeshPrimative_Sphere:
		mesh = GenMeshSphere(res.radius, res.rings, res.slices);
		break;
	case jam::MeshPrimative_HemiSphere:
		mesh = GenMeshHemiSphere(res.radius, res.rings, res.slices);
		break;
	case jam::MeshPrimative_Cylinder:
		mesh = GenMeshCylinder(res.radius, res.height, res.slices);
		break;
	case jam::MeshPrimative_Cone:
		mesh = GenMeshCone(res.radius, res.height, res.slices);
		break;
	case jam::MeshPrimative_Torus:
		mesh = GenMeshTorus(res.radius, res.size, res.radSeg, res.sides);
		break;
	case jam::MeshPrimative_Knot:
		mesh = GenMeshKnot(res.radius, res.size, res.radSeg, res.sides);
		break;
	case jam::MeshPrimative_Heightmap: {
		Image img = ImageGenParam_Generate(res.imageGenParameters);
		mesh = GenMeshHeightmap(img, Vector3{ 1,1,1 });
		UnloadImage(img);
		}
		break;
	case jam::MeshPrimative_Cubicmap: {
		Image img = ImageGenParam_Generate(res.imageGenParameters);
		mesh = GenMeshCubicmap(img, Vector3{ 1,1,1 });
		UnloadImage(img);
		}
		break;
	case jam::MeshPrimative_Import:
	default:
		mesh = GenMeshCube(0.5f, 1, 0.5f);
		break;
	}
	return mesh;
}
#include "resource_inspector.hpp"
#include "modules.hpp"
#include "jam/core/resourceManager.hpp"
#include "modules.hpp"
#include "editor/imgui/imgui_stdlib.h"
#include "editor/rlImgui/rlImGui.h"
#include "editor/editor.hpp"
#include "editor/tools/dragResources.hpp"
#include "editor/tools/fileTools.hpp"

void jam::editor::ShowSetTextureFilter(const Texture& texture)
{
	if (ImGui::Button("Point"))
		SetTextureFilter(texture, TEXTURE_FILTER_POINT);

	ImGui::SameLine();
	if (ImGui::Button("Bi"))
		SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);

	ImGui::SameLine();
	if (ImGui::Button("Tri"))
		SetTextureFilter(texture, TEXTURE_FILTER_TRILINEAR);

	ImGui::SameLine();
	if (ImGui::Button("A4x"))
		SetTextureFilter(texture, TEXTURE_FILTER_ANISOTROPIC_4X);

	ImGui::SameLine();
	if (ImGui::Button("A8x"))
		SetTextureFilter(texture, TEXTURE_FILTER_ANISOTROPIC_8X);

	ImGui::SameLine();
	if (ImGui::Button("A16x"))
		SetTextureFilter(texture, TEXTURE_FILTER_ANISOTROPIC_16X);

	ImGui::SameLine();
	if (ImGui::Button("Mip"))
		GenTextureMipmaps((Texture*)&texture);

}
void jam::editor::InspectTexture(const Texture& texture, float width)
{
	if (width <= 0)
		width = ImGui::GetContentRegionAvail().x;

	float height = width * (texture.height / (float)texture.width);
	rlImGuiImageSize(&texture, (int)width, (int)height);
	ImGui::Text("ID:%d W:%d H:%d", texture.id, texture.width, texture.height);
	if (texture.mipmaps > 1)
		ImGui::Text("Mipmap Levels:%d", texture.mipmaps);

	ShowSetTextureFilter(texture);
}
static const char* s_imagePrimative_Names[] = {
	"Color",
	"Linear",
	"Radial",
	"Square",
	"Checked",
	"White Noise",
	"Perlin Noise",
	"Cellular",
	"Text",
	"Import",
	"Cubemap",
};
static bool InspectImageGenParam(jam::ImageGenParam& p)
{
	ImGui::Combo("type", &p.type, s_imagePrimative_Names, 10);

	ImGui::DragInt("width", &p.width);
	ImGui::DragInt("height", &p.height);

	switch (p.type)
	{
	case jam::ImagePrimative_Color:
		jam::editor::EditColor("Primary", &p.primary);
		break;
	case jam::ImagePrimative_Linear:
		ImGui::DragInt("direction", &p.direction);
		jam::editor::EditColor("Primary", &p.primary);
		jam::editor::EditColor("Secondary", &p.secondary);
		break;
	case jam::ImagePrimative_Radial:
	case jam::ImagePrimative_Square:
		ImGui::DragFloat("density", &p.density);
		jam::editor::EditColor("Primary", &p.primary);
		jam::editor::EditColor("Secondary", &p.secondary);
		break;
	case jam::ImagePrimative_Checked:
		ImGui::DragInt("checks X", &p.checksX);
		ImGui::DragInt("checks Y", &p.checksY);
		jam::editor::EditColor("Primary", &p.primary);
		jam::editor::EditColor("Secondary", &p.secondary);
		break;
	case jam::ImagePrimative_WhiteNoise:
		ImGui::DragFloat("factor", &p.factor);
		break;
	case jam::ImagePrimative_PerlinNoise:
		ImGui::DragInt("offset X", &p.offsetX);
		ImGui::DragInt("offset Y", &p.offsetY);
		ImGui::DragFloat("factor", &p.factor);
		break;
	case jam::ImagePrimative_Cellular:
		ImGui::DragInt("Tile size", &p.tileSize);
		break;
	case jam::ImagePrimative_Text:
		ImGui::InputText("text", &p.text);
		break;
	case jam::ImagePrimative_Import:
	case jam::ImagePrimative_CubeMap:
		ImGui::InputText("file path###ImgGen", &p.filepath);
		ImGui::SameLine();
		if (ImGui::Button("Open###ImgGen"))
		{
			std::string openLoc = jam::editor::tools::ShowOpenFileDialog("");
			if (!openLoc.empty()) {
				p.filepath = openLoc;
			}
		}
		break;
	case jam::ImagePrimative_null:
	default:

		break;
	}


	return ImGui::Button("Regenerate");
}

static const char* s_MeshPrimative_Names[] = {
	"Poly",
	"Plane",
	"Cube",
	"Sphere",
	"Hemisphere",
	"Cylinder",
	"Cone",
	"Torus",
	"Knot",
	"Heightmap",
	"Cubicmap",
	"Import",
};

static bool InspectMeshGenParam(jam::MeshGenParam& p)
{
	ImGui::Combo("type###mesh_primative", &p.type, s_MeshPrimative_Names, 12);
	bool canGen = false;
	switch (p.type)
	{
	case jam::MeshPrimative_Poly:
		ImGui::DragInt("sides", &p.sides);
		ImGui::DragFloat("radius", &p.radius);
		break;
	case jam::MeshPrimative_Plane:
		ImGui::DragFloat("width", &p.width);
		ImGui::DragFloat("height", &p.height);
		ImGui::DragInt("res X", &p.resX);
		ImGui::DragInt("res Y", &p.resY);
		break;
	case jam::MeshPrimative_Cube:
		ImGui::DragFloat("width", &p.width);
		ImGui::DragFloat("height", &p.height);
		ImGui::DragFloat("length", &p.length);
		break;
	case jam::MeshPrimative_Sphere:
	case jam::MeshPrimative_HemiSphere:
		ImGui::DragFloat("radius", &p.radius);
		ImGui::DragInt("rings", &p.rings);
		ImGui::DragInt("slices", &p.slices);
		break;
	case jam::MeshPrimative_Cylinder:
	case jam::MeshPrimative_Cone:
		ImGui::DragFloat("radius", &p.radius);
		ImGui::DragFloat("height", &p.height);
		ImGui::DragInt("slices", &p.slices);
		break;
	case jam::MeshPrimative_Torus:
	case jam::MeshPrimative_Knot:
		ImGui::DragFloat("radius", &p.radius);
		ImGui::DragFloat("size", &p.size);
		ImGui::DragInt("radSeg", &p.radSeg);
		ImGui::DragInt("sides", &p.sides);
		break;
	case jam::MeshPrimative_Heightmap:
	case jam::MeshPrimative_Cubicmap:
		canGen = InspectImageGenParam(p.imageGenParameters);
		break;
	case jam::MeshPrimative_Import:
		ImGui::InputText("file path###MeshGenInput", &p.filepath);
		ImGui::SameLine();
		if (ImGui::Button("Open###MeshGenButton"))
		{
			std::string openLoc = jam::editor::tools::ShowOpenFileDialog("");
			if (!openLoc.empty()) {
				p.filepath = openLoc;
			}
		}
		break;
	case jam::MeshPrimative_null:
	default:
		break;
	}

	canGen = canGen || ImGui::Button("Generate Mesh");

	return canGen;
}


void jam::editor::InspectMesh(Mesh* mesh)
{
	ImGui::Text("Verts: %d Tris:%d", mesh->vertexCount, mesh->triangleCount);
}

static bool s_show_empty_maps = false;
void jam::editor::InspectMaterial(Material* mat, UUID* resourceID)
{
	ImGui::Checkbox("show empty maps", &s_show_empty_maps);
	if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		InspectMaterialMap("Albedo", mat->maps[MATERIAL_MAP_ALBEDO], resourceID);
		InspectMaterialMap("Metalness", mat->maps[MATERIAL_MAP_METALNESS], resourceID);
		InspectMaterialMap("Normal", mat->maps[MATERIAL_MAP_NORMAL], resourceID);
		InspectMaterialMap("rougness", mat->maps[MATERIAL_MAP_ROUGHNESS], resourceID);
		InspectMaterialMap("occlusion", mat->maps[MATERIAL_MAP_OCCLUSION], resourceID);
		InspectMaterialMap("emission", mat->maps[MATERIAL_MAP_EMISSION], resourceID);
		InspectMaterialMap("Height map", mat->maps[MATERIAL_MAP_HEIGHT], resourceID);
		InspectMaterialMap("cubemap", mat->maps[MATERIAL_MAP_CUBEMAP], resourceID);
		InspectMaterialMap("Irradiance", mat->maps[MATERIAL_MAP_IRRADIANCE], resourceID);
		InspectMaterialMap("Prefilter", mat->maps[MATERIAL_MAP_PREFILTER], resourceID);
		InspectMaterialMap("BRDF", mat->maps[MATERIAL_MAP_BRDF], resourceID);

		ImGui::TreePop();
	}
}

void jam::editor::InspectMaterialMap(const char* label, MaterialMap& item, UUID* resourceID)
{
	if (item.texture.id == 0 && !s_show_empty_maps) 
	{
		return;
	}
	if (ImGui::TreeNode(label))
	{
		ImColor color = editor::EditColor("color", &item.color);
		if (resourceID != nullptr)
		{
			if (editor::tools::DragAndDropTextureRecieve(item.texture, *resourceID))
			{
				TraceLog(LOG_INFO, TextFormat("the %s material texture has changed!", label));
			}

		}
		ImGui::DragFloat("Value", &item.value, 0.001f, 0, 1.0f);
		ImGui::TreePop();
	}
}


void jam::editor::InspectTextureResource(jam::TextureResource& resource)
{
	if (InspectImageGenParam(resource.parameters))
	{
		Image img = jam::ImageGenParam_Generate(resource.parameters);
		resource.Load(img);
		UnloadImage(img);
	}

	ImGui::Separator();
	InspectTexture(resource.res, 150);
}

void jam::editor::InspectModelResource(jam::ModelResource& resource)
{
	if (InspectMeshGenParam(resource.parameters))
	{

		if (resource.parameters.type == MeshPrimative_Import)
		{
			resource.Load(resource.parameters.filepath);
		}
		else
		{
			Mesh m = MeshGenParam_Generate(resource.parameters);
			resource.Load(m);
		}


	}

	ImGui::Separator();

	if(IsModelValid(resource.res))
		InspectMesh(&resource.res.meshes[0]);
}

const static jam::UUID c_null_resource_id = 99;
const static jam::UUID c_null_resource_mesh_id= 99;

static jam::UUID s_selected_resource_id = c_null_resource_id;
static jam::UUID s_selected_mesh_resource_id = c_null_resource_id;

namespace jam
{
	class Editor
	{
	public:
		void render(ResourceManager& rm)
		{
			ImGui::SeparatorText("Textures");
			for (auto&& [id, resource] : rm.textures)
			{
				if (ImGui::Selectable(resource.name.c_str(), id == s_selected_resource_id))
				{
					if (s_selected_resource_id == id)
						s_selected_resource_id = c_null_resource_id;
					else
						s_selected_resource_id = id;

				}
				editor::tools::DragAndDropTextureGive(resource);

			}
			ImGui::SeparatorText("Meshes");
			for (auto&& [id, resource] : rm.meshes)
			{
				if (ImGui::Selectable(resource.name.c_str(), id == s_selected_mesh_resource_id))
				{
					if (s_selected_mesh_resource_id == id)
						s_selected_mesh_resource_id = c_null_resource_id;
					else
						s_selected_mesh_resource_id = id;
				}
				editor::tools::DragAndDropMeshGive(resource);
			}


			if (s_selected_resource_id != c_null_resource_id)
			{
				auto& res = rm.textures[s_selected_resource_id];

				editor::InspectTextureResource(res);
			}
			if (s_selected_mesh_resource_id  != c_null_resource_id)
			{
				auto& res = rm.meshes[s_selected_mesh_resource_id ];

				editor::InspectModelResource(res);
			}


		}
	};
}


bool jam::editor::renderResourceInspector(EditorLayer& editorLayer, EditorSettings& s, Scene* scene)
{
	if (!s.resourceInspectorVisible)
		return false;
	static jam::Editor editor;

	auto& rm = ResourceManager::Get();
	ImGui::Begin("Resources", &s.resourceInspectorVisible);

	editor.render(rm);

	ImGui::End();
	return false;
}
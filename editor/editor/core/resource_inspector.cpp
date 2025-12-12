#include "modules.hpp"
#include "jam/core/resourceManager.hpp"
#include "modules.hpp"
#include "editor/rlImgui/rlImGui.h"
#include "editor/editor.hpp"

inline void ShowSetTextureFilter(const Texture& texture)
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
static void InspectTexture(const Texture& texture, float width = 0)
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
		break;
	case jam::ImagePrimative_Import:
		break;
	case jam::ImagePrimative_CubeMap:
		break;
	case jam::ImagePrimative_null:
	default:

		break;
	}


	return ImGui::Button("Regenerate");
}

static void InspectTextureResource(jam::TextureResource& resource)
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
const static jam::UUID c_null_resource_id= 99;

static jam::UUID s_selected_resource_id = c_null_resource_id;

namespace jam
{
	class Editor
	{
	public:
		void render(ResourceManager& rm)
		{
			for (auto&& [id, resource] : rm.textures)
			{
				if (ImGui::Selectable(resource.name.c_str(), id == s_selected_resource_id))
				{
					if (s_selected_resource_id == id)
						s_selected_resource_id = c_null_resource_id;
					else
						s_selected_resource_id = id;
				}
			}


			if (s_selected_resource_id != c_null_resource_id)
			{
				auto& res = rm.textures[s_selected_resource_id];

				InspectTextureResource(res);
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
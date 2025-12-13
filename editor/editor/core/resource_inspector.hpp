#pragma once
#include "jam/core/resourceManager.hpp"

namespace jam::editor
{
	void ShowSetTextureFilter(const Texture& texture);
	void InspectTexture(const Texture& texture, float width = 0);

	void InspectMesh(Mesh* mesh);
	void InspectMaterial(Material* material);
	void InspectMaterialMap(const char* label, MaterialMap& materialMap);
	void InspectTextureResource(jam::TextureResource& resource);
}
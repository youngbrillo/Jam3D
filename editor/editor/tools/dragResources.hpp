#pragma once

#include "jam/core/resource.hpp"
#include <imgui.h>

namespace jam::editor::tools
{
	bool DragAndDropTextureGive(jam::TextureResource& resource, const char* type = "TEXTURE_RESOURCE");
	bool DragAndDropTextureRecieve(Texture& target, UUID& id, const char* type = "TEXTURE_RESOURCE");

	bool DragAndDropMeshGive(jam::ModelResource& resource, const char* type = "MESH_RESOURCE");
	bool DragAndDropMeshRecieve(Mesh** target, UUID& id, Material* material,  int meshIndex = 0, const char* type = "MESH_RESOURCE");

}
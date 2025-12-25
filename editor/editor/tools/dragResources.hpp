#pragma once

#include "jam/core/resource.hpp"
#include <imgui.h>

namespace jam::editor::tools
{
	bool DragAndDropTextureGive(jam::TextureResource& resource, const char* type = "TEXTURE_RESOURCE");
	bool DragAndDropTextureRecieve(Texture& texture, UUID& id, const char* type = "TEXTURE_RESOURCE");

}
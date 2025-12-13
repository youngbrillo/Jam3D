#pragma once
#include <raylib.h>
#include "jam/core/uuid.hpp"

namespace jam::components
{
	struct MeshInstance3D
	{
		UUID mesh_id = 0;
		UUID material_id = 0;
		UUID shader_id = 0;
		Mesh* mesh = nullptr;
		Material material = LoadMaterialDefault();

		void Render(Matrix matrix);
	};
}
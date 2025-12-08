#pragma once
#include <raylib.h>
#include "jam/core/uuid.hpp"

namespace jam::components
{
	struct MeshInstance3D
	{
		UUID id = 0;
		Mesh* mesh = nullptr;
		Material material = LoadMaterialDefault();

		void Render(Matrix matrix);
	};
}
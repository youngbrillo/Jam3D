#include "mesh3d.hpp"

void jam::components::MeshInstance3D::Render(Matrix matrix)
{
	if(mesh)
		DrawMesh(*mesh, material, matrix);
}

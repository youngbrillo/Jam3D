#include "cameraComponent3d.hpp"
#include "transform3d.hpp"

jam::components::SceneCamera3D::SceneCamera3D()
{
	camera.position = Vector3{ 0,10,10 };
	camera.target = Vector3{ 0,0,0 };
	camera.up = Vector3{ 0,1,0 };
	camera.fovy = 75.0f;
	camera.projection = CameraProjection::CAMERA_PERSPECTIVE;

}

void jam::components::CameraEditorComponent::Update(Transform3D& transform, Camera3D& camera, const Clock& clock)
{
	
}

void jam::components::EditorCamera::Update()
{
}

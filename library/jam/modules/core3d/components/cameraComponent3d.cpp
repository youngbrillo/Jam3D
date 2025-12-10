#include "cameraComponent3d.hpp"
#include "transform3d.hpp"
#include "jam/app/application.hpp"

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
	camera.position = camera.position;
	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
	{
		UpdateCamera(&camera, mode);
	}
	transform.position = camera.position;
}

void jam::components::EditorCamera::Update()
{
	editor.Update(transform, camera.camera, Application::Instance()->GetTime());
}

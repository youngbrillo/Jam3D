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
#if true
	camera.position = camera.position;
	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
	{
		UpdateCamera(&camera, mode);
	}

	transform.position = camera.position;
	transform.LookAt(camera.target);
#else
	if (!enabled)
		return;
	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
		lmp = GetMousePosition();
		DisableCursor();
		HideCursor();
		return;
	}

	if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
		EnableCursor();
		ShowCursor();
	}

	if (!IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
		return;

	Vector2 cmp = GetMousePosition();
	Vector2 mouse_delta = lmp - cmp;
	lmp = cmp;

	//rotation
	float yaw = mouse_delta.x * look_speed * look_sensitivity;
	float pitch = mouse_delta.y * look_speed * look_sensitivity;

	Quaternion qYaw = QuaternionFromAxisAngle(Vector3{ 0,1,0 }, -yaw * DEG2RAD);
	Quaternion qPitch = QuaternionFromAxisAngle(Vector3{ 1, 0,0 }, pitch * DEG2RAD);

	Quaternion cOrientation = transform.orientation;
	cOrientation = QuaternionMultiply(cOrientation, qYaw);
	cOrientation = QuaternionMultiply(cOrientation, qPitch);
	cOrientation = QuaternionNormalize(cOrientation);

	transform.orientation = cOrientation;
	
	//translation
	Vector3 forward = transform.Front();
	Vector3 right = transform.Right() * -1.0f;
	
	int forward_dir	= IsKeyDown(KEY_W) - IsKeyDown(KEY_S);
	int right_dir	= IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
	int up_dir		= IsKeyDown(KEY_SPACE) - IsKeyDown(KEY_LEFT_CONTROL);

	float vel_x = right_dir * move_speed * clock.deltaTime;
	float vel_y = up_dir * move_speed * clock.deltaTime;
	float vel_z = forward_dir * move_speed * clock.deltaTime;

	//move forward
	transform.position += forward * vel_z;

	//move sideways
	transform.position += right * vel_x;

	//move vertically
	transform.position += Vector3{ 0, 1,0 } *vel_y;

	float zoom = GetMouseWheelMove();
	if (zoom)
		transform.position += zoom > 0 ? forward * zoom_multiplier : forward * -zoom_multiplier;

#endif
}

void jam::components::EditorCamera::Update()
{
	editor.Update(transform, camera.camera, Application::Instance()->GetTime());
	//It keeps messing up... oh well, this is all smoke and mirrors anyhow
	
	//camera.camera.position = transform.position;
	//camera.camera.target = transform.position + transform.Front();
}

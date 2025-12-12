#pragma once
#include <raylib.h>
#include "transform3d.hpp"
#include "jam/core/clock.hpp"

namespace jam::components
{
	struct SceneCamera3D
	{
		SceneCamera3D();
		SceneCamera3D(const SceneCamera3D& other) = default;

		Camera3D camera;
	};

	struct CameraEditorComponent
	{
		bool enabled = true;
		float move_speed = 5.5f;
		float sprint_speed = 10.0f;
		float look_speed = 30.0f;
		float look_sensitivity = 0.01f;
		float zoom_multiplier = 1.1f;
		int mode = CameraMode::CAMERA_FREE;

		Vector2 lmp;	//last mouse position
		void Update(Transform3D& transform, Camera3D& camera, const Clock& clock);
	};


	struct EditorCamera
	{
		Transform3D transform;
		SceneCamera3D camera;
		CameraEditorComponent editor;

		void Update();
	};

	struct ActiveCameraTag { char intern; };
}
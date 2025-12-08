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
		float speed = 3.0f;
		float flyMultiplier = 1.5f;
		int mode = CameraMode::CAMERA_FREE;

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
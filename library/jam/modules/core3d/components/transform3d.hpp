#pragma once
#include <raylib.h>
#include <raymath.h>

namespace jam::components
{
	struct Transform3D
	{
		Vector3 position;
		Vector3 size;
		Quaternion orientation;
		float   scale;

		Transform3D();
		Transform3D(Vector3 Position);
		Transform3D(Vector3 Position, Vector3 Size);
		Transform3D(Vector3 Position, Vector3 Size, Quaternion Orientation);
		Transform3D(const Transform3D& other) = default;

		Matrix  toMatrix() const;
		Vector3 GetEuler() const;
		void SetEuler(Vector3 euler);
		Vector3 Front() const;
		Vector3 Right() const;
		Vector3 Up() const;

		Transform toTransform() const;
	};
}
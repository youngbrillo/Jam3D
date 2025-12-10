#pragma once
#include <raylib.h>
#include <raymath.h>

namespace jam::components
{
	struct Transform2D
	{
		Vector2 position = { 0,0 };
		Vector2 size = { 1,1 };
		Vector2 origin = { 0.5f, 0.5f };
		float scale = 1.0f;
		float angle = 0.0f;


		Transform2D() {};
		Transform2D(float x, float y) : position({ x,y }) {}
		Transform2D(Vector2 v) : position(v) {};
		Transform2D(const Transform2D& o) = default;

		Rectangle	getRectangle() const;
		Rectangle	getAnchoredRectangle() const;
		Vector2		getOrigin() const;
		Vector2		getHeading() const;
	};

	//struct Velocity2D {
	//	Vector2 linear = { 0,0 };
	//	float	angluar = 0.0f;

	//	void move(Vector2& position, float& rotation, float deltaTime) const;
	//};

	//struct Acceleration2D {
	//	Vector2 acceleration = { 0,0 };
	//	float torque = 0.0f;

	//	void addForce(Vector2 force);
	//	void addTorque(float direction);
	//	void apply(Velocity2D& velocity);
	//};

	//struct VelocityLimit {
	//	float maximumMagnitude = 5.0f;
	//};
}
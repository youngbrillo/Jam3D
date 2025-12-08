#pragma once
#include <raylib.h>

#include <map>
#include <vector>
#include <string>

namespace jam::io
{
	enum InputEvent_
	{
		InputEvent_Key = 0,
		InputEvent_MouseButton,
		InputEvent_GamepadButton,
		InputEvent_GamepadJoystick
	};

	struct InputEvent
	{
		int type = InputEvent_Key;
		int value = 0;
	};

	class Input
	{
		std::map < std::string, std::vector<InputEvent>> map;
		int defaultgamepad = 0;

		// Set axis deadzones
		float leftStickDeadzoneX = 0.1f;
		float leftStickDeadzoneY = 0.1f;
		float rightStickDeadzoneX = 0.1f;
		float rightStickDeadzoneY = 0.1f;
		float leftTriggerDeadzone = -0.9f;
		float rightTriggerDeadzone = -0.9f;
		static Input* instance;

		Input();
		~Input();
	public:

		static Input& Get();
		static void ShutDown();

		//generic helpers
		void RegisterInput(std::string, int type, int value);
		bool isActionPressed(std::string label);
		bool isActionDown(std::string label);
		bool isActionReleased(std::string label);
		bool isActionRepeated(std::string label);
		//common helpers


		//wasd, left joystick
		Vector2 GetMoveDirection();
		//get x and y values as -1, 0, 1 only
		Vector2 GetMoveDirectionClamped();
		Vector2 GetPressedMoveDirection();
		Vector2 GetReleasedMoveDirection();
		Vector2 GetMoveDirectionJoystick();
		Vector2 GetDpad_HeldDirection();
		Vector2 GetDpad_PressedDirection();
		Vector2 GetDpad_ReleasedDirection();
		//'mouse movement', right joystick
		Vector2 GetLookDirection();

		bool isConfirmedPressed();
		bool isConfirmedReleased();

		bool isCancelPressed();
		bool isCancelReleased();

		float getRightTrigger();
		float getLeftTrigger();

		//only restarts scene...
		bool getRestartCommand();
	};

}
#include "InputHandler.hpp"
jam::io::Input* jam::io::Input::instance = nullptr;

jam::io::Input::Input()
{
	//left
	RegisterInput("left", InputEvent_Key, KEY_A);
	RegisterInput("left", InputEvent_Key, KEY_LEFT);
	RegisterInput("left", InputEvent_GamepadButton, GAMEPAD_BUTTON_LEFT_FACE_LEFT);

	//right
	RegisterInput("right", InputEvent_Key, KEY_D);
	RegisterInput("right", InputEvent_Key, KEY_RIGHT);
	RegisterInput("right", InputEvent_GamepadButton, GAMEPAD_BUTTON_LEFT_FACE_RIGHT);

	//up
	RegisterInput("up", InputEvent_Key, KEY_W);
	RegisterInput("up", InputEvent_Key, KEY_UP);
	RegisterInput("up", InputEvent_GamepadButton, GAMEPAD_BUTTON_LEFT_FACE_UP);

	//down
	RegisterInput("down", InputEvent_Key, KEY_S);
	RegisterInput("down", InputEvent_Key, KEY_DOWN);
	RegisterInput("down", InputEvent_GamepadButton, GAMEPAD_BUTTON_LEFT_FACE_DOWN);

	//down
	RegisterInput("confirm", InputEvent_Key, KEY_Z);
	RegisterInput("confirm", InputEvent_Key, KEY_ENTER);
	RegisterInput("confirm", InputEvent_GamepadButton, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);

	//down
	RegisterInput("cancel", InputEvent_Key, KEY_X);
	RegisterInput("cancel", InputEvent_Key, KEY_BACKSPACE);
	RegisterInput("cancel", InputEvent_GamepadButton, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT);

	RegisterInput("fire", InputEvent_GamepadButton, GAMEPAD_BUTTON_RIGHT_FACE_LEFT);
	RegisterInput("fire1", InputEvent_GamepadButton, GAMEPAD_BUTTON_RIGHT_FACE_LEFT);
	RegisterInput("fire2", InputEvent_GamepadButton, GAMEPAD_BUTTON_RIGHT_FACE_UP);

}

jam::io::Input::~Input()
{
}

jam::io::Input& jam::io::Input::Get()
{
    // TODO: insert return statement here
    if (instance == nullptr)
        instance = new Input();
    return *instance;
}

void jam::io::Input::ShutDown()
{
    if (instance)
        delete instance;
    instance = nullptr;
}

void jam::io::Input::RegisterInput(std::string label, int type, int value)
{
	map[label].emplace_back(InputEvent{ .type = type, .value = value });
}

bool jam::io::Input::isActionPressed(std::string label)
{
	if (map.find(label) == map.end()) return false;

	for (auto& e : map[label])
	{
		switch (e.type)
		{
		case jam::io::InputEvent_Key:
			if (IsKeyPressed(e.value)) 
				return true;
			break;
		case jam::io::InputEvent_MouseButton:
			if (IsMouseButtonPressed(e.value)) 
				return true;
			break;
		case jam::io::InputEvent_GamepadButton:
			if (IsGamepadAvailable(defaultgamepad) && IsGamepadButtonPressed(defaultgamepad, e.value)) 
				return true;
			break;
		case jam::io::InputEvent_GamepadJoystick:
			break;
		default:
			break;
		}
	}
	return false;
}

bool jam::io::Input::isActionDown(std::string label)
{
	if (map.find(label) == map.end()) return false;

	for (auto& e : map[label])
	{
		switch (e.type)
		{
		case jam::io::InputEvent_Key:
			if (IsKeyDown(e.value)) 
				return true;
			break;
		case jam::io::InputEvent_MouseButton:
			if (IsMouseButtonDown(e.value)) 
				return true;
			break;
		case jam::io::InputEvent_GamepadButton:
			if (IsGamepadAvailable(defaultgamepad) && IsGamepadButtonDown(defaultgamepad, e.value)) 
				return true;
			break;
		case jam::io::InputEvent_GamepadJoystick:
			break;
		default:
			break;
		}
	}
	return false;
}

bool jam::io::Input::isActionReleased(std::string label)
{
	if (map.find(label) == map.end()) return false;

	for (auto& e : map[label])
	{
		switch (e.type)
		{
		case jam::io::InputEvent_Key:
			if (IsKeyReleased(e.value)) 
				return true;
			break;
		case jam::io::InputEvent_MouseButton:
			if (IsMouseButtonReleased(e.value)) 
				return true;
			break;
		case jam::io::InputEvent_GamepadButton:
			if (IsGamepadAvailable(defaultgamepad) && IsGamepadButtonReleased(defaultgamepad, e.value)) 
				return true;
			break;
		case jam::io::InputEvent_GamepadJoystick:
			break;
		default:
			break;
		}
	}
	return false;
}

bool jam::io::Input::isActionRepeated(std::string label)
{
	if (map.find(label) == map.end()) return false;

	for (auto& e : map[label])
	{
		switch (e.type)
		{
		case jam::io::InputEvent_Key:
			if (IsKeyPressedRepeat(e.value))
				return true;
			break;
		}
	}
	return false;
}

Vector2 jam::io::Input::GetMoveDirection()
{
	Vector2 v;
	v.x = (this->isActionDown("right") ? 1 : 0) - (this->isActionDown("left") ? 1 : 0);
	v.y = (this->isActionDown("down") ? 1 : 0) - (this->isActionDown("up") ? 1 : 0);

	return v;
}

Vector2 jam::io::Input::GetMoveDirectionClamped()
{
	Vector2 v = GetMoveDirectionJoystick();
	if (v.x > 0) v.x = 1;
	else if (v.x < 0) v.x = -1;
	else v.x = 0;

	if (v.y > 0) v.y = 1;
	else if (v.y < 0) v.y = -1;
	else v.y = 0;

	return v;
}

Vector2 jam::io::Input::GetPressedMoveDirection()
{
	Vector2 v;
	v.x = (this->isActionPressed("right") ? 1 : 0) - (this->isActionPressed("left") ? 1 : 0);
	v.y = (this->isActionPressed("down") ? 1 : 0) - (this->isActionPressed("up") ? 1 : 0);

	return v;
}

Vector2 jam::io::Input::GetReleasedMoveDirection()
{
	Vector2 v;
	v.x = (this->isActionReleased("right") ? 1 : 0) - (this->isActionReleased("left") ? 1 : 0);
	v.y = (this->isActionReleased("down") ? 1 : 0) - (this->isActionReleased("up") ? 1 : 0);

	return v;
}

Vector2 jam::io::Input::GetMoveDirectionJoystick()
{
	Vector2 v = { 0.0f, 0.0f };

	if (!IsGamepadAvailable(defaultgamepad))
		return GetMoveDirection();

	float leftStickX = GetGamepadAxisMovement(defaultgamepad, GAMEPAD_AXIS_LEFT_X);
	float leftStickY = GetGamepadAxisMovement(defaultgamepad, GAMEPAD_AXIS_LEFT_Y);
	// Calculate deadzones
	if (leftStickX > -leftStickDeadzoneX && leftStickX < leftStickDeadzoneX) leftStickX = 0.0f;
	if (leftStickY > -leftStickDeadzoneY && leftStickY < leftStickDeadzoneY) leftStickY = 0.0f;

	v.x = leftStickX;
	v.y = leftStickY;

	return v;
}

Vector2 jam::io::Input::GetDpad_HeldDirection()
{
	if (!IsGamepadAvailable(defaultgamepad))
		return Vector2(0.0f, 0.0f);

	Vector2 dir;
	dir.x = IsGamepadButtonDown(defaultgamepad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) - IsGamepadButtonDown(defaultgamepad, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
	dir.y = IsGamepadButtonDown(defaultgamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN) - IsGamepadButtonDown(defaultgamepad, GAMEPAD_BUTTON_LEFT_FACE_UP);
	return dir;
}

Vector2 jam::io::Input::GetDpad_PressedDirection()
{
	if (!IsGamepadAvailable(defaultgamepad))
		return Vector2(0.0f, 0.0f);

	Vector2 dir;
	dir.x = IsGamepadButtonPressed(defaultgamepad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) - IsGamepadButtonPressed(defaultgamepad, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
	dir.y = IsGamepadButtonPressed(defaultgamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN) - IsGamepadButtonPressed(defaultgamepad, GAMEPAD_BUTTON_LEFT_FACE_UP);
	return dir;
}

Vector2 jam::io::Input::GetDpad_ReleasedDirection()
{
	if (!IsGamepadAvailable(defaultgamepad))
		return Vector2(0.0f, 0.0f);

	Vector2 dir;
	dir.x = IsGamepadButtonReleased(defaultgamepad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) - IsGamepadButtonReleased(defaultgamepad, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
	dir.y = IsGamepadButtonReleased(defaultgamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN) - IsGamepadButtonReleased(defaultgamepad, GAMEPAD_BUTTON_LEFT_FACE_UP);
	return dir;
}

Vector2 jam::io::Input::GetLookDirection()
{
	Vector2 v = { 0.0f, 0.0f };

	if (!IsGamepadAvailable(defaultgamepad))
		return v;

	v.x = GetGamepadAxisMovement(defaultgamepad, GAMEPAD_AXIS_RIGHT_X);
	v.y = GetGamepadAxisMovement(defaultgamepad, GAMEPAD_AXIS_RIGHT_Y);

	return v;
}

bool jam::io::Input::isConfirmedPressed()
{
	return this->isActionPressed("confirm");
}

bool jam::io::Input::isConfirmedReleased()
{
	return this->isActionReleased("confirm");
}

bool jam::io::Input::isCancelPressed()
{
	return this->isActionPressed("cancel");
}

bool jam::io::Input::isCancelReleased()
{
	return this->isActionReleased("cancel");
}

float jam::io::Input::getRightTrigger()
{
	if (IsGamepadAvailable(defaultgamepad))
		return GetGamepadAxisMovement(defaultgamepad, GAMEPAD_AXIS_RIGHT_TRIGGER);
	return 0.0f;
}

float jam::io::Input::getLeftTrigger()
{
	if (IsGamepadAvailable(defaultgamepad))
		return GetGamepadAxisMovement(defaultgamepad, GAMEPAD_AXIS_LEFT_TRIGGER);
	return 0.0f;
}

bool jam::io::Input::getRestartCommand()
{
	bool controllerCommands = false;
	if (IsGamepadAvailable(defaultgamepad))
	{
		controllerCommands =
			IsGamepadButtonDown(defaultgamepad, GAMEPAD_BUTTON_MIDDLE_LEFT)
			&& IsGamepadButtonDown(defaultgamepad, GAMEPAD_BUTTON_MIDDLE_RIGHT)
			&& IsGamepadButtonDown(defaultgamepad, GAMEPAD_BUTTON_LEFT_TRIGGER_1)
			&& IsGamepadButtonDown(defaultgamepad, GAMEPAD_BUTTON_LEFT_TRIGGER_2)
			&& GetGamepadAxisMovement(defaultgamepad, GAMEPAD_AXIS_RIGHT_TRIGGER) >= 1.0f
			&& GetGamepadAxisMovement(defaultgamepad, GAMEPAD_AXIS_LEFT_TRIGGER) >= 1.0f
			;
	}

	bool keyCommands = IsKeyDown(KEY_LEFT_CONTROL) && IsKeyReleased(KEY_R);
	return keyCommands || controllerCommands;
}

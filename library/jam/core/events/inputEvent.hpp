#pragma once
#include "jam/core/event.h"
#include <format>
namespace jam
{
	//key events
	class KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const {}
	protected:
		KeyEvent(int keycode) : _key_code(keycode) {}
		int _key_code;
	};


	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, bool isRepeat)
			: KeyEvent(keycode), _IsRepeat(isRepeat) {}

		inline bool IsRepeat() const { return _IsRepeat; }

		std::string ToString() const override
		{
			return std::format("KeyPressedEvent: {} (repeat={})", _key_code, _IsRepeat);
		}

		EVENT_CLASS_TYPE(EVENT_TYPE_KEY_PRESSED)
	private:
		bool _IsRepeat;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			return std::format("KeyReleasedEvent: {}", _key_code);
		}

		EVENT_CLASS_TYPE(EVENT_TYPE_KEY_RELEASED)
	};


	//mouse events

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(double x, double y)
			: m_MouseX(x), m_MouseY(y) {}

		inline double GetX() const { return m_MouseX; }
		inline double GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			return std::format("MouseMovedEvent: {}, {}", m_MouseX, m_MouseY);
		}

		EVENT_CLASS_TYPE(EVENT_TYPE_MOUSE_MOVED)
	private:
		double m_MouseX, m_MouseY;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(double xOffset, double yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) {}

		inline double GetXOffset() const { return m_XOffset; }
		inline double GetYOffset() const { return m_YOffset; }

		std::string ToString() const override
		{
			return std::format("MouseScrolledEvent: {}, {}", m_XOffset, m_YOffset);
		}

		EVENT_CLASS_TYPE(EVENT_TYPE_MOUSE_SCROLLED)
	private:
		double m_XOffset, m_YOffset;
	};

	class MouseButtonEvent : public Event
	{
	public:
		inline int GetMouseButton() const { return m_Button; }
	protected:
		MouseButtonEvent(int button)
			: m_Button(button) {}

		int m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			return std::format("MouseButtonPressedEvent: {}", m_Button);
		}

		EVENT_CLASS_TYPE(EVENT_TYPE_MOUSE_BUTTON_PRESSED)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			return std::format("MouseButtonReleasedEvent: {}", m_Button);
		}

		EVENT_CLASS_TYPE(EVENT_TYPE_MOUSE_BUTTON_RELASED)
	};

	//gamepad events

	class GamepadJoystickEvent : public Event
	{
	public:
		GamepadJoystickEvent(float x, float y)
			: _x(x), _y(y) {}

		inline float GetX() const { return _x; }
		inline float GetY() const { return _y; }

		std::string ToString() const override
		{
			return std::format("GamepadJoystickEvent: {}, {}", _x, _y);
		}

		EVENT_CLASS_TYPE(EVENT_TYPE_GAMEPAD_JOYSTICK_MOVED)
	private:
		float _x, _y;
	};

	class GamepadButtonEvent : public Event
	{
	public:
		inline int GetGamepadButton() const { return m_Button; }
		inline int GetGamepad() const { return m_gamepad; }
	protected:
		GamepadButtonEvent(int padId, int button)
			: m_gamepad(padId), m_Button(button) {}

		int m_gamepad;
		int m_Button;
	};

	class GamepadButtonPressedEvent : public GamepadButtonEvent
	{
	public:
		GamepadButtonPressedEvent(int gamepad, int button)
			: GamepadButtonEvent(gamepad, button) {}

		std::string ToString() const override
		{
			return std::format("GamepadButtonPressedEvent: {}", m_Button);
		}

		EVENT_CLASS_TYPE(EVENT_TYPE_GAMEPAD_BUTTON_PRESSED)
	};

	class GamepadButtonReleasedEvent : public GamepadButtonEvent
	{
	public:
		GamepadButtonReleasedEvent(int gamepad, int button)
			: GamepadButtonEvent(gamepad, button) {}

		std::string ToString() const override
		{
			return std::format("GamepadButtonReleasedEvent: {}", m_Button);
		}

		EVENT_CLASS_TYPE(EVENT_TYPE_GAMEPAD_BUTTON_RELEASED)
	};

	class GamepadTriggerPressedEvent : public GamepadButtonEvent
	{
	public:
		GamepadTriggerPressedEvent(int gamepad, int button)
			: GamepadButtonEvent(gamepad, button) {}

		std::string ToString() const override
		{
			return std::format("GamepadTriggerPressedEvent: {}", m_Button);
		}

		EVENT_CLASS_TYPE(EVENT_TYPE_GAMEPAD_TRIGGER_PRESSED)
	};

	class GamepadTriggerReleasedEvent : public GamepadButtonEvent
	{
	public:
		GamepadTriggerReleasedEvent(int gamepad, int button)
			: GamepadButtonEvent(gamepad, button) {}

		std::string ToString() const override
		{
			return std::format("GamepadTriggerReleasedEvent: {}", m_Button);
		}

		EVENT_CLASS_TYPE(EVENT_TYPE_GAMEPAD_TRIGGER_RELEASED)
	};

}
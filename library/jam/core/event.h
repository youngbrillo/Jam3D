#pragma once
#include <string>
#include <functional>

namespace jam
{
	enum class EVENT_TYPE
	{
		EVENT_TYPE_NONE = 0,
		EVENT_TYPE_WINDOW_CLOSE,
		EVENT_TYPE_WINDOW_RESIZE,
		EVENT_TYPE_KEY_PRESSED,
		EVENT_TYPE_KEY_RELEASED,
		EVENT_TYPE_MOUSE_BUTTON_PRESSED,
		EVENT_TYPE_MOUSE_BUTTON_RELASED,
		EVENT_TYPE_MOUSE_MOVED,
		EVENT_TYPE_MOUSE_SCROLLED,
		EVENT_TYPE_GAMEPAD_BUTTON_PRESSED,
		EVENT_TYPE_GAMEPAD_BUTTON_RELEASED,
		EVENT_TYPE_GAMEPAD_TRIGGER_PRESSED,
		EVENT_TYPE_GAMEPAD_TRIGGER_RELEASED,
		EVENT_TYPE_GAMEPAD_JOYSTICK_MOVED,
		EVENT_TYPE_ENTITY_CREATED,
		EVENT_TYPE_ENTITY_DELETED
	};


#define EVENT_CLASS_TYPE(type)	static EVENT_TYPE GetStaticType() { return EVENT_TYPE::type; }\
								virtual EVENT_TYPE GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

	class Event
	{
	public:
		bool handled = false;

		virtual ~Event() {}
		virtual EVENT_TYPE GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			: m_Event(event) { }

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType() && !m_Event.Handled)
			{
				m_Event.Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};
}
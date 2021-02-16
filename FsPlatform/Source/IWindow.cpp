#include <stdafx.h>
#include <FsPlatform/Include/IWindow.h>


namespace fs
{
	namespace Window
	{
		EventData::EventValue::EventValue()
			: _raw{ 0 }
		{
			__noop;
		}

		EventData::EventValue::EventValue(const EventValue& rhs)
			: _raw{ rhs._raw[0], rhs._raw[1] }
		{
			__noop;
		}
		
		void EventData::EventValue::setKeyCode(const EventData::KeyCode keyCode) noexcept
		{
			_keyCode = keyCode;
		}
		const EventData::KeyCode EventData::EventValue::getKeyCode() const noexcept
		{
			return _keyCode;
		}
		
		void EventData::EventValue::setMousePosition(const fs::Float2& mousePosition) noexcept
		{
			_mousePosition = mousePosition;
		}

		const fs::Float2& EventData::EventValue::getMousePosition() const noexcept
		{
			return _mousePosition;
		}
		
		void EventData::EventValue::setMouseWheel(const float mouseWheel) noexcept
		{
			_mouseInfoF = mouseWheel;
		}
		
		const float EventData::EventValue::getMouseWheel() const noexcept
		{
			return _mouseInfoF;
		}
		
		void EventData::EventValue::setInputWchar(const wchar_t inputWchar) noexcept
		{
			_inputWchar = inputWchar;
		}
		
		const wchar_t EventData::EventValue::getInputWchar() const noexcept
		{
			return _inputWchar;
		}
	}
}

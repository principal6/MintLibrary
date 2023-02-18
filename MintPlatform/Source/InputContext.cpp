#include <MintPlatform/Include/InputContext.h>

#include <MintContainer/Include/Queue.hpp>


namespace mint
{
	namespace Platform
	{
		InputEvent::InputEvent()
			: _type{ InputEventType::COUNT }
			, _mouseData{}
			, _window{ nullptr }
		{
			__noop;
		}

		void MouseState::Process() noexcept
		{
			for (uint32 mouseButtonIndex = 0; mouseButtonIndex < GetMouseButtonCount(); ++mouseButtonIndex)
			{
				if (_buttonStates[mouseButtonIndex] == MouseButtonState::Pressed)
				{
					_buttonStates[mouseButtonIndex] = MouseButtonState::Down;
				}
				else if (_buttonStates[mouseButtonIndex] == MouseButtonState::Released)
				{
					_buttonStates[mouseButtonIndex] = MouseButtonState::Up;
				}
			}

			_wheelScroll = 0.0f;

			_pressedButton = MouseButton::COUNT;
			_releasedButton = MouseButton::COUNT;
			_clickedButton = MouseButton::COUNT;
			_doubleClickedButton = MouseButton::COUNT;

			_deltaPosition.SetZero();
			_isPointerMoved = false;
		}

		bool MouseState::IsPointerMoved() const noexcept
		{
			return _isPointerMoved;
		}

		void MouseState::SetPosition(const Float2& position) noexcept
		{
			_position = position;
			_isPointerMoved = true;
		}

		void MouseState::SetDeltaPosition(const Float2& deltaPosition) noexcept
		{
			_deltaPosition = deltaPosition;
			_isPointerMoved = true;
		}

		const Float2& MouseState::GetPosition() const noexcept
		{
			return _position;
		}

		const Float2& MouseState::GetDeltaPosition() const noexcept
		{
			return _deltaPosition;
		}


		void KeyboardState::Process() noexcept
		{
			for (uint32 keyIndex = 0; keyIndex < GetKeyCodeCount(); keyIndex++)
			{
				if (_keyStates[keyIndex] == KeyState::Pressed)
				{
					_keyStates[keyIndex] = KeyState::Down;
				}
				else if (_keyStates[keyIndex] == KeyState::Released)
				{
					_keyStates[keyIndex] = KeyState::Up;
				}
			}

			_pressedKeyCode = KeyCode::NONE;
			_releasedKeyCode = KeyCode::NONE;
			_character = L'\0';
			_characterCandidate = L'\0';
		}


		InputContext::InputContext()
		{
			__noop;
		}

		InputContext::~InputContext()
		{
			__noop;
		}

		InputContext& InputContext::GetInstance() noexcept
		{
			static InputContext instance;
			return instance;
		}

		void InputContext::FlushInputEvents() noexcept
		{
			_events.Flush();
		}

		void InputContext::PushInputEvent(const InputEvent& inputEvent) noexcept
		{
			_events.Push(inputEvent);
		}

		void InputContext::ProcessEvents() noexcept
		{
			_mouseState.Process();
			_keyboardState.Process();

			while (_events.IsEmpty() == false)
			{
				InputEvent inputEvent = _events.Peek();

				switch (inputEvent._type)
				{
				case InputEventType::Mouse:
				{
					const InputEventMouseData& mouseData = inputEvent._mouseData;
					const int32 mouseButtonIndex = static_cast<int32>(mouseData._button);
					switch (mouseData._type)
					{
					case InputMouseEventType::ButtonPressed:
					{
						_mouseState._buttonStates[mouseButtonIndex] = MouseButtonState::Pressed;
						_mouseState._pressedButton = mouseData._button;
						break;
					}
					case InputMouseEventType::ButtonReleased:
					{
						if (IsMouseButtonDown(mouseData._button) == true)
						{
							_mouseState._clickedButton = mouseData._button;
						}

						_mouseState._buttonStates[mouseButtonIndex] = MouseButtonState::Released;
						_mouseState._releasedButton = mouseData._button;
						break;
					}
					case InputMouseEventType::ButtonDoubleClicked:
					{
						_mouseState._buttonStates[mouseButtonIndex] = MouseButtonState::DoubleClicked;
						_mouseState._doubleClickedButton = mouseData._button;
						break;
					}
					case InputMouseEventType::PointerMoved:
					{
						_mouseState.SetPosition(mouseData._position);
						break;
					}
					case InputMouseEventType::PointerMovedDelta:
					{
						_mouseState.SetDeltaPosition(mouseData._position);
						break;
					}
					case InputMouseEventType::WheelScrolled:
					{
						_mouseState._wheelScroll = mouseData._wheelScroll;
						break;
					}
					default:
						break;
					}
					break;
				}
				case InputEventType::Keyboard:
				{
					const InputEventKeyboardData& keyboardData = inputEvent._keyboardData;
					const int32 keyIndex = static_cast<int32>(keyboardData._keyCode);
					switch (keyboardData._type)
					{
					case InputKeyboardEventType::KeyPressed:
					{
						_keyboardState._keyStates[keyIndex] = KeyState::Pressed;
						_keyboardState._pressedKeyCode = keyboardData._keyCode;
						break;
					}
					case InputKeyboardEventType::KeyReleased:
					{
						_keyboardState._keyStates[keyIndex] = KeyState::Released;
						_keyboardState._releasedKeyCode = keyboardData._keyCode;
						break;
					}
					case InputKeyboardEventType::CharacterInput:
					{
						_keyboardState._character = keyboardData._character;
						break;
					}
					case InputKeyboardEventType::CharacterInputCandidate:
					{
						_keyboardState._characterCandidate = keyboardData._character;
						break;
					}
					default:
						break;
					}
					break;
				}
				default:
					break;
				}

				_events.Pop();
			}
		}

		const MouseState& InputContext::GetMouseState() const noexcept
		{
			return _mouseState;
		}

		MouseButtonState InputContext::GetMouseButtonState(const MouseButton mouseButton) const noexcept
		{
			const int32 mouseButtonIndex = static_cast<int32>(mouseButton);
			return _mouseState._buttonStates[mouseButtonIndex];
		}

		bool InputContext::IsMouseButtonPressed() const noexcept
		{
			return _mouseState._pressedButton != MouseButton::COUNT;
		}

		bool InputContext::IsMouseButtonReleased() const noexcept
		{
			return _mouseState._releasedButton != MouseButton::COUNT;
		}

		bool InputContext::IsMouseButtonClicked() const noexcept
		{
			return _mouseState._clickedButton != MouseButton::COUNT;
		}

		bool InputContext::IsMouseButtonDoubleClicked() const noexcept
		{
			return _mouseState._doubleClickedButton != MouseButton::COUNT;
		}

		bool InputContext::IsMouseButtonDown(const MouseButton mouseButton) const noexcept
		{
			return GetMouseButtonState(mouseButton) == MouseButtonState::Down || GetMouseButtonState(mouseButton) == MouseButtonState::Pressed;
		}

		bool InputContext::IsMouseButtonUp(const MouseButton mouseButton) const noexcept
		{
			return GetMouseButtonState(mouseButton) == MouseButtonState::Released || GetMouseButtonState(mouseButton) == MouseButtonState::Up;
		}

		bool InputContext::IsMousePointerMoved() const noexcept
		{
			return _mouseState.IsPointerMoved();
		}

		bool InputContext::IsMouseWheelScrolled() const noexcept
		{
			return _mouseState._wheelScroll != 0.0f;
		}

		Float2 InputContext::GetMousePosition() const noexcept
		{
			return _mouseState.GetPosition();
		}

		Float2 InputContext::GetMouseDeltaPosition() const noexcept
		{
			return _mouseState.GetDeltaPosition();
		}

		float InputContext::GetMouseWheelScroll() const noexcept
		{
			return _mouseState._wheelScroll;
		}

		const KeyboardState& InputContext::GetKeyboardState() const noexcept
		{
			return _keyboardState;
		}

		KeyState InputContext::GetKeyState(const KeyCode keyCode) const noexcept
		{
			const int32 keyIndex = static_cast<int32>(keyCode);
			return _keyboardState._keyStates[keyIndex];
		}

		bool InputContext::IsKeyPressed() const noexcept
		{
			return _keyboardState._pressedKeyCode != KeyCode::NONE;
		}

		bool InputContext::IsKeyReleased() const noexcept
		{
			return _keyboardState._releasedKeyCode != KeyCode::NONE;
		}

		bool InputContext::IsKeyInputCharacter() const noexcept
		{
			return _keyboardState._character != L'\0';
		}

		bool InputContext::IsKeyInputCharacterCandidate() const noexcept
		{
			return _keyboardState._characterCandidate != L'\0';
		}

		bool InputContext::IsKeyDown(const KeyCode keyCode) const noexcept
		{
			return GetKeyState(keyCode) == KeyState::Down || GetKeyState(keyCode) == KeyState::Pressed;
		}

		bool InputContext::IsKeyUp(const KeyCode keyCode) const noexcept
		{
			return GetKeyState(keyCode) == KeyState::Up || GetKeyState(keyCode) == KeyState::Released;
		}
	}
}

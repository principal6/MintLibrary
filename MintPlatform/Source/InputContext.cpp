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

		void MouseState::process() noexcept
		{
			for (uint32 mouseButtonIndex = 0; mouseButtonIndex < getMouseButtonCount(); ++mouseButtonIndex)
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

			_deltaPosition.setZero();
			_isPointerMoved = false;
		}

		bool MouseState::isPointerMoved() const noexcept
		{
			return _isPointerMoved;
		}

		void MouseState::setPosition(const Float2& position) noexcept
		{
			_position = position;
			_isPointerMoved = true;
		}

		void MouseState::setDeltaPosition(const Float2& deltaPosition) noexcept
		{
			_deltaPosition = deltaPosition;
			_isPointerMoved = true;
		}

		const Float2& MouseState::getPosition() const noexcept
		{
			return _position;
		}

		const Float2& MouseState::getDeltaPosition() const noexcept
		{
			return _deltaPosition;
		}


		void KeyboardState::process() noexcept
		{
			for (uint32 keyIndex = 0; keyIndex < getKeyCodeCount(); keyIndex++)
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

		InputContext& InputContext::getInstance() noexcept
		{
			static InputContext instance;
			return instance;
		}

		void InputContext::flushInputEvents() noexcept
		{
			_events.Flush();
		}

		void InputContext::pushInputEvent(const InputEvent& inputEvent) noexcept
		{
			_events.Push(inputEvent);
		}

		void InputContext::processEvents() noexcept
		{
			_mouseState.process();
			_keyboardState.process();

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
						if (isMouseButtonDown(mouseData._button) == true)
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
						_mouseState.setPosition(mouseData._position);
						break;
					}
					case InputMouseEventType::PointerMovedDelta:
					{
						_mouseState.setDeltaPosition(mouseData._position);
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

		const MouseState& InputContext::getMouseState() const noexcept
		{
			return _mouseState;
		}

		MouseButtonState InputContext::getMouseButtonState(const MouseButton mouseButton) const noexcept
		{
			const int32 mouseButtonIndex = static_cast<int32>(mouseButton);
			return _mouseState._buttonStates[mouseButtonIndex];
		}

		bool InputContext::isMouseButtonPressed() const noexcept
		{
			return _mouseState._pressedButton != MouseButton::COUNT;
		}

		bool InputContext::isMouseButtonReleased() const noexcept
		{
			return _mouseState._releasedButton != MouseButton::COUNT;
		}

		bool InputContext::isMouseButtonClicked() const noexcept
		{
			return _mouseState._clickedButton != MouseButton::COUNT;
		}

		bool InputContext::isMouseButtonDoubleClicked() const noexcept
		{
			return _mouseState._doubleClickedButton != MouseButton::COUNT;
		}

		bool InputContext::isMouseButtonDown(const MouseButton mouseButton) const noexcept
		{
			return getMouseButtonState(mouseButton) == MouseButtonState::Down || getMouseButtonState(mouseButton) == MouseButtonState::Pressed;
		}

		bool InputContext::isMouseButtonUp(const MouseButton mouseButton) const noexcept
		{
			return getMouseButtonState(mouseButton) == MouseButtonState::Released || getMouseButtonState(mouseButton) == MouseButtonState::Up;
		}

		bool InputContext::isMousePointerMoved() const noexcept
		{
			return _mouseState.isPointerMoved();
		}

		bool InputContext::isMouseWheelScrolled() const noexcept
		{
			return _mouseState._wheelScroll != 0.0f;
		}

		Float2 InputContext::getMousePosition() const noexcept
		{
			return _mouseState.getPosition();
		}

		Float2 InputContext::getMouseDeltaPosition() const noexcept
		{
			return _mouseState.getDeltaPosition();
		}

		float InputContext::getMouseWheelScroll() const noexcept
		{
			return _mouseState._wheelScroll;
		}

		const KeyboardState& InputContext::getKeyboardState() const noexcept
		{
			return _keyboardState;
		}

		KeyState InputContext::getKeyState(const KeyCode keyCode) const noexcept
		{
			const int32 keyIndex = static_cast<int32>(keyCode);
			return _keyboardState._keyStates[keyIndex];
		}

		bool InputContext::isKeyPressed() const noexcept
		{
			return _keyboardState._pressedKeyCode != KeyCode::NONE;
		}

		bool InputContext::isKeyReleased() const noexcept
		{
			return _keyboardState._releasedKeyCode != KeyCode::NONE;
		}

		bool InputContext::isKeyInputCharacter() const noexcept
		{
			return _keyboardState._character != L'\0';
		}

		bool InputContext::isKeyInputCharacterCandidate() const noexcept
		{
			return _keyboardState._characterCandidate != L'\0';
		}

		bool InputContext::isKeyDown(const KeyCode keyCode) const noexcept
		{
			return getKeyState(keyCode) == KeyState::Down || getKeyState(keyCode) == KeyState::Pressed;
		}

		bool InputContext::isKeyUp(const KeyCode keyCode) const noexcept
		{
			return getKeyState(keyCode) == KeyState::Up || getKeyState(keyCode) == KeyState::Released;
		}
	}
}

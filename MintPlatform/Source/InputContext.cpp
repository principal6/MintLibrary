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

        const bool MouseState::isPointerMoved() const noexcept
        {
            return _isPointerMoved;
        }

        void MouseState::setPosition(const mint::Float2& position) noexcept
        {
            _position = position;
            _isPointerMoved = true;
        }

        void MouseState::setDeltaPosition(const mint::Float2& deltaPosition) noexcept
        {
            _deltaPosition = deltaPosition;
            _isPointerMoved = true;
        }

        const mint::Float2& MouseState::getPosition() const noexcept
        {
            return _position;
        }

        const mint::Float2& MouseState::getDeltaPosition() const noexcept
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

        void InputContext::pushInputEvent(const InputEvent& inputEvent) noexcept
        {
            _events.push(inputEvent);
        }

        void InputContext::processEvents() noexcept
        {
            _mouseState.process();
            _keyboardState.process();

            while (_events.empty() == false)
            {
                InputEvent inputEvent = _events.peek();

                switch (inputEvent._type)
                {
                    case mint::Platform::InputEventType::Mouse:
                    {
                        const InputEventMouseData& mouseData = inputEvent._mouseData;
                        const int32 mouseButtonIndex = static_cast<int32>(mouseData._button);
                        switch (mouseData._type)
                        {
                            case mint::Platform::InputMouseEventType::ButtonPressed:
                            {
                                _mouseState._buttonStates[mouseButtonIndex] = MouseButtonState::Pressed;
                                _mouseState._pressedButton = mouseData._button;
                                break;
                            }
                            case mint::Platform::InputMouseEventType::ButtonReleased:
                            {
                                if (isMouseButtonDown(mouseData._button) == true)
                                {
                                    _mouseState._clickedButton = mouseData._button;
                                }

                                _mouseState._buttonStates[mouseButtonIndex] = MouseButtonState::Released;
                                _mouseState._releasedButton = mouseData._button;
                                break;
                            }
                            case mint::Platform::InputMouseEventType::ButtonDoubleClicked:
                            {
                                _mouseState._buttonStates[mouseButtonIndex] = MouseButtonState::DoubleClicked;
                                _mouseState._doubleClickedButton = mouseData._button;
                                break;
                            }
                            case mint::Platform::InputMouseEventType::PointerMoved:
                            {
                                _mouseState.setPosition(mouseData._position);
                                break;
                            }
                            case mint::Platform::InputMouseEventType::PointerMovedDelta:
                            {
                                _mouseState.setDeltaPosition(mouseData._position);
                                break;
                            }
                            case mint::Platform::InputMouseEventType::WheelScrolled:
                            {
                                _mouseState._wheelScroll = mouseData._wheelScroll;
                                break;
                            }
                            default:
                                break;
                        }
                        break;
                    }
                    case mint::Platform::InputEventType::Keyboard:
                    {
                        const InputEventKeyboardData& keyboardData = inputEvent._keyboardData;
                        const int32 keyIndex = static_cast<int32>(keyboardData._keyCode);
                        switch (keyboardData._type)
                        {
                            case mint::Platform::InputKeyboardEventType::KeyPressed:
                            {
                                _keyboardState._keyStates[keyIndex] = KeyState::Pressed;
                                _keyboardState._pressedKeyCode = keyboardData._keyCode;
                                break;
                            }
                            case mint::Platform::InputKeyboardEventType::KeyReleased:
                            {
                                _keyboardState._keyStates[keyIndex] = KeyState::Released;
                                _keyboardState._releasedKeyCode = keyboardData._keyCode;
                                break;
                            }
                            case mint::Platform::InputKeyboardEventType::CharacterInput:
                            {
                                _keyboardState._character = keyboardData._character;
                                break;
                            }
                            case mint::Platform::InputKeyboardEventType::CharacterInputCandidate:
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

                _events.pop();
            }
        }

        const MouseState& InputContext::getMouseState() const noexcept
        {
            return _mouseState;
        }

        const MouseButtonState InputContext::getMouseButtonState(const MouseButton mouseButton) const noexcept
        {
            const int32 mouseButtonIndex = static_cast<int32>(mouseButton);
            return _mouseState._buttonStates[mouseButtonIndex];
        }

        const bool InputContext::isMouseButtonPressed() const noexcept
        {
            return _mouseState._pressedButton != MouseButton::COUNT;
        }

        const bool InputContext::isMouseButtonReleased() const noexcept
        {
            return _mouseState._releasedButton != MouseButton::COUNT;
        }

        const bool InputContext::isMouseButtonClicked() const noexcept
        {
            return _mouseState._clickedButton != MouseButton::COUNT;
        }

        const bool InputContext::isMouseButtonDoubleClicked() const noexcept
        {
            return _mouseState._doubleClickedButton != MouseButton::COUNT;
        }

        const bool InputContext::isMouseButtonDown(const MouseButton mouseButton) const noexcept
        {
            return getMouseButtonState(mouseButton) == MouseButtonState::Down || getMouseButtonState(mouseButton) == MouseButtonState::Pressed;
        }

        const bool InputContext::isMouseButtonUp(const MouseButton mouseButton) const noexcept
        {
            return getMouseButtonState(mouseButton) == MouseButtonState::Released || getMouseButtonState(mouseButton) == MouseButtonState::Up;
        }

        const bool InputContext::isMousePointerMoved() const noexcept
        {
            return _mouseState.isPointerMoved();
        }

        const bool InputContext::isMouseWheelScrolled() const noexcept
        {
            return _mouseState._wheelScroll != 0.0f;
        }

        const mint::Float2 InputContext::getMousePosition() const noexcept
        {
            return _mouseState.getPosition();
        }

        const mint::Float2 InputContext::getMouseDeltaPosition() const noexcept
        {
            return _mouseState.getDeltaPosition();
        }

        const float InputContext::getMouseWheelScroll() const noexcept
        {
            return _mouseState._wheelScroll;
        }

        const KeyboardState& InputContext::getKeyboardState() const noexcept
        {
            return _keyboardState;
        }

        const KeyState InputContext::getKeyState(const KeyCode keyCode) const noexcept
        {
            const int32 keyIndex = static_cast<int32>(keyCode);
            return _keyboardState._keyStates[keyIndex];
        }

        const bool InputContext::isKeyPressed() const noexcept
        {
            return _keyboardState._pressedKeyCode != KeyCode::NONE;
        }

        const bool InputContext::isKeyReleased() const noexcept
        {
            return _keyboardState._releasedKeyCode != KeyCode::NONE;
        }

        const bool InputContext::isKeyInputCharacter() const noexcept
        {
            return _keyboardState._character != L'\0';
        }
        
        const bool InputContext::isKeyInputCharacterCandidate() const noexcept
        {
            return _keyboardState._characterCandidate != L'\0';
        }

        const bool InputContext::isKeyDown(const KeyCode keyCode) const noexcept
        {
            return getKeyState(keyCode) == KeyState::Down || getKeyState(keyCode) == KeyState::Pressed;
        }

        const bool InputContext::isKeyUp(const KeyCode keyCode) const noexcept
        {
            return getKeyState(keyCode) == KeyState::Up || getKeyState(keyCode) == KeyState::Released;
        }
    }
}

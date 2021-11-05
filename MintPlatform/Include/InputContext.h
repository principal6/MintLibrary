#pragma once


#ifndef MINT_PLATFORM_INPUT_CONTEXT_H
#define MINT_PLATFORM_INPUT_CONTEXT_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Queue.h>

#include <MintMath/Include/Float2.h>

#include <MintPlatform/Include/PlatformCommon.h>


namespace mint
{
    namespace Window
    {
        class IWindow;
    }


    namespace Platform
    {
        enum class InputEventType
        {
            Mouse,
            Keyboard,
            COUNT
        };

        enum class InputMouseEventType
        {
            ButtonPressed,
            ButtonReleased,
            ButtonDoubleClicked,
            PointerMoved,
            PointerMovedDelta,
            WheelScrolled,
            COUNT
        };

        enum class InputKeyboardEventType
        {
            KeyPressed,
            KeyReleased,
            CharacterInput,
            CharacterInputCandidate,
            COUNT
        };

        struct InputEventMouseData
        {
            InputMouseEventType     _type{ InputMouseEventType::COUNT };
            MouseButton             _button{ MouseButton::COUNT };
            Float2                  _position;
            float                   _wheelScroll{ 0.0f };
        };

        struct InputEventKeyboardData
        {
            InputKeyboardEventType  _type{ InputKeyboardEventType::COUNT };
            KeyCode                 _keyCode{ KeyCode::NONE };
            wchar_t                 _character{};
        };

        struct InputEvent
        {
        public:
                                        InputEvent();

        public:
            InputEventType              _type;
            union
            {
                InputEventMouseData     _mouseData;
                InputEventKeyboardData  _keyboardData;
            };
            Window::IWindow*            _window;
        };

        enum class MouseButtonState
        {
            Up,
            Pressed,
            Down,
            Released, // Click µµ ¿©±â
            DoubleClicked,
            COUNT
        };

        enum class KeyState
        {
            Up,
            Pressed,
            Down,
            Released,
            COUNT
        };

        struct MouseState
        {
        public:
            void                    process() noexcept;

        public:
            const bool              isPointerMoved() const noexcept;
            void                    setPosition(const Float2& position) noexcept;
            void                    setDeltaPosition(const Float2& deltaPosition) noexcept;
            const Float2&           getPosition() const noexcept;
            const Float2&           getDeltaPosition() const noexcept;

        public:
            MouseButtonState        _buttonStates[getMouseButtonCount()]{};
            float                   _wheelScroll = 0.0f;

        public:
            MouseButton             _pressedButton = MouseButton::COUNT;
            MouseButton             _releasedButton = MouseButton::COUNT;
            MouseButton             _clickedButton = MouseButton::COUNT;
            MouseButton             _doubleClickedButton = MouseButton::COUNT;

        private:
            Float2                  _position;
            Float2                  _deltaPosition;
            bool                    _isPointerMoved = false;
        };

        struct KeyboardState
        {
            void                    process() noexcept;

            KeyState                _keyStates[getKeyCodeCount()]{};

            KeyCode                 _pressedKeyCode;
            KeyCode                 _releasedKeyCode;
            wchar_t                 _character = L'\0';
            wchar_t                 _characterCandidate = L'\0';
        };


        class InputContext
        {
        private:
                                    InputContext();

        public:
                                    ~InputContext();

        public:
            static InputContext&    getInstance() noexcept;

        public:
            void                    flushInputEvents() noexcept;
            void                    pushInputEvent(const InputEvent& inputEvent) noexcept;
        
        public:
            void                    processEvents() noexcept;

        public:
            const MouseState&       getMouseState() const noexcept;
            const MouseButtonState  getMouseButtonState(const MouseButton mouseButton) const noexcept;
            const bool              isMouseButtonPressed() const noexcept;
            const bool              isMouseButtonReleased() const noexcept;
            const bool              isMouseButtonClicked() const noexcept;
            const bool              isMouseButtonDoubleClicked() const noexcept;
            const bool              isMouseButtonDown(const MouseButton mouseButton) const noexcept;
            const bool              isMouseButtonUp(const MouseButton mouseButton) const noexcept;
            const bool              isMousePointerMoved() const noexcept;
            const bool              isMouseWheelScrolled() const noexcept;
            const Float2            getMousePosition() const noexcept;
            const Float2            getMouseDeltaPosition() const noexcept;
            const float             getMouseWheelScroll() const noexcept;

        public:
            const KeyboardState&    getKeyboardState() const noexcept;
            const KeyState          getKeyState(const KeyCode keyCode) const noexcept;
            const bool              isKeyPressed() const noexcept;
            const bool              isKeyReleased() const noexcept;
            const bool              isKeyInputCharacter() const noexcept;
            const bool              isKeyInputCharacterCandidate() const noexcept;
            const bool              isKeyDown(const KeyCode keyCode) const noexcept;
            const bool              isKeyUp(const KeyCode keyCode) const noexcept;

        private:
            Queue<InputEvent>       _events;

        private:
            MouseState              _mouseState;
            KeyboardState           _keyboardState;
        };
    }
}


#endif // !MINT_PLATFORM_INPUT_CONTEXT_H

#include <MintPlatform/Include/IWindow.h>


namespace mint
{
    namespace Window
    {
        EventData::EventValue::EventValue()
            : _raw{ 0 }
        {
            __noop;
        }

        EventData::EventValue::EventValue(const EventValue& rhs)
            : _raw{ rhs._raw[0], rhs._raw[1], rhs._raw[2] }
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

        const bool EventData::EventValue::isKeyCode(const EventData::KeyCode keyCode) const noexcept
        {
            return keyCode == _keyCode;
        }
        
        void EventData::EventValue::setMousePosition(const mint::Float2& mousePosition) noexcept
        {
            _mousePosition = mousePosition;
        }

        void EventData::EventValue::setMouseDeltaPosition(const mint::Float2& mouseDeltaPosition) noexcept
        {
            _mouseDeltaPosition = mouseDeltaPosition;
        }

        void EventData::EventValue::setMouseButton(const MouseButton mouseButton) noexcept
        {
            _mouseButton = mouseButton;
        }

        const mint::Float2& EventData::EventValue::getMousePosition() const noexcept
        {
            return _mousePosition;
        }

        const mint::Float2 EventData::EventValue::getAndClearMouseDeltaPosition() const noexcept
        {
            mint::Float2 result = _mouseDeltaPosition;
            _mouseDeltaPosition.setZero();
            return result;
        }

        const EventData::MouseButton EventData::EventValue::getMouseButton() const noexcept
        {
            return _mouseButton;
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

        void EventData::EventValue::setSize(const mint::Float2& size) noexcept
        {
            _size = size;
        }

        const mint::Float2& EventData::EventValue::getSize() const noexcept
        {
            return _size;
        }
        
        
        IWindow::IWindow()
            : IWindow(mint::Platform::PlatformType::INVALID)
        {
            __noop;
        }
        
        IWindow::IWindow(const mint::Platform::PlatformType platformType)
            : _isRunning{ false }
            , _platformType{ platformType }
            , _creationError{ CreationError::None }
            , _currentCursorType{ CursorType::Arrow }
        {
            __noop;
        }
    }
}

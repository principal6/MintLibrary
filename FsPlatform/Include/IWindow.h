#pragma once


#ifndef FS_I_WINDOW_H
#define FS_I_WINDOW_H


#include <FsCommon/Include/CommonDefinitions.h>

#include <FsPlatform/Include/PlatformCommon.h>

#include <FsMath/Include/Int2.h>
#include <FsMath/Include/Float3.h>


namespace fs
{
    namespace Window
    {
        enum class Style
        {
            Default,
            Clean,
            Resizable,
        };

        enum class CreationError
        {
            None,
            FailedToGetInstanceHandle,
            FailedToCreateWindow,
            FailedToRegisterRawInputDevices,
            NullptrString,
        };

        enum class MessageBoxType
        {
            Ok,
            Warning,
            Error,
        };

        struct CreationData
        {
            Int2                _size{ 800, 600 };
            Int2                _position{ kInt32Min, kInt32Min };
            const wchar_t*      _title{ L"FsLibrary" };
            Style               _style{ Style::Default };
            Float3              _bgColor{ 1.0f, 1.0f, 1.0f };
        };

        enum class EventType
        {
            None,
            KeyDown,
            KeyUp,
            KeyStroke,
            KeyStrokeCandidate,
            MouseMove,
            MouseMoveDelta,
            MouseDown,
            MouseUp,
            MouseDoubleClicked,
            MouseWheel,
            WindowResized,
        };

        struct EventData
        {
            enum class KeyCode : uint64
            {
                NONE,
                Escape,
                Enter,
                Up,
                Down,
                Left,
                Right,
                Delete,
                Home,
                End,
                Shift,
                Control,
                Alt,
                Q,
                W,
                E,
                R,
                T,
                Y,
                A,
                S,
                D,
                F,
                Z,
                X,
                C,
                V,
                Num0,
                Num1,
                Num2,
                Num3,
                Num4,
                Num5,
                Num6,
                Num7,
                Num8,
                Num9,
            };

            enum class MouseButton : int32
            {
                Left,
                Middle,
                Right,
            };

            class EventValue
            {
            public:
                                            EventValue();
                                            EventValue(const EventValue& rhs);
                                            ~EventValue() = default;

            public:
                void                        setKeyCode(const EventData::KeyCode keyCode) noexcept;
                const EventData::KeyCode    getKeyCode() const noexcept;
                const bool                  isKeyCode(const EventData::KeyCode keyCode) const noexcept;

                void                        setMousePosition(const fs::Float2& mousePosition) noexcept;
                void                        setMouseDeltaPosition(const fs::Float2& mouseDeltaPosition) noexcept;
                const fs::Float2&           getMousePosition() const noexcept;
                const fs::Float2            getAndClearMouseDeltaPosition() const noexcept;

                void                        setMouseWheel(const float mouseWheel) noexcept;
                const float                 getMouseWheel() const noexcept;

                void                        setInputWchar(const wchar_t inputWchar) noexcept;
                const wchar_t               getInputWchar() const noexcept;

                void                        setSize(const fs::Float2& size) noexcept;
                const fs::Float2&           getSize() const noexcept;

            private:
                union
                {
                    uint64                  _raw[3]{};
                    struct
                    {
                        fs::Float2          _mousePosition;
                        mutable fs::Float2  _mouseDeltaPosition;
                        MouseButton         _mouseButton;
                        float               _mouseInfoF;
                    };
                    struct
                    {
                        KeyCode             _keyCode;
                        wchar_t             _inputWchar;
                    };
                    struct
                    {
                        fs::Float2          _size;
                    };
                };
            };

            EventType               _type{ EventType::None };
            EventValue              _value{};
        };

        enum class CursorType
        {
            Arrow,
            SizeVert,
            SizeHorz,
            SizeLeftTilted,
            SizeRightTilted,

            COUNT
        };


        class IWindow abstract
        {
        public:
                                            IWindow();
                                            IWindow(const fs::Platform::PlatformType platformType);
            virtual                         ~IWindow() = default;

        public:
            virtual bool                    create(const CreationData& creationData) noexcept abstract;
            virtual void                    destroy() noexcept { _isRunning = false; }

        public:
            virtual bool                    isRunning() noexcept { return _isRunning; }
            bool                            hasEvent() const noexcept { return (0 < _eventQueue.size()); }
            void                            pushEvent(EventData&& eventData)
            {
                if (_eventQueue.size() == kEventQueueCapacity)
                {
                    // 처리되지 않은 Event 에 대해 손실 발생!!!
                    _eventQueue.pop();
                }
                _eventQueue.push(std::move(eventData));
            }
            EventData                       popEvent()
            {
                EventData event = _eventQueue.front();
                _eventQueue.pop();
                return event;
            }
            const EventData&                peekEvent() const
            {
                return _eventQueue.front();
            }

        public:
            CreationError                   getCreationError() const noexcept { return _creationError; }

        public:
            virtual void                    setSize(const Int2& newSize) abstract;
            const Int2&                     getSize() const noexcept { return _creationData._size; }
            
            const Int2&                     getEntireSize() const noexcept { return _entireSize; }

            virtual void                    setPosition(const Int2& newPosition) abstract;
            const Int2&                     getPosition() const noexcept { return _creationData._position; }
            
            const fs::Float3&               getBackgroundColor() const noexcept { return _creationData._bgColor; }

            virtual void                    setCursorType(const CursorType cursorType) noexcept { _currentCursorType = cursorType; }
            const CursorType                getCursorType() const noexcept { return _currentCursorType; }

            virtual const uint32            getCaretBlinkIntervalMs() const noexcept abstract;

            virtual const bool              isKeyDown(const EventData::KeyCode keyCode) const noexcept abstract;
            virtual const bool              isKeyDownFirst(const EventData::KeyCode keyCode) const noexcept abstract;
            virtual const bool              isMouseDown(const EventData::MouseButton mouseButton) const noexcept abstract;
            virtual const bool              isMouseDownFirst(const EventData::MouseButton mouseButton) const noexcept abstract;

            virtual void                    textToClipboard(const wchar_t* const text, const uint32 textLength) const noexcept abstract;
            virtual void                    textFromClipboard(std::wstring& outText) const noexcept abstract;
            virtual void                    showMessageBox(const std::wstring& title, const std::wstring& message, const MessageBoxType messageBoxType) const noexcept abstract;

        protected:
            static constexpr uint32         kEventQueueCapacity = 128;

        protected:
            bool                            _isRunning;
            fs::Platform::PlatformType      _platformType;
            CreationData                    _creationData;
            fs::Int2                        _entireSize;
            CreationError                   _creationError;

        private:
            std::queue<EventData>           _eventQueue;
        
        protected:
            fs::Float2                      _previousMousePosition;

        protected:
            CursorType                      _currentCursorType;
        };
    }
}


#endif // !FS_I_WINDOW_H

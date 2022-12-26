#pragma once


#ifndef _MINT_PLATFORM_I_WINDOW_H_
#define _MINT_PLATFORM_I_WINDOW_H_


#include <string>

#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/String.h>

#include <MintPlatform/Include/PlatformCommon.h>

#include <MintMath/Include/Int2.h>
#include <MintMath/Include/Float3.h>


namespace mint
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
            const wchar_t*      _title{ L"MintLibrary" };
            Style               _style{ Style::Default };
            Float3              _bgColor{ 1.0f, 1.0f, 1.0f };
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
                                        IWindow(const Platform::PlatformType platformType);
            virtual                     ~IWindow() = default;

        public:
            virtual bool                create(const CreationData& creationData) noexcept abstract;
            virtual void                destroy() noexcept { _isRunning = false; }
    
        public:
            virtual bool                isRunning() noexcept { return _isRunning; }

        public:
            CreationError               getCreationError() const noexcept { return _creationError; }

        public:
            virtual void                setSize(const Int2& newSize, const bool onlyUpdateData) noexcept { _isWindowResized = true;  }
            const Int2&                 getSize() const noexcept { return _creationData._size; }
            MINT_INLINE bool            isResized() const noexcept { bool result = _isWindowResized; _isWindowResized = false; return result; }
        
            const Int2&                 getEntireSize() const noexcept { return _entireSize; }

            virtual void                setPosition(const Int2& newPosition) abstract;
            const Int2&                 getPosition() const noexcept { return _creationData._position; }
        
            const Float3&               getBackgroundColor() const noexcept { return _creationData._bgColor; }

            virtual void                setCursorType(const CursorType cursorType) noexcept { _currentCursorType = cursorType; }
            CursorType                  getCursorType() const noexcept { return _currentCursorType; }

            virtual uint32              getCaretBlinkIntervalMs() const noexcept abstract;

            virtual void                textToClipboard(const wchar_t* const text, const uint32 textLength) const noexcept abstract;
            virtual void                textFromClipboard(StringW& outText) const noexcept abstract;
            virtual void                showMessageBox(const std::wstring& title, const std::wstring& message, const MessageBoxType messageBoxType) const noexcept abstract;

        protected:
            static constexpr uint32     kEventQueueCapacity = 128;

        protected:
            bool                        _isRunning;
            Platform::PlatformType      _platformType;
            CreationData                _creationData;
            Int2                        _entireSize;
            CreationError               _creationError;

        private:
            mutable bool                _isWindowResized;

        protected:
            CursorType                  _currentCursorType;
        };
    }
}


#endif // !_MINT_PLATFORM_I_WINDOW_H_

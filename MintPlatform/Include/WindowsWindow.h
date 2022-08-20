#pragma once


#ifndef MINT_WINDOWS_WINDOW_H
#define MINT_WINDOWS_WINDOW_H


#include <MintPlatform/Include/IWindow.h>

#include <MintContainer/Include/OwnPtr.h>
#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/HashMap.h>


struct HWND__;
using HWND = HWND__*;

struct HINSTANCE__;
using HINSTANCE = HINSTANCE__*;

struct HICON__;
using HICON = HICON__*;
using HCURSOR = HICON;

struct tagMSG;
using MSG = tagMSG;

using LRESULT = long long;
using LPARAM = long long;
using WPARAM = unsigned long long;
using UINT = unsigned int;
using BYTE = unsigned char;

constexpr uint32 kRawInputByteSize = 48;


namespace mint
{
    namespace Window
    {
#pragma region Windows Window Pool
        class WindowsWindow;
        class WindowsWindowPool
        {
        public:
                                        WindowsWindowPool() = default;
                                        ~WindowsWindowPool() = default;

        public:
            void                        insertWindow(const HWND hWnd, WindowsWindow* const windowsWindow);
            LRESULT                     redirectMessage(const HWND hWnd, const UINT Msg, const WPARAM wParam, const LPARAM lParam);

        private:
            Vector<WindowsWindow*>      _windowArray;
            HashMap<HWND, uint8>        _hWndMap;
        };
#pragma endregion


#pragma region Windows Window
        class WindowsWindow final : public IWindow
        {
            friend WindowsWindowPool;

            struct WparamKeyCodePair
            {
                WparamKeyCodePair() = default;
                WparamKeyCodePair(const WPARAM wParam, const Platform::KeyCode keyCode);

                WPARAM                  _wParam;
                Platform::KeyCode       _keyCode;
            };

        public:
                                        WindowsWindow();
            virtual                     ~WindowsWindow() = default;

        public:
            virtual bool                create(const CreationData& creationData) noexcept override;
            virtual void                destroy() noexcept override;

        private:
            void                        buildWparamKeyCodePairArray() noexcept;
            Platform::KeyCode           convertWparamToKeyCode(const WPARAM wParam) const noexcept;
            WPARAM                      convertKeyCodeToWparam(const Platform::KeyCode keyCode) const noexcept;

        public:
            virtual bool                isRunning() noexcept override;

        public:
            virtual void                setSize(const Int2& newSize, const bool onlyUpdateData) noexcept override final;

        private:
            void                        setSizeData(const Int2& newSize);

        public:
            virtual void                setPosition(const Int2& newPosition) override final;
            HWND                        getHandle() const noexcept;

        public:
            virtual void                setCursorType(const CursorType cursorType) noexcept override final;

        public:
            virtual uint32              getCaretBlinkIntervalMs() const noexcept override final;

        public:
            virtual void                textToClipboard(const wchar_t* const text, const uint32 textLength) const noexcept override final;
            virtual void                textFromClipboard(StringW& outText) const noexcept override final;

        public:
            virtual void                showMessageBox(const std::wstring& title, const std::wstring& message, const MessageBoxType messageBoxType) const noexcept override final;

        protected:
            LRESULT                     processDefaultMessage(const UINT Msg, const WPARAM wParam, const LPARAM lParam);

        protected:
            uint32                      _windowStyle;
            HWND                        _hWnd;
            HINSTANCE                   _hInstance;
            OwnPtr<MSG>                 _msg;
        
        private:
            HCURSOR                     _cursorArray[static_cast<uint32>(CursorType::COUNT)];
            Vector<WparamKeyCodePair>   _wParamKeyCodePairArray;
            BYTE                        _byteArrayForRawInput[kRawInputByteSize];
        };
#pragma endregion
    }
}


#endif // !MINT_WINDOWS_WINDOW_H

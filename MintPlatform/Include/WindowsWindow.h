#pragma once


#ifndef _MINT_PLATFORM_WINDOWS_WINDOW_H_
#define _MINT_PLATFORM_WINDOWS_WINDOW_H_


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
	namespace Platform
	{
#pragma region Windows Window Pool
		class WindowsWindow;
		class WindowsWindowPool
		{
		public:
			WindowsWindowPool() = default;
			~WindowsWindowPool() = default;

		public:
			void InsertWindow(const HWND hWnd, WindowsWindow* const windowsWindow);
			LRESULT RedirectMessage(const HWND hWnd, const UINT Msg, const WPARAM wParam, const LPARAM lParam);

		private:
			Vector<WindowsWindow*> _windowArray;
			HashMap<HWND, uint8> _hWndMap;
		};
#pragma endregion


#pragma region Windows Window
		class WindowsWindow final : public IWindow
		{
			friend WindowsWindowPool;

			struct WPARAMKeyCodePair
			{
				WPARAMKeyCodePair() = default;
				WPARAMKeyCodePair(const WPARAM wParam, const Platform::KeyCode keyCode);

				WPARAM _wParam;
				Platform::KeyCode _keyCode;
			};

		public:
			WindowsWindow();
			virtual ~WindowsWindow() = default;

		public:
			virtual bool Create(const WindowCreationDesc& windowCreationDesc) noexcept override;
			virtual void Destroy() noexcept override;

		private:
			void BuildWPARAMKeyCodePairArray() noexcept;
			Platform::KeyCode ConvertWPARAMToKeyCode(const WPARAM wParam) const noexcept;
			WPARAM ConvertKeyCodeToWPARAM(const Platform::KeyCode keyCode) const noexcept;

		public:
			virtual bool IsRunning() noexcept override;

		public:
			virtual void SetSize(const Int2& newSize, const bool onlyUpdateData) noexcept override final;

		private:
			void SetSizeData(const Int2& newSize);

		public:
			virtual void SetPosition(const Int2& newPosition) override final;
			HWND GetHandle() const noexcept;

		public:
			virtual void SetCursorType(const CursorType cursorType) noexcept override final;

		public:
			virtual uint32 GetCaretBlinkIntervalMs() const noexcept override final;

		public:
			virtual void TextToClipboard(const wchar_t* const text, const uint32 textLength) const noexcept override final;
			virtual void TextFromClipboard(StringW& outText) const noexcept override final;

		public:
			virtual void ShowMessageBox(const std::wstring& title, const std::wstring& message, const MessageBoxType messageBoxType) const noexcept override final;

		protected:
			LRESULT ProcessDefaultMessage(const UINT Msg, const WPARAM wParam, const LPARAM lParam);

		protected:
			uint32 _windowStyle;
			HWND _hWnd;
			HINSTANCE _hInstance;
			OwnPtr<MSG> _msg;

		private:
			HCURSOR _cursorArray[static_cast<uint32>(CursorType::COUNT)];
			Vector<WPARAMKeyCodePair> _WPARAMKeyCodePairArray;
			BYTE _byteArrayForRawInput[kRawInputByteSize];
		};
#pragma endregion
	}
}


#endif // !_MINT_PLATFORM_WINDOWS_WINDOW_H_

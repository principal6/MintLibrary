#pragma once


#ifndef _MINT_PLATFORM_WINDOW_H_
#define _MINT_PLATFORM_WINDOW_H_


#include <string>

#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/String.h>
#include <MintContainer/Include/Color.h>
#include <MintContainer/Include/OwnPtr.h>
#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/HashMap.h>

#include <MintMath/Include/Int2.h>
#include <MintMath/Include/Float3.h>

#include <MintPlatform/Include/PlatformCommon.h>


#if defined(MINT_PLATFORM_WINDOWS)
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
#endif // defined(MINT_PLATFORM_WINDOWS)

namespace mint
{
	class Window;
}


namespace mint
{
	enum class WindowStyle
	{
		Default,
		Clean,
		Resizable,
	};

	struct WindowCreationDesc
	{
		Int2 _size{ 800, 600 };
		Int2 _position{ kInt32Min, kInt32Min };
		const wchar_t* _title{ L"MintLibrary" };
		WindowStyle _style{ WindowStyle::Default };
		Color _backgroundColor{ 1.0f, 1.0f, 1.0f };
	};

	enum class WindowCreationError
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

	enum class CursorType
	{
		Arrow,
		SizeVert,
		SizeHorz,
		SizeLeftTilted,
		SizeRightTilted,

		COUNT
	};


#pragma region WindowPool
	class WindowPool
	{
	public:
		WindowPool() = default;
		~WindowPool() = default;

	public:
		static WindowPool& GetInstance();
#if defined(MINT_PLATFORM_WINDOWS)
		void InsertWindow(const HWND hWnd, Window* const window);
		LRESULT RedirectMessage(const HWND hWnd, const UINT Msg, const WPARAM wParam, const LPARAM lParam);
#endif // defined(MINT_PLATFORM_WINDOWS)

	private:
		Vector<Window*> _windowArray;
#if defined(MINT_PLATFORM_WINDOWS)
		HashMap<HWND, uint8> _hWndMap;
#endif // defined(MINT_PLATFORM_WINDOWS)
	};
#pragma endregion


#pragma region Window
	class Window
	{
		friend WindowPool;

	public:
		Window();
		~Window() = default;

	public:
		bool Create(const WindowCreationDesc& windowCreationDesc) noexcept;
		void Destroy() noexcept;
		WindowCreationError GetWindowCreationError() const noexcept { return _windowCreationError; }

	public:
		bool IsRunning() noexcept;

	public:
		void SetSize(const Int2& newSize, const bool onlyUpdateData) noexcept;
		const Int2& GetSize() const noexcept { return _windowCreationDesc._size; }
		MINT_INLINE bool IsResized() const noexcept { bool result = _isWindowResized; _isWindowResized = false; return result; }

		void SetPosition(const Int2& newPosition);
		const Int2& GetPosition() const noexcept { return _windowCreationDesc._position; }

		const Color& GetBackgroundColor() const noexcept { return _windowCreationDesc._backgroundColor; }

		void SetCursorType(const CursorType cursorType) noexcept;
		CursorType GetCursorType() const noexcept { return _currentCursorType; }

		uint32 GetCaretBlinkIntervalMs() const noexcept;

	public:
		void TextToClipboard(const wchar_t* const text, const uint32 textLength) const noexcept;
		void TextFromClipboard(StringW& outText) const noexcept;
		void ShowMessageBox(const std::wstring& title, const std::wstring& message, const MessageBoxType messageBoxType) const noexcept;

#if defined(MINT_PLATFORM_WINDOWS)
	public:
		HWND GetHandle() const noexcept;

	private:
		LRESULT ProcessDefaultMessage(const UINT Msg, const WPARAM wParam, const LPARAM lParam);
		void BuildWPARAMKeyCodePairArray() noexcept;
		KeyCode ConvertWPARAMToKeyCode(const WPARAM wParam) const noexcept;
		WPARAM ConvertKeyCodeToWPARAM(const KeyCode keyCode) const noexcept;

#endif // defined(MINT_PLATFORM_WINDOWS)

	private:
		void SetSizeData(const Int2& newSize);

	private:
		static constexpr uint32 kEventQueueCapacity = 128;

	private:
		bool _isRunning;
		WindowCreationDesc _windowCreationDesc;
		WindowCreationError _windowCreationError;
		Int2 _entireWindowSize;

	private:
		mutable bool _isWindowResized;

	private:
		CursorType _currentCursorType;

#if defined(MINT_PLATFORM_WINDOWS)
		struct WPARAMKeyCodePair
		{
			WPARAMKeyCodePair() = default;
			WPARAMKeyCodePair(const WPARAM wParam, const KeyCode keyCode);

			WPARAM _wParam;
			KeyCode _keyCode;
		};

	private:
		uint32 _windowStyle;
		HWND _hWnd;
		HINSTANCE _hInstance;
		OwnPtr<MSG> _msg;

	private:
		HCURSOR _cursorArray[static_cast<uint32>(CursorType::COUNT)];
		Vector<WPARAMKeyCodePair> _WPARAMKeyCodePairArray;
		BYTE _byteArrayForRawInput[kRawInputByteSize];
#endif // defined(MINT_PLATFORM_WINDOWS)
	};
#pragma endregion
}


#endif // !_MINT_PLATFORM_WINDOW_H_

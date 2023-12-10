﻿#pragma once


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
		Float3 _backgroundColor{ 1.0f, 1.0f, 1.0f };
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


	class IWindow abstract
	{
	public:
		IWindow();
		IWindow(const PlatformType platformType);
		virtual ~IWindow() = default;

	public:
		virtual bool Create(const WindowCreationDesc& windowCreationDesc) noexcept abstract;
		virtual void Destroy() noexcept { _isRunning = false; }
		WindowCreationError GetWindowCreationError() const noexcept { return _windowCreationError; }

	public:
		virtual bool IsRunning() noexcept { return _isRunning; }

	public:
		virtual void SetSize(const Int2& newSize, const bool onlyUpdateData) noexcept { _isWindowResized = true; }
		const Int2& GetSize() const noexcept { return _windowCreationDesc._size; }
		MINT_INLINE bool IsResized() const noexcept { bool result = _isWindowResized; _isWindowResized = false; return result; }

		virtual void SetPosition(const Int2& newPosition) abstract;
		const Int2& GetPosition() const noexcept { return _windowCreationDesc._position; }

		const Float3& GetBackgroundColor() const noexcept { return _windowCreationDesc._backgroundColor; }

		virtual void SetCursorType(const CursorType cursorType) noexcept { _currentCursorType = cursorType; }
		CursorType GetCursorType() const noexcept { return _currentCursorType; }

		virtual uint32 GetCaretBlinkIntervalMs() const noexcept abstract;

	public:
		virtual void TextToClipboard(const wchar_t* const text, const uint32 textLength) const noexcept abstract;
		virtual void TextFromClipboard(StringW& outText) const noexcept abstract;

		virtual void ShowMessageBox(const std::wstring& title, const std::wstring& message, const MessageBoxType messageBoxType) const noexcept abstract;

	protected:
		static constexpr uint32 kEventQueueCapacity = 128;

	protected:
		bool _isRunning;
		PlatformType _platformType;
		WindowCreationDesc _windowCreationDesc;
		WindowCreationError _windowCreationError;
		Int2 _entireWindowSize;

	private:
		mutable bool _isWindowResized;

	protected:
		CursorType _currentCursorType;
	};
}


#endif // !_MINT_PLATFORM_I_WINDOW_H_

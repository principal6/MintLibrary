#include <MintPlatform/Include/WindowsWindow.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/String.hpp>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/HashMap.hpp>

#include <MintMath/Include/Rect.h>

#include <MintPlatform/Include/InputContext.h>

#include <immdev.h>
#include <windowsx.h>
#include <hidusage.h>
#include <mutex>


#pragma comment(lib, "imm32")


namespace mint
{
	namespace Platform
	{
#define MINT_WINDOW_RETURN_OK _windowCreationError = WindowCreationError::None; return true
#define MINT_WINDOW_RETURN_FAIL(error) _windowCreationError = error; return false


#pragma region Windows Window Pool
		static WindowsWindowPool windowsWindowPool;
		void WindowsWindowPool::InsertWindow(const HWND hWnd, WindowsWindow* const windowsWindow)
		{
			static std::mutex mutex;
			std::lock_guard<std::mutex> lock{ mutex };

			if (_hWndMap.Find(hWnd).IsValid() == false)
			{
				_windowArray.PushBack(windowsWindow);
				_hWndMap.Insert(hWnd, static_cast<uint8>(_windowArray.Size() - 1));
			}
		}

		LRESULT WindowsWindowPool::RedirectMessage(const HWND hWnd, const UINT Msg, const WPARAM wParam, const LPARAM lParam)
		{
			if (_hWndMap.Find(hWnd).IsValid() == true)
			{
				const uint8 at = _hWndMap.At(hWnd);
				return _windowArray[at]->ProcessDefaultMessage(Msg, wParam, lParam);
			}
			return ::DefWindowProcW(hWnd, Msg, wParam, lParam);
		}
#pragma endregion


#pragma region Windows Window
		LRESULT WINAPI WndProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam)
		{
			return windowsWindowPool.RedirectMessage(hWnd, Msg, wParam, lParam);
		}


		WindowsWindow::WPARAMKeyCodePair::WPARAMKeyCodePair(const WPARAM wParam, const Platform::KeyCode keyCode)
			: _wParam{ wParam }
			, _keyCode{ keyCode }
		{
			__noop;
		}


		WindowsWindow::WindowsWindow()
			: IWindow(Platform::PlatformType::Windows)
			, _windowStyle{}
			, _hWnd{}
			, _hInstance{}
			, _msg{ MINT_NEW(MSG) }
			, _cursorArray{}
			, _byteArrayForRawInput{}
		{
			__noop;
		}

		bool WindowsWindow::Create(const WindowCreationDesc& windowCreationDesc) noexcept
		{
			_windowCreationDesc = windowCreationDesc;

			_hInstance = ::GetModuleHandleW(nullptr);
			if (_hInstance == nullptr)
			{
				MINT_WINDOW_RETURN_FAIL(WindowCreationError::FailedToGetInstanceHandle);
			}

			if (_windowCreationDesc._title == nullptr)
			{
				MINT_WINDOW_RETURN_FAIL(WindowCreationError::NullptrString);
			}

			const COLORREF backgroundColor = RGB(
				static_cast<uint8>(_windowCreationDesc._backgroundColor[0] * 255.0f),
				static_cast<uint8>(_windowCreationDesc._backgroundColor[1] * 255.0f),
				static_cast<uint8>(_windowCreationDesc._backgroundColor[2] * 255.0f));

			WNDCLASSEXW windowClass{};
			windowClass.cbClsExtra = 0;
			windowClass.cbSize = sizeof(windowClass);
			windowClass.cbWndExtra = 0;
			windowClass.hbrBackground = ::CreateSolidBrush(backgroundColor);
			windowClass.hCursor = nullptr;
			windowClass.hIconSm = windowClass.hIcon = ::LoadIconW(nullptr, IDI_SHIELD);
			windowClass.hInstance = _hInstance;
			windowClass.lpfnWndProc = WndProc;
			windowClass.lpszClassName = L"WindowsWindow";
			windowClass.lpszMenuName = nullptr;
			windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
			::RegisterClassExW(&windowClass);

			_windowStyle = WS_OVERLAPPED;
			switch (_windowCreationDesc._style)
			{
			case WindowStyle::Default:
				_windowStyle = WS_OVERLAPPEDWINDOW;
				break;
			case WindowStyle::Clean:
				_windowStyle = WS_POPUP;
				break;
			case WindowStyle::Resizable:
				_windowStyle = WS_THICKFRAME;
				break;
			default:
				break;
			}

			const int32 x = (_windowCreationDesc._position._x == kInt32Min) ? CW_USEDEFAULT : _windowCreationDesc._position._x;
			const int32 y = (_windowCreationDesc._position._y == kInt32Min) ? CW_USEDEFAULT : _windowCreationDesc._position._y;
			_hWnd = ::CreateWindowExW(0, windowClass.lpszClassName, _windowCreationDesc._title, _windowStyle, x, y,
				_windowCreationDesc._size._x, _windowCreationDesc._size._y, nullptr, nullptr, _hInstance, nullptr);
			if (_hWnd == nullptr)
			{
				MINT_WINDOW_RETURN_FAIL(WindowCreationError::FailedToCreateWindow);
			}

			RECT rawWindowRect;
			::GetWindowRect(_hWnd, &rawWindowRect);
			_windowCreationDesc._position.Set(rawWindowRect.left, rawWindowRect.top);

			SetSize(_windowCreationDesc._size, false);

			::ShowWindow(_hWnd, SW_SHOWDEFAULT);

			_isRunning = true;

			windowsWindowPool.InsertWindow(_hWnd, this);

			// Cursors
			_cursorArray[static_cast<uint32>(CursorType::Arrow)] = LoadCursorW(nullptr, IDC_ARROW);
			_cursorArray[static_cast<uint32>(CursorType::SizeVert)] = LoadCursorW(nullptr, IDC_SIZENS);
			_cursorArray[static_cast<uint32>(CursorType::SizeHorz)] = LoadCursorW(nullptr, IDC_SIZEWE);
			_cursorArray[static_cast<uint32>(CursorType::SizeLeftTilted)] = LoadCursorW(nullptr, IDC_SIZENWSE);
			_cursorArray[static_cast<uint32>(CursorType::SizeRightTilted)] = LoadCursorW(nullptr, IDC_SIZENESW);

			SetCursorType(CursorType::Arrow);

			BuildWPARAMKeyCodePairArray();


			// For Mouse Input
			RAWINPUTDEVICE rawInputDevice{};
			rawInputDevice.dwFlags = 0;
			rawInputDevice.hwndTarget = nullptr;
			rawInputDevice.usUsage = HID_USAGE_GENERIC_MOUSE;
			rawInputDevice.usUsagePage = HID_USAGE_PAGE_GENERIC;
			if (::RegisterRawInputDevices(&rawInputDevice, 1, sizeof(RAWINPUTDEVICE)) == FALSE)
			{
				MINT_WINDOW_RETURN_FAIL(WindowCreationError::FailedToRegisterRawInputDevices);
			}

			MINT_WINDOW_RETURN_OK;
		}

		void WindowsWindow::Destroy() noexcept
		{
			__super::Destroy();

			::DestroyWindow(_hWnd);
		}

		void WindowsWindow::BuildWPARAMKeyCodePairArray() noexcept
		{
			_WPARAMKeyCodePairArray.Clear();

			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair(VK_ESCAPE, Platform::KeyCode::Escape));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair(VK_RETURN, Platform::KeyCode::Enter));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair(VK_UP, Platform::KeyCode::Up));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair(VK_DOWN, Platform::KeyCode::Down));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair(VK_LEFT, Platform::KeyCode::Left));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair(VK_RIGHT, Platform::KeyCode::Right));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair(VK_DELETE, Platform::KeyCode::Delete));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair(VK_HOME, Platform::KeyCode::Home));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair(VK_END, Platform::KeyCode::End));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair(VK_SHIFT, Platform::KeyCode::Shift));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair(VK_CONTROL, Platform::KeyCode::Control));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair(VK_MENU, Platform::KeyCode::Alt));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair(VK_SPACE, Platform::KeyCode::SpaceBar));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('A', Platform::KeyCode::A));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('B', Platform::KeyCode::B));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('C', Platform::KeyCode::C));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('D', Platform::KeyCode::D));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('E', Platform::KeyCode::E));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('F', Platform::KeyCode::F));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('G', Platform::KeyCode::G));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('H', Platform::KeyCode::H));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('I', Platform::KeyCode::I));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('J', Platform::KeyCode::J));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('K', Platform::KeyCode::K));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('L', Platform::KeyCode::L));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('M', Platform::KeyCode::M));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('N', Platform::KeyCode::N));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('O', Platform::KeyCode::O));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('P', Platform::KeyCode::P));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('Q', Platform::KeyCode::Q));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('R', Platform::KeyCode::R));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('S', Platform::KeyCode::S));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('T', Platform::KeyCode::T));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('U', Platform::KeyCode::U));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('V', Platform::KeyCode::V));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('W', Platform::KeyCode::W));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('X', Platform::KeyCode::X));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('Y', Platform::KeyCode::Y));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('Z', Platform::KeyCode::Z));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('0', Platform::KeyCode::Num0));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('1', Platform::KeyCode::Num1));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('2', Platform::KeyCode::Num2));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('3', Platform::KeyCode::Num3));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('4', Platform::KeyCode::Num4));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('5', Platform::KeyCode::Num5));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('6', Platform::KeyCode::Num6));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('7', Platform::KeyCode::Num7));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('8', Platform::KeyCode::Num8));
			_WPARAMKeyCodePairArray.PushBack(WPARAMKeyCodePair('9', Platform::KeyCode::Num9));
		}

		Platform::KeyCode WindowsWindow::ConvertWPARAMToKeyCode(const WPARAM wParam) const noexcept
		{
			const uint32 count = static_cast<uint32>(_WPARAMKeyCodePairArray.Size());
			for (uint32 iter = 0; iter < count; ++iter)
			{
				if (_WPARAMKeyCodePairArray[iter]._wParam == wParam)
				{
					return _WPARAMKeyCodePairArray[iter]._keyCode;
				}
			}
			return Platform::KeyCode::NONE;
		}

		WPARAM WindowsWindow::ConvertKeyCodeToWPARAM(const Platform::KeyCode keyCode) const noexcept
		{
			const uint32 count = static_cast<uint32>(_WPARAMKeyCodePairArray.Size());
			for (uint32 iter = 0; iter < count; ++iter)
			{
				if (_WPARAMKeyCodePairArray[iter]._keyCode == keyCode)
				{
					return _WPARAMKeyCodePairArray[iter]._wParam;
				}
			}
			return 0;
		}

		bool WindowsWindow::IsRunning() noexcept
		{
			Platform::InputContext& inputContext = Platform::InputContext::GetInstance();
			inputContext.FlushInputEvents();

			if (::PeekMessageW(_msg.Get(), nullptr, 0, 0, PM_REMOVE) == TRUE)
			{
				if (_msg->message == WM_QUIT)
				{
					Destroy();
				}

				::TranslateMessage(_msg.Get());
				::DispatchMessageW(_msg.Get());
			}
			inputContext.ProcessEvents();

			return __super::IsRunning();
		}

		void WindowsWindow::SetSize(const Int2& newSize, const bool onlyUpdateData) noexcept
		{
			__super::SetSize(newSize, onlyUpdateData);

			SetSizeData(newSize);

			if (onlyUpdateData)
			{
				return;
			}

			::SetWindowPos(_hWnd, nullptr, _windowCreationDesc._position._x, _windowCreationDesc._position._y, _entireWindowSize._x, _entireWindowSize._y, 0);
		}

		void WindowsWindow::SetSizeData(const Int2& newSize)
		{
			_windowCreationDesc._size = newSize;

			RECT windowRect;
			windowRect.left = _windowCreationDesc._position._x;
			windowRect.top = _windowCreationDesc._position._y;
			windowRect.bottom = windowRect.top + newSize._y;
			windowRect.right = windowRect.left + newSize._x;
			::AdjustWindowRect(&windowRect, _windowStyle, FALSE);

			_entireWindowSize = Int2(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
		}

		void WindowsWindow::SetPosition(const Int2& newPosition)
		{
			_windowCreationDesc._position = newPosition;

			::SetWindowPos(_hWnd, nullptr, _windowCreationDesc._position._x, _windowCreationDesc._position._y, _entireWindowSize._x, _entireWindowSize._y, 0);
		}

		HWND WindowsWindow::GetHandle() const noexcept
		{
			return _hWnd;
		}

		void WindowsWindow::SetCursorType(const CursorType cursorType) noexcept
		{
			__super::SetCursorType(cursorType);

			POINT rawCursorPosition;
			::GetCursorPos(&rawCursorPosition);
			const Float2 cursorPosition = Float2(static_cast<float>(rawCursorPosition.x), static_cast<float>(rawCursorPosition.y));

			RECT rawClientRect;
			::GetClientRect(_hWnd, &rawClientRect);
			POINT leftTop{ rawClientRect.left, rawClientRect.top };
			POINT rightBottom{ rawClientRect.right, rawClientRect.bottom };
			::ClientToScreen(_hWnd, &leftTop);
			::ClientToScreen(_hWnd, &rightBottom);

			const Rect clientRect = Rect::FromLongs(leftTop.x, rightBottom.x, leftTop.y, rightBottom.y);
			if (clientRect.Contains(cursorPosition) == true)
			{
				::SetCursor(_cursorArray[static_cast<uint32>(_currentCursorType)]);
			}
		}

		uint32 WindowsWindow::GetCaretBlinkIntervalMs() const noexcept
		{
			return ::GetCaretBlinkTime();
		}

		void WindowsWindow::TextToClipboard(const wchar_t* const text, const uint32 textLength) const noexcept
		{
			if (StringUtil::IsNullOrEmpty(text) == true)
			{
				return;
			}

			::OpenClipboard(_hWnd);

			const uint32 bufferSize = (textLength + 1) * sizeof(wchar_t);
			const HGLOBAL hGlobalText = ::GlobalAlloc(GMEM_MOVEABLE, bufferSize); // Clipboard 에 넘어간 데이터는 System 이 관리하므로 GlobalFree 를 호출하지 않는다!!!
			if (hGlobalText != nullptr)
			{
				wchar_t* const lockedWstring = reinterpret_cast<wchar_t*>(::GlobalLock(hGlobalText));
				if (lockedWstring != nullptr)
				{
					::memcpy(lockedWstring, text, bufferSize);
					lockedWstring[textLength] = L'\0';

					::GlobalUnlock(hGlobalText);
				}
			}
			::SetClipboardData(CF_UNICODETEXT, hGlobalText);

			::CloseClipboard();
		}

		void WindowsWindow::TextFromClipboard(StringW& outText) const noexcept
		{
			::OpenClipboard(_hWnd);

			const HANDLE hClipboardData = ::GetClipboardData(CF_UNICODETEXT);
			if (hClipboardData != nullptr)
			{
				wchar_t* const lockedWstring = reinterpret_cast<wchar_t*>(::GlobalLock(hClipboardData));
				if (lockedWstring != nullptr)
				{
					const uint32 textLength = StringUtil::Length(lockedWstring);
					outText = lockedWstring;
					::GlobalUnlock(hClipboardData);
				}
			}

			::CloseClipboard();
		}

		void WindowsWindow::ShowMessageBox(const std::wstring& title, const std::wstring& message, const MessageBoxType messageBoxType) const noexcept
		{
			UINT type = MB_OK;
			switch (messageBoxType)
			{
			case MessageBoxType::Ok:
				break;
			case MessageBoxType::Warning:
				type = MB_ICONEXCLAMATION;
				break;
			case MessageBoxType::Error:
				type = MB_ICONERROR;
				break;
			default:
				break;
			}
			::MessageBoxW(_hWnd, message.c_str(), title.c_str(), type);
		}

		LRESULT WindowsWindow::ProcessDefaultMessage(const UINT Msg, const WPARAM wParam, const LPARAM lParam)
		{
			Platform::InputContext& inputContext = Platform::InputContext::GetInstance();
			Platform::InputEvent inputEvent;
			switch (Msg)
			{
			case WM_DESTROY:
			{
				::PostQuitMessage(0);
				return 0;
			}
			case WM_CHAR:
			{
				inputEvent._type = Platform::InputEventType::Keyboard;
				inputEvent._keyboardData._type = Platform::InputKeyboardEventType::CharacterInput;
				inputEvent._keyboardData._character = static_cast<wchar_t>(wParam);
				inputContext.PushInputEvent(inputEvent);
				return 0;
			}
			case WM_IME_CHAR:
			{
				inputEvent._type = Platform::InputEventType::Keyboard;
				inputEvent._keyboardData._type = Platform::InputKeyboardEventType::CharacterInput;
				inputEvent._keyboardData._character = static_cast<wchar_t>(wParam);
				inputContext.PushInputEvent(inputEvent);
				return 0;
			}
			case WM_IME_COMPOSITION:
			{
				if (lParam & GCS_COMPSTR)
				{
					const HIMC hInputMethodContext{ ImmGetContext(_hWnd) };
					const LONG length{ ::ImmGetCompositionStringW(hInputMethodContext, GCS_COMPSTR, NULL, 0) };

					wchar_t wcharArray[3]{};
					::ImmGetCompositionStringW(hInputMethodContext, GCS_COMPSTR, wcharArray, length);

					::ImmReleaseContext(_hWnd, hInputMethodContext);

					inputEvent._type = Platform::InputEventType::Keyboard;
					inputEvent._keyboardData._type = Platform::InputKeyboardEventType::CharacterInputCandidate;
					inputEvent._keyboardData._character = static_cast<wchar_t>(wParam);
					inputContext.PushInputEvent(inputEvent);
					return 0;
				}
				break;
			}
			case WM_KEYDOWN:
			{
				inputEvent._type = Platform::InputEventType::Keyboard;
				inputEvent._keyboardData._type = Platform::InputKeyboardEventType::KeyPressed;
				inputEvent._keyboardData._keyCode = ConvertWPARAMToKeyCode(wParam);
				inputContext.PushInputEvent(inputEvent);
				return 0;
			}
			case WM_KEYUP:
			{
				inputEvent._type = Platform::InputEventType::Keyboard;
				inputEvent._keyboardData._type = Platform::InputKeyboardEventType::KeyReleased;
				inputEvent._keyboardData._keyCode = ConvertWPARAMToKeyCode(wParam);
				inputContext.PushInputEvent(inputEvent);
				return 0;
			}
			case WM_MOUSEMOVE:
			{
				inputEvent._type = Platform::InputEventType::Mouse;
				inputEvent._mouseData._type = Platform::InputMouseEventType::PointerMoved;
				const Float2& mousePosition = Float2(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
				inputEvent._mouseData._position = mousePosition;
				inputContext.PushInputEvent(inputEvent);
				return 0;
			}
			case WM_LBUTTONDOWN:
			{
				inputEvent._type = Platform::InputEventType::Mouse;
				inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonPressed;
				inputEvent._mouseData._button = Platform::MouseButton::Left;
				inputContext.PushInputEvent(inputEvent);
				return 0;
			}
			case WM_LBUTTONUP:
			{
				inputEvent._type = Platform::InputEventType::Mouse;
				inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonReleased;
				inputEvent._mouseData._button = Platform::MouseButton::Left;
				inputContext.PushInputEvent(inputEvent);
				return 0;
			}
			case WM_LBUTTONDBLCLK:
			{
				inputEvent._type = Platform::InputEventType::Mouse;
				inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonDoubleClicked;
				inputEvent._mouseData._button = Platform::MouseButton::Left;
				inputContext.PushInputEvent(inputEvent);
				return 0;
			}
			case WM_MBUTTONDOWN:
			{
				inputEvent._type = Platform::InputEventType::Mouse;
				inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonPressed;
				inputEvent._mouseData._button = Platform::MouseButton::Middle;
				inputContext.PushInputEvent(inputEvent);
				return 0;
			}
			case WM_MBUTTONUP:
			{
				inputEvent._type = Platform::InputEventType::Mouse;
				inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonReleased;
				inputEvent._mouseData._button = Platform::MouseButton::Middle;
				inputContext.PushInputEvent(inputEvent);
				return 0;
			}
			case WM_MBUTTONDBLCLK:
			{
				inputEvent._type = Platform::InputEventType::Mouse;
				inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonDoubleClicked;
				inputEvent._mouseData._button = Platform::MouseButton::Middle;
				inputContext.PushInputEvent(inputEvent);
				return 0;
			}
			case WM_RBUTTONDOWN:
			{
				inputEvent._type = Platform::InputEventType::Mouse;
				inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonPressed;
				inputEvent._mouseData._button = Platform::MouseButton::Right;
				inputContext.PushInputEvent(inputEvent);
				return 0;
			}
			case WM_RBUTTONUP:
			{
				inputEvent._type = Platform::InputEventType::Mouse;
				inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonReleased;
				inputEvent._mouseData._button = Platform::MouseButton::Right;
				inputContext.PushInputEvent(inputEvent);
				return 0;
			}
			case WM_RBUTTONDBLCLK:
			{
				inputEvent._type = Platform::InputEventType::Mouse;
				inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonDoubleClicked;
				inputEvent._mouseData._button = Platform::MouseButton::Right;
				inputContext.PushInputEvent(inputEvent);
				return 0;
			}
			case WM_MOUSEWHEEL:
			{
				inputEvent._type = Platform::InputEventType::Mouse;
				inputEvent._mouseData._type = Platform::InputMouseEventType::WheelScrolled;
				inputEvent._mouseData._wheelScroll = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
				const Float2& mousePosition = Float2(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
				inputEvent._mouseData._position = mousePosition;
				inputContext.PushInputEvent(inputEvent);
				return 0;
			}
			case WM_INPUT:
			{
				UINT byteCount = sizeof(RAWINPUT);
				::GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, _byteArrayForRawInput, &byteCount, sizeof(RAWINPUTHEADER));

				const RAWINPUT& rawInput = *reinterpret_cast<RAWINPUT*>(_byteArrayForRawInput);
				if (rawInput.header.dwType == RIM_TYPEMOUSE)
				{
					const RAWMOUSE& rawMouse = rawInput.data.mouse;

					// Detect mouse button events even when the cursor is outside of the window
					if ((rawMouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) && inputContext.GetMouseState().GetMouseButtonState(MouseButton::Left) == MouseButtonState::Down)
					{
						inputEvent._type = Platform::InputEventType::Mouse;
						inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonReleased;
						inputEvent._mouseData._button = Platform::MouseButton::Left;
						inputContext.PushInputEvent(inputEvent);
					}
					if ((rawMouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) && inputContext.GetMouseState().GetMouseButtonState(MouseButton::Right) == MouseButtonState::Down)
					{
						inputEvent._type = Platform::InputEventType::Mouse;
						inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonReleased;
						inputEvent._mouseData._button = Platform::MouseButton::Right;
						inputContext.PushInputEvent(inputEvent);
					}

					if ((rawMouse.usFlags & MOUSE_MOVE_RELATIVE) == MOUSE_MOVE_RELATIVE)
					{
						inputEvent._type = Platform::InputEventType::Mouse;
						inputEvent._mouseData._type = Platform::InputMouseEventType::PointerMovedDelta;
						inputEvent._mouseData._position = Float2(static_cast<float>(rawMouse.lLastX), static_cast<float>(rawMouse.lLastY));
						inputContext.PushInputEvent(inputEvent);
					}
				}
				return 0;
			}
			case WM_SIZE:
			{
				const Int2 size{ LOWORD(lParam), HIWORD(lParam) };
				SetSize(size, true);
				return 0;
			}
			default:
				break;
			}
			return ::DefWindowProcW(_hWnd, Msg, wParam, lParam);
		}
#pragma endregion
	}
}

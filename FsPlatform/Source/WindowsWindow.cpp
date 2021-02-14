#include <stdafx.h>
#include <FsPlatform/Include/WindowsWindow.h>

#include <windowsx.h>
#include <mutex>


namespace fs
{
	namespace Window
	{
#define FS_WINDOW_RETURN_OK _creationError = CreationError::None; return true
#define FS_WINDOW_RETURN_FAIL(error) _creationError = error; return false


#pragma region Windows Window Pool
		static WindowsWindowPool windowsWindowPool;
		void WindowsWindowPool::insertWindow(const HWND hWnd, WindowsWindow* const windowsWindow)
		{
			static std::mutex mutex;
			std::lock_guard<std::mutex> lock{ mutex };

			if (_hWndMap.find(hWnd) == _hWndMap.end())
			{
				_windowArray.push_back(windowsWindow);
				_hWndMap[hWnd] = static_cast<uint8>(_windowArray.size() - 1);
			}
		}

		LRESULT WindowsWindowPool::redirectMessage(const HWND hWnd, const UINT Msg, const WPARAM wParam, const LPARAM lParam)
		{
			if (_hWndMap.find(hWnd) != _hWndMap.end())
			{
				const uint8 at = _hWndMap.at(hWnd);
				return _windowArray[at]->processDefaultMessage(Msg, wParam, lParam);
			}
			return DefWindowProcW(hWnd, Msg, wParam, lParam);
		}
#pragma endregion


#pragma region Windows Window
		LRESULT WINAPI WndProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam)
		{
			return windowsWindowPool.redirectMessage(hWnd, Msg, wParam, lParam);
		}

		WindowsWindow::WindowsWindow()
			: _windowStyle{}
			, _hWnd{}
			, _hInstance{}
			, _msg{}
		{
			__noop;
		}

		bool WindowsWindow::create(const CreationData& creationData) noexcept
		{
			_creationData = creationData;

			_hInstance = GetModuleHandleW(nullptr);
			if (_hInstance == nullptr)
			{
				FS_WINDOW_RETURN_FAIL(CreationError::FailedToGetInstanceHandle);
			}

			if (creationData._title == nullptr)
			{
				FS_WINDOW_RETURN_FAIL(CreationError::NullptrString);
			}

			const COLORREF backgroundColor = RGB(
				static_cast<uint8>(creationData._bgColor[0] * 255.0f),
				static_cast<uint8>(creationData._bgColor[1] * 255.0f), 
				static_cast<uint8>(creationData._bgColor[2] * 255.0f));

			WNDCLASSEXW windowClass{};
			windowClass.cbClsExtra = 0;
			windowClass.cbSize = sizeof(windowClass);
			windowClass.cbWndExtra = 0;
			windowClass.hbrBackground = CreateSolidBrush(backgroundColor);
			windowClass.hCursor = nullptr;
			windowClass.hIconSm = windowClass.hIcon = LoadIconW(nullptr, IDI_SHIELD);
			windowClass.hInstance = _hInstance;
			windowClass.lpfnWndProc = WndProc;
			windowClass.lpszClassName = L"WindowsWindow";
			windowClass.lpszMenuName = nullptr;
			windowClass.style = CS_VREDRAW | CS_HREDRAW;
			RegisterClassExW(&windowClass);

			_windowStyle = WS_OVERLAPPED;
			switch (creationData._style)
			{
			case Style::Default:
				_windowStyle = WS_OVERLAPPEDWINDOW;
				break;
			case Style::Clean:
				_windowStyle = WS_POPUP;
				break;
			case Style::Resizable:
				_windowStyle = WS_THICKFRAME;
				break;
			default:
				break;
			}

			const int32 x = (creationData._position._x == kInt32Min) ? CW_USEDEFAULT : creationData._position._x;
			const int32 y = (creationData._position._y == kInt32Min) ? CW_USEDEFAULT : creationData._position._y;
			_hWnd = CreateWindowExW(0, windowClass.lpszClassName, creationData._title, _windowStyle, x, y,
				creationData._size._x, creationData._size._y, nullptr, nullptr, _hInstance, nullptr);
			if (_hWnd == nullptr)
			{
				FS_WINDOW_RETURN_FAIL(CreationError::FailedToCreateWindow);
			}

			setSize(creationData._size);

			ShowWindow(_hWnd, SW_SHOWDEFAULT);

			_isRunning = true;

			windowsWindowPool.insertWindow(_hWnd, this);

			// Cursors
			_cursorArray[static_cast<uint32>(CursorType::Arrow)]	= LoadCursorW(nullptr, IDC_ARROW);
			_cursorArray[static_cast<uint32>(CursorType::SizeVert)]	= LoadCursorW(nullptr, IDC_SIZENS);
			_cursorArray[static_cast<uint32>(CursorType::SizeHorz)]	= LoadCursorW(nullptr, IDC_SIZEWE);
			_cursorArray[static_cast<uint32>(CursorType::SizeLeftTilted)]	= LoadCursorW(nullptr, IDC_SIZENWSE);
			_cursorArray[static_cast<uint32>(CursorType::SizeRightTilted)]	= LoadCursorW(nullptr, IDC_SIZENESW);

			setCursorType(CursorType::Arrow);
			
			FS_WINDOW_RETURN_OK;
		}

		void WindowsWindow::destroy() noexcept
		{
			__super::destroy();

			DestroyWindow(_hWnd);
		}

		bool WindowsWindow::isRunning() noexcept
		{
			if (PeekMessageW(&_msg, nullptr, 0, 0, PM_REMOVE) == TRUE)
			{
				if (_msg.message == WM_QUIT)
				{
					destroy();
				}

				TranslateMessage(&_msg);
				DispatchMessageW(&_msg);
			}
			return __super::isRunning();
		}

		void WindowsWindow::setSize(const Int2& newSize)
		{
			_creationData._size = newSize;

			RECT windowRect;
			windowRect.left = _creationData._position._x;
			windowRect.top = _creationData._position._y;
			windowRect.bottom = windowRect.top + newSize._y;
			windowRect.right = windowRect.left +newSize._x;
			AdjustWindowRect(&windowRect, _windowStyle, FALSE);

			_entireSize = Int2(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
			SetWindowPos(_hWnd, nullptr, _creationData._position._x, _creationData._position._y, _entireSize._x, _entireSize._y, 0);
		}

		void WindowsWindow::setPosition(const Int2& newPosition)
		{
			_creationData._position = newPosition;

			SetWindowPos(_hWnd, nullptr, _creationData._position._x, _creationData._position._y, _entireSize._x, _entireSize._y, 0);
		}

		HWND WindowsWindow::getHandle() const noexcept
		{
			return _hWnd;
		}

		void WindowsWindow::setCursorType(const CursorType cursorType) noexcept
		{
			__super::setCursorType(cursorType);

			::SetCursor(_cursorArray[static_cast<uint32>(_currentCursorType)]);
		}

		LRESULT WindowsWindow::processDefaultMessage(const UINT Msg, const WPARAM wParam, const LPARAM lParam)
		{
			EventData eventData;
			switch (Msg)
			{
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
			case WM_KEYDOWN:
			{
				eventData._type = EventType::KeyDown;
				if (wParam == VK_ESCAPE)
				{
					eventData._data._keyCode = EventData::KeyCode::Escape;
				}
				else if (wParam == VK_RETURN)
				{
					eventData._data._keyCode = EventData::KeyCode::Enter;
				}
				else if (wParam == VK_UP)
				{
					eventData._data._keyCode = EventData::KeyCode::Up;
				}
				else if (wParam == VK_DOWN)
				{
					eventData._data._keyCode = EventData::KeyCode::Down;
				}
				else if (wParam == VK_LEFT)
				{
					eventData._data._keyCode = EventData::KeyCode::Left;
				}
				else if (wParam == VK_RIGHT)
				{
					eventData._data._keyCode = EventData::KeyCode::Right;
				}
				_eventQueue.push(eventData);
				return 0;
			}
			case WM_MOUSEMOVE:
			{
				eventData._type = EventType::MouseMove;
				int32 mouseInfo = 0;
				if (wParam == MK_LBUTTON)
				{
					mouseInfo = 1;
				}
				eventData._data._mouseInfo = mouseInfo;
				eventData._data._mousePosition._x = GET_X_LPARAM(lParam);
				eventData._data._mousePosition._y = GET_Y_LPARAM(lParam);
				_eventQueue.push(eventData);
				return 0;
			}
			case WM_LBUTTONDOWN:
			{
				eventData._type = EventType::MouseDown;
				eventData._data._mouseInfo = 1;
				eventData._data._mousePosition._x = GET_X_LPARAM(lParam);
				eventData._data._mousePosition._y = GET_Y_LPARAM(lParam);
				_eventQueue.push(eventData);
				return 0;
			}
			case WM_LBUTTONUP:
			{
				eventData._type = EventType::MouseUp;
				eventData._data._mouseInfo = 1;
				eventData._data._mousePosition._x = GET_X_LPARAM(lParam);
				eventData._data._mousePosition._y = GET_Y_LPARAM(lParam);
				_eventQueue.push(eventData);
				return 0;
			}
			case WM_LBUTTONDBLCLK:
			{
				eventData._type = EventType::MouseDoubleClicked;
				eventData._data._mouseInfo = 1;
				eventData._data._mousePosition._x = GET_X_LPARAM(lParam);
				eventData._data._mousePosition._y = GET_Y_LPARAM(lParam);
				_eventQueue.push(eventData);
				return 0;
			}
			default:
				break;
			}
			return DefWindowProcW(_hWnd, Msg, wParam, lParam);
		}
#pragma endregion
	}
}

#include "WindowsWindow.h"
#include <mutex>


namespace fs
{
	namespace Window
	{
#define RETURN_OK _creationError = CreationError::None; return true
#define RETURN_FAIL(error) _creationError = error; return false


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

		bool WindowsWindow::create(const CreationData& creationData) noexcept
		{
			_creationData = creationData;

			_hInstance = GetModuleHandleW(nullptr);
			if (_hInstance == nullptr)
			{
				RETURN_FAIL(CreationError::FailedToGetInstanceHandle);
			}

			if (creationData._title == nullptr)
			{
				RETURN_FAIL(CreationError::NullptrString);
			}

			const COLORREF backgroundColor = RGB(
				static_cast<uint8>(creationData._bgColor[0] * 255.0f),
				static_cast<uint8>(creationData._bgColor[1] * 255.0f), 
				static_cast<uint8>(creationData._bgColor[2] * 255.0f));

			WNDCLASSEXW windowClass{};
			windowClass.cbClsExtra = 0;
			windowClass.cbSize = sizeof(windowClass);
			windowClass.cbWndExtra = 0;
			windowClass.hbrBackground = CreateSolidBrush(backgroundColor); // 배경을 그리지 않게 한다
			windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
			windowClass.hIconSm = windowClass.hIcon = LoadIconW(nullptr, IDI_SHIELD);
			windowClass.hInstance = _hInstance;
			windowClass.lpfnWndProc = WndProc;
			windowClass.lpszClassName = L"WindowsWindow";
			windowClass.lpszMenuName = nullptr;
			windowClass.style = CS_VREDRAW | CS_HREDRAW;
			RegisterClassExW(&windowClass);

			uint32 windowStyle = WS_OVERLAPPED;
			switch (creationData._style)
			{
			case Style::Default:
				windowStyle = WS_OVERLAPPEDWINDOW;
				break;
			case Style::Clean:
				windowStyle = WS_POPUP;
				break;
			case Style::Resizable:
				windowStyle = WS_THICKFRAME;
				break;
			default:
				break;
			}

			const int32 x = (creationData._position.x() == kInt32Min) ? CW_USEDEFAULT : creationData._position.x();
			const int32 y = (creationData._position.y() == kInt32Min) ? CW_USEDEFAULT : creationData._position.y();
			_hWnd = CreateWindowExW(0, windowClass.lpszClassName, creationData._title, windowStyle, x, y,
				creationData._size.x(), creationData._size.y(), nullptr, nullptr, _hInstance, nullptr);
			if (_hWnd == nullptr)
			{
				RETURN_FAIL(CreationError::FailedToCreateWindow);
			}

			RECT rect;
			GetWindowRect(_hWnd, &rect);
			_creationData._position.set(rect.left, rect.top);

			ShowWindow(_hWnd, SW_SHOWDEFAULT);

			_isRunning = true;

			windowsWindowPool.insertWindow(_hWnd, this);

			RETURN_OK;
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

		void WindowsWindow::size(const Int2& newSize)
		{
			_creationData._size = newSize;
		
			SetWindowPos(_hWnd, nullptr, _creationData._position.x(), _creationData._position.y(),
				_creationData._size.x(), _creationData._size.y(), 0);
		}

		const Int2& WindowsWindow::size() const noexcept
		{
			return _creationData._size;
		}

		void WindowsWindow::position(const Int2& newPosition)
		{
			_creationData._position = newPosition;

			SetWindowPos(_hWnd, nullptr, _creationData._position.x(), _creationData._position.y(),
				_creationData._size.x(), _creationData._size.y(), 0);
		}

		const Int2& WindowsWindow::position() const noexcept
		{
			return _creationData._position;
		}

		HWND WindowsWindow::getHandle() const noexcept
		{
			return _hWnd;
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
			default:
				break;
			}
			return DefWindowProcW(_hWnd, Msg, wParam, lParam);
		}
#pragma endregion
	}
}

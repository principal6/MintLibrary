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
    namespace Window
    {
#define MINT_WINDOW_RETURN_OK _windowCreationError = WindowCreationError::None; return true
#define MINT_WINDOW_RETURN_FAIL(error) _windowCreationError = error; return false


#pragma region Windows Window Pool
        static WindowsWindowPool windowsWindowPool;
        void WindowsWindowPool::insertWindow(const HWND hWnd, WindowsWindow* const windowsWindow)
        {
            static std::mutex mutex;
            std::lock_guard<std::mutex> lock{ mutex };

            if (_hWndMap.find(hWnd).isValid() == false)
            {
                _windowArray.push_back(windowsWindow);
                _hWndMap.insert(hWnd, static_cast<uint8>(_windowArray.size() - 1));
            }
        }

        LRESULT WindowsWindowPool::redirectMessage(const HWND hWnd, const UINT Msg, const WPARAM wParam, const LPARAM lParam)
        {
            if (_hWndMap.find(hWnd).isValid() == true)
            {
                const uint8 at = _hWndMap.at(hWnd);
                return _windowArray[at]->processDefaultMessage(Msg, wParam, lParam);
            }
            return ::DefWindowProcW(hWnd, Msg, wParam, lParam);
        }
#pragma endregion


#pragma region Windows Window
        LRESULT WINAPI wndProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam)
        {
            return windowsWindowPool.redirectMessage(hWnd, Msg, wParam, lParam);
        }


        WindowsWindow::WparamKeyCodePair::WparamKeyCodePair(const WPARAM wParam, const Platform::KeyCode keyCode)
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

        bool WindowsWindow::create(const WindowCreationDesc& windowCreationDesc) noexcept
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
            windowClass.lpfnWndProc = wndProc;
            windowClass.lpszClassName = L"WindowsWindow";
            windowClass.lpszMenuName = nullptr;
            windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
            ::RegisterClassExW(&windowClass);

            _windowStyle = WS_OVERLAPPED;
            switch (_windowCreationDesc._style)
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
            _windowCreationDesc._position.set(rawWindowRect.left, rawWindowRect.top);

            setSize(_windowCreationDesc._size, false);

            ::ShowWindow(_hWnd, SW_SHOWDEFAULT);

            _isRunning = true;

            windowsWindowPool.insertWindow(_hWnd, this);

            // Cursors
            _cursorArray[static_cast<uint32>(CursorType::Arrow)]    = LoadCursorW(nullptr, IDC_ARROW);
            _cursorArray[static_cast<uint32>(CursorType::SizeVert)]    = LoadCursorW(nullptr, IDC_SIZENS);
            _cursorArray[static_cast<uint32>(CursorType::SizeHorz)]    = LoadCursorW(nullptr, IDC_SIZEWE);
            _cursorArray[static_cast<uint32>(CursorType::SizeLeftTilted)]    = LoadCursorW(nullptr, IDC_SIZENWSE);
            _cursorArray[static_cast<uint32>(CursorType::SizeRightTilted)]    = LoadCursorW(nullptr, IDC_SIZENESW);

            setCursorType(CursorType::Arrow);

            buildWparamKeyCodePairArray();


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

        void WindowsWindow::destroy() noexcept
        {
            __super::destroy();

            ::DestroyWindow(_hWnd);
        }

        void WindowsWindow::buildWparamKeyCodePairArray() noexcept
        {
            _wParamKeyCodePairArray.clear();
            
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair(VK_ESCAPE, Platform::KeyCode::Escape));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair(VK_RETURN, Platform::KeyCode::Enter));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair(VK_UP, Platform::KeyCode::Up));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair(VK_DOWN, Platform::KeyCode::Down));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair(VK_LEFT, Platform::KeyCode::Left));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair(VK_RIGHT, Platform::KeyCode::Right));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair(VK_DELETE, Platform::KeyCode::Delete));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair(VK_HOME, Platform::KeyCode::Home));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair(VK_END, Platform::KeyCode::End));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair(VK_SHIFT, Platform::KeyCode::Shift));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair(VK_CONTROL, Platform::KeyCode::Control));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair(VK_MENU, Platform::KeyCode::Alt));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('A', Platform::KeyCode::A));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('B', Platform::KeyCode::B));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('C', Platform::KeyCode::C));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('D', Platform::KeyCode::D));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('E', Platform::KeyCode::E));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('F', Platform::KeyCode::F));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('G', Platform::KeyCode::G));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('H', Platform::KeyCode::H));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('I', Platform::KeyCode::I));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('J', Platform::KeyCode::J));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('K', Platform::KeyCode::K));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('L', Platform::KeyCode::L));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('M', Platform::KeyCode::M));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('N', Platform::KeyCode::N));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('O', Platform::KeyCode::O));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('P', Platform::KeyCode::P));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('Q', Platform::KeyCode::Q));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('R', Platform::KeyCode::R));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('S', Platform::KeyCode::S));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('T', Platform::KeyCode::T));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('U', Platform::KeyCode::U));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('V', Platform::KeyCode::V));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('W', Platform::KeyCode::W));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('X', Platform::KeyCode::X));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('Y', Platform::KeyCode::Y));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('Z', Platform::KeyCode::Z));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('0', Platform::KeyCode::Num0));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('1', Platform::KeyCode::Num1));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('2', Platform::KeyCode::Num2));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('3', Platform::KeyCode::Num3));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('4', Platform::KeyCode::Num4));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('5', Platform::KeyCode::Num5));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('6', Platform::KeyCode::Num6));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('7', Platform::KeyCode::Num7));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('8', Platform::KeyCode::Num8));
            _wParamKeyCodePairArray.push_back(WparamKeyCodePair('9', Platform::KeyCode::Num9));
        }

        Platform::KeyCode WindowsWindow::convertWparamToKeyCode(const WPARAM wParam) const noexcept
        {
            const uint32 count = static_cast<uint32>(_wParamKeyCodePairArray.size());
            for (uint32 iter = 0; iter < count; ++iter)
            {
                if (_wParamKeyCodePairArray[iter]._wParam == wParam)
                {
                    return _wParamKeyCodePairArray[iter]._keyCode;
                }
            }
            return Platform::KeyCode::NONE;
        }

        WPARAM WindowsWindow::convertKeyCodeToWparam(const Platform::KeyCode keyCode) const noexcept
        {
            const uint32 count = static_cast<uint32>(_wParamKeyCodePairArray.size());
            for (uint32 iter = 0; iter < count; ++iter)
            {
                if (_wParamKeyCodePairArray[iter]._keyCode == keyCode)
                {
                    return _wParamKeyCodePairArray[iter]._wParam;
                }
            }
            return 0;
        }

        bool WindowsWindow::isRunning() noexcept
        {
            Platform::InputContext& inputContext = Platform::InputContext::getInstance();
            inputContext.flushInputEvents();

            if (::PeekMessageW(&_msg, nullptr, 0, 0, PM_REMOVE) == TRUE)
            {
                if (_msg->message == WM_QUIT)
                {
                    destroy();
                }

                ::TranslateMessage(&_msg);
                ::DispatchMessageW(&_msg);
            }
            return __super::isRunning();
        }

        void WindowsWindow::setSize(const Int2& newSize, const bool onlyUpdateData) noexcept
        {
            __super::setSize(newSize, onlyUpdateData);

            setSizeData(newSize);
            
            if (onlyUpdateData)
            {
                return;
            }

            ::SetWindowPos(_hWnd, nullptr, _windowCreationDesc._position._x, _windowCreationDesc._position._y, _entireSize._x, _entireSize._y, 0);
        }

        void WindowsWindow::setSizeData(const Int2& newSize)
        {
            _windowCreationDesc._size = newSize;

            RECT windowRect;
            windowRect.left = _windowCreationDesc._position._x;
            windowRect.top = _windowCreationDesc._position._y;
            windowRect.bottom = windowRect.top + newSize._y;
            windowRect.right = windowRect.left + newSize._x;
            ::AdjustWindowRect(&windowRect, _windowStyle, FALSE);

            _entireSize = Int2(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
        }

        void WindowsWindow::setPosition(const Int2& newPosition)
        {
            _windowCreationDesc._position = newPosition;

            ::SetWindowPos(_hWnd, nullptr, _windowCreationDesc._position._x, _windowCreationDesc._position._y, _entireSize._x, _entireSize._y, 0);
        }

        HWND WindowsWindow::getHandle() const noexcept
        {
            return _hWnd;
        }

        void WindowsWindow::setCursorType(const CursorType cursorType) noexcept
        {
            __super::setCursorType(cursorType);

            POINT rawCursorPosition;
            ::GetCursorPos(&rawCursorPosition);
            const Float2 cursorPosition = Float2(static_cast<float>(rawCursorPosition.x), static_cast<float>(rawCursorPosition.y));

            RECT rawClientRect;
            ::GetClientRect(_hWnd, &rawClientRect);
            POINT leftTop{ rawClientRect.left, rawClientRect.top };
            POINT rightBottom{ rawClientRect.right, rawClientRect.bottom };
            ::ClientToScreen(_hWnd, &leftTop);
            ::ClientToScreen(_hWnd, &rightBottom);
            
            const Rect clientRect = Rect::fromLongs(leftTop.x, rightBottom.x, leftTop.y, rightBottom.y);
            if (clientRect.contains(cursorPosition) == true)
            {
                ::SetCursor(_cursorArray[static_cast<uint32>(_currentCursorType)]);
            }
        }

        uint32 WindowsWindow::getCaretBlinkIntervalMs() const noexcept
        {
            return ::GetCaretBlinkTime();
        }

        void WindowsWindow::textToClipboard(const wchar_t* const text, const uint32 textLength) const noexcept
        {
            if (StringUtil::isNullOrEmpty(text) == true)
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

        void WindowsWindow::textFromClipboard(StringW& outText) const noexcept
        {
            ::OpenClipboard(_hWnd);

            const HANDLE hClipboardData = ::GetClipboardData(CF_UNICODETEXT);
            if (hClipboardData != nullptr)
            {
                wchar_t* const lockedWstring = reinterpret_cast<wchar_t*>(::GlobalLock(hClipboardData));
                if (lockedWstring != nullptr)
                {
                    const uint32 textLength = StringUtil::length(lockedWstring);
                    outText = lockedWstring;
                    ::GlobalUnlock(hClipboardData);
                }
            }

            ::CloseClipboard();
        }

        void WindowsWindow::showMessageBox(const std::wstring& title, const std::wstring& message, const MessageBoxType messageBoxType) const noexcept
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

        LRESULT WindowsWindow::processDefaultMessage(const UINT Msg, const WPARAM wParam, const LPARAM lParam)
        {
            const Float2& mousePosition = Float2(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));

            Platform::InputContext& inputContext = Platform::InputContext::getInstance();
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
                inputContext.pushInputEvent(inputEvent);
                return 0;
            }
            case WM_IME_CHAR:
            {
                inputEvent._type = Platform::InputEventType::Keyboard;
                inputEvent._keyboardData._type = Platform::InputKeyboardEventType::CharacterInput;
                inputEvent._keyboardData._character = static_cast<wchar_t>(wParam);
                inputContext.pushInputEvent(inputEvent);
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
                    inputContext.pushInputEvent(inputEvent);
                    return 0;
                }
                break;
            }    
            case WM_KEYDOWN:
            {
                inputEvent._type = Platform::InputEventType::Keyboard;
                inputEvent._keyboardData._type = Platform::InputKeyboardEventType::KeyPressed;
                inputEvent._keyboardData._keyCode = convertWparamToKeyCode(wParam);
                inputContext.pushInputEvent(inputEvent);
                return 0;
            }
            case WM_KEYUP:
            {
                inputEvent._type = Platform::InputEventType::Keyboard;
                inputEvent._keyboardData._type = Platform::InputKeyboardEventType::KeyReleased;
                inputEvent._keyboardData._keyCode = convertWparamToKeyCode(wParam);
                inputContext.pushInputEvent(inputEvent);
                return 0;
            }
            case WM_MOUSEMOVE:
            {
                //int32 mouseInfo = 0;
                //if (wParam == MK_LBUTTON)
                //{
                //    mouseInfo = 1;
                //}
                inputEvent._type = Platform::InputEventType::Mouse;
                inputEvent._mouseData._type = Platform::InputMouseEventType::PointerMoved;
                inputEvent._mouseData._position = mousePosition;
                inputContext.pushInputEvent(inputEvent);
                return 0;
            }
            case WM_LBUTTONDOWN:
            {
                inputEvent._type = Platform::InputEventType::Mouse;
                inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonPressed;
                inputEvent._mouseData._button = Platform::MouseButton::Left;
                inputContext.pushInputEvent(inputEvent);
                return 0;
            }
            case WM_LBUTTONUP:
            {
                inputEvent._type = Platform::InputEventType::Mouse;
                inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonReleased;
                inputEvent._mouseData._button = Platform::MouseButton::Left;
                inputContext.pushInputEvent(inputEvent);
                return 0;
            }
            case WM_LBUTTONDBLCLK:
            {
                inputEvent._type = Platform::InputEventType::Mouse;
                inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonDoubleClicked;
                inputEvent._mouseData._button = Platform::MouseButton::Left;
                inputContext.pushInputEvent(inputEvent);
                return 0;
            }
            case WM_MBUTTONDOWN:
            {
                inputEvent._type = Platform::InputEventType::Mouse;
                inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonPressed;
                inputEvent._mouseData._button = Platform::MouseButton::Middle;
                inputContext.pushInputEvent(inputEvent);
                return 0;
            }
            case WM_MBUTTONUP:
            {
                inputEvent._type = Platform::InputEventType::Mouse;
                inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonReleased;
                inputEvent._mouseData._button = Platform::MouseButton::Middle;
                inputContext.pushInputEvent(inputEvent);
                return 0;
            }
            case WM_MBUTTONDBLCLK:
            {
                inputEvent._type = Platform::InputEventType::Mouse;
                inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonDoubleClicked;
                inputEvent._mouseData._button = Platform::MouseButton::Middle;
                inputContext.pushInputEvent(inputEvent);
                return 0;
            }
            case WM_RBUTTONDOWN:
            {
                inputEvent._type = Platform::InputEventType::Mouse;
                inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonPressed;
                inputEvent._mouseData._button = Platform::MouseButton::Right;
                inputContext.pushInputEvent(inputEvent);
                return 0;
            }
            case WM_RBUTTONUP:
            {
                inputEvent._type = Platform::InputEventType::Mouse;
                inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonReleased;
                inputEvent._mouseData._button = Platform::MouseButton::Right;
                inputContext.pushInputEvent(inputEvent);
                return 0;
            }
            case WM_RBUTTONDBLCLK:
            {
                inputEvent._type = Platform::InputEventType::Mouse;
                inputEvent._mouseData._type = Platform::InputMouseEventType::ButtonDoubleClicked;
                inputEvent._mouseData._button = Platform::MouseButton::Right;
                inputContext.pushInputEvent(inputEvent);
                return 0;
            }
            case WM_MOUSEWHEEL:
            {
                //const WORD fwKeys = GET_KEYSTATE_WPARAM(wParam);
                //(fwKeys & MK_MBUTTON) ? 1 : 0;

                inputEvent._type = Platform::InputEventType::Mouse;
                inputEvent._mouseData._type = Platform::InputMouseEventType::WheelScrolled;
                inputEvent._mouseData._wheelScroll = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
                inputEvent._mouseData._position = mousePosition;
                inputContext.pushInputEvent(inputEvent);
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
                    if ((rawMouse.usFlags & MOUSE_MOVE_RELATIVE) == MOUSE_MOVE_RELATIVE)
                    {
                        inputEvent._type = Platform::InputEventType::Mouse;
                        inputEvent._mouseData._type = Platform::InputMouseEventType::PointerMovedDelta;
                        inputEvent._mouseData._position = Float2(static_cast<float>(rawMouse.lLastX), static_cast<float>(rawMouse.lLastY));
                        inputContext.pushInputEvent(inputEvent);
                    }
                }
                return 0;
            }
            case WM_SIZE:
            {
                const Int2 size{ LOWORD(lParam), HIWORD(lParam) };
                setSize(size, true);
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

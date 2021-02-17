#pragma once


#ifndef FS_WINDOWS_WINDOW_H
#define FS_WINDOWS_WINDOW_H


#include <FsPlatform/Include/IWindow.h>


namespace fs
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
			void								insertWindow(const HWND hWnd, WindowsWindow* const windowsWindow);
			LRESULT								redirectMessage(const HWND hWnd, const UINT Msg, const WPARAM wParam, const LPARAM lParam);

		private:
			std::vector<WindowsWindow*>			_windowArray;
			std::unordered_map<HWND, uint8>		_hWndMap;
		};
#pragma endregion


#pragma region Windows Window
		class WindowsWindow final : public IWindow
		{
			friend WindowsWindowPool;

			struct WparamKeyCodePair
			{
				WparamKeyCodePair() = default;
				WparamKeyCodePair(const WPARAM wParam, const EventData::KeyCode keyCode);

				WPARAM				_wParam;
				EventData::KeyCode	_keyCode;
			};

		public:
												WindowsWindow();
			virtual								~WindowsWindow() = default;

		public:
			virtual bool						create(const CreationData& creationData) noexcept override;
			virtual void						destroy() noexcept override;

		private:
			void								buildWparamKeyCodePairArray() noexcept;
			EventData::KeyCode					convertWparamToKeyCode(const WPARAM wParam) const noexcept;
			WPARAM								convertKeyCodeToWparam(const EventData::KeyCode keyCode) const noexcept;

		public:
			virtual bool						isRunning() noexcept override;

		public:
			virtual void						setSize(const Int2& newSize) override final;
			virtual void						setPosition(const Int2& newPosition) override final;
			HWND								getHandle() const noexcept;

		public:
			virtual void						setCursorType(const CursorType cursorType) noexcept override final;
		
		public:
			virtual const uint32				getCaretBlinkIntervalMs() const noexcept override final;
			virtual const bool					isKeyDown(const EventData::KeyCode keyCode) const noexcept override final;

		protected:
			LRESULT								processDefaultMessage(const UINT Msg, const WPARAM wParam, const LPARAM lParam);

		protected:
			uint32								_windowStyle;
			HWND								_hWnd;
			HINSTANCE							_hInstance;
			MSG									_msg;
		
		private:
			HCURSOR								_cursorArray[static_cast<uint32>(CursorType::COUNT)];
			std::vector<WparamKeyCodePair>		_wParamKeyCodePairArray;
		};
#pragma endregion
	}
}


#endif // !FS_WINDOWS_WINDOW_H

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
		class WindowsWindow : public IWindow
		{
			friend WindowsWindowPool;

		public:
												WindowsWindow();
			virtual								~WindowsWindow() = default;

		public:
			virtual bool						create(const CreationData& creationData) noexcept override;
			virtual void						destroy() noexcept override;

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

		protected:
			LRESULT								processDefaultMessage(const UINT Msg, const WPARAM wParam, const LPARAM lParam);

		protected:
			uint32								_windowStyle;
			HWND								_hWnd;
			HINSTANCE							_hInstance;
			MSG									_msg;
		
		private:
			HCURSOR								_cursorArray[static_cast<uint32>(CursorType::COUNT)];
		};
#pragma endregion
	}
}


#endif // !FS_WINDOWS_WINDOW_H

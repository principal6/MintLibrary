#pragma once


#ifndef FS_WINDOWS_WINDOW_H
#define FS_WINDOWS_WINDOW_H


#include <Platform/IWindow.h>
#include <Windows.h>
#include <vector>
#include <unordered_map>


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
												WindowsWindow() = default;
			virtual								~WindowsWindow() = default;

		public:
			virtual bool						create(const CreationData& creationData) noexcept override;
			virtual void						destroy() noexcept override;

		public:
			virtual bool						isRunning() noexcept override;

		public:
			virtual void						size(const Int2& newSize) override;
			virtual const Int2&					size() const noexcept override;
			virtual void						position(const Int2& newPosition) override;
			virtual const Int2&					position() const noexcept override;
			HWND								getHandle() const noexcept;

		protected:
			LRESULT								processDefaultMessage(const UINT Msg, const WPARAM wParam, const LPARAM lParam);

		protected:
			HWND								_hWnd{};
			HINSTANCE							_hInstance{};
			MSG									_msg;
		};
#pragma endregion
	}
}


#endif // !FS_WINDOWS_WINDOW_H

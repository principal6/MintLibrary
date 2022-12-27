#include <MintPlatform/Include/IWindow.h>


namespace mint
{
	namespace Platform
	{
		IWindow::IWindow()
			: IWindow(Platform::PlatformType::INVALID)
		{
			__noop;
		}

		IWindow::IWindow(const Platform::PlatformType platformType)
			: _isRunning{ false }
			, _platformType{ platformType }
			, _windowCreationError{ WindowCreationError::None }
			, _isWindowResized{ false }
			, _currentCursorType{ CursorType::Arrow }
		{
			__noop;
		}
	}
}

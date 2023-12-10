#include <MintPlatform/Include/IWindow.h>


namespace mint
{
	IWindow::IWindow()
		: IWindow(PlatformType::INVALID)
	{
		__noop;
	}

	IWindow::IWindow(const PlatformType platformType)
		: _isRunning{ false }
		, _platformType{ platformType }
		, _windowCreationError{ WindowCreationError::None }
		, _isWindowResized{ false }
		, _currentCursorType{ CursorType::Arrow }
	{
		__noop;
	}
}

#include <MintPlatform/Include/IWindow.h>


namespace mint
{
    namespace Window
    {
        IWindow::IWindow()
            : IWindow(Platform::PlatformType::INVALID)
        {
            __noop;
        }
        
        IWindow::IWindow(const Platform::PlatformType platformType)
            : _isRunning{ false }
            , _platformType{ platformType }
            , _creationError{ CreationError::None }
            , _isWindowResized{ false }
            , _currentCursorType{ CursorType::Arrow }
        {
            __noop;
        }
    }
}

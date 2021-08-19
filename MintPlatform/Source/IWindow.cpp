#include <MintPlatform/Include/IWindow.h>


namespace mint
{
    namespace Window
    {
        IWindow::IWindow()
            : IWindow(mint::Platform::PlatformType::INVALID)
        {
            __noop;
        }
        
        IWindow::IWindow(const mint::Platform::PlatformType platformType)
            : _isRunning{ false }
            , _platformType{ platformType }
            , _creationError{ CreationError::None }
            , _isWindowResized{ false }
            , _currentCursorType{ CursorType::Arrow }
        {
            __noop;
        }
        
        void IWindow::resetPerFrameStates() noexcept
        {
            _isWindowResized = false;
        }
    }
}

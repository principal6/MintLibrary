#include <MintApp/Include/App.h>

#include <MintPlatform/Include/Window.h>
#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintApp/Include/ObjectPool.hpp>


namespace mint
{
	App::App(const WindowCreationDesc& windowCreationDesc, bool useMSAA)
		: _window{ MINT_NEW(Window) }
		, _objectPool{ MINT_NEW(ObjectPool) }
	{
		if (_window->Create(windowCreationDesc) == false)
		{
			MINT_NEVER;
			return;
		}

		_graphicDevice.Assign(MINT_NEW(Rendering::GraphicDevice, *_window, useMSAA));
		if (_graphicDevice->Initialize() == false)
		{
			MINT_NEVER;
			return;
		}
	}

	App::~App()
	{
	}

	bool App::IsRunning()
	{
		if (_window->IsResized())
		{
			_objectPool->UpdateScreenSize(Float2(_window->GetSize()));
		}

		return _window->IsRunning();
	}

	Window& App::GetWindow()
	{
		return *_window;
	}

	Rendering::GraphicDevice& App::GetGraphicDevice()
	{
		return *_graphicDevice;
	}
}

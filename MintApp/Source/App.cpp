#include <MintApp/Include/App.h>

#include <MintPlatform/Include/Window.h>
#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintGUI/Include/GUISystem.h>
#include <MintApp/Include/ObjectPool.hpp>
#include <MintApp/Include/ObjectRenderer.h>


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

		_objectRenderer.Assign(MINT_NEW(ObjectRenderer, *_graphicDevice));
		_guiSystem.Assign(MINT_NEW(GUI::GUISystem, *_graphicDevice));
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

		if (_window->IsRunning())
		{
			_objectPool->ComputeDeltaTime();
			_guiSystem->Update();
			return true;
		}
		return false;
	}
	
	void App::BeginRendering()
	{
		_graphicDevice->BeginRendering();
	}

	void App::EndRendering()
	{
		_objectRenderer->Render(*_objectPool);
		
		_guiSystem->Render();

		_graphicDevice->EndRendering();
	}

	Window& App::GetWindow()
	{
		return *_window;
	}

	Rendering::GraphicDevice& App::GetGraphicDevice()
	{
		return *_graphicDevice;
	}

	ObjectPool& App::GetObjectPool()
	{
		return *_objectPool;
	}

	GUI::GUISystem& App::GetGUISystem()
	{
		return *_guiSystem;
	}
}

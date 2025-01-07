#include <MintApp/Include/App.h>

#include <MintPlatform/Include/Window.h>
#include <MintRenderingBase/Include/GraphicsDevice.h>
#include <MintGUI/Include/GUISystem.h>
#include <MintApp/Include/ObjectPool.hpp>
#include <MintApp/Include/ObjectRenderer.h>


namespace mint
{
	App::App(const WindowCreationDesc& windowCreationDesc, bool useMSAA)
		: _window{ MINT_NEW(Window) }
		, _objectPool{ MINT_NEW(ObjectPool) }
		, _guiEntityPool{ MINT_NEW(GUI::GUIEntityPool) }
		, _frameNumber{ 0 }
	{
		if (_window->Create(windowCreationDesc) == false)
		{
			MINT_NEVER;
			return;
		}

		_graphicsDevice.Assign(MINT_NEW(Rendering::GraphicsDevice, *_window, useMSAA));
		if (_graphicsDevice->Initialize() == false)
		{
			MINT_NEVER;
			return;
		}

		_objectRenderer.Assign(MINT_NEW(ObjectRenderer, *_graphicsDevice));
	}

	App::~App()
	{
	}

	bool App::IsRunning()
	{
		++_frameNumber;

		if (_window->IsResized())
		{
			_objectPool->UpdateScreenSize(Float2(_window->GetSize()));
		}

		if (_window->IsRunning())
		{
			DeltaTimer::GetInstance().ComputeDeltaTime(_frameNumber);

			GUI::GUISystem::InputSystem(_guiEntityPool->GetEntities());
			return true;
		}
		return false;
	}
	
	void App::BeginRendering()
	{
		_graphicsDevice->BeginRendering();
	}

	void App::EndRendering()
	{
		_objectRenderer->Render(*_objectPool);
		
		GUI::GUISystem::RenderSystem(_guiEntityPool->GetEntities(), *_graphicsDevice);

		_graphicsDevice->EndRendering();
	}

	Window& App::GetWindow()
	{
		return *_window;
	}

	Rendering::GraphicsDevice& App::GetGraphicsDevice()
	{
		return *_graphicsDevice;
	}

	ObjectPool& App::GetObjectPool()
	{
		return *_objectPool;
	}

	GUI::GUIEntityPool& App::GetGUIEntityPool()
	{
		return *_guiEntityPool;
	}
}

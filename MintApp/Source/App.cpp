#include <MintApp/Include/App.h>

#include <MintPlatform/Include/Window.h>
#include <MintRenderingBase/Include/GraphicsDevice.h>
#include <MintGUI/Include/GUISystem.h>
#include <MintApp/Include/SceneObjectPool.hpp>
#include <MintApp/Include/SceneObjectRenderer.h>


namespace mint
{
	App::App(const WindowCreationDesc& windowCreationDesc, const AppCreationDesc& appCreationDesc)
		: _window{ MINT_NEW(Window) }
		, _sceneObjectPool{ MINT_NEW(SceneObjectPool) }
		, _guiSystem{ MINT_NEW(GUI::GUISystem) }
		, _frameNumber{ 0 }
	{
		if (_window->Create(windowCreationDesc) == false)
		{
			MINT_NEVER;
			return;
		}

		_graphicsDevice.Assign(MINT_NEW(Rendering::GraphicsDevice, *_window, appCreationDesc._useMSAA));
		if (_graphicsDevice->Initialize() == false)
		{
			MINT_NEVER;
			return;
		}

		_sceneObjectRenderer.Assign(MINT_NEW(SceneObjectRenderer, *_graphicsDevice));

		const Float2 windowSize{ GetWindow().GetSize() };
		_defaultCameraObject = _sceneObjectPool->CreateSceneObject();
		switch (appCreationDesc._appType)
		{
		case AppType::Default3D:
		{
			CameraComponent* cameraComponent = _sceneObjectPool->CreateSceneObjectComponent<CameraComponent>();
			cameraComponent->SetPerspectiveCamera(Math::ToRadian(60.0f), 0.01f, 100.0f, windowSize._x / windowSize._y);
			cameraComponent->RotatePitch(0.125f);
			_defaultCameraObject->GetObjectTransform()._translation._z = 5.0f;
			_defaultCameraObject->AttachComponent(cameraComponent);
			_is3DMode = true;
			break;
		}
		case AppType::Default2D:
		{
			CameraComponent* cameraComponent = _sceneObjectPool->CreateSceneObjectComponent<CameraComponent>();
			cameraComponent->SetPerspectiveCamera(Math::ToRadian(60.0f), 1.0f, 100.0f, windowSize._x / windowSize._y);
			_defaultCameraObject->GetObjectTransform()._translation._z = 5.0f;
			_defaultCameraObject->AttachComponent(cameraComponent);
			_is3DMode = false;
			break;
		}
		default:
			MINT_ASSERT(false, "Invalid AppType!");
			break;
		}

		_currentCameraObject = _defaultCameraObject;
	}

	App::~App()
	{
	}

	bool App::IsRunning()
	{
		++_frameNumber;

		if (_window->IsResized())
		{
			_sceneObjectPool->UpdateScreenSize(Float2(_window->GetSize()));
		}

		if (_window->IsRunning())
		{
			DeltaTimer::GetInstance().ComputeDeltaTime(_frameNumber);

			_guiSystem->Update();

			const float deltaTime = DeltaTimer::GetInstance().GetDeltaTimeS();
			const InputContext& inputContext = InputContext::GetInstance();
			CameraComponent* const cameraComponent = static_cast<CameraComponent*>(_currentCameraObject->GetComponent(SceneObjectComponentType::CameraComponent));
			cameraComponent->SteerDefault(deltaTime, inputContext, _is3DMode);
			return true;
		}
		return false;
	}

	void App::BeginRendering()
	{
		_graphicsDevice->BeginRendering();

		CameraComponent* const cameraComponent = static_cast<CameraComponent*>(_currentCameraObject->GetComponent(SceneObjectComponentType::CameraComponent));
		_graphicsDevice->SetViewProjectionMatrix(cameraComponent->GetViewMatrix(), cameraComponent->GetProjectionMatrix());
	}

	void App::EndRendering()
	{
		_sceneObjectRenderer->Render(*_sceneObjectPool);

		_guiSystem->Render(*_graphicsDevice);

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

	SceneObjectPool& App::GetObjectPool()
	{
		return *_sceneObjectPool;
	}

	GUI::GUISystem& App::GetGUISystem()
	{
		return *_guiSystem;
	}
}

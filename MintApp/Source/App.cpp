#include <MintApp/Include/App.h>

#include <MintPlatform/Include/Window.h>
#include <MintRendering/Include/GraphicsDevice.h>
#include <MintRendering/Include/ShapeRenderer.h>
#include <MintRendering/Include/FontRenderer.h>
#include <MintRendering/Include/SpriteRenderer.h>
#include <MintGUI/Include/GUISystem.h>
#include <MintECS/Include/Entity.hpp>
#include <MintApp/Include/SceneObjectRegistry.hpp>
#include <MintApp/Include/SceneObjectRenderer.h>
#include <MintApp/Include/SceneObjectSystems.h>


namespace mint
{
	App::App(const WindowCreationDesc& windowCreationDesc, const AppCreationDesc& appCreationDesc)
		: _window{ MINT_NEW(Window) }
		, _sceneObjectRegistry{ MINT_NEW(SceneObjectRegistry) }
		, _sceneObjectSystems{ MINT_NEW(SceneObjectSystems, *_sceneObjectRegistry) }
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
		_guiSystem.Assign(MINT_NEW(GUI::GUISystem, *_graphicsDevice));

		_sceneObjectRenderer.Assign(MINT_NEW(SceneObjectRenderer, *_graphicsDevice));

		Rendering::GraphicsDevice::DelegateExecuteRenderPhase delegateExecuteRenderPhase;
		delegateExecuteRenderPhase.Bind<App, &App::ExecuteRenderPhase>(this);
		_graphicsDevice->SetDelegateExecuteRenderPhase(delegateExecuteRenderPhase);

		const Float2 windowSize{ GetWindow().GetSize() };
		_defaultCameraObject = _sceneObjectRegistry->CreateSceneObject();
		switch (appCreationDesc._appType)
		{
		case AppType::Default3D:
		{
			CameraComponent cameraComponent;
			SceneObjectComponentHelpers::MakePerspectiveCamera(Math::ToRadian(60.0f), 0.01f, 100.0f, windowSize._x / windowSize._y, cameraComponent);
			SceneObjectComponentHelpers::RotateCameraPitch(0.125f, cameraComponent);
			_sceneObjectRegistry->AttachComponent(_defaultCameraObject, std::move(cameraComponent));

			TransformComponent& transformComponent = _sceneObjectRegistry->GetComponentMust<TransformComponent>(_defaultCameraObject);
			transformComponent._transform._translation._z = 5.0f;

			_is3DMode = true;
			break;
		}
		case AppType::Default2D:
		{
			CameraComponent cameraComponent;
			SceneObjectComponentHelpers::MakePerspectiveCamera(Math::ToRadian(60.0f), 1.0f, 100.0f, windowSize._x / windowSize._y, cameraComponent);
			SceneObjectComponentHelpers::RotateCameraPitch(0.125f, cameraComponent);
			_sceneObjectRegistry->AttachComponent(_defaultCameraObject, std::move(cameraComponent));

			TransformComponent& transformComponent = _sceneObjectRegistry->GetComponentMust<TransformComponent>(_defaultCameraObject);
			transformComponent._transform._translation._z = 5.0f;

			_is3DMode = false;
			break;
		}
		default:
			MINT_ASSERT(false, "Invalid AppType!");
			break;
		}

		_sceneObjectSystems->GetCameraSystem().SetCurrentCameraObject(_defaultCameraObject);
	}

	App::~App()
	{
	}

	bool App::IsRunning()
	{
		++_frameNumber;

		if (_window->IsResized())
		{
			_sceneObjectSystems->GetCameraSystem().UpdateScreenSize(Float2(_window->GetSize()));
		}

		if (_window->IsRunning())
		{
			DeltaTimer::GetInstance().ComputeDeltaTime(_frameNumber);

			_guiSystem->Update();

			const float deltaTime = DeltaTimer::GetInstance().GetDeltaTimeS();
			const InputContext& inputContext = InputContext::GetInstance();
			_sceneObjectSystems->GetCameraSystem().DefaultSteerCurrentCamera(deltaTime, inputContext, _is3DMode);
			return true;
		}
		return false;
	}

	void App::ExecuteRenderPhase(const Rendering::ScopedRenderPhase& scopedRenderPhase)
	{
		if (scopedRenderPhase.IsPhase(Rendering::RenderPhaseLabel::WorldSpace))
		{
			const SceneObject currentCameraObject = _sceneObjectSystems->GetCameraSystem().GetCurrentCameraObject();
			const Float4x4& viewMatrix = _sceneObjectSystems->GetCameraSystem().MakeViewMatrix(currentCameraObject);
			CameraComponent* const cameraComponent = _sceneObjectRegistry->GetComponent<CameraComponent>(currentCameraObject);
			_graphicsDevice->SetViewProjectionMatrix(viewMatrix, cameraComponent->_projectionMatrix);

			_sceneObjectRenderer->Render(*_sceneObjectRegistry);
		}
		else if (scopedRenderPhase.IsPhase(Rendering::RenderPhaseLabel::ScreenSpace))
		{
			_guiSystem->Render();
		}
	}

	Window& App::GetWindow()
	{
		return *_window;
	}

	Rendering::GraphicsDevice& App::GetGraphicsDevice()
	{
		return *_graphicsDevice;
	}

	SceneObjectRegistry& App::GetObjectPool()
	{
		return *_sceneObjectRegistry;
	}

	SceneObjectSystems& App::GetSceneObjectSystems()
	{
		return *_sceneObjectSystems;
	}

	GUI::GUISystem& App::GetGUISystem()
	{
		return *_guiSystem;
	}
}

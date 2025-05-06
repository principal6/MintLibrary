#include <MintApp/Include/App.h>

#include <MintPlatform/Include/Window.h>
#include <MintRenderingBase/Include/GraphicsDevice.h>
#include <MintRenderingBase/Include/ShapeRenderer.h>
#include <MintRenderingBase/Include/SpriteRenderer.h>
#include <MintGUI/Include/GUISystem.h>
#include <MintApp/Include/SceneObjectPool.hpp>
#include <MintApp/Include/SceneObjectRenderer.h>
#include <MintApp/Include/SceneObjectSystems.h>


namespace mint
{
	App::App(const WindowCreationDesc& windowCreationDesc, const AppCreationDesc& appCreationDesc)
		: _window{ MINT_NEW(Window) }
		, _sceneObjectPool{ MINT_NEW(SceneObjectPool) }
		, _sceneObjectSystems{ MINT_NEW(SceneObjectSystems) }
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

		const Float2 windowSize{ GetWindow().GetSize() };
		_defaultCameraObject = _sceneObjectPool->CreateSceneObject();
		switch (appCreationDesc._appType)
		{
		case AppType::Default3D:
		{
			CameraComponent cameraComponent;
			SceneObjectComponentHelpers::MakePerspectiveCamera(Math::ToRadian(60.0f), 0.01f, 100.0f, windowSize._x / windowSize._y, cameraComponent);
			SceneObjectComponentHelpers::RotateCameraPitch(0.125f, cameraComponent);
			_sceneObjectPool->AttachComponent(_defaultCameraObject, std::move(cameraComponent));

			TransformComponent* const transformComponent = _sceneObjectPool->GetComponent<TransformComponent>(_defaultCameraObject);
			transformComponent->_transform._translation._z = 5.0f;

			_is3DMode = true;
			break;
		}
		case AppType::Default2D:
		{
			CameraComponent cameraComponent;
			SceneObjectComponentHelpers::MakePerspectiveCamera(Math::ToRadian(60.0f), 1.0f, 100.0f, windowSize._x / windowSize._y, cameraComponent);
			SceneObjectComponentHelpers::RotateCameraPitch(0.125f, cameraComponent);
			_sceneObjectPool->AttachComponent(_defaultCameraObject, std::move(cameraComponent));

			TransformComponent* const transformComponent = _sceneObjectPool->GetComponent<TransformComponent>(_defaultCameraObject);
			transformComponent->_transform._translation._z = 5.0f;

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

	void App::BeginRendering()
	{
		_graphicsDevice->BeginRendering();

		const SceneObject currentCameraObject = _sceneObjectSystems->GetCameraSystem().GetCurrentCameraObject();
		const Float4x4& viewMatrix = _sceneObjectSystems->GetCameraSystem().MakeViewMatrix(currentCameraObject);
		CameraComponent* const cameraComponent = _sceneObjectPool->GetComponent<CameraComponent>(currentCameraObject);
		_graphicsDevice->SetViewProjectionMatrix(viewMatrix, cameraComponent->_projectionMatrix);

		_isInRenderingScope = true;
		_hasScreenSpaceRenderingScopeInRenderingScope = false;
	}

	void App::BeginScreenSpaceRendering()
	{
		MINT_ASSERT(_isInRenderingScope == true, "반드시 BeginRendering() 과 EndRendering() 사이에 호출되어야 합니다.");
		MINT_ASSERT(_isInScreenSpaceRenderingScope == false, "BeginScreenSpaceRendering() 을 두 번 연달아 호출할 수 없습니다. 먼저 EndScreenSpaceRendering() 을 호출해 주세요!");

		_hasScreenSpaceRenderingScopeInRenderingScope = true;

		// Before proceeding to ScreenSpace rendering, flush all render commands to the GPU.
		_graphicsDevice->GetShapeRenderer().Render();

		_isInScreenSpaceRenderingScope = true;
		_graphicsDevice->SetViewProjectionMatrix(Float4x4::kIdentity, _graphicsDevice->GetScreenSpace2DProjectionMatrix());
		_graphicsDevice->GetSpriteRenderer().SetCoordinateSpace(Rendering::CoordinateSpace::Screen);
		_graphicsDevice->GetShapeRenderer().SetCoordinateSpace(Rendering::CoordinateSpace::Screen);
	}

	void App::EndScreenSpaceRendering()
	{
		MINT_ASSERT(_isInRenderingScope == true, "반드시 BeginRendering() 과 EndRendering() 사이에 호출되어야 합니다.");
		MINT_ASSERT(_isInScreenSpaceRenderingScope == true, "반드시 BeginScreenSpaceRendering() 이후에 호출되어야 합니다.");

		_guiSystem->Render();

		_graphicsDevice->GetSpriteRenderer().Render();
		_graphicsDevice->GetShapeRenderer().Render();

		const SceneObject& currentCameraObject = _sceneObjectSystems->GetCameraSystem().GetCurrentCameraObject();
		CameraComponent& cameraComponent = _sceneObjectPool->GetComponentMust<CameraComponent>(currentCameraObject);
		const Float4x4& currentCameraViewMatrix = _sceneObjectSystems->GetCameraSystem().MakeViewMatrix(currentCameraObject);
		_graphicsDevice->SetViewProjectionMatrix(currentCameraViewMatrix, cameraComponent._projectionMatrix);
		_graphicsDevice->GetShapeRenderer().SetCoordinateSpace(Rendering::CoordinateSpace::World);
		_isInScreenSpaceRenderingScope = false;
	}

	void App::EndRendering()
	{
		_sceneObjectRenderer->Render(*_sceneObjectPool);

		if (_hasScreenSpaceRenderingScopeInRenderingScope == false)
		{
			BeginScreenSpaceRendering();

			_guiSystem->Render();
			
			EndScreenSpaceRendering();
		}

		_graphicsDevice->EndRendering();

		_isInRenderingScope = false;
		_hasScreenSpaceRenderingScopeInRenderingScope = false;
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

	SceneObjectSystems& App::GetSceneObjectSystems()
	{
		return *_sceneObjectSystems;
	}

	GUI::GUISystem& App::GetGUISystem()
	{
		return *_guiSystem;
	}
}

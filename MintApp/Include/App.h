#pragma once


#ifndef _MINT_APP_APP_H_
#define _MINT_APP_APP_H_


#include <MintContainer/Include/OwnPtr.h>


namespace mint
{
	using SceneObject = ECS::EntityBase<uint64, 40>;
	class SceneObjectPool;
	class SceneObjectRenderer;
	class SceneObjectSystems;
	class Window;
	struct WindowCreationDesc;

	namespace Rendering
	{
		class GraphicsDevice;
		class ScopedRenderPhase;
	}

	namespace GUI
	{
		class GUISystem;
	}
}

namespace mint
{
	enum class AppType
	{
		Default3D,
		Default2D,
	};
	struct AppCreationDesc
	{
		bool _useMSAA = false;
		AppType _appType = AppType::Default3D;
	};

	class App
	{
	public:
		App(const WindowCreationDesc& windowCreationDesc, const AppCreationDesc& appCreationDesc);
		virtual ~App();

	public:
		virtual bool IsRunning();

	public:
		Window& GetWindow();
		Rendering::GraphicsDevice& GetGraphicsDevice();
		SceneObjectPool& GetObjectPool();
		SceneObjectSystems& GetSceneObjectSystems();
		GUI::GUISystem& GetGUISystem();

	protected:
		void ExecuteRenderPhase(const Rendering::ScopedRenderPhase& scopedRenderPhase);

	protected:
		OwnPtr<Window> _window;
		OwnPtr<Rendering::GraphicsDevice> _graphicsDevice;
		OwnPtr<SceneObjectPool> _sceneObjectPool;
		OwnPtr<SceneObjectRenderer> _sceneObjectRenderer;
		OwnPtr<SceneObjectSystems> _sceneObjectSystems;
		OwnPtr<GUI::GUISystem> _guiSystem;
		uint64 _frameNumber;

	protected:
		bool _is3DMode = true;
		SceneObject _defaultCameraObject;
	};
}


#endif // !_MINT_APP_APP_H_

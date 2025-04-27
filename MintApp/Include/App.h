#pragma once


#ifndef _MINT_APP_APP_H_
#define _MINT_APP_APP_H_


#include <MintContainer/Include/OwnPtr.h>


namespace mint
{
	class ObjectPool;
	class ObjectRenderer;
	class Window;
	struct WindowCreationDesc;

	namespace Rendering
	{
		class GraphicsDevice;
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
		void BeginRendering();
		void EndRendering();

	public:
		Window& GetWindow();
		Rendering::GraphicsDevice& GetGraphicsDevice();
		ObjectPool& GetObjectPool();
		GUI::GUISystem& GetGUISystem();
		const SharedPtr<Object>& GetCurrentCameraObject() const { return _currentCameraObject; }

	protected:
		OwnPtr<Window> _window;
		OwnPtr<Rendering::GraphicsDevice> _graphicsDevice;
		OwnPtr<ObjectPool> _objectPool;
		OwnPtr<ObjectRenderer> _objectRenderer;
		OwnPtr<GUI::GUISystem> _guiSystem;
		uint64 _frameNumber;

	protected:
		bool _is3DMode = true;
		SharedPtr<Object> _defaultCameraObject;
		SharedPtr<Object> _currentCameraObject;
	};
}


#endif // !_MINT_APP_APP_H_

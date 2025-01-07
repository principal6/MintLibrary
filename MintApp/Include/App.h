#pragma once


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
		class GUIEntityPool;
	}
}

namespace mint
{
	class App
	{
	public:
		App(const WindowCreationDesc& windowCreationDesc, bool useMSAA);
		virtual ~App();

	public:
		virtual bool IsRunning();
		void BeginRendering();
		void EndRendering();

	public:
		Window& GetWindow();
		Rendering::GraphicsDevice& GetGraphicsDevice();
		ObjectPool& GetObjectPool();
		GUI::GUIEntityPool& GetGUIEntityPool();

	protected:
		OwnPtr<Window> _window;
		OwnPtr<Rendering::GraphicsDevice> _graphicsDevice;
		OwnPtr<ObjectPool> _objectPool;
		OwnPtr<ObjectRenderer> _objectRenderer;
		OwnPtr<GUI::GUIEntityPool> _guiEntityPool;
		uint64 _frameNumber;
	};
}

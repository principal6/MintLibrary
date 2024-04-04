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
		class GraphicDevice;
	}

	namespace GUI
	{
		class GUISystem;
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
		Rendering::GraphicDevice& GetGraphicDevice();
		ObjectPool& GetObjectPool();
		GUI::GUISystem& GetGUISystem();

	protected:
		OwnPtr<Window> _window;
		OwnPtr<Rendering::GraphicDevice> _graphicDevice;
		OwnPtr<ObjectPool> _objectPool;
		OwnPtr<ObjectRenderer> _objectRenderer;
		OwnPtr<GUI::GUISystem> _guiSystem;
	};
}

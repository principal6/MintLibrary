#pragma once


#include <MintContainer/Include/OwnPtr.h>


namespace mint
{
	class ObjectPool;
	class Window;
	struct WindowCreationDesc;

	namespace Rendering
	{
		class GraphicDevice;
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

	public:
		Window& GetWindow();
		Rendering::GraphicDevice& GetGraphicDevice();

	protected:
		OwnPtr<Window> _window;
		OwnPtr<Rendering::GraphicDevice> _graphicDevice;
		OwnPtr<ObjectPool> _objectPool;
	};
}

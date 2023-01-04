﻿#include <MintLibrary/Include/AllHeaders.h>
#include <MintLibrary/Include/AllHpps.h>


#include <MintMath/Include/Test.h>
#include <MintContainer/Include/Test.h>
#include <MintPlatform/Include/Test.h>
#include <MintLanguage/Include/Test.h>
#include <MintReflection/Include/Test.h>
#include <MintRendering/Include/Test.h>
#include <MintLibrary/Include/Test.h>


#pragma comment(lib, "MintLibrary.lib")

#pragma optimize("", off)


bool run2DTestWindow(mint::Platform::IWindow& window, mint::Rendering::GraphicDevice& graphicDevice);
bool run3DTestWindow(mint::Platform::IWindow& window, mint::Rendering::GraphicDevice& graphicDevice);


int main()
{
	mint::Library::initialize();

	using namespace mint;
	using namespace Platform;
	using namespace Rendering;

	WindowCreationDesc windowCreationDesc;
	windowCreationDesc._style = WindowStyle::Default;
	windowCreationDesc._position.set(200, 100);
	windowCreationDesc._size.set(1024, 768);
	windowCreationDesc._title = L"HI";
	windowCreationDesc._backgroundColor.set(0.875f, 0.875f, 0.875f);

	WindowsWindow window;
	if (window.create(windowCreationDesc) == false)
	{
		WindowCreationError windowCreationError = window.getWindowCreationError();
		return false;
	}

	GraphicDevice graphicDevice{ window };
	graphicDevice.initialize();

#if defined MINT_DEBUG
	//Logger::setOutputFileName("LOG.txt");
	TestMath::test();
	TestContainers::test();
	TestPlatform::test();
	TestLanguage::test();
	TestReflection::test();
	//TestRendering::test(graphicDevice);
	TestLibrary::test();
#endif

	//run2DTestWindow(window, graphicDevice);
	run3DTestWindow(window, graphicDevice);
	return 0;
}


bool run2DTestWindow(mint::Platform::IWindow& window, mint::Rendering::GraphicDevice& graphicDevice)
{
	using namespace mint;
	using namespace Platform;
	using namespace Rendering;

	const Float2 windowSize = graphicDevice.getWindowSizeFloat2();
	const Float4x4 projectionMatrix = Float4x4::projectionMatrix2DFromTopLeft(windowSize._x, windowSize._y);
	ImageRenderer imageRenderer{ graphicDevice, 0, ByteColor(0, 0, 0, 0) };
	ByteColorImage byteColorImage;
	ImageLoader imageLoader;
	imageLoader.loadImage("Assets/test_image.png", byteColorImage);
	DxResourcePool& resourcePool = graphicDevice.getResourcePool();
	const GraphicObjectID textureID = resourcePool.addTexture2D(DxTextureFormat::R8G8B8A8_UNORM, byteColorImage.getBytes(), byteColorImage.getWidth(), byteColorImage.getHeight());
	resourcePool.getResource(textureID).bindToShader(GraphicShaderType::PixelShader, 0);
	const Platform::InputContext& inputContext = Platform::InputContext::getInstance();
	while (window.isRunning() == true)
	{
		if (inputContext.isKeyPressed())
		{
			if (inputContext.isKeyDown(Platform::KeyCode::Enter) == true)
			{
				graphicDevice.getShaderPool().recompileAllShaders();
			}
			else if (inputContext.isKeyDown(Platform::KeyCode::Num1) == true)
			{
				graphicDevice.useSolidCullBackRasterizer();
			}
			else if (inputContext.isKeyDown(Platform::KeyCode::Num2) == true)
			{
				graphicDevice.useWireFrameCullBackRasterizer();
			}
			else if (inputContext.isKeyDown(Platform::KeyCode::Num3) == true)
			{
				graphicDevice.useWireFrameNoCullingRasterizer();
			}
		}
		else if (inputContext.isKeyReleased())
		{
			__noop;
		}
		else if (inputContext.isMouseWheelScrolled())
		{
			const float mouseWheelScroll = inputContext.getMouseWheelScroll();
		}

		// Rendering
		{
			graphicDevice.beginRendering();

			graphicDevice.setViewProjectionMatrix(Float4x4::kIdentity, projectionMatrix);

			imageRenderer.drawImage(Float2(50, 50), Float2(80, 20), Float2(0, 0), Float2(1, 1));
			imageRenderer.render();
			imageRenderer.flush();

			graphicDevice.endRendering();
		}

		Profiler::FPSCounter::count();
	}
	return true;
}

bool run3DTestWindow(mint::Platform::IWindow& window, mint::Rendering::GraphicDevice& graphicDevice)
{
	using namespace mint;
	using namespace Platform;
	using namespace Rendering;


	ObjectPool objectPool;
	Object* const testObject = objectPool.createObject();
	CameraObject* const testCameraObject = objectPool.createCameraObject();
	Float2 windowSize = graphicDevice.getWindowSizeFloat2();
	testCameraObject->setPerspectiveZRange(0.01f, 1000.0f);
	testCameraObject->setPerspectiveScreenRatio(windowSize._x / windowSize._y);
	{
		testObject->attachComponent(objectPool.createMeshComponent());

		Transform& transform = testObject->getObjectTransform();
		transform._translation._z = -4.0f;
	}
	testCameraObject->rotatePitch(0.125f);

	MeshRenderer meshRenderer{ graphicDevice };
	InstantRenderer instantRenderer{ graphicDevice };
	Game::SkeletonGenerator testSkeletonGenerator;
	Float4x4 testSkeletonWorldMatrix;
	testSkeletonWorldMatrix.setTranslation(1.0f, 0.0f, -4.0f);
	Float4x4 bindPoseLocalMatrix;
	testSkeletonGenerator.createJoint(-1, "Root", bindPoseLocalMatrix);
	bindPoseLocalMatrix.setTranslation(1.0f, 0.0f, 0.0f);
	testSkeletonGenerator.createJoint(0, "Elbow", bindPoseLocalMatrix);
	bindPoseLocalMatrix.setTranslation(1.0f, 0.0f, 0.0f);
	testSkeletonGenerator.createJoint(1, "Tip", bindPoseLocalMatrix);
	testSkeletonGenerator.buildBindPoseModelSpace();
	Game::Skeleton testSkeleton(testSkeletonGenerator);

	const Platform::InputContext& inputContext = Platform::InputContext::getInstance();
	GUI::GUIContext& guiContext = graphicDevice.getGUIContext();
	uint64 previousFrameTimeMs = 0;
	while (window.isRunning() == true)
	{
		objectPool.computeDeltaTime();

		// Events
		guiContext.processEvent();

		if (inputContext.isKeyPressed())
		{
			if (inputContext.isKeyDown(Platform::KeyCode::Enter) == true)
			{
				graphicDevice.getShaderPool().recompileAllShaders();
			}
			else if (inputContext.isKeyDown(Platform::KeyCode::Num1) == true)
			{
				graphicDevice.useSolidCullBackRasterizer();
			}
			else if (inputContext.isKeyDown(Platform::KeyCode::Num2) == true)
			{
				graphicDevice.useWireFrameCullBackRasterizer();
			}
			else if (inputContext.isKeyDown(Platform::KeyCode::Num3) == true)
			{
				graphicDevice.useWireFrameNoCullingRasterizer();
			}
			else if (inputContext.isKeyDown(Platform::KeyCode::Num4) == true)
			{
				MeshComponent* const meshComponent = static_cast<MeshComponent*>(testObject->getComponent(ObjectComponentType::MeshComponent));
				meshComponent->shouldDrawNormals(!meshComponent->shouldDrawNormals());
			}
			else if (inputContext.isKeyDown(Platform::KeyCode::Num5) == true)
			{
				MeshComponent* const meshComponent = static_cast<MeshComponent*>(testObject->getComponent(ObjectComponentType::MeshComponent));
				meshComponent->shouldDrawEdges(!meshComponent->shouldDrawEdges());
			}
			else if (inputContext.isKeyDown(Platform::KeyCode::Shift) == true)
			{
				testCameraObject->setBoostMode(true);
			}
		}
		else if (inputContext.isKeyReleased())
		{
			if (inputContext.isKeyUp(Platform::KeyCode::Shift) == true)
			{
				testCameraObject->setBoostMode(false);
			}
		}
		else if (inputContext.isMouseWheelScrolled())
		{
			const float mouseWheelScroll = inputContext.getMouseWheelScroll();
			if (mouseWheelScroll > 0.0f)
			{
				testCameraObject->increaseMoveSpeed();
			}
			else
			{
				testCameraObject->decreaseMoveSpeed();
			}
		}

		if (window.isResized())
		{
			objectPool.updateScreenSize(graphicDevice.getWindowSizeFloat2());
		}

		testCameraObject->steer(inputContext, false);

		// Rendering
		{
			graphicDevice.beginRendering();

			testSkeleton.renderSkeleton(instantRenderer, testSkeletonWorldMatrix);

			graphicDevice.setViewProjectionMatrix(testCameraObject->getViewMatrix(), testCameraObject->getProjectionMatrix());

			meshRenderer.render(objectPool);

			// # ShapeRendererContext 테스트
			//ShapeRendererContext& shapeFontRendererContext = graphicDevice.getShapeRendererContext();
			//shapeFontRendererContext.testDraw(Float2(30, 60));

			instantRenderer.render();

			graphicDevice.endRendering();
		}

		Profiler::FPSCounter::count();
	}
	return true;
}

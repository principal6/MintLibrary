#include <MintLibrary/Include/AllHeaders.h>
#include <MintLibrary/Include/AllHpps.h>


#include <MintMath/Include/Test.h>
#include <MintContainer/Include/TestContainer.h>
#include <MintPlatform/Include/Test.h>
#include <MintLanguage/Include/Test.h>
#include <MintReflection/Include/TestReflection.h>
#include <MintRendering/Include/Test.h>
#include <MintLibrary/Include/Test.h>


#pragma comment(lib, "MintLibrary.lib")

#pragma optimize("", off)


bool run2DTestWindow(mint::Platform::IWindow& window, mint::Rendering::GraphicDevice& graphicDevice);
bool run3DTestWindow(mint::Platform::IWindow& window, mint::Rendering::GraphicDevice& graphicDevice);


int main()
{
	mint::Library::Initialize();

	using namespace mint;
	using namespace Platform;
	using namespace Rendering;

	WindowCreationDesc windowCreationDesc;
	windowCreationDesc._style = WindowStyle::Default;
	windowCreationDesc._position.Set(200, 100);
	windowCreationDesc._size.Set(1024, 768);
	windowCreationDesc._title = L"HI";
	windowCreationDesc._backgroundColor.Set(0.875f, 0.875f, 0.875f);

	WindowsWindow window;
	if (window.Create(windowCreationDesc) == false)
	{
		WindowCreationError windowCreationError = window.GetWindowCreationError();
		return false;
	}

	GraphicDevice graphicDevice{ window };
	graphicDevice.Initialize();

#if defined MINT_DEBUG
	//Logger::setOutputFileName("LOG.txt");
	TestMath::Test();
	TestContainers::Test();
	TestPlatform::Test();
	TestLanguage::Test();
	TestReflection::Test();
	//TestRendering::Test(graphicDevice);
	TestLibrary::Test();
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

	const Float2 windowSize = graphicDevice.GetWindowSizeFloat2();
	const Float4x4 projectionMatrix = Float4x4::ProjectionMatrix2DFromTopLeft(windowSize._x, windowSize._y);
	ImageRenderer imageRenderer{ graphicDevice, 0, ByteColor(0, 0, 0, 0) };
	ByteColorImage byteColorImage;
	ImageLoader imageLoader;
	imageLoader.LoadImage_("Assets/Test_image.png", byteColorImage);
	DxResourcePool& resourcePool = graphicDevice.GetResourcePool();
	const GraphicObjectID textureID = resourcePool.AddTexture2D(DxTextureFormat::R8G8B8A8_UNORM, byteColorImage.GetBytes(), byteColorImage.GetWidth(), byteColorImage.GetHeight());
	resourcePool.GetResource(textureID).BindToShader(GraphicShaderType::PixelShader, 0);
	const Platform::InputContext& inputContext = Platform::InputContext::GetInstance();
	while (window.IsRunning() == true)
	{
		if (inputContext.IsKeyPressed())
		{
			if (inputContext.IsKeyDown(Platform::KeyCode::Enter) == true)
			{
				graphicDevice.GetShaderPool().RecompileAllShaders();
			}
			else if (inputContext.IsKeyDown(Platform::KeyCode::Num1) == true)
			{
				graphicDevice.UseSolidCullBackRasterizer();
			}
			else if (inputContext.IsKeyDown(Platform::KeyCode::Num2) == true)
			{
				graphicDevice.UseWireFrameCullBackRasterizer();
			}
			else if (inputContext.IsKeyDown(Platform::KeyCode::Num3) == true)
			{
				graphicDevice.UseWireFrameNoCullingRasterizer();
			}
		}
		else if (inputContext.IsKeyReleased())
		{
			__noop;
		}
		else if (inputContext.IsMouseWheelScrolled())
		{
			const float mouseWheelScroll = inputContext.GetMouseWheelScroll();
		}

		// Rendering
		{
			graphicDevice.BeginRendering();

			graphicDevice.SetViewProjectionMatrix(Float4x4::kIdentity, projectionMatrix);

			imageRenderer.DrawImage(Float2(50, 50), Float2(80, 20), Float2(0, 0), Float2(1, 1));
			imageRenderer.Render();
			imageRenderer.Flush();

			graphicDevice.EndRendering();
		}

		Profiler::FPSCounter::Count();
	}
	return true;
}

bool run3DTestWindow(mint::Platform::IWindow& window, mint::Rendering::GraphicDevice& graphicDevice)
{
	using namespace mint;
	using namespace Platform;
	using namespace Rendering;


	ObjectPool objectPool;
	Object* const testObject = objectPool.CreateObject();
	CameraObject* const testCameraObject = objectPool.CreateCameraObject();
	Float2 windowSize = graphicDevice.GetWindowSizeFloat2();
	testCameraObject->SetPerspectiveZRange(0.01f, 1000.0f);
	testCameraObject->SetPerspectiveScreenRatio(windowSize._x / windowSize._y);
	{
		testObject->AttachComponent(objectPool.CreateMeshComponent());

		Transform& transform = testObject->GetObjectTransform();
		transform._translation._z = -4.0f;
	}
	testCameraObject->RotatePitch(0.125f);

	MeshRenderer meshRenderer{ graphicDevice };
	InstantRenderer instantRenderer{ graphicDevice };
	Game::SkeletonGenerator testSkeletonGenerator;
	Float4x4 testSkeletonWorldMatrix;
	testSkeletonWorldMatrix.SetTranslation(1.0f, 0.0f, -4.0f);
	Float4x4 bindPoseLocalMatrix;
	testSkeletonGenerator.CreateJoint(-1, "Root", bindPoseLocalMatrix);
	bindPoseLocalMatrix.SetTranslation(1.0f, 0.0f, 0.0f);
	testSkeletonGenerator.CreateJoint(0, "Elbow", bindPoseLocalMatrix);
	bindPoseLocalMatrix.SetTranslation(1.0f, 0.0f, 0.0f);
	testSkeletonGenerator.CreateJoint(1, "Tip", bindPoseLocalMatrix);
	testSkeletonGenerator.BuildBindPoseModelSpace();
	Game::Skeleton testSkeleton(testSkeletonGenerator);

	const Platform::InputContext& inputContext = Platform::InputContext::GetInstance();
	GUI::GUIContext& guiContext = graphicDevice.GetGUIContext();
	uint64 previousFrameTimeMs = 0;
	while (window.IsRunning() == true)
	{
		objectPool.ComputeDeltaTime();

		// Events
		guiContext.ProcessEvent();

		if (inputContext.IsKeyPressed())
		{
			if (inputContext.IsKeyDown(Platform::KeyCode::Enter) == true)
			{
				graphicDevice.GetShaderPool().RecompileAllShaders();
			}
			else if (inputContext.IsKeyDown(Platform::KeyCode::Num1) == true)
			{
				graphicDevice.UseSolidCullBackRasterizer();
			}
			else if (inputContext.IsKeyDown(Platform::KeyCode::Num2) == true)
			{
				graphicDevice.UseWireFrameCullBackRasterizer();
			}
			else if (inputContext.IsKeyDown(Platform::KeyCode::Num3) == true)
			{
				graphicDevice.UseWireFrameNoCullingRasterizer();
			}
			else if (inputContext.IsKeyDown(Platform::KeyCode::Num4) == true)
			{
				MeshComponent* const meshComponent = static_cast<MeshComponent*>(testObject->GetComponent(ObjectComponentType::MeshComponent));
				meshComponent->ShouldDrawNormals(!meshComponent->ShouldDrawNormals());
			}
			else if (inputContext.IsKeyDown(Platform::KeyCode::Num5) == true)
			{
				MeshComponent* const meshComponent = static_cast<MeshComponent*>(testObject->GetComponent(ObjectComponentType::MeshComponent));
				meshComponent->ShouldDrawEdges(!meshComponent->ShouldDrawEdges());
			}
			else if (inputContext.IsKeyDown(Platform::KeyCode::Shift) == true)
			{
				testCameraObject->SetBoostMode(true);
			}
		}
		else if (inputContext.IsKeyReleased())
		{
			if (inputContext.IsKeyUp(Platform::KeyCode::Shift) == true)
			{
				testCameraObject->SetBoostMode(false);
			}
		}
		else if (inputContext.IsMouseWheelScrolled())
		{
			const float mouseWheelScroll = inputContext.GetMouseWheelScroll();
			if (mouseWheelScroll > 0.0f)
			{
				testCameraObject->IncreaseMoveSpeed();
			}
			else
			{
				testCameraObject->DecreaseMoveSpeed();
			}
		}

		if (window.IsResized())
		{
			objectPool.UpdateScreenSize(graphicDevice.GetWindowSizeFloat2());
		}

		testCameraObject->Steer(inputContext, false);

		// Rendering
		{
			graphicDevice.BeginRendering();

			testSkeleton.RenderSkeleton(instantRenderer, testSkeletonWorldMatrix);

			graphicDevice.SetViewProjectionMatrix(testCameraObject->GetViewMatrix(), testCameraObject->GetProjectionMatrix());

			meshRenderer.Render(objectPool);

			// # ShapeRendererContext 테스트
			//ShapeRendererContext& shapeFontRendererContext = graphicDevice.GetShapeRendererContext();
			//shapeFontRendererContext.TestDraw(Float2(30, 60));

			instantRenderer.Render();

			graphicDevice.EndRendering();
		}

		Profiler::FPSCounter::Count();
	}
	return true;
}

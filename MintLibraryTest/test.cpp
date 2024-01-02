#include <MintLibrary/Include/AllHeaders.h>
#include <MintLibrary/Include/AllHpps.h>

#include <MintContainer/Include/TestContainer.h>
#include <MintMath/Include/Test.h>
#include <MintPhysics/Include/TestPhysics.h>
#include <MintPlatform/Include/Test.h>
#include <MintLanguage/Include/Test.h>
#include <MintReflection/Include/TestReflection.h>
#include <MintRendering/Include/TestRendering.h>
#include <MintLibrary/Include/Test.h>


#pragma comment(lib, "MintLibrary.lib")

#pragma optimize("", off)


bool Run2DTestWindow(mint::Window& window, mint::Rendering::GraphicDevice& graphicDevice);
bool Run3DTestWindow(mint::Window& window, mint::Rendering::GraphicDevice& graphicDevice);


int main()
{
	mint::Library::Initialize();

	using namespace mint;
	using namespace Rendering;

	WindowCreationDesc windowCreationDesc;
	windowCreationDesc._position.Set(200, 100);
	windowCreationDesc._size.Set(1024, 768);
	windowCreationDesc._title = L"HI";
	windowCreationDesc._backgroundColor = ByteColor(224, 224, 224);

	Window window;
	if (window.Create(windowCreationDesc) == false)
	{
		WindowCreationError windowCreationError = window.GetWindowCreationError();
		return false;
	}

	GraphicDevice graphicDevice{ window, true };
	graphicDevice.Initialize();

#if defined MINT_DEBUG
	//Logger::SetOutputFileName("LOG.txt");
	TestContainers::Test();
	TestMath::Test();
	TestPhysics::Test();
	TestPlatform::Test();
	TestLanguage::Test();
	TestReflection::Test();
	TestLibrary::Test();
#endif

	AudioSystem audioSystem;
	AudioObject audioObject0;
	//audioSystem.LoadAudioWAV("Assets/Christmas_Jazz-SoundStreet.wav", audioObject0);
	audioSystem.LoadAudioMP3("Assets/Christmas_Jazz-SoundStreet.mp3", audioObject0);
	audioObject0.Play();

	Run2DTestWindow(window, graphicDevice);
	//Run3DTestWindow(window, graphicDevice);
	return 0;
}


bool Run2DTestWindow(mint::Window& window, mint::Rendering::GraphicDevice& graphicDevice)
{
	using namespace mint;
	using namespace Rendering;
	using namespace Physics;
	using namespace Game;

	ByteColorImage byteColorImage;
	ImageLoader imageLoader;
	imageLoader.LoadImage_("Assets/Test_image.png", byteColorImage);
	GraphicResourcePool& resourcePool = graphicDevice.GetResourcePool();
	const GraphicObjectID textureID = resourcePool.AddTexture2D(TextureFormat::R8G8B8A8_UNORM, byteColorImage.GetBytes(), byteColorImage.GetWidth(), byteColorImage.GetHeight());

	const InputContext& inputContext = InputContext::GetInstance();

	ObjectPool objectPool;
	Object* const object0 = objectPool.CreateObject();
	{
		Mesh2DComponent* mesh2DComponent = objectPool.CreateMesh2DComponent();
		Shape shape;
		ShapeGenerator::GenerateCircle(1.0f, 16, ByteColor(0, 0, 255), shape);
		mesh2DComponent->SetShape(std::move(shape));
		object0->AttachComponent(mesh2DComponent);
	}
	Object* const object1 = objectPool.CreateObject();
	{
		Mesh2DComponent* mesh2DComponent = objectPool.CreateMesh2DComponent();
		Shape shape;
		Vector<Float2> points;
		points.PushBack(Float2(1, 1));
		points.PushBack(Float2(1, 0));
		points.PushBack(Float2(2, 0));
		ShapeGenerator::GenerateConvexShape(points, ByteColor(0, 128, 255), shape);
		mesh2DComponent->SetShape(std::move(shape));
		object1->AttachComponent(mesh2DComponent);
	}

	CameraObject* const testCameraObject = objectPool.CreateCameraObject();
	testCameraObject->SetPerspectiveZRange(1.0f, 100.0f);
	Float2 windowSize{ graphicDevice.GetWindowSize() };
	testCameraObject->SetPerspectiveScreenRatio(windowSize._x / windowSize._y);
	testCameraObject->GetObjectTransform()._translation._z = 5.0f;

	ObjectRenderer objectRenderer{ graphicDevice };
	InstantRenderer instantRenderer{ graphicDevice };
	ImageRenderer imageRenderer{ graphicDevice, 1, ByteColor(0, 0, 0, 0) };
	while (window.IsRunning() == true)
	{
		objectPool.ComputeDeltaTime();

		if (inputContext.IsKeyPressed())
		{
			if (inputContext.IsKeyDown(KeyCode::Enter) == true)
			{
				graphicDevice.GetShaderPool().RecompileAllShaders();
			}
			else if (inputContext.IsKeyDown(KeyCode::Num1) == true)
			{
				graphicDevice.UseSolidCullBackRasterizer();
			}
			else if (inputContext.IsKeyDown(KeyCode::Num2) == true)
			{
				graphicDevice.UseWireFrameCullBackRasterizer();
			}
			else if (inputContext.IsKeyDown(KeyCode::Num3) == true)
			{
				graphicDevice.UseWireFrameNoCullingRasterizer();
			}
		}

		testCameraObject->SteerDefault(inputContext, false);

		// Rendering
		{
			graphicDevice.BeginRendering();

			graphicDevice.SetViewProjectionMatrix(testCameraObject->GetViewMatrix(), testCameraObject->GetProjectionMatrix());

			objectRenderer.Render(objectPool);

			//instantRenderer.DrawTriangle({ Float3(0, 1, 0), Float3(-1, 0, 0), Float3(1, 0, 0) }, { Float2(0.5, 0), Float2(0, 1), Float2(1, 1) }, Color::kYellow);
			//instantRenderer.Render();

			//resourcePool.GetResource(textureID).BindToShader(GraphicShaderType::PixelShader, 1);
			//imageRenderer.DrawImage(Float2(50, 50), Float2(80, 20), Float2(0, 0), Float2(1, 1));
			//imageRenderer.Render();
			//imageRenderer.Flush();

			graphicDevice.SetSolidCullFrontRasterizer();
			graphicDevice.SetScreenSpace2DProjectionMatrix();
			ShapeRendererContext& shapeRendererContext = graphicDevice.GetShapeRendererContext();
			StackStringW<100> fpsString;
			FormatString(fpsString, L"FPS: %d", Profiler::FPSCounter::GetFPS());
			shapeRendererContext.SetTextColor(Color::kBlack);
			shapeRendererContext.DrawDynamicText(fpsString.CString(), Float2(10, 10), FontRenderingOption());
			shapeRendererContext.Render();
			shapeRendererContext.Flush();

			graphicDevice.EndRendering();
		}

		Profiler::FPSCounter::Count();
	}
	return true;
}

bool Run3DTestWindow(mint::Window& window, mint::Rendering::GraphicDevice& graphicDevice)
{
	using namespace mint;
	using namespace Rendering;
	using namespace GUI;
	using namespace Game;

	ObjectRenderer objectRenderer{ graphicDevice };
	InstantRenderer instantRenderer{ graphicDevice };
	const InputContext& inputContext = InputContext::GetInstance();

	ObjectPool objectPool;
	CameraObject* const testCameraObject = objectPool.CreateCameraObject();
	Float2 windowSize{ graphicDevice.GetWindowSize() };
	testCameraObject->SetPerspectiveZRange(1.0f, 100.0f);
	testCameraObject->SetPerspectiveScreenRatio(windowSize._x / windowSize._y);
	testCameraObject->GetObjectTransform()._translation._z = 5.0f;
	testCameraObject->RotatePitch(0.125f);
	Object* const testObject = objectPool.CreateObject();
	{
		testObject->AttachComponent(objectPool.CreateMeshComponent());

		Transform& transform = testObject->GetObjectTransform();
		transform._translation._z = -1.0f;
	}

	GUISystem guiSystem{ graphicDevice };
	GUIControlTemplateID roundButton0TemplateID;
	{
		GUIControlTemplate controlTemplate;
		Vector<SharedPtr<GUIControlComponent>>& components = controlTemplate.AccessComponents();
		{
			GUIControlShapeComponent shapeComponent;
			ShapeGenerator::GenerateCircle(16.0f, 16, ByteColor(255, 0, 0), shapeComponent._shapes[0]);
			ShapeGenerator::GenerateCircle(17.0f, 16, ByteColor(255, 64, 32), shapeComponent._shapes[1]);
			ShapeGenerator::GenerateCircle(17.0f, 16, ByteColor(255, 128, 64), shapeComponent._shapes[2]);
			controlTemplate.SetCollisionShape(Physics::ConvexCollisionShape2D::MakeFromRenderingShape(Float2::kZero, shapeComponent._shapes[0]));
			components.PushBack(MakeShared<GUIControlComponent>(shapeComponent));
		}
		{
			GUIControlTextComponent textComponent;
			textComponent._text = L"RoundButton0";
			components.PushBack(MakeShared<GUIControlComponent>(textComponent));
		}
		roundButton0TemplateID = guiSystem.RegisterTemplate(u8"RoundButton0", std::move(controlTemplate));
	}
	const GUIControlID buttonControlID = guiSystem.AddControl(roundButton0TemplateID);
	GUIControl& buttonControl = guiSystem.AccessControl(buttonControlID);
	buttonControl.SetPosition(Float2(100, 100));

	while (window.IsRunning() == true)
	{
		objectPool.ComputeDeltaTime();

		guiSystem.Update();

		if (inputContext.IsKeyPressed())
		{
			if (inputContext.IsKeyDown(KeyCode::Enter) == true)
			{
				graphicDevice.GetShaderPool().RecompileAllShaders();
			}
			else if (inputContext.IsKeyDown(KeyCode::Num1) == true)
			{
				graphicDevice.UseSolidCullBackRasterizer();
			}
			else if (inputContext.IsKeyDown(KeyCode::Num2) == true)
			{
				graphicDevice.UseWireFrameCullBackRasterizer();
			}
			else if (inputContext.IsKeyDown(KeyCode::Num3) == true)
			{
				graphicDevice.UseWireFrameNoCullingRasterizer();
			}
			else if (inputContext.IsKeyDown(KeyCode::Num4) == true)
			{
				MeshComponent* const meshComponent = static_cast<MeshComponent*>(testObject->GetComponent(ObjectComponentType::MeshComponent));
				meshComponent->ShouldDrawNormals(!meshComponent->ShouldDrawNormals());
			}
			else if (inputContext.IsKeyDown(KeyCode::Num5) == true)
			{
				MeshComponent* const meshComponent = static_cast<MeshComponent*>(testObject->GetComponent(ObjectComponentType::MeshComponent));
				meshComponent->ShouldDrawEdges(!meshComponent->ShouldDrawEdges());
			}
			else if (inputContext.IsKeyDown(KeyCode::Shift) == true)
			{
				testCameraObject->SetBoostMode(true);
			}
		}
		else if (inputContext.IsKeyReleased())
		{
			if (inputContext.IsKeyUp(KeyCode::Shift) == true)
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
			objectPool.UpdateScreenSize(Float2(graphicDevice.GetWindowSize()));
		}

		testCameraObject->SteerDefault(inputContext, true);

		// Rendering
		{
			graphicDevice.BeginRendering();

			graphicDevice.SetViewProjectionMatrix(testCameraObject->GetViewMatrix(), testCameraObject->GetProjectionMatrix());

			objectRenderer.Render(objectPool);
			
			instantRenderer.Render();
			
			guiSystem.Render();

			ShapeRendererContext& shapeRendererContext = graphicDevice.GetShapeRendererContext();
			// # ShapeRendererContext 테스트
			//shapeRendererContext.TestDraw(Float2(200, 100));
			//Shape testShapeSet;
			//ShapeGenerator::GenerateRectangle(Float2(32, 32), ByteColor(0,255,255), testShapeSet);
			//shapeRendererContext.AddShape(testShapeSet);
			graphicDevice.SetSolidCullFrontRasterizer();
			graphicDevice.SetScreenSpace2DProjectionMatrix(Float4x4::kIdentity);
			StackStringW<100> fpsString;
			FormatString(fpsString, L"FPS: %d", Profiler::FPSCounter::GetFPS());
			shapeRendererContext.SetTextColor(Color::kBlack);
			shapeRendererContext.DrawDynamicText(fpsString.CString(), Float2(10, 10), FontRenderingOption());
			shapeRendererContext.Render();
			shapeRendererContext.Flush();

			graphicDevice.EndRendering();
		}

		Profiler::FPSCounter::Count();
	}
	return true;
}

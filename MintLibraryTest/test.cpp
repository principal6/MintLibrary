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
	DxResourcePool& resourcePool = graphicDevice.GetResourcePool();
	const GraphicObjectID textureID = resourcePool.AddTexture2D(DxTextureFormat::R8G8B8A8_UNORM, byteColorImage.GetBytes(), byteColorImage.GetWidth(), byteColorImage.GetHeight());
	
	//ConvexCollisionShape2D circleCollisionShape = ConvexCollisionShape2D::MakeFromRenderingShape(Float2::kZero, circleShape);
	const InputContext& inputContext = InputContext::GetInstance();

	ObjectPool objectPool;
	Object* const object0 = objectPool.CreateObject();
	object0->GetObjectTransform()._translation = Float3(100, 100, 0);
	{
		Mesh2DComponent* mesh2DComponent = objectPool.CreateMesh2DComponent();
		Shape shape;
		ShapeGenerator::GenerateCircle(32.0f, 16, ByteColor(0, 0, 255), shape);
		mesh2DComponent->SetShape(std::move(shape));
		object0->AttachComponent(mesh2DComponent);
	}
	Object* const object1 = objectPool.CreateObject();
	object1->GetObjectTransform()._translation = Float3(200, 100, 0);
	{
		Mesh2DComponent* mesh2DComponent = objectPool.CreateMesh2DComponent();
		Shape shape;
		ShapeGenerator::GenerateRectangle(Float2(100, 50), ByteColor(0, 128, 255), shape);
		mesh2DComponent->SetShape(std::move(shape));
		object1->AttachComponent(mesh2DComponent);
	}

	ObjectRenderer objectRenderer{ graphicDevice };
	ImageRenderer imageRenderer{ graphicDevice, 1, ByteColor(0, 0, 0, 0) };
	while (window.IsRunning() == true)
	{
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

			//circleCollisionShape._center = Float2(100, 100);
			//circleCollisionShape.DebugDrawShape(shapeRendererContext, ByteColor(127, 0, 0, 127));

			objectRenderer.Render(objectPool);

			resourcePool.GetResource(textureID).BindToShader(GraphicShaderType::PixelShader, 1);
			imageRenderer.DrawImage(Float2(50, 50), Float2(80, 20), Float2(0, 0), Float2(1, 1));
			imageRenderer.Render();
			imageRenderer.Flush();

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
	Object* const testObject = objectPool.CreateObject();
	CameraObject* const testCameraObject = objectPool.CreateCameraObject();
	Float2 windowSize{ graphicDevice.GetWindowSize() };
	testCameraObject->SetPerspectiveZRange(0.01f, 1000.0f);
	testCameraObject->SetPerspectiveScreenRatio(windowSize._x / windowSize._y);
	{
		testObject->AttachComponent(objectPool.CreateMeshComponent());

		Transform& transform = testObject->GetObjectTransform();
		transform._translation._z = -4.0f;
	}
	testCameraObject->RotatePitch(0.125f);

	Game::Skeleton testSkeleton;
	{
		Game::SkeletonGenerator testSkeletonGenerator;
		Float4x4 bindPoseLocalMatrix;
		bindPoseLocalMatrix.SetTranslation(Float3::kZero);
		testSkeletonGenerator.CreateJoint(-1, "Root", bindPoseLocalMatrix);
		bindPoseLocalMatrix.SetTranslation(1.0f, 0.0f, 0.0f);
		testSkeletonGenerator.CreateJoint(0, "Elbow", bindPoseLocalMatrix);
		bindPoseLocalMatrix.SetTranslation(1.0f, 0.0f, 0.0f);
		testSkeletonGenerator.CreateJoint(1, "Tip", bindPoseLocalMatrix);
		testSkeletonGenerator.BuildBindPoseModelSpace();
		testSkeleton.CreateFromGenerator(testSkeletonGenerator);
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

	Plotter plotter{ graphicDevice.GetShapeRendererContext() };
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

		testCameraObject->Steer(inputContext, false);

		// Rendering
		{
			graphicDevice.BeginRendering();

			const Float4x4 testSkeletonWorldMatrix = Float4x4::TranslationMatrix(1.0f, 0.0f, -4.0f);
			testSkeleton.RenderSkeleton(instantRenderer, testSkeletonWorldMatrix);

			graphicDevice.SetViewProjectionMatrix(testCameraObject->GetViewMatrix(), testCameraObject->GetProjectionMatrix());

			objectRenderer.Render(objectPool);

			instantRenderer.Render();

			//TestRendering::Test_Plotter(plotter);

			ShapeRendererContext& shapeRendererContext = graphicDevice.GetShapeRendererContext();

			// # ShapeRendererContext 테스트
			//shapeRendererContext.TestDraw(Float2(30, 60));
			//Shape testShapeSet;
			//ShapeGenerator::GenerateTestShapeSet(testShapeSet);
			//shapeRendererContext.AddShape(testShapeSet);

			guiSystem.Render();

			StackStringW<100> fpsString;
			FormatString(fpsString, L"FPS: %d", Profiler::FPSCounter::GetFPS());
			shapeRendererContext.SetTextColor(Color::kBlack);
			shapeRendererContext.DrawDynamicText(fpsString.CString(), Float2(10, 10), FontRenderingOption());

			graphicDevice.EndRendering();
		}

		Profiler::FPSCounter::Count();
	}
	return true;
}

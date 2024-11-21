﻿#include <MintLibrary/Include/AllHeaders.h>
#include <MintLibrary/Include/AllHpps.h>

#include <MintContainer/Include/TestContainer.h>
#include <MintMath/Include/TestMath.h>
#include <MintPhysics/Include/TestPhysics.h>
#include <MintPlatform/Include/Test.h>
#include <MintLanguage/Include/Test.h>
#include <MintReflection/Include/TestReflection.h>
#include <MintRendering/Include/TestRendering.h>
#include <MintLibrary/Include/Test.h>


#pragma comment(lib, "MintLibrary.lib")

#pragma optimize("", off)


void RunGJKTestWindow();
void RunSplineTestWindow();
bool Run2DTestWindow();
bool Run3DTestWindow();


int main()
{
	mint::Library::Initialize();

#if defined MINT_DEBUG
	//mint::Logger::SetOutputFileName("LOG.txt");
	mint::TestContainers::Test();
	mint::TestMath::Test();
	mint::TestPhysics::Test();
	mint::TestPlatform::Test();
	mint::TestLanguage::Test();
	mint::TestReflection::Test();
	mint::TestLibrary::Test();

	mint::AudioSystem audioSystem;
	mint::AudioObject audioObject0;
	audioSystem.LoadAudioMP3("Assets/Christmas_Jazz-SoundStreet.mp3", audioObject0);
	audioObject0.Play();
#endif

	//RunGJKTestWindow();
	RunSplineTestWindow();
	//Run2DTestWindow();
	//Run3DTestWindow();
	return 0;
}


void RunGJKTestWindow()
{
	using namespace mint;
	using namespace Rendering;
	using namespace Physics;

	WindowCreationDesc windowCreationDesc;
	windowCreationDesc._position.Set(200, 100);
	windowCreationDesc._size.Set(1024, 768);
	windowCreationDesc._title = L"GJK Test";
	windowCreationDesc._backgroundColor = ByteColor(224, 224, 224);

	mint::App app{ windowCreationDesc , true };

	GJK2DInfo gjk2DInfo;
	EPA2DInfo epa2DInfo;
	GraphicsDevice& graphicsDevice = app.GetGraphicsDevice();
	ShapeRenderer& shapeRenderer = graphicsDevice.GetShapeRenderer();
	const InputContext& inputContext = InputContext::GetInstance();
	enum class SelectionMode
	{
		None,
		ShapeA,
		ShapeB
	};
	SelectionMode selectionMode{ SelectionMode::None };
	Transform2D shapeATransform2D{ Float2(128, 128) };
	Transform2D shapeBTransform2D{ Float2(128 + 64 + 32, 128 - 32) };
	while (app.IsRunning() == true)
	{
		const float deltaTime = DeltaTimer::GetInstance().GetDeltaTimeS();

		if (inputContext.IsKeyPressed())
		{
			if (inputContext.IsKeyDown(KeyCode::Escape) == true)
			{
				return;
			}

			if (inputContext.IsKeyDown(KeyCode::W) == true)
			{
				++gjk2DInfo._maxLoopCount;
			}
			else if (inputContext.IsKeyDown(KeyCode::Q) == true)
			{
				--gjk2DInfo._maxLoopCount;
			}
			else if (inputContext.IsKeyDown(KeyCode::S) == true)
			{
				++epa2DInfo._maxIterationCount;
			}
			else if (inputContext.IsKeyDown(KeyCode::A) == true)
			{
				--epa2DInfo._maxIterationCount;
			}
			else if (inputContext.IsKeyDown(KeyCode::Num1) == true)
			{
				selectionMode = SelectionMode::None;
			}
			else if (inputContext.IsKeyDown(KeyCode::Num2) == true)
			{
				selectionMode = SelectionMode::ShapeA;
			}
			else if (inputContext.IsKeyDown(KeyCode::Num3) == true)
			{
				selectionMode = SelectionMode::ShapeB;
			}
		}

		Transform2D* targetTransform2D = nullptr;
		if (selectionMode == SelectionMode::ShapeA)
		{
			targetTransform2D = &shapeATransform2D;
		}
		else if (selectionMode == SelectionMode::ShapeB)
		{
			targetTransform2D = &shapeBTransform2D;
		}
		if (targetTransform2D != nullptr)
		{
			if (inputContext.IsKeyDown(KeyCode::Up) == true)
			{
				targetTransform2D->_translation._y += 64.0f * deltaTime;
			}
			if (inputContext.IsKeyDown(KeyCode::Down) == true)
			{
				targetTransform2D->_translation._y -= 64.0f * deltaTime;
			}
			if (inputContext.IsKeyDown(KeyCode::Left) == true)
			{
				targetTransform2D->_translation._x -= 64.0f * deltaTime;
			}
			if (inputContext.IsKeyDown(KeyCode::Right) == true)
			{
				targetTransform2D->_translation._x += 64.0f * deltaTime;
			}
			if (inputContext.IsKeyDown(KeyCode::Insert) == true)
			{
				targetTransform2D->_rotation += Math::kPiOverTwo * deltaTime;
			}
			if (inputContext.IsKeyDown(KeyCode::Delete) == true)
			{
				targetTransform2D->_rotation -= Math::kPiOverTwo * deltaTime;
			}
		}

		const Float2 windowSize{ app.GetWindow().GetSize() };
		app.BeginRendering();
		{
			{
				graphicsDevice.SetViewProjectionMatrix(Float4x4::kIdentity, Float4x4::ProjectionMatrix2DNormal(windowSize._x, windowSize._y));

				CircleCollisionShape2D shapeA = CircleCollisionShape2D(64, shapeATransform2D);
				//EdgeCollisionShape2D shapeB = EdgeCollisionShape2D(Float2(-64, 0), Float2(64, 0), shapeBTransform2D);
				BoxCollisionShape2D shapeB = BoxCollisionShape2D(Float2(32, 32), shapeBTransform2D);
				//ConvexCollisionShape2D shapeB = ConvexCollisionShape2D({ Float2(-10, 80), Float2(-10, -20), Float2(80, -10), Float2(40, 70) }, shapeBTransform2D);
				const bool intersects = Intersect2D_GJK(shapeA, shapeB, &gjk2DInfo);

				const ByteColor kShapeAColor(255, 0, 0);
				const ByteColor kShapeBColor(64, 128, 0);
				const ByteColor kIntersectedColor(32, 196, 32);
				shapeA.DebugDrawShape(shapeRenderer, (intersects ? kIntersectedColor : kShapeAColor), Transform2D());
				shapeB.DebugDrawShape(shapeRenderer, (intersects ? kIntersectedColor : kShapeBColor), Transform2D());

				// Minkowski Difference Shape
				const ByteColor kShapeMDColor(64, 64, 64);
				ConvexCollisionShape2D shapeMD{ ConvexCollisionShape2D::MakeMinkowskiDifferenceShape(shapeA, shapeB) };
				shapeMD.DebugDrawShape(shapeRenderer, kShapeMDColor, Transform2D());

				// Simplex
				gjk2DInfo._simplex.DebugDrawShape(shapeRenderer, ByteColor(255, 0, 255), Transform2D());

				// Grid
				shapeRenderer.SetColor(kShapeMDColor);
				shapeRenderer.DrawLine(Float2(0, -800), Float2(0, 800), 1.0f);
				shapeRenderer.DrawLine(Float2(-800, 0), Float2(800, 0), 1.0f);

				//// Direction
				//shapeRenderer.DrawArrow(shapeATransform2D._translation, shapeATransform2D._translation + gjk2DInfo._direction * 50.0f, 1.0f, 0.125f, 4.0f);
				//shapeRenderer.DrawArrow(shapeBTransform2D._translation, shapeBTransform2D._translation - gjk2DInfo._direction * 50.0f, 1.0f, 0.125f, 4.0f);
				//shapeRenderer.DrawArrow(Float2::kZero, gjk2DInfo._direction * 100.0f, 2.0f, 0.125f, 4.0f);

				if (intersects)
				{
					Float2 normal = Float2(0, 1);
					float distance = 0.0f;
					ComputePenetration_EPA(shapeA, shapeB, gjk2DInfo, normal, distance, epa2DInfo);
					//epa2DInfo._simplex.DebugDrawShape(shapeRenderer, ByteColor(0, 64, 255), Transform2D());

					shapeRenderer.SetColor(ByteColor(0, 64, 255));
					shapeRenderer.SetPosition(Float4(shapeA.ComputeSupportPoint(+normal)));
					shapeRenderer.DrawCircle(4.0f);
					shapeRenderer.SetPosition(Float4(shapeB.ComputeSupportPoint(-normal)));
					shapeRenderer.DrawCircle(4.0f);

					const uint32 pointCount = epa2DInfo._points.Size();
					for (uint32 i = 1; i < pointCount; ++i)
					{
						shapeRenderer.DrawLine(epa2DInfo._points[i - 1], epa2DInfo._points[i], 2.0f);
					}
					if (pointCount > 0)
					{
						shapeRenderer.DrawLine(epa2DInfo._points.Back(), epa2DInfo._points[0], 2.0f);
					}

					shapeRenderer.SetColor(ByteColor(255, 64, 0));
					shapeRenderer.SetPosition(Float4(normal * distance));
					shapeRenderer.DrawCircle(4.0f);
				}

				shapeRenderer.Render();
			}
			{
				graphicsDevice.SetViewProjectionMatrix(Float4x4::kIdentity, Float4x4::ProjectionMatrix2DFromTopLeft(windowSize._x, windowSize._y));

				shapeRenderer.SetTextColor(Color::kBlack);
				StackStringW<100> buffer;
				FormatString(buffer, L"GJK Iteration: %d / Max %u (Q/W)", gjk2DInfo._loopCount, gjk2DInfo._maxLoopCount);
				shapeRenderer.DrawDynamicText(buffer.CString(), Float2(10, 10), FontRenderingOption());
				FormatString(buffer, L"EPA Iteration: %d / Max %u (A/S)", epa2DInfo._iteration, epa2DInfo._maxIterationCount);
				shapeRenderer.DrawDynamicText(buffer.CString(), Float2(10, 30), FontRenderingOption());
				FormatString(buffer, L"Selected: %s (1: None / 2: A / 3: B)", (selectionMode == SelectionMode::None ? L"None" : (selectionMode == SelectionMode::ShapeA ? L"ShapeA" : L"ShapeB")));
				shapeRenderer.DrawDynamicText(buffer.CString(), Float2(10, 50), FontRenderingOption());

				shapeRenderer.Render();
			}
		}
		app.EndRendering();
	}
}

void RunSplineTestWindow()
{
	using namespace mint;
	using namespace Rendering;
	using namespace GUI;
	using namespace Game;

	WindowCreationDesc windowCreationDesc;
	windowCreationDesc._position.Set(200, 100);
	windowCreationDesc._size.Set(1024, 768);
	windowCreationDesc._title = L"HI";
	windowCreationDesc._backgroundColor = ByteColor(224, 224, 224);

	mint::App app{ windowCreationDesc , true };

	GUISystem& guiSystem = app.GetGUISystem();
	GUIObjectTemplateID guiControlPointObjectTemplateID;
	{
		GUIObjectTemplate guiObjectTemplate;
		{
			SharedPtr<GUIShapeComponent> guiShapeComponent = MakeShared<GUIShapeComponent>();
			Shape defaultShape;
			Shape hoveredShape;
			Shape pressedShape;
			ShapeGenerator::GenerateCircle(8.0f, 16, ByteColor(255, 0, 0), defaultShape);
			ShapeGenerator::GenerateCircle(8.0f, 16, ByteColor(255, 64, 32), hoveredShape);
			ShapeGenerator::GenerateCircle(8.0f, 16, ByteColor(255, 128, 64), pressedShape);
			guiShapeComponent->SetShape(GUIObjectInteractionState::None, defaultShape);
			guiShapeComponent->SetShape(GUIObjectInteractionState::Hovered, hoveredShape);
			guiShapeComponent->SetShape(GUIObjectInteractionState::Pressed, pressedShape);
			guiObjectTemplate.SetCollisionShape(Physics::ConvexCollisionShape2D::MakeFromRenderingShape(Float2::kZero, defaultShape));
			guiObjectTemplate.AddComponent(guiShapeComponent);
		}
		{
			SharedPtr<GUITextComponent> guiTextComponent = MakeShared<GUITextComponent>();
			guiTextComponent->SetOffset(Float2(0, 16));
			guiTextComponent->SetText(L"CP");
			guiObjectTemplate.AddComponent(guiTextComponent);

			guiObjectTemplate.AddComponent(MakeShared<GUIDraggableComponent>());
		}
		guiControlPointObjectTemplateID = guiSystem.RegisterTemplate(u8"CP", std::move(guiObjectTemplate));
	}
	const GUIObjectID guiControlPointObjectID0 = guiSystem.AddObject(guiControlPointObjectTemplateID);
	GUIObject& guiControlPointObject0 = guiSystem.AccessObject(guiControlPointObjectID0);
	guiControlPointObject0.SetPosition(Float2(80, 160));
	guiControlPointObject0.GetComponent<GUITextComponent>()->SetText(L"CP0");
	const GUIObjectID guiControlPointObjectID1 = guiSystem.AddObject(guiControlPointObjectTemplateID);
	GUIObject& guiControlPointObject1 = guiSystem.AccessObject(guiControlPointObjectID1);
	guiControlPointObject1.SetPosition(Float2(160, 80));
	guiControlPointObject1.GetComponent<GUITextComponent>()->SetText(L"CP1");
	const GUIObjectID guiControlPointObjectID2 = guiSystem.AddObject(guiControlPointObjectTemplateID);
	GUIObject& guiControlPointObject2 = guiSystem.AccessObject(guiControlPointObjectID2);
	guiControlPointObject2.SetPosition(Float2(240, 80));
	guiControlPointObject2.GetComponent<GUITextComponent>()->SetText(L"CP2");
	const GUIObjectID guiControlPointObjectID3 = guiSystem.AddObject(guiControlPointObjectTemplateID);
	GUIObject& guiControlPointObject3 = guiSystem.AccessObject(guiControlPointObjectID3);
	guiControlPointObject3.SetPosition(Float2(320, 160));
	guiControlPointObject3.GetComponent<GUITextComponent>()->SetText(L"CP3");

	GraphicsDevice& graphicsDevice = app.GetGraphicsDevice();
	const InputContext& inputContext = InputContext::GetInstance();
	while (app.IsRunning() == true)
	{
		// Rendering
		app.BeginRendering();
		{
			ShapeRenderer& shapeRenderer = graphicsDevice.GetShapeRenderer();
			graphicsDevice.SetViewProjectionMatrix(Float4x4::kIdentity, graphicsDevice.GetScreenSpace2DProjectionMatrix());
			{
				StackStringW<100> fpsString;
				FormatString(fpsString, L"FPS: %d", Profiler::FPSCounter::GetFPS());
				shapeRenderer.SetTextColor(Color::kBlack);
				shapeRenderer.DrawDynamicText(fpsString.CString(), Float2(10, 10), FontRenderingOption());
			}
			shapeRenderer.Render();
		}
		app.EndRendering();

		Profiler::FPSCounter::Count();
	}
}

bool Run2DTestWindow()
{
	using namespace mint;
	using namespace Rendering;
	using namespace Physics;
	using namespace Game;

	WindowCreationDesc windowCreationDesc;
	windowCreationDesc._position.Set(200, 100);
	windowCreationDesc._size.Set(1024, 768);
	windowCreationDesc._title = L"HI";
	windowCreationDesc._backgroundColor = ByteColor(224, 224, 224);

	mint::App app{ windowCreationDesc , true };

	ByteColorImage corgiSpriteSheet;
	ImageLoader imageLoader;
	imageLoader.LoadImage_("Assets/corgi-asset_Miniyeti.png", corgiSpriteSheet);
	GraphicsDevice& graphicsDevice = app.GetGraphicsDevice();
	GraphicsResourcePool& resourcePool = graphicsDevice.GetResourcePool();
	const GraphicsObjectID corgiSpriteSheetTextureID = resourcePool.AddTexture2D(corgiSpriteSheet);

	ObjectPool& objectPool = app.GetObjectPool();
	SharedPtr<Object> object0 = objectPool.CreateObject();
	{
		Mesh2DComponent* mesh2DComponent = objectPool.CreateObjectComponent<Mesh2DComponent>();
		Shape shape;
		ShapeGenerator::GenerateCircle(1.0f, 16, ByteColor(0, 0, 255), shape);
		mesh2DComponent->SetShape(std::move(shape));
		object0->AttachComponent(mesh2DComponent);
	}
	{
		const Shape& shape = ((Mesh2DComponent*)(object0->GetComponent(ObjectComponentType::Mesh2DComponent)))->GetShape();
		Collision2DComponent* collision2DComponent = objectPool.CreateObjectComponent<Collision2DComponent>();
		ConvexCollisionShape2D collisionShape2D = ConvexCollisionShape2D::MakeFromRenderingShape(Float2::kZero, shape);
		collision2DComponent->SetCollisionShape2D(MakeShared<CollisionShape2D>(collisionShape2D));
		object0->AttachComponent(collision2DComponent);
	}
	SharedPtr<Object> object1 = objectPool.CreateObject();
	{
		Mesh2DComponent* mesh2DComponent = objectPool.CreateObjectComponent<Mesh2DComponent>();
		Shape shape;
		Vector<Float2> points;
		points.PushBack(Float2(1, 1));
		points.PushBack(Float2(1, 0));
		points.PushBack(Float2(2, 0));
		ShapeGenerator::GenerateConvexShape(points, ByteColor(0, 128, 255), shape);
		mesh2DComponent->SetShape(std::move(shape));
		object1->AttachComponent(mesh2DComponent);
	}
	{
		const Shape& shape = ((Mesh2DComponent*)(object1->GetComponent(ObjectComponentType::Mesh2DComponent)))->GetShape();
		Collision2DComponent* collision2DComponent = objectPool.CreateObjectComponent<Collision2DComponent>();
		ConvexCollisionShape2D collisionShape2D = ConvexCollisionShape2D::MakeFromRenderingShape(Float2::kZero, shape);
		collision2DComponent->SetCollisionShape2D(MakeShared<CollisionShape2D>(collisionShape2D));
		object1->AttachComponent(collision2DComponent);
	}

	SharedPtr<Object> testCameraObject = objectPool.CreateObject();
	CameraComponent* testCameraComponent = objectPool.CreateObjectComponent<CameraComponent>();
	{
		Float2 windowSize{ app.GetWindow().GetSize() };
		testCameraComponent->SetPerspectiveCamera(Math::ToRadian(60.0f), 1.0f, 100.0f, windowSize._x / windowSize._y);
		testCameraObject->GetObjectTransform()._translation._z = 5.0f;
		testCameraObject->AttachComponent(testCameraComponent);
	}
	
	SpriteAnimationSet corgiAnimationSet;
	{
		const float kTimePerFrame = 0.125f;
		const Float2 kOffsetInTexture{ 96.0f, 0.0f };
		const Float2 kSizeInTexture{ 64.0f, 64.0f };
		const Float2 kTextureSize{ 800, 512 };
		corgiAnimationSet.AddAnimation("IDLE", SpriteAnimation(kTextureSize, kTimePerFrame, kOffsetInTexture, kSizeInTexture, 0, 5, 2));
		corgiAnimationSet.AddAnimation("IDLE_HAPPY", SpriteAnimation(kTextureSize, kTimePerFrame, kOffsetInTexture, kSizeInTexture, 0, 5, 1));
		corgiAnimationSet.AddAnimation("WALK", SpriteAnimation(kTextureSize, kTimePerFrame, kOffsetInTexture, kSizeInTexture, 0, 5, 4));
		corgiAnimationSet.AddAnimation("RUN", SpriteAnimation(kTextureSize, kTimePerFrame, kOffsetInTexture, kSizeInTexture, 0, 8, 5));
		corgiAnimationSet.AddAnimation("JUMP", SpriteAnimation(kTextureSize, kTimePerFrame, kOffsetInTexture, kSizeInTexture, 0, 10, 0));
		corgiAnimationSet.AddAnimation("SIT_BEGIN", SpriteAnimation(kTextureSize, kTimePerFrame, kOffsetInTexture, kSizeInTexture, 0, 3, 3));
		corgiAnimationSet.AddAnimation("SIT_ING", SpriteAnimation(kTextureSize, kTimePerFrame, kOffsetInTexture, kSizeInTexture, 3, 6, 3));
		corgiAnimationSet.AddAnimation("SNIFF", SpriteAnimation(kTextureSize, kTimePerFrame, kOffsetInTexture, kSizeInTexture, 0, 8, 6));
		corgiAnimationSet.AddAnimation("SNIFF_WALK", SpriteAnimation(kTextureSize, kTimePerFrame, kOffsetInTexture, kSizeInTexture, 0, 8, 7));
	}


	ImageRenderer imageRenderer{ graphicsDevice, 1 };
	const InputContext& inputContext = InputContext::GetInstance();
	while (app.IsRunning() == true)
	{
		const float deltaTime = DeltaTimer::GetInstance().GetDeltaTimeS();

		if (inputContext.IsKeyPressed())
		{
			if (inputContext.IsKeyDown(KeyCode::Enter) == true)
			{
				graphicsDevice.GetShaderPool().RecompileAllShaders();
			}
			else if (inputContext.IsKeyDown(KeyCode::Num1) == true)
			{
				corgiAnimationSet.SetAnimationNextInOrder();
			}
		}

		testCameraComponent->SteerDefault(deltaTime, inputContext, false);

		// Rendering
		app.BeginRendering();
		{
			graphicsDevice.SetViewProjectionMatrix(testCameraComponent->GetViewMatrix(), testCameraComponent->GetProjectionMatrix());

			graphicsDevice.SetViewProjectionMatrix(Float4x4::kIdentity, graphicsDevice.GetScreenSpace2DProjectionMatrix());
			resourcePool.GetResource(corgiSpriteSheetTextureID).BindToShader(GraphicsShaderType::PixelShader, 1);
			//imageRenderer.DrawImageScreenSpace(Float2(0, 0), Float2(800, 512), Float2(0, 0), Float2(1, 1));
			corgiAnimationSet.Update(deltaTime);
			const SpriteAnimation& corgiCurrentAnimation = corgiAnimationSet.GetCurrentAnimation();
			imageRenderer.DrawImageScreenSpace(Float2(64, 64), Float2(128, 128), corgiCurrentAnimation.GetCurrentFrameUV0(), corgiCurrentAnimation.GetCurrentFrameUV1());
			imageRenderer.Render();

			ShapeRenderer& shapeRenderer = graphicsDevice.GetShapeRenderer();
			{
				StackStringW<100> fpsString;
				FormatString(fpsString, L"FPS: %d", Profiler::FPSCounter::GetFPS());
				shapeRenderer.SetTextColor(Color::kBlack);
				shapeRenderer.DrawDynamicText(fpsString.CString(), Float2(10, 10), FontRenderingOption());
			}
			shapeRenderer.Render();
		}
		app.EndRendering();

		Profiler::FPSCounter::Count();
	}
	return true;
}

bool Run3DTestWindow()
{
	using namespace mint;
	using namespace Rendering;
	using namespace GUI;
	using namespace Game;

	WindowCreationDesc windowCreationDesc;
	windowCreationDesc._position.Set(200, 100);
	windowCreationDesc._size.Set(1024, 768);
	windowCreationDesc._title = L"HI";
	windowCreationDesc._backgroundColor = ByteColor(224, 224, 224);

	mint::App app{ windowCreationDesc , true };


	ObjectPool& objectPool = app.GetObjectPool();
	SharedPtr<Object> testCameraObject = objectPool.CreateObject();
	CameraComponent* testCameraComponent = objectPool.CreateObjectComponent<CameraComponent>();
	{
		Float2 windowSize{ app.GetWindow().GetSize() };
		testCameraComponent->SetPerspectiveCamera(Math::ToRadian(60.0f), 0.01f, 100.0f, windowSize._x / windowSize._y);
		testCameraComponent->RotatePitch(0.125f);
		testCameraObject->GetObjectTransform()._translation._z = 5.0f;
		testCameraObject->AttachComponent(testCameraComponent);
	}

	SharedPtr<Object> testObject = objectPool.CreateObject();
	{
		testObject->AttachComponent(objectPool.CreateObjectComponent<MeshComponent>());

		Transform& transform = testObject->GetObjectTransform();
		transform._translation._z = -1.0f;
	}

	GUISystem& guiSystem = app.GetGUISystem();
	GUIObjectTemplateID roundButton0TemplateID;
	{
		GUIObjectTemplate guiObjectTemplate;
		{
			SharedPtr<GUIShapeComponent> guiShapeComponent = MakeShared<GUIShapeComponent>();
			Shape defaultShape;
			Shape hoveredShape;
			Shape pressedShape;
			ShapeGenerator::GenerateCircle(16.0f, 16, ByteColor(255, 0, 0), defaultShape);
			ShapeGenerator::GenerateCircle(17.0f, 16, ByteColor(255, 64, 32), hoveredShape);
			ShapeGenerator::GenerateCircle(17.0f, 16, ByteColor(255, 128, 64), pressedShape);
			guiShapeComponent->SetShape(GUIObjectInteractionState::None, defaultShape);
			guiShapeComponent->SetShape(GUIObjectInteractionState::Hovered, hoveredShape);
			guiShapeComponent->SetShape(GUIObjectInteractionState::Pressed, pressedShape);
			guiObjectTemplate.SetCollisionShape(Physics::ConvexCollisionShape2D::MakeFromRenderingShape(Float2::kZero, defaultShape));
			guiObjectTemplate.AddComponent(guiShapeComponent);
		}
		{
			SharedPtr<GUITextComponent> guiTextComponent = MakeShared<GUITextComponent>();
			guiTextComponent->SetText(L"RoundButton0");
			guiObjectTemplate.AddComponent(guiTextComponent);
		}
		roundButton0TemplateID = guiSystem.RegisterTemplate(u8"RoundButton0", std::move(guiObjectTemplate));
	}
	const GUIObjectID buttonObjectID = guiSystem.AddObject(roundButton0TemplateID);
	GUIObject& buttonObject = guiSystem.AccessObject(buttonObjectID);
	buttonObject.AddComponent(MakeShared<GUIDraggableComponent>());
	buttonObject.SetPosition(Float2(100, 100));

	GraphicsDevice& graphicsDevice = app.GetGraphicsDevice();
	const InputContext& inputContext = InputContext::GetInstance();
	while (app.IsRunning() == true)
	{
		const float deltaTime = DeltaTimer::GetInstance().GetDeltaTimeS();

		if (inputContext.IsKeyPressed())
		{
			if (inputContext.IsKeyDown(KeyCode::Enter) == true)
			{
				graphicsDevice.GetShaderPool().RecompileAllShaders();
			}
			else if (inputContext.IsKeyDown(KeyCode::Num1) == true)
			{
				graphicsDevice.UseSolidCullBackRasterizer();
			}
			else if (inputContext.IsKeyDown(KeyCode::Num2) == true)
			{
				graphicsDevice.UseWireFrameCullBackRasterizer();
			}
			else if (inputContext.IsKeyDown(KeyCode::Num3) == true)
			{
				graphicsDevice.UseWireFrameNoCullingRasterizer();
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
				testCameraComponent->SetBoostMode(true);
			}
		}
		else if (inputContext.IsKeyReleased())
		{
			if (inputContext.IsKeyUp(KeyCode::Shift) == true)
			{
				testCameraComponent->SetBoostMode(false);
			}
		}
		else if (inputContext.IsMouseWheelScrolled())
		{
			const float mouseWheelScroll = inputContext.GetMouseWheelScroll();
			if (mouseWheelScroll > 0.0f)
			{
				testCameraComponent->IncreaseMoveSpeed();
			}
			else
			{
				testCameraComponent->DecreaseMoveSpeed();
			}
		}

		testCameraComponent->SteerDefault(deltaTime, inputContext, true);

		// Rendering
		app.BeginRendering();
		{
			ShapeRenderer& shapeRenderer = graphicsDevice.GetShapeRenderer();
			// # ShapeRenderer 테스트
			//shapeRenderer.TestDraw(Float2(200, 100));
			//Shape testShapeSet;
			//ShapeGenerator::GenerateRectangle(Float2(32, 32), ByteColor(0,255,255), testShapeSet);
			//shapeRenderer.AddShape(testShapeSet);
			graphicsDevice.SetViewProjectionMatrix(Float4x4::kIdentity, graphicsDevice.GetScreenSpace2DProjectionMatrix());
			{
				StackStringW<100> fpsString;
				FormatString(fpsString, L"FPS: %d", Profiler::FPSCounter::GetFPS());
				shapeRenderer.SetTextColor(Color::kBlack);
				shapeRenderer.DrawDynamicText(fpsString.CString(), Float2(10, 10), FontRenderingOption());
			}
			shapeRenderer.Render();

			graphicsDevice.SetViewProjectionMatrix(testCameraComponent->GetViewMatrix(), testCameraComponent->GetProjectionMatrix());
		}
		app.EndRendering();

		Profiler::FPSCounter::Count();
	}
	return true;
}

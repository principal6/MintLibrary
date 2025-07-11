#include <MintLibrary/Include/AllHeaders.h>
#include <MintLibrary/Include/AllHpps.h>

#include <MintContainer/Include/TestContainer.h>
#include <MintMath/Include/TestMath.h>
#include <MintPhysics/Include/TestPhysics.h>
#include <MintPlatform/Include/Test.h>
#include <MintLanguage/Include/Test.h>
#include <MintReflection/Include/TestReflection.h>
#include <MintRendering/Include/TestRendering.h>
#include <MintECS/Include/TestECS.h>
#include <MintLibrary/Include/Test.h>


#pragma comment(lib, "MintLibrary.lib")

#pragma optimize("", off)


void RunGJKTestWindow();
void RunSplineTestWindow();
bool Run2DTestWindow();
bool Run3DTestWindow();
bool RunPhysics2DTestWindow();


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
	mint::TestECS::Test();
	mint::TestLibrary::Test();

	//mint::AudioSystem audioSystem;
	//mint::AudioObject audioObject0;
	//audioSystem.LoadAudioMP3("Assets/Christmas_Jazz-SoundStreet.mp3", audioObject0);
	//audioObject0.Play();
#endif

	//RunGJKTestWindow();
	//RunSplineTestWindow();
	//Run2DTestWindow();
	//Run3DTestWindow();
	RunPhysics2DTestWindow();
	return 0;
}


void RunGJKTestWindow()
{
	using namespace mint;
	using namespace Rendering;
	using namespace Physics2D;

	WindowCreationDesc windowCreationDesc;
	windowCreationDesc._position.Set(200, 100);
	windowCreationDesc._size.Set(1024, 768);
	windowCreationDesc._title = L"GJK Test";
	windowCreationDesc._backgroundColor = ByteColor(224, 224, 224);
	AppCreationDesc appCreationDesc;
	appCreationDesc._useMSAA = true;
	appCreationDesc._appType = AppType::Default2D;
	mint::App app{ windowCreationDesc, appCreationDesc };

	GJKInfo gjkInfo;
	EPAInfo epaInfo;
	GraphicsDevice& graphicsDevice = app.GetGraphicsDevice();
	FontRenderer& fontRenderer = graphicsDevice.GetFontRenderer();
	ShapeRenderer& shapeRenderer = graphicsDevice.GetShapeRenderer();
	MaterialPool& materialPool = graphicsDevice.GetMaterialPool();
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
				++gjkInfo._maxLoopCount;
			}
			else if (inputContext.IsKeyDown(KeyCode::Q) == true)
			{
				--gjkInfo._maxLoopCount;
			}
			else if (inputContext.IsKeyDown(KeyCode::S) == true)
			{
				++epaInfo._maxIterationCount;
			}
			else if (inputContext.IsKeyDown(KeyCode::A) == true)
			{
				--epaInfo._maxIterationCount;
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
		// Rendering
		for (ScopedRenderPhase renderPhase : graphicsDevice.IterateRenderPhases())
		{
			if (renderPhase.Is(RenderPhaseLabel::WorldSpace))
			{
				graphicsDevice.SetViewProjectionMatrix(Float4x4::kIdentity, Float4x4::ProjectionMatrix2DNormal(windowSize._x, windowSize._y));

				CircleCollisionShape shapeA = CircleCollisionShape(64, shapeATransform2D);
				//EdgeCollisionShape shapeB = EdgeCollisionShape(Float2(-64, 0), Float2(64, 0), shapeBTransform2D);
				BoxCollisionShape shapeB = BoxCollisionShape(Float2(32, 32), shapeBTransform2D);
				//ConvexCollisionShape shapeB = ConvexCollisionShape({ Float2(-10, 80), Float2(-10, -20), Float2(80, -10), Float2(40, 70) }, shapeBTransform2D);
				const bool intersects = Intersect_GJK(shapeA, shapeB, &gjkInfo);

				const ByteColor kShapeAColor(255, 0, 0);
				const ByteColor kShapeBColor(64, 128, 0);
				const ByteColor kIntersectedColor(32, 196, 32);
				shapeA.DebugDrawShape(shapeRenderer, (intersects ? kIntersectedColor : kShapeAColor), Transform2D::GetIdentity());
				shapeB.DebugDrawShape(shapeRenderer, (intersects ? kIntersectedColor : kShapeBColor), Transform2D::GetIdentity());

				// Minkowski Difference Shape
				const ByteColor kShapeMDColor(64, 64, 64);
				ConvexCollisionShape shapeMD{ ConvexCollisionShape::MakeMinkowskiDifferenceShape(shapeA, shapeB) };
				shapeMD.DebugDrawShape(shapeRenderer, kShapeMDColor, Transform2D::GetIdentity());

				// Simplex
				gjkInfo._simplex.DebugDrawShape(shapeRenderer, ByteColor(255, 0, 255), Transform2D::GetIdentity());

				// Grid
				shapeRenderer.SetColor(kShapeMDColor);
				shapeRenderer.DrawLine(Float2(0, -800), Float2(0, 800), 1.0f);
				shapeRenderer.DrawLine(Float2(-800, 0), Float2(800, 0), 1.0f);

				//// Direction
				//shapeRenderer.DrawArrow(shapeATransform2D._translation, shapeATransform2D._translation + gjkInfo._direction * 50.0f, 1.0f, 0.125f, 4.0f);
				//shapeRenderer.DrawArrow(shapeBTransform2D._translation, shapeBTransform2D._translation - gjkInfo._direction * 50.0f, 1.0f, 0.125f, 4.0f);
				//shapeRenderer.DrawArrow(Float2::kZero, gjkInfo._direction * 100.0f, 2.0f, 0.125f, 4.0f);

				if (intersects)
				{
					Float2 normal = Float2(0, 1);
					float distance = 0.0f;
					ComputePenetration_EPA(shapeA, shapeB, gjkInfo, normal, distance, epaInfo);
					//epaInfo._simplex.DebugDrawShape(shapeRenderer, ByteColor(0, 64, 255), Transform2D::GetIdentity());

					shapeRenderer.SetColor(ByteColor(0, 64, 255));
					shapeRenderer.DrawCircle(Float3(shapeA.ComputeSupportPoint(+normal)), 4.0f);
					shapeRenderer.DrawCircle(Float3(shapeB.ComputeSupportPoint(-normal)), 4.0f);

					const uint32 pointCount = epaInfo._points.Size();
					for (uint32 i = 1; i < pointCount; ++i)
					{
						shapeRenderer.DrawLine(epaInfo._points[i - 1], epaInfo._points[i], 2.0f);
					}
					if (pointCount > 0)
					{
						shapeRenderer.DrawLine(epaInfo._points.Back(), epaInfo._points[0], 2.0f);
					}

					shapeRenderer.SetColor(ByteColor(255, 64, 0));
					shapeRenderer.DrawCircle(Float3(normal * distance), 4.0f);
				}
			}
			else if (renderPhase.Is(RenderPhaseLabel::ScreenSpace))
			{
				fontRenderer.SetTextColor(Color::kBlack);
				StackStringW<100> buffer;
				FormatString(buffer, L"GJK Iteration: %d / Max %u (Q/W)", gjkInfo._loopCount, gjkInfo._maxLoopCount);
				fontRenderer.DrawDynamicText(buffer.CString(), Float2(10, 10), FontRenderingOption());
				FormatString(buffer, L"EPA Iteration: %d / Max %u (A/S)", epaInfo._iteration, epaInfo._maxIterationCount);
				fontRenderer.DrawDynamicText(buffer.CString(), Float2(10, 30), FontRenderingOption());
				FormatString(buffer, L"Selected: %s (1: None / 2: A / 3: B)", (selectionMode == SelectionMode::None ? L"None" : (selectionMode == SelectionMode::ShapeA ? L"ShapeA" : L"ShapeB")));
				fontRenderer.DrawDynamicText(buffer.CString(), Float2(10, 50), FontRenderingOption());
			}
		}
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
	AppCreationDesc appCreationDesc;
	appCreationDesc._useMSAA = true;
	appCreationDesc._appType = AppType::Default2D;
	mint::App app{ windowCreationDesc, appCreationDesc };

	GUISystem& guiSystem = app.GetGUISystem();
	GUIControlTemplate guiControlPointControlTemplate = guiSystem.CreateTemplate();
	{
		GUITransform2DComponent transform2DComponent;
		transform2DComponent._transform2D._translation = Float2(60, 40);
		guiSystem.AttachComponent(guiControlPointControlTemplate, std::move(transform2DComponent));

		guiSystem.AttachComponent(guiControlPointControlTemplate, GUIInteractionStateComponent());
		guiSystem.AttachComponent(guiControlPointControlTemplate, GUIDraggableComponent());

		GUIShapeComponent shapeComponent;
		Shape defaultShape;
		Shape hoveredShape;
		Shape pressedShape;
		ShapeGenerator::GenerateCircle(8.0f, 16, ByteColor(255, 0, 0), defaultShape);
		ShapeGenerator::GenerateCircle(8.0f, 16, ByteColor(255, 64, 32), hoveredShape);
		ShapeGenerator::GenerateCircle(8.0f, 16, ByteColor(255, 128, 64), pressedShape);
		shapeComponent._shapes[static_cast<uint32>(GUIInteractionState::None)] = defaultShape;
		shapeComponent._shapes[static_cast<uint32>(GUIInteractionState::Hovered)] = hoveredShape;
		shapeComponent._shapes[static_cast<uint32>(GUIInteractionState::Pressed)] = pressedShape;
		guiSystem.AttachComponent(guiControlPointControlTemplate, std::move(shapeComponent));

		GUICollisionShapeComponent collisionShapeComponent;
		collisionShapeComponent._collisionShape = MakeShared<Physics2D::CollisionShape>(Physics2D::ConvexCollisionShape::MakeFromRenderingShape(Float2::kZero, defaultShape));
		guiSystem.AttachComponent(guiControlPointControlTemplate, std::move(collisionShapeComponent));

		GUITextComponent textComponent;
		textComponent._offset = Float2(0, 16);
		textComponent._text = L"CP";
		guiSystem.AttachComponent(guiControlPointControlTemplate, std::move(textComponent));
	}

	const GUIControl guiControlPointControl0 = guiSystem.CreateControl(guiControlPointControlTemplate);
	guiSystem.GetComponent<GUITransform2DComponent>(guiControlPointControl0)->_transform2D._translation = Float2(80, 160);
	guiSystem.GetComponent<GUITextComponent>(guiControlPointControl0)->_text = L"CP0";
	const GUIControl guiControlPointControl1 = guiSystem.CreateControl(guiControlPointControlTemplate);
	guiSystem.GetComponent<GUITransform2DComponent>(guiControlPointControl1)->_transform2D._translation = Float2(160, 80);
	guiSystem.GetComponent<GUITextComponent>(guiControlPointControl1)->_text = L"CP1";
	const GUIControl guiControlPointControl2 = guiSystem.CreateControl(guiControlPointControlTemplate);
	guiSystem.GetComponent<GUITransform2DComponent>(guiControlPointControl2)->_transform2D._translation = Float2(240, 80);
	guiSystem.GetComponent<GUITextComponent>(guiControlPointControl2)->_text = L"CP2";
	const GUIControl guiControlPointControl3 = guiSystem.CreateControl(guiControlPointControlTemplate);
	guiSystem.GetComponent<GUITransform2DComponent>(guiControlPointControl3)->_transform2D._translation = Float2(320, 160);
	guiSystem.GetComponent<GUITextComponent>(guiControlPointControl3)->_text = L"CP3";

	GraphicsDevice& graphicsDevice = app.GetGraphicsDevice();
	const InputContext& inputContext = InputContext::GetInstance();
	FontRenderer& fontRenderer = graphicsDevice.GetFontRenderer();
	ShapeRenderer& shapeRenderer = graphicsDevice.GetShapeRenderer();
	MaterialPool& materialPool = graphicsDevice.GetMaterialPool();
	while (app.IsRunning() == true)
	{
		// RenderingSS
		for (ScopedRenderPhase renderPhase : graphicsDevice.IterateRenderPhases())
		{
			if (renderPhase.Is(RenderPhaseLabel::ScreenSpace))
			{
				StackStringW<100> fpsString;
				FormatString(fpsString, L"FPS: %d", Profiler::FPSCounter::GetFPS());
				fontRenderer.SetTextColor(Color::kBlack);
				fontRenderer.DrawDynamicText(fpsString.CString(), Float2(10, 10), FontRenderingOption());
			}
		}

		Profiler::FPSCounter::Count();
	}
}

bool Run2DTestWindow()
{
	using namespace mint;
	using namespace Rendering;
	using namespace Physics2D;
	using namespace Game;

	WindowCreationDesc windowCreationDesc;
	windowCreationDesc._position.Set(200, 100);
	windowCreationDesc._size.Set(1024, 768);
	windowCreationDesc._title = L"HI";
	windowCreationDesc._backgroundColor = ByteColor(224, 224, 224);
	AppCreationDesc appCreationDesc;
	appCreationDesc._useMSAA = true;
	appCreationDesc._appType = AppType::Default2D;
	mint::App app{ windowCreationDesc, appCreationDesc };

	GraphicsDevice& graphicsDevice = app.GetGraphicsDevice();
	GraphicsResourcePool& resourcePool = graphicsDevice.GetResourcePool();
	MaterialPool& materialPool = graphicsDevice.GetMaterialPool();
	FontRenderer& fontRenderer = graphicsDevice.GetFontRenderer();
	ShapeRenderer& shapeRenderer = graphicsDevice.GetShapeRenderer();
	SpriteRenderer& spriteRenderer = graphicsDevice.GetSpriteRenderer();
	ByteColorImage corgiSpriteSheet;
	ImageLoader imageLoader;
	imageLoader.LoadImage_("Assets/corgi-asset_Miniyeti.png", corgiSpriteSheet);
	const GraphicsObjectID corgiSpriteSheetTextureID = resourcePool.AddTexture2D(corgiSpriteSheet);
	GraphicsObjectID corgiMaterialID;
	{
		MaterialDesc materialDesc;
		materialDesc._materialName = "Corgi";
		materialDesc._shaderPipelineID = spriteRenderer.GetDefaultShaderPipelineID();
		materialDesc._textureID = corgiSpriteSheetTextureID;
		materialDesc._textureSlot = 1;
		corgiMaterialID = materialPool.CreateMaterial(materialDesc);
	}

	SceneObjectRegistry& sceneObjectRegistry = app.GetObjectPool();
	SceneObject sceneObject0 = sceneObjectRegistry.CreateSceneObject();
	{
		Mesh2DComponent mesh2DComponent;
		ShapeGenerator::GenerateCircle(1.0f, 16, ByteColor(0, 0, 255), mesh2DComponent._shape);
		sceneObjectRegistry.AttachComponent(sceneObject0, std::move(mesh2DComponent));
	}

	SceneObject sceneObject1 = sceneObjectRegistry.CreateSceneObject();
	{
		Mesh2DComponent mesh2DComponent;
		Vector<Float2> points;
		points.PushBack(Float2(1, 1));
		points.PushBack(Float2(1, 0));
		points.PushBack(Float2(2, 0));
		ShapeGenerator::GenerateConvexShape(points, ByteColor(0, 128, 255), mesh2DComponent._shape);
		sceneObjectRegistry.AttachComponent(sceneObject1, std::move(mesh2DComponent));
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

		// Rendering
		for (ScopedRenderPhase renderPhase : graphicsDevice.IterateRenderPhases())
		{
			if (renderPhase.Is(RenderPhaseLabel::ScreenSpace))
			{
				spriteRenderer.SetMaterial(corgiMaterialID);
				corgiAnimationSet.Update(deltaTime);
				const SpriteAnimation& corgiCurrentAnimation = corgiAnimationSet.GetCurrentAnimation();
				spriteRenderer.DrawImage(Float2(64, 64), Float2(128, 128), corgiCurrentAnimation.GetCurrentFrameUV0(), corgiCurrentAnimation.GetCurrentFrameUV1());

				StackStringW<100> fpsString;
				FormatString(fpsString, L"FPS: %d", Profiler::FPSCounter::GetFPS());
				fontRenderer.SetTextColor(Color::kBlack);
				fontRenderer.DrawDynamicText(fpsString.CString(), Float2(10, 10), FontRenderingOption());
			}
		}

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
	windowCreationDesc._title = L"3DTestWindow";
	windowCreationDesc._backgroundColor = ByteColor(224, 224, 224);
	AppCreationDesc appCreationDesc;
	appCreationDesc._useMSAA = true;
	appCreationDesc._appType = AppType::Default3D;
	mint::App app{ windowCreationDesc, appCreationDesc };


	SceneObjectRegistry& sceneObjectRegistry = app.GetObjectPool();
	SceneObject testObject = sceneObjectRegistry.CreateSceneObject();
	{
		MeshComponent meshComponent;
		Rendering::MeshGenerator::GeoSphereParam geosphereParam;
		geosphereParam._radius = 1.0f;
		geosphereParam._subdivisionIteration = 3;
		geosphereParam._smooth = true;
		Rendering::MeshGenerator::GenerateGeoSphere(geosphereParam, meshComponent._meshData);
		sceneObjectRegistry.AttachComponent(testObject, std::move(meshComponent));

		Transform& transform = sceneObjectRegistry.GetTransform(testObject);
		transform._translation._z = -1.0f;
	}

	GUISystem& guiSystem = app.GetGUISystem();
	GraphicsDevice& graphicsDevice = app.GetGraphicsDevice();
	SceneObjectSystems& sceneObjectSystems = app.GetSceneObjectSystems();
	const InputContext& inputContext = InputContext::GetInstance();
	FontRenderer& fontRenderer = graphicsDevice.GetFontRenderer();
	ShapeRenderer& shapeRenderer = graphicsDevice.GetShapeRenderer();
	MaterialPool& materialPool = graphicsDevice.GetMaterialPool();
	while (app.IsRunning() == true)
	{
		const float deltaTime = DeltaTimer::GetInstance().GetDeltaTimeS();
		const SceneObject& currentCameraObject = sceneObjectSystems.GetCameraSystem().GetCurrentCameraObject();
		CameraComponent& cameraComponent = sceneObjectRegistry.GetComponentMust<CameraComponent>(currentCameraObject);
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
				MeshComponent* const meshComponent = sceneObjectRegistry.GetComponent<MeshComponent>(testObject);
				meshComponent->_shouldDrawNormals = !meshComponent->_shouldDrawNormals;
			}
			else if (inputContext.IsKeyDown(KeyCode::Num5) == true)
			{
				MeshComponent* const meshComponent = sceneObjectRegistry.GetComponent<MeshComponent>(testObject);
				meshComponent->_shouldDrawEdges = !meshComponent->_shouldDrawEdges;
			}
			else if (inputContext.IsKeyDown(KeyCode::Shift) == true)
			{
				cameraComponent._isBoostMode = true;
			}
		}
		else if (inputContext.IsKeyReleased())
		{
			if (inputContext.IsKeyUp(KeyCode::Shift) == true)
			{
				cameraComponent._isBoostMode = false;
			}
		}
		else if (inputContext.IsMouseWheelScrolled())
		{
			const float mouseWheelScroll = inputContext.GetMouseWheelScroll();
			if (mouseWheelScroll > 0.0f)
			{
				sceneObjectSystems.GetCameraSystem().IncreaseCurrentCameraMoveSpeed();
			}
			else
			{
				sceneObjectSystems.GetCameraSystem().DecreaseCurrentCameraMoveSpeed();
			}
		}

		// Rendering
		for (ScopedRenderPhase renderPhase : graphicsDevice.IterateRenderPhases())
		{
			if (renderPhase.Is(RenderPhaseLabel::ScreenSpace))
			{
				StackStringW<100> fpsString;
				FormatString(fpsString, L"FPS: %d", Profiler::FPSCounter::GetFPS());
				fontRenderer.SetTextColor(Color::kBlack);
				fontRenderer.DrawDynamicText(fpsString.CString(), Float2(10, 10), FontRenderingOption());
			}
		}

		Profiler::FPSCounter::Count();
	}
	return true;
}

bool RunPhysics2DTestWindow()
{
	using namespace mint;
	using namespace Rendering;
	using namespace Physics2D;
	using namespace Game;

	WindowCreationDesc windowCreationDesc;
	windowCreationDesc._position.Set(200, 100);
	windowCreationDesc._size.Set(1024, 768);
	windowCreationDesc._title = L"Physics2D Test";
	windowCreationDesc._backgroundColor = ByteColor(224, 224, 224);
	AppCreationDesc appCreationDesc;
	appCreationDesc._useMSAA = true;
	appCreationDesc._appType = AppType::Default2D;
	mint::App app{ windowCreationDesc, appCreationDesc };

	GraphicsDevice& graphicsDevice = app.GetGraphicsDevice();
	GraphicsResourcePool& resourcePool = graphicsDevice.GetResourcePool();
	MaterialPool& materialPool = graphicsDevice.GetMaterialPool();
	FontRenderer& fontRenderer = graphicsDevice.GetFontRenderer();
	ShapeRenderer& shapeRenderer = graphicsDevice.GetShapeRenderer();

	SceneObjectRegistry& sceneObjectRegistry = app.GetObjectPool();

	Physics2D::World physicsWorld;

	SceneObject sceneObject0 = sceneObjectRegistry.CreateSceneObject();
	{
		Mesh2DComponent mesh2DComponent;
		ShapeGenerator::GenerateCircle(0.125f, 16, ByteColor(0, 0, 255), mesh2DComponent._shape);
		sceneObjectRegistry.AttachComponent(sceneObject0, std::move(mesh2DComponent));
	}
	{
		RigidBodyComponent rigidBodyComponent;
		{
			const Shape& shape = sceneObjectRegistry.GetComponentMust<Mesh2DComponent>(sceneObject0)._shape;
			BodyCreationDesc bodyCreationDesc;
			bodyCreationDesc._collisionShape = MakeShared<CollisionShape>(ConvexCollisionShape::MakeFromRenderingShape(Float2::kZero, shape));
			bodyCreationDesc._inverseMass = 0.0f; // Static body
			rigidBodyComponent._bodyID = physicsWorld.CreateBody(bodyCreationDesc);
		}
		sceneObjectRegistry.AttachComponent(sceneObject0, std::move(rigidBodyComponent));
	}

	SceneObject sceneObject1 = sceneObjectRegistry.CreateSceneObject();
	{
		Mesh2DComponent mesh2DComponent;
		ShapeGenerator::GenerateCircle(0.125f, 16, ByteColor(0, 128, 64), mesh2DComponent._shape);
		sceneObjectRegistry.AttachComponent(sceneObject1, std::move(mesh2DComponent));
	}
	{
		RigidBodyComponent rigidBodyComponent;
		{
			const Shape& shape = sceneObjectRegistry.GetComponentMust<Mesh2DComponent>(sceneObject1)._shape;
			BodyCreationDesc bodyCreationDesc;
			bodyCreationDesc._collisionShape = MakeShared<CollisionShape>(ConvexCollisionShape::MakeFromRenderingShape(Float2::kZero, shape));
			bodyCreationDesc._transform2D._translation = Float2(0.5f, 0.0f);
			bodyCreationDesc._inverseMass = 1.0f; // Dynamic body
			bodyCreationDesc._bodyMotionType = BodyMotionType::Dynamic;
			rigidBodyComponent._bodyID = physicsWorld.CreateBody(bodyCreationDesc);
		}
		sceneObjectRegistry.AttachComponent(sceneObject1, std::move(rigidBodyComponent));
	}

	SceneObject sceneObjectFloor = sceneObjectRegistry.CreateSceneObject();
	{
		Mesh2DComponent mesh2DComponent;
		ShapeGenerator::GenerateRectangle(Float2(4.0f, 0.25f), ByteColor(64, 64, 64), mesh2DComponent._shape);
		sceneObjectRegistry.AttachComponent(sceneObjectFloor, std::move(mesh2DComponent));
	}
	{
		RigidBodyComponent rigidBodyComponent;
		{
			const Shape& shape = sceneObjectRegistry.GetComponentMust<Mesh2DComponent>(sceneObjectFloor)._shape;
			BodyCreationDesc bodyCreationDesc;
			bodyCreationDesc._collisionShape = MakeShared<CollisionShape>(ConvexCollisionShape::MakeFromRenderingShape(Float2::kZero, shape));
			bodyCreationDesc._transform2D._translation = Float2(0.0f, -3.0f);
			bodyCreationDesc._inverseMass = 0.0f; // Static body
			rigidBodyComponent._bodyID = physicsWorld.CreateBody(bodyCreationDesc);
		}
		sceneObjectRegistry.AttachComponent(sceneObjectFloor, std::move(rigidBodyComponent));
	}

	const InputContext& inputContext = InputContext::GetInstance();
	float accumulatedTimeForPhysics = 0.0f;
	while (app.IsRunning() == true)
	{
		const float deltaTime = DeltaTimer::GetInstance().GetDeltaTimeS();
		accumulatedTimeForPhysics += deltaTime;

		if (inputContext.IsKeyPressed())
		{
			if (inputContext.IsKeyDown(KeyCode::Enter) == true)
			{
				graphicsDevice.GetShaderPool().RecompileAllShaders();
			}
		}

		// Physics Update
		if (accumulatedTimeForPhysics >= (1.0f / 60.0f))
		{
			accumulatedTimeForPhysics -= (1.0f / 60.0f);

			const float kTimeStep = 1.0f / 60.0f;

			const RigidBodyComponent& rigidBodyComponent0 = sceneObjectRegistry.GetComponentMust<RigidBodyComponent>(sceneObject0);
			const RigidBodyComponent& rigidBodyComponent1 = sceneObjectRegistry.GetComponentMust<RigidBodyComponent>(sceneObject1);
			const RigidBodyComponent& rigidBodyComponentFloor = sceneObjectRegistry.GetComponentMust<RigidBodyComponent>(sceneObjectFloor);
			const Body& bodyA = physicsWorld.GetBody(rigidBodyComponent0._bodyID);
			const Body& bodyB = physicsWorld.GetBody(rigidBodyComponent1._bodyID);
			const Body& bodyFloor = physicsWorld.GetBody(rigidBodyComponentFloor._bodyID);

			ParticleDistanceConstraint particleDistanceConstraint;
			particleDistanceConstraint._bodyAID = bodyA._bodyID;
			particleDistanceConstraint._bodyBID = bodyB._bodyID;
			particleDistanceConstraint._distance = 0.5f;

			const uint32 kConstraintSubStepCount = 4;
			for (uint32 constraintSubStep = 0; constraintSubStep < kConstraintSubStepCount; ++constraintSubStep)
			{
				SolveParticleDistanceConstraint(physicsWorld, kTimeStep, particleDistanceConstraint);
			}

			physicsWorld.Step(kTimeStep);

			Transform& transform0 = sceneObjectRegistry.GetComponentMust<TransformComponent>(sceneObject0)._transform;
			Transform& transform1 = sceneObjectRegistry.GetComponentMust<TransformComponent>(sceneObject1)._transform;
			Transform& transformFloor = sceneObjectRegistry.GetComponentMust<TransformComponent>(sceneObjectFloor)._transform;
			transform0._translation.Set(bodyA._transform2D._translation);
			transform1._translation.Set(bodyB._transform2D._translation);
			transformFloor._translation.Set(bodyFloor._transform2D._translation);
		}

		// Rendering
		for (ScopedRenderPhase renderPhase : graphicsDevice.IterateRenderPhases())
		{
			if (renderPhase.Is(RenderPhaseLabel::ScreenSpace))
			{
				StackStringW<100> fpsString;
				FormatString(fpsString, L"FPS: %d", Profiler::FPSCounter::GetFPS());
				fontRenderer.SetTextColor(Color::kBlack);
				fontRenderer.DrawDynamicText(fpsString.CString(), Float2(10, 10), FontRenderingOption());
			}
		}

		Profiler::FPSCounter::Count();
	}
	return true;
}

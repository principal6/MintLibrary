#include <MintLibrary/Include/AllHeaders.h>
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

	//mint::AudioSystem audioSystem;
	//mint::AudioObject audioObject0;
	//audioSystem.LoadAudioMP3("Assets/Christmas_Jazz-SoundStreet.mp3", audioObject0);
	//audioObject0.Play();
#endif

	//RunGJKTestWindow();
	//RunSplineTestWindow();
	//Run2DTestWindow();
	Run3DTestWindow();
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
	AppCreationDesc appCreationDesc;
	appCreationDesc._useMSAA = true;
	appCreationDesc._appType = AppType::Default2D;
	mint::App app{ windowCreationDesc, appCreationDesc };

	GJK2DInfo gjk2DInfo;
	EPA2DInfo epa2DInfo;
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
		// Rendering
		for (ScopedRenderPhase renderPhase : graphicsDevice.IterateRenderPhases())
		{
			if (renderPhase.Is(RenderPhaseLabel::WorldSpace))
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
				shapeA.DebugDrawShape(shapeRenderer, (intersects ? kIntersectedColor : kShapeAColor), Transform2D::GetIdentity());
				shapeB.DebugDrawShape(shapeRenderer, (intersects ? kIntersectedColor : kShapeBColor), Transform2D::GetIdentity());

				// Minkowski Difference Shape
				const ByteColor kShapeMDColor(64, 64, 64);
				ConvexCollisionShape2D shapeMD{ ConvexCollisionShape2D::MakeMinkowskiDifferenceShape(shapeA, shapeB) };
				shapeMD.DebugDrawShape(shapeRenderer, kShapeMDColor, Transform2D::GetIdentity());

				// Simplex
				gjk2DInfo._simplex.DebugDrawShape(shapeRenderer, ByteColor(255, 0, 255), Transform2D::GetIdentity());

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
					//epa2DInfo._simplex.DebugDrawShape(shapeRenderer, ByteColor(0, 64, 255), Transform2D::GetIdentity());

					shapeRenderer.SetColor(ByteColor(0, 64, 255));
					shapeRenderer.DrawCircle(Float3(shapeA.ComputeSupportPoint(+normal)), 4.0f);
					shapeRenderer.DrawCircle(Float3(shapeB.ComputeSupportPoint(-normal)), 4.0f);

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
					shapeRenderer.DrawCircle(Float3(normal * distance), 4.0f);
				}
			}
			else if (renderPhase.Is(RenderPhaseLabel::ScreenSpace))
			{
				fontRenderer.SetTextColor(Color::kBlack);
				StackStringW<100> buffer;
				FormatString(buffer, L"GJK Iteration: %d / Max %u (Q/W)", gjk2DInfo._loopCount, gjk2DInfo._maxLoopCount);
				fontRenderer.DrawDynamicText(buffer.CString(), Float2(10, 10), FontRenderingOption());
				FormatString(buffer, L"EPA Iteration: %d / Max %u (A/S)", epa2DInfo._iteration, epa2DInfo._maxIterationCount);
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

		GUICollisionShape2DComponent collisionShape2DComponent;
		collisionShape2DComponent._collisionShape2D = MakeShared<Physics::CollisionShape2D>(Physics::ConvexCollisionShape2D::MakeFromRenderingShape(Float2::kZero, defaultShape));
		guiSystem.AttachComponent(guiControlPointControlTemplate, std::move(collisionShape2DComponent));

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
	using namespace Physics;
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

	SceneObjectPool& sceneObjectPool = app.GetObjectPool();
	SceneObject sceneObject0 = sceneObjectPool.CreateSceneObject();
	{
		Mesh2DComponent mesh2DComponent;
		ShapeGenerator::GenerateCircle(1.0f, 16, ByteColor(0, 0, 255), mesh2DComponent._shape);
		sceneObjectPool.AttachComponent(sceneObject0, std::move(mesh2DComponent));
	}
	{
		const Shape& shape = sceneObjectPool.GetComponentMust<Mesh2DComponent>(sceneObject0)._shape;
		Collision2DComponent collision2DComponent;
		collision2DComponent._collisionShape2D = MakeShared<CollisionShape2D>(ConvexCollisionShape2D::MakeFromRenderingShape(Float2::kZero, shape));
		sceneObjectPool.AttachComponent(sceneObject0, std::move(collision2DComponent));
	}

	SceneObject sceneObject1 = sceneObjectPool.CreateSceneObject();
	{
		Mesh2DComponent mesh2DComponent;
		Vector<Float2> points;
		points.PushBack(Float2(1, 1));
		points.PushBack(Float2(1, 0));
		points.PushBack(Float2(2, 0));
		ShapeGenerator::GenerateConvexShape(points, ByteColor(0, 128, 255), mesh2DComponent._shape);
		sceneObjectPool.AttachComponent(sceneObject1, std::move(mesh2DComponent));
	}
	{
		const Shape& shape = sceneObjectPool.GetComponentMust<Mesh2DComponent>(sceneObject1)._shape;
		Collision2DComponent collision2DComponent;
		collision2DComponent._collisionShape2D = MakeShared<CollisionShape2D>(ConvexCollisionShape2D::MakeFromRenderingShape(Float2::kZero, shape));
		sceneObjectPool.AttachComponent(sceneObject1, std::move(collision2DComponent));
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


	SceneObjectPool& sceneObjectPool = app.GetObjectPool();
	SceneObject testObject = sceneObjectPool.CreateSceneObject();
	{
		MeshComponent meshComponent;
		Rendering::MeshGenerator::GeoSphereParam geosphereParam;
		geosphereParam._radius = 1.0f;
		geosphereParam._subdivisionIteration = 3;
		geosphereParam._smooth = true;
		Rendering::MeshGenerator::GenerateGeoSphere(geosphereParam, meshComponent._meshData);
		sceneObjectPool.AttachComponent(testObject, std::move(meshComponent));

		Transform& transform = sceneObjectPool.GetTransform(testObject);
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
		CameraComponent& cameraComponent = sceneObjectPool.GetComponentMust<CameraComponent>(currentCameraObject);
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
				MeshComponent* const meshComponent = sceneObjectPool.GetComponent<MeshComponent>(testObject);
				meshComponent->_shouldDrawNormals = !meshComponent->_shouldDrawNormals;
			}
			else if (inputContext.IsKeyDown(KeyCode::Num5) == true)
			{
				MeshComponent* const meshComponent = sceneObjectPool.GetComponent<MeshComponent>(testObject);
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

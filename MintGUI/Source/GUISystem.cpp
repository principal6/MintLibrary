#include <MintGUI/Include/GUISystem.h>
#include <MintGUI/Include/GUISystem.hpp>
#include <MintContainer/Include/HashMap.hpp>
#include <MintContainer/Include/StringReference.hpp>
#include <MintContainer/Include/Algorithm.hpp>
#include <MintRenderingBase/Include/GraphicsDevice.h>
#include <MintRenderingBase/Include/ShapeRenderer.h>
#include <MintPlatform/Include/InputContext.h>
#include <MintPhysics/Include/CollisionShape.h>
#include <MintPhysics/Include/Intersection.hpp>
#include <MintGUI/Include/GUIComponents.hpp>


namespace mint
{
	namespace GUI
	{
#pragma region GUISystem
		GUISystem::GUISystem()
			: _nextEntityID{ 0 }
		{
			RegisterComponentPool(GUIComponentPool<GUITransform2DComponent>::GetInstance());
			RegisterComponentPool(GUIComponentPool<GUIInteractionStateComponent>::GetInstance());
			RegisterComponentPool(GUIComponentPool<GUICollisionShape2DComponent>::GetInstance());
			RegisterComponentPool(GUIComponentPool<GUITextComponent>::GetInstance());
			RegisterComponentPool(GUIComponentPool<GUIShapeComponent>::GetInstance());
			RegisterComponentPool(GUIComponentPool<GUIDraggableComponent>::GetInstance());
		}

		GUISystem::~GUISystem()
		{
			__noop;
		}

		void GUISystem::RegisterComponentPool(IGUIComponentPool& componentPool)
		{
			_componentPools.PushBack(&componentPool);
		}

		GUIEntity GUISystem::CreateEntity()
		{
			GUIEntity entity;
			entity.Assign(_nextEntityID);
			++_nextEntityID;
			_entities.PushBack(entity);
			return entity;
		}

		GUIEntity GUISystem::CloneEntity(const GUIEntity& sourceEntity)
		{
			GUIEntity targetEntity = CreateEntity();
			for (IGUIComponentPool* const componentPool : _componentPools)
			{
				componentPool->CopyComponent(sourceEntity, targetEntity);
			}
			return targetEntity;
		}

		void GUISystem::Update()
		{
			InputSystem(_entities);
		}

		void GUISystem::Render(Rendering::GraphicsDevice& graphicsDevice)
		{
			RenderSystem(_entities, graphicsDevice);
		}

		void GUISystem::InputSystem(const Vector<GUIEntity>& entities)
		{
			static Float2 sMouseLeftButtonPressedPosition;

			InputContext& inputContext = InputContext::GetInstance();
			const Float2& mousePosition = inputContext.GetMousePosition();
			const MouseButtonState leftMouseButtonState = inputContext.GetMouseButtonState(MouseButton::Left);

			if (leftMouseButtonState == MouseButtonState::Pressed)
			{
				sMouseLeftButtonPressedPosition = mousePosition;
			}

			const bool isLeftMouseButtonReleasedOrUp = leftMouseButtonState == MouseButtonState::Released || leftMouseButtonState == MouseButtonState::Up;
			for (const GUIEntity& entity : entities)
			{
				GUITransform2DComponent* const transform2DComponent = GetComponent<GUITransform2DComponent>(entity);
				GUICollisionShape2DComponent* const collisionShape2DComponent = GetComponent<GUICollisionShape2DComponent>(entity);
				GUIInteractionStateComponent* const interactionStateComponent = GetComponent<GUIInteractionStateComponent>(entity);

				if (isLeftMouseButtonReleasedOrUp)
				{
					interactionStateComponent->_interactionState = GUIInteractionState::None;
				}

				const Float2 objectSpaceMousePosition = transform2DComponent->_transform2D.GetInverted() * mousePosition;
				const bool intersects = Physics::Intersect2D_GJK(*collisionShape2DComponent->_collisionShape2D, Physics::PointCollisionShape2D(objectSpaceMousePosition));
				if (intersects == true)
				{
					if (interactionStateComponent->_interactionState != GUIInteractionState::Pressed)
						interactionStateComponent->_interactionState = GUIInteractionState::Hovered;

					const MouseButtonState leftMouseButtonState = inputContext.GetMouseButtonState(MouseButton::Left);
					if (leftMouseButtonState == MouseButtonState::Pressed || leftMouseButtonState == MouseButtonState::Down || leftMouseButtonState == MouseButtonState::DoubleClicked)
					{
						const Float2 objectSpaceMouseLeftButtonPressedPosition = transform2DComponent->_transform2D.GetInverted() * sMouseLeftButtonPressedPosition;
						const bool intersects1 = Physics::Intersect2D_GJK(*collisionShape2DComponent->_collisionShape2D, Physics::PointCollisionShape2D(objectSpaceMouseLeftButtonPressedPosition));
						if (intersects1)
						{
							interactionStateComponent->_interactionState = GUIInteractionState::Pressed;
						}
					}
				}

				if (interactionStateComponent->_interactionState == GUIInteractionState::Pressed)
				{
					GUIDraggableComponent* const draggableComponent = GetComponent<GUIDraggableComponent>(entity);
					if (draggableComponent != nullptr)
					{
						if (leftMouseButtonState == MouseButtonState::Pressed)
						{
							draggableComponent->_localPressedPosition = sMouseLeftButtonPressedPosition - transform2DComponent->_transform2D._translation;
						}
						else
						{
							transform2DComponent->_transform2D._translation = mousePosition - draggableComponent->_localPressedPosition;
						}
					}
				}
			}
		}

		void GUISystem::RenderSystem(const Vector<GUIEntity>& entities, Rendering::GraphicsDevice& graphicsDevice)
		{
			using namespace Rendering;

			graphicsDevice.SetSolidCullNoneRasterizer();
			graphicsDevice.SetViewProjectionMatrix(Float4x4::kIdentity, graphicsDevice.GetScreenSpace2DProjectionMatrix());

			Rendering::ShapeRenderer& shapeRenderer = graphicsDevice.GetShapeRenderer();
			for (const GUIEntity& entity : entities)
			{
				GUITransform2DComponent* const transform2DComponent = GetComponent<GUITransform2DComponent>(entity);
				GUIInteractionStateComponent* const interactionStateComponent = GetComponent<GUIInteractionStateComponent>(entity);

				GUITextComponent* const textComponent = GetComponent<GUITextComponent>(entity);
				if (textComponent != nullptr)
				{
					FontRenderingOption fontRenderingOption;
					fontRenderingOption._directionHorz = TextRenderDirectionHorz::Centered;
					fontRenderingOption._directionVert = TextRenderDirectionVert::Centered;
					shapeRenderer.DrawDynamicText(textComponent->_text.CString(), transform2DComponent->_transform2D._translation + textComponent->_offset, fontRenderingOption);
				}

				GUIShapeComponent* const shapeComponent = GetComponent<GUIShapeComponent>(entity);
				if (shapeComponent != nullptr)
				{
					uint32 shapeIndex = static_cast<uint32>(interactionStateComponent->_interactionState);
					if (shapeComponent->_shapes[shapeIndex]._vertices.IsEmpty())
					{
						shapeIndex = 0;
					}

					shapeRenderer.AddShape(shapeComponent->_shapes[shapeIndex], transform2DComponent->_transform2D);
				}
			}

			shapeRenderer.Render();
		}
#pragma endregion
	}
}

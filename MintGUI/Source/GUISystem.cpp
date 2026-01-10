#include <MintGUI/Include/GUISystem.h>
#include <MintGUI/Include/GUISystem.hpp>
#include <MintContainer/Include/HashMap.hpp>
#include <MintContainer/Include/StringReference.hpp>
#include <MintContainer/Include/Algorithm.hpp>
#include <MintRendering/Include/GraphicsDevice.h>
#include <MintRendering/Include/FontRenderer.h>
#include <MintRendering/Include/ShapeRenderer.h>
#include <MintPlatform/Include/InputContext.h>
#include <MintPhysics/Include/CollisionShape.h>
#include <MintPhysics/Include/Intersection.hpp>
#include <MintGUI/Include/GUIComponents.hpp>


namespace mint
{
	namespace GUI
	{
#pragma region GUISystem
		GUISystem::GUISystem(Rendering::GraphicsDevice& graphicsDevice)
			: _graphicsDevice{ graphicsDevice }
			, _nextControlID{ 0 }
			, _nextControlTemplateID{ 0 }
		{
			Initialize();
		}

		GUISystem::~GUISystem()
		{
			Terminate();
		}

		GUIControl GUISystem::CreateControl()
		{
			GUIControl control;
			control.Assign(_nextControlID);
			++_nextControlID;
			_controls.PushBack(control);
			return control;
		}
		
		GUIControl GUISystem::CreateControl(const GUIControlTemplate& controlTemplate)
		{
			const GUIControl control = CreateControl();
			const Vector<IGUIComponentPool*>& componentPools = GUIComponentPoolRegistry::GetInstance().GetComponentPools();
			for (IGUIComponentPool* const componentPool : componentPools)
			{
				componentPool->CopyComponentFromTemplate(controlTemplate, control);
			}
			return control;
		}

		GUIControl GUISystem::CloneControl(const GUIControl& sourceControl)
		{
			const GUIControl targetControl = CreateControl();
			const Vector<IGUIComponentPool*>& componentPools = GUIComponentPoolRegistry::GetInstance().GetComponentPools();
			for (IGUIComponentPool* const componentPool : componentPools)
			{
				componentPool->CopyComponent(sourceControl, targetControl);
			}
			return targetControl;
		}

		GUIControlTemplate GUISystem::CreateTemplate()
		{
			GUIControlTemplate controlTemplate;
			controlTemplate.Assign(_nextControlTemplateID);
			++_nextControlTemplateID;
			_controlTemplates.PushBack(controlTemplate);
			return controlTemplate;
		}

		GUIControlTemplate GUISystem::CreateTemplate(const GUIControl& sourceControl)
		{
			const GUIControlTemplate controlTemplate = CreateTemplate();
			const Vector<IGUIComponentPool*>& componentPools = GUIComponentPoolRegistry::GetInstance().GetComponentPools();
			for (IGUIComponentPool* const componentPool : componentPools)
			{
				componentPool->CopyComponentToTemplate(sourceControl, controlTemplate);
			}
			return controlTemplate;
		}

		void GUISystem::Update()
		{
			InputSystem(_controls);
		}

		void GUISystem::Render()
		{
			RenderSystem(_controls, _graphicsDevice);
		}

		void GUISystem::Initialize()
		{
			using namespace Rendering;
			MaterialPool& materialPool = _graphicsDevice.GetMaterialPool();
			MaterialDesc materialDesc;
			materialDesc._materialName = "GUI";
			materialDesc._shaderPipelineID = _graphicsDevice.GetShapeRenderer().GetDefaultShaderPipelineID();
			_defaultMaterialID = materialPool.CreateMaterial(materialDesc);
		}

		void GUISystem::Terminate()
		{
			using namespace Rendering;
			MaterialPool& materialPool = _graphicsDevice.GetMaterialPool();
			materialPool.DestroyMaterial(_defaultMaterialID);
		}

		void GUISystem::InputSystem(const Vector<GUIControl>& controls)
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
			for (const GUIControl& control : controls)
			{
				GUITransform2DComponent* const transform2DComponent = GetComponent<GUITransform2DComponent>(control);
				GUICollisionShapeComponent* const collisionShapeComponent = GetComponent<GUICollisionShapeComponent>(control);
				GUIInteractionStateComponent* const interactionStateComponent = GetComponent<GUIInteractionStateComponent>(control);

				if (isLeftMouseButtonReleasedOrUp)
				{
					interactionStateComponent->_interactionState = GUIInteractionState::None;
				}

				const Float2 objectSpaceMousePosition = transform2DComponent->_transform2D.GetInverted() * mousePosition;
				const bool intersects = Physics2D::Intersect_GJK(*collisionShapeComponent->_collisionShape, Physics2D::PointCollisionShape(objectSpaceMousePosition));
				if (intersects == true)
				{
					if (interactionStateComponent->_interactionState != GUIInteractionState::Pressed)
						interactionStateComponent->_interactionState = GUIInteractionState::Hovered;

					const MouseButtonState leftMouseButtonState = inputContext.GetMouseButtonState(MouseButton::Left);
					if (leftMouseButtonState == MouseButtonState::Pressed || leftMouseButtonState == MouseButtonState::Down || leftMouseButtonState == MouseButtonState::DoubleClicked)
					{
						const Float2 objectSpaceMouseLeftButtonPressedPosition = transform2DComponent->_transform2D.GetInverted() * sMouseLeftButtonPressedPosition;
						const bool intersects1 = Physics2D::Intersect_GJK(*collisionShapeComponent->_collisionShape, Physics2D::PointCollisionShape(objectSpaceMouseLeftButtonPressedPosition));
						if (intersects1)
						{
							interactionStateComponent->_interactionState = GUIInteractionState::Pressed;
						}
					}
				}

				if (interactionStateComponent->_interactionState == GUIInteractionState::Pressed)
				{
					GUIDraggableComponent* const draggableComponent = GetComponent<GUIDraggableComponent>(control);
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

		void GUISystem::RenderSystem(const Vector<GUIControl>& controls, Rendering::GraphicsDevice& graphicsDevice)
		{
			using namespace Rendering;

			graphicsDevice.SetSolidCullNoneRasterizer();

			Rendering::FontRenderer& fontRenderer = graphicsDevice.GetFontRenderer();
			Rendering::ShapeRenderer& shapeRenderer = graphicsDevice.GetShapeRenderer();
			shapeRenderer.SetMaterial(_defaultMaterialID);

			for (const GUIControl& control : controls)
			{
				GUITransform2DComponent* const transform2DComponent = GetComponent<GUITransform2DComponent>(control);
				GUIInteractionStateComponent* const interactionStateComponent = GetComponent<GUIInteractionStateComponent>(control);

				GUITextComponent* const textComponent = GetComponent<GUITextComponent>(control);
				if (textComponent != nullptr)
				{
					FontRenderingOption fontRenderingOption;
					fontRenderingOption._directionHorz = TextRenderDirectionHorz::Centered;
					fontRenderingOption._directionVert = TextRenderDirectionVert::Centered;
					fontRenderer.DrawDynamicText(textComponent->_text.CString(), transform2DComponent->_transform2D._translation + textComponent->_offset, fontRenderingOption);
				}

				GUIShapeComponent* const shapeComponent = GetComponent<GUIShapeComponent>(control);
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

			shapeRenderer.SetMaterial(shapeRenderer.GetDefaultMaterialID());
		}
#pragma endregion
	}
}

#include <MintGUI/Include/GUIObject.h>
#include <MintContainer/Include/SharedPtr.hpp>
#include <MintContainer/Include/String.hpp>
#include <MintRenderingBase/Include/GraphicsDevice.h>
#include <MintPhysics/Include/Intersection.hpp>


namespace mint
{
	namespace GUI
	{
#pragma region GUIComponent
		SharedPtr<GUIComponent> GUITextComponent::Clone() const
		{
			SharedPtr<GUITextComponent> cloned = MakeShared<GUITextComponent>();
			*cloned = *this;
			return cloned;
		}

		void GUITextComponent::SetText(const StringReferenceW& text)
		{
			_text = text;
		}

		void GUITextComponent::SetOffset(const Float2& offset)
		{
			_offset = offset;
		}

		void GUITextComponent::Render(Rendering::ShapeRenderer& shapeRenderer, const Float2& objectPosition, const GUIObjectInteractionState& objectInteractionState) const
		{
			using namespace Rendering;

			if (_text.IsEmpty())
			{
				return;
			}

			FontRenderingOption fontRenderingOption;
			fontRenderingOption._directionHorz = TextRenderDirectionHorz::Centered;
			fontRenderingOption._directionVert = TextRenderDirectionVert::Centered;
			shapeRenderer.DrawDynamicText(_text.CString(), objectPosition + _offset, fontRenderingOption);
		}

		SharedPtr<GUIComponent> GUIShapeComponent::Clone() const
		{
			SharedPtr<GUIShapeComponent> cloned = MakeShared<GUIShapeComponent>();
			*cloned = *this;
			for (uint32 i = 0; i < static_cast<uint32>(GUIObjectInteractionState::COUNT); ++i)
			{
				cloned->_shapes[i] = _shapes[i];
			}
			return cloned;
		}

		void GUIShapeComponent::SetShape(const GUIObjectInteractionState& objectInteractionState, const Rendering::Shape& shape)
		{
			_shapes[static_cast<uint32>(objectInteractionState)] = shape;
		}

		void GUIShapeComponent::Render(Rendering::ShapeRenderer& shapeRenderer, const Float2& objectPosition, const GUIObjectInteractionState& objectInteractionState) const
		{
			using namespace Rendering;

			const Float4 objectPosition4{ objectPosition };
			shapeRenderer.SetPosition(objectPosition4);

			uint32 shapeIndex = static_cast<uint32>(objectInteractionState);
			if (_shapes[shapeIndex]._vertices.IsEmpty())
			{
				shapeIndex = 0;
			}

			shapeRenderer.AddShape(_shapes[shapeIndex]);
		}

		SharedPtr<GUIComponent> GUIDraggableComponent::Clone() const
		{
			SharedPtr<GUIDraggableComponent> cloned = MakeShared<GUIDraggableComponent>();
			*cloned = *this;
			return cloned;
		}
#pragma endregion

#pragma region GUIObjectTemplate
		void GUIObjectTemplate::AddComponent(SharedPtr<GUIComponent> guiComponent)
		{
			_components.PushBack(guiComponent);
		}

		void GUIObjectTemplate::SetCollisionShape(Physics::ConvexCollisionShape2D&& collisionShape)
		{
			_collisionShape = MakeShared(std::move(collisionShape));
		}
#pragma endregion

#pragma region GUIObject
		GUIObject::~GUIObject()
		{
			__noop;
		}

		void GUIObject::AddComponent(SharedPtr<GUIComponent> guiComponent)
		{
			_components.PushBack(guiComponent);
		}

		void GUIObject::Render(Rendering::ShapeRenderer& shapeRenderer, const GUIObjectInteractionState& objectInteractionState) const
		{
			for (const SharedPtr<GUIComponent>& component : _components)
			{
				if (component->IsRenderable() == false)
				{
					continue;
				}

				static_cast<const GUIRenderableComponent*>(component.Get())->Render(shapeRenderer, _transform2D._translation, objectInteractionState);
			}
		}
#pragma endregion
	}
}

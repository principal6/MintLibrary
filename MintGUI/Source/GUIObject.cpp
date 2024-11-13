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

		void GUIShapeComponent::Render(Rendering::ShapeRenderer& shapeRenderer, const Float2& objectPosition, const GUIObjectInteractionState& objectInteractionState) const
		{
			using namespace Rendering;

			const Float4 objectPosition4{ objectPosition };
			shapeRenderer.SetPosition(objectPosition4);

			const uint32 shapeIndex = static_cast<uint32>(objectInteractionState);
			shapeRenderer.AddShape(_shapes[shapeIndex]);
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

		void GUIObject::Render(Rendering::ShapeRenderer& shapeRenderer, const GUIObjectInteractionState& objectInteractionState) const
		{
			for (const SharedPtr<GUIComponent>& component : _components)
			{
				component->Render(shapeRenderer, _position, objectInteractionState);
			}
		}
#pragma endregion
	}
}

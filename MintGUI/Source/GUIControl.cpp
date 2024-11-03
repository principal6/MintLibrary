#include <MintGUI/Include/GUIControl.h>
#include <MintContainer/Include/String.hpp>
#include <MintRenderingBase/Include/GraphicsDevice.h>
#include <MintPhysics/Include/Intersection.hpp>


namespace mint
{
	namespace GUI
	{
#pragma region GUIControlComponent
		void GUIControlTextComponent::Render(Rendering::ShapeRendererContext& shapeRendererContext, const Float2& controlPosition, const GUIControlInteractionState& controlInteractionState) const
		{
			using namespace Rendering;

			if (_text.IsEmpty())
			{
				return;
			}

			FontRenderingOption fontRenderingOption;
			fontRenderingOption._directionHorz = TextRenderDirectionHorz::Centered;
			fontRenderingOption._directionVert = TextRenderDirectionVert::Centered;
			shapeRendererContext.DrawDynamicText(_text.CString(), controlPosition + _offset, fontRenderingOption);
		}

		void GUIControlShapeComponent::Render(Rendering::ShapeRendererContext& shapeRendererContext, const Float2& controlPosition, const GUIControlInteractionState& controlInteractionState) const
		{
			using namespace Rendering;

			const Float4 controlPosition4{ controlPosition };
			shapeRendererContext.SetPosition(controlPosition4);

			const uint32 shapeIndex = static_cast<uint32>(controlInteractionState);
			shapeRendererContext.AddShape(_shapes[shapeIndex]);
		}
#pragma endregion

#pragma region GUIControlTemplate
		Vector<SharedPtr<GUIControlComponent>>& GUIControlTemplate::AccessComponents()
		{
			return _components;
		}

		void GUIControlTemplate::SetCollisionShape(Physics::ConvexCollisionShape2D&& collisionShape)
		{
			_collisionShape = MakeShared(std::move(collisionShape));
		}
#pragma endregion

#pragma region GUIControl
		void GUIControl::Render(Rendering::ShapeRendererContext& shapeRendererContext, const GUIControlInteractionState& controlInteractionState) const
		{
			for (const SharedPtr<GUIControlComponent>& component : _components)
			{
				component->Render(shapeRendererContext, _position, controlInteractionState);
			}
		}
#pragma endregion
	}
}

#include <MintRenderingBase/Include/GUI/GUIControl.h>
#include <MintContainer/Include/String.hpp>
#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintPhysics/Include/Intersection.hpp>


#pragma optimize("", off)


namespace mint
{
	namespace Rendering
	{
#pragma region GUIControlComponent
		void GUIControlTextComponent::Render(GraphicDevice& graphicDevice, const Float2& controlPosition, const GUIControlInteractionState& controlInteractionState) const
		{
			if (_text.IsEmpty())
			{
				return;
			}

			ShapeRendererContext& shapeRendererContext = graphicDevice.GetShapeRendererContext();
			FontRenderingOption fontRenderingOption;
			fontRenderingOption._directionHorz = TextRenderDirectionHorz::Centered;
			fontRenderingOption._directionVert = TextRenderDirectionVert::Centered;
			shapeRendererContext.DrawDynamicText(_text.CString(), controlPosition + _offset, fontRenderingOption);
		}

		void GUIControlShapeComponent::Render(GraphicDevice& graphicDevice, const Float2& controlPosition, const GUIControlInteractionState& controlInteractionState) const
		{
			ShapeRendererContext& shapeRendererContext = graphicDevice.GetShapeRendererContext();
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

		void GUIControlTemplate::SetCollisionShape(Physics::ConvexShape2D&& collisionShape)
		{
			_collisionShape = MakeShared(std::move(collisionShape));
		}
#pragma endregion

#pragma region GUIControl
		void GUIControl::Render(GraphicDevice& graphicDevice, const GUIControlInteractionState& controlInteractionState) const
		{
			for (const SharedPtr<GUIControlComponent>& component : _components)
			{
				component->Render(graphicDevice, _position, controlInteractionState);
			}
		}
#pragma endregion
	}
}

#include <MintRenderingBase/Include/GUI/GUIInteractionModules.h>


namespace mint
{
	namespace Rendering
	{
		namespace GUI
		{
#pragma region ResizingModule
			void ResizingModule::makeOuterAndInenrRects(const ControlData& controlData, const Rect& outerResizingDistances, const Rect& innerResizingDistances, Rect& outerRect, Rect& innerRect)
			{
				const Rect controlRect = Rect(controlData._absolutePosition, controlData._size);
				outerRect = controlRect;
				outerRect.ExpandByQuantity(outerResizingDistances);
				innerRect = controlRect;
				innerRect.ShrinkByQuantity(innerResizingDistances);
			}

			ControlData::ResizingFlags ResizingModule::makeResizingFlags(const Float2& mousePosition, const ControlData& controlData, const Rect& outerRect, const Rect& innerRect)
			{
				ControlData::ResizingFlags resizingInteraction;
				if (mousePosition._y >= outerRect.Top() && mousePosition._y <= innerRect.Top())
				{
					resizingInteraction._top = true;
				}
				if (mousePosition._y <= outerRect.Bottom() && mousePosition._y >= innerRect.Bottom())
				{
					resizingInteraction._bottom = true;
				}
				if (mousePosition._x >= outerRect.Left() && mousePosition._x <= innerRect.Left())
				{
					resizingInteraction._left = true;
				}
				if (mousePosition._x <= outerRect.Right() && mousePosition._x >= innerRect.Right())
				{
					resizingInteraction._right = true;
				}
				resizingInteraction.maskBy(controlData._resizingMask);
				return resizingInteraction;
			}

			bool ResizingModule::begin(const ResizingModuleInput& resizingModuleInput)
			{
				if (resizingModuleInput._resizingInteraction.isAllFalse() == true)
				{
					return false;
				}

				return __super::begin(resizingModuleInput);
			}
#pragma endregion
		}
	}
}

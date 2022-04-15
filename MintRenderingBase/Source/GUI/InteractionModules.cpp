#include <stdafx.h>
#include <MintRenderingBase/Include/GUI/InteractionModules.h>


namespace mint
{
    namespace Rendering
    {
#pragma region InteractionModule
        const bool InteractionModule::isInteracting() const
        {
            return _controlID.isValid();
        }

        const bool InteractionModule::isInteracting(const ControlData& controlData) const
        {
            return _controlID == controlData.getID();
        }

        Float2 InteractionModule::computeRelativeMousePressedPosition() const
        {
            return _mousePressedPosition - _initialControlPosition;
        }

        const bool InteractionModule::beginInternal(const ControlData& controlData, const Float2& mousePressedPosition)
        {
            if (isInteracting())
            {
                return false;
            }

            _controlID = controlData.getID();
            _initialControlPosition = controlData._absolutePosition;
            _mousePressedPosition = mousePressedPosition;
            return true;
        }
#pragma endregion


#pragma region ResizingModule
        void ResizingModule::makeOuterAndInenrRects(const ControlData& controlData, const Rect& outerResizingDistances, const Rect& innerResizingDistances, Rect& outerRect, Rect& innerRect)
        {
            const Rect controlRect = Rect(controlData._absolutePosition, controlData._size);
            outerRect = controlRect;
            outerRect.expandByQuantity(outerResizingDistances);
            innerRect = controlRect;
            innerRect.shrinkByQuantity(innerResizingDistances);
        }

        ResizingFlags ResizingModule::makeResizingFlags(const Float2& mousePosition, const ControlData& controlData, const Rect& outerRect, const Rect& innerRect)
        {
            ResizingFlags resizingInteraction;
            if (mousePosition._y >= outerRect.top() && mousePosition._y <= innerRect.top())
            {
                resizingInteraction._top = true;
            }
            if (mousePosition._y <= outerRect.bottom() && mousePosition._y >= innerRect.bottom())
            {
                resizingInteraction._bottom = true;
            }
            if (mousePosition._x >= outerRect.left() && mousePosition._x <= innerRect.left())
            {
                resizingInteraction._left = true;
            }
            if (mousePosition._x <= outerRect.right() && mousePosition._x >= innerRect.right())
            {
                resizingInteraction._right = true;
            }
            resizingInteraction.maskBy(controlData._resizingMask);
            return resizingInteraction;
        }

        const bool ResizingModule::begin(const ControlData& controlData, const Float2& mousePressedPosition, const void* const customData)
        {
            if (customData == nullptr)
            {
                MINT_NEVER;
                return false;
            }

            const ResizingFlags& resizingFlags = *reinterpret_cast<const ResizingFlags*>(customData);
            if (resizingFlags.isAllFalse() == true)
            {
                return false;
            }

            if (beginInternal(controlData, mousePressedPosition) == false)
            {
                return false;
            }

            _initialControlSize = controlData._size;
            _resizingFlags = resizingFlags;
            return true;
        }
#pragma endregion
    }
}

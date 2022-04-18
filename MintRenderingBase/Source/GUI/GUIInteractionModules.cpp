#include <stdafx.h>
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
                outerRect.expandByQuantity(outerResizingDistances);
                innerRect = controlRect;
                innerRect.shrinkByQuantity(innerResizingDistances);
            }

            ControlData::ResizingFlags ResizingModule::makeResizingFlags(const Float2& mousePosition, const ControlData& controlData, const Rect& outerRect, const Rect& innerRect)
            {
                ControlData::ResizingFlags resizingInteraction;
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

            bool ResizingModule::begin(const ResizingModuleInput& resizingModuleInput)
            {
                if (resizingModuleInput._resizingInteraction.isAllFalse() == true)
                {
                    return false;
                }

                return __super::begin(resizingModuleInput);
            }
#pragma endregion


#pragma region DockingModule
            bool DockingModule::isInteracting() const
            {
                return _input._shipControlID.isValid();
            }

            bool DockingModule::isDockControl(const ControlID& controlID) const
            {
                return _input._dockControlID == controlID;
            }

            bool DockingModule::isShipControl(const ControlID& controlID) const
            {
                return _input._shipControlID == controlID;
            }

            bool DockingModule::begin(const DockingModuleInput& input)
            {
                if (isInteracting())
                {
                    return false;
                }

                _input = input;
                return true;
            }

            void DockingModule::end()
            {
                _input._dockControlID.invalidate();
                _input._shipControlID.invalidate();
            }
#pragma endregion
        }
    }
}

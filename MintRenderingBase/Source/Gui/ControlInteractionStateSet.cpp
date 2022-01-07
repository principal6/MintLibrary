#include <stdafx.h>
#include <MintRenderingBase/Include/Gui/ControlInteractionStateSet.h>

#include <MintRenderingBase/Include/Gui/InputHelpers.hpp>

#include <MintLibrary/Include/ScopedCPUProfiler.h>


namespace mint
{
    namespace Gui
    {

        void ControlInteractionStateSet::setControlHovered(const ControlData& controlData) noexcept
        {
            resetHover();

            _hoveredControlID = controlData.getID();

            if (_hoverStarted == false)
            {
                _hoverStarted = true;
            }
        }

        const bool ControlInteractionStateSet::setControlPressed(const ControlData& controlData) noexcept
        {
            if (isControlHovered(controlData) == true)
            {
                resetHover();
            }

            if (_pressedControlID != controlData.getID())
            {
                _pressedControlID = controlData.getID();
                _pressedControlInitialPosition = controlData._position;

                return true;
            }

            return false;
        }

        const bool ControlInteractionStateSet::setControlClicked(const ControlData& controlData) noexcept
        {
            if (_pressedControlID == controlData.getID())
            {
                _clickedControlIDPerFrame = controlData.getID();

                return true;
            }
            return false;
        }

        void ControlInteractionStateSet::setControlFocused(const ControlData& controlData) noexcept
        {
            _focusedControlID = controlData.getID();
        }

        const bool ControlInteractionStateSet::isControlHovered(const ControlData& controlData) const noexcept
        {
            return (controlData.getID() == _hoveredControlID);
        }

        const bool ControlInteractionStateSet::isControlPressed(const ControlData& controlData) const noexcept
        {
            return (controlData.getID() == _pressedControlID);
        }

        const bool ControlInteractionStateSet::isControlClicked(const ControlData& controlData) const noexcept
        {
            return (controlData.getID() == _clickedControlIDPerFrame);
        }

        const bool ControlInteractionStateSet::isControlFocused(const ControlData& controlData) const noexcept
        {
            return (controlData.getID() == _focusedControlID);
        }

        const bool ControlInteractionStateSet::isHoveringMoreThan(const uint64 durationMs) const noexcept
        {
            return (_hoverStarted == true && _hoverStartTimeMs + durationMs < Profiler::getCurrentTimeMs());
        }

        void ControlInteractionStateSet::resetPerFrameStates(const MouseStates& mouseStates) noexcept
        {
            if (mouseStates.isButtonDownUp(Platform::MouseButton::Left) == true)
            {
                if (_pressedControlID == ControlID(1))
                {
                    _focusedControlID.reset();
                }

                _pressedControlID.reset();
            }

            _isMouseInteractionDoneThisFrame = false;
            _clickedControlIDPerFrame.reset();
        }

        void ControlInteractionStateSet::resetHover() noexcept
        {
            _hoveredControlID.reset();
            _hoverStartTimeMs = Profiler::getCurrentTimeMs();
            _tooltipPosition.setZero();
            _tooltipParentWindowID.reset();
        }

        void ControlInteractionStateSet::resetHoverIf(const ControlData& controlData) noexcept
        {
            if (controlData.getID() == _hoveredControlID)
            {
                resetHover();
            }
        }

        void ControlInteractionStateSet::resetPressIf(const ControlData& controlData) noexcept
        {
            if (controlData.getID() == _pressedControlID)
            {
                _pressedControlID.reset();
                _pressedControlInitialPosition.setZero();
            }
        }

        const Float2 ControlInteractionStateSet::getTooltipWindowPosition(const ControlData& tooltipParentWindow) const noexcept
        {
            return _tooltipPosition - tooltipParentWindow._position + Float2(12.0f, -16.0f);
        }

        void ControlInteractionStateSet::setTooltipData(const MouseStates& mouseStates, const wchar_t* const tooltipText, const ControlID& tooltipParentWindowID) noexcept
        {
            _tooltipTextFinal = tooltipText;
            _tooltipPosition = mouseStates.getPosition();
            _tooltipParentWindowID = tooltipParentWindowID;

            _hoverStarted = false;
        }
    }
}

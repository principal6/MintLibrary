#include <stdafx.h>
#include <MintRenderingBase/Include/Gui/ControlInteractionStateSet.h>

#include <MintRenderingBase/Include/Gui/InputHelpers.hpp>

#include <MintLibrary/Include/ScopedCpuProfiler.h>


namespace mint
{
    namespace Gui
    {

        void ControlInteractionStateSet::setControlHovered(const ControlData& controlData) noexcept
        {
            resetHover();

            _hoveredControlId = controlData.getId();

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

            if (_pressedControlId != controlData.getId())
            {
                _pressedControlId = controlData.getId();
                _pressedControlInitialPosition = controlData._position;

                return true;
            }

            return false;
        }

        const bool ControlInteractionStateSet::setControlClicked(const ControlData& controlData) noexcept
        {
            if (_pressedControlId == controlData.getId())
            {
                _clickedControlIdPerFrame = controlData.getId();

                return true;
            }
            return false;
        }

        void ControlInteractionStateSet::setControlFocused(const ControlData& controlData) noexcept
        {
            _focusedControlId = controlData.getId();
        }

        const bool ControlInteractionStateSet::isControlHovered(const ControlData& controlData) const noexcept
        {
            return (controlData.getId() == _hoveredControlId);
        }

        const bool ControlInteractionStateSet::isControlPressed(const ControlData& controlData) const noexcept
        {
            return (controlData.getId() == _pressedControlId);
        }

        const bool ControlInteractionStateSet::isControlClicked(const ControlData& controlData) const noexcept
        {
            return (controlData.getId() == _clickedControlIdPerFrame);
        }

        const bool ControlInteractionStateSet::isControlFocused(const ControlData& controlData) const noexcept
        {
            return (controlData.getId() == _focusedControlId);
        }

        const bool ControlInteractionStateSet::isHoveringMoreThan(const uint64 durationMs) const noexcept
        {
            return (_hoverStarted == true && _hoverStartTimeMs + durationMs < Profiler::getCurrentTimeMs());
        }

        void ControlInteractionStateSet::resetPerFrameStates(const MouseStates& mouseStates) noexcept
        {
            if (mouseStates.isButtonDownUp(Platform::MouseButton::Left) == true)
            {
                if (_pressedControlId == ControlId(1))
                {
                    _focusedControlId.reset();
                }

                _pressedControlId.reset();
            }

            _isMouseInteractionDoneThisFrame = false;
            _clickedControlIdPerFrame.reset();
        }

        void ControlInteractionStateSet::resetHover() noexcept
        {
            _hoveredControlId.reset();
            _hoverStartTimeMs = Profiler::getCurrentTimeMs();
            _tooltipPosition.setZero();
            _tooltipParentWindowId.reset();
        }

        void ControlInteractionStateSet::resetHoverIf(const ControlData& controlData) noexcept
        {
            if (controlData.getId() == _hoveredControlId)
            {
                resetHover();
            }
        }

        void ControlInteractionStateSet::resetPressIf(const ControlData& controlData) noexcept
        {
            if (controlData.getId() == _pressedControlId)
            {
                _pressedControlId.reset();
                _pressedControlInitialPosition.setZero();
            }
        }

        const Float2 ControlInteractionStateSet::getTooltipWindowPosition(const ControlData& tooltipParentWindow) const noexcept
        {
            return _tooltipPosition - tooltipParentWindow._position + Float2(12.0f, -16.0f);
        }

        void ControlInteractionStateSet::setTooltipData(const MouseStates& mouseStates, const wchar_t* const tooltipText, const ControlId& tooltipParentWindowId) noexcept
        {
            _tooltipTextFinal = tooltipText;
            _tooltipPosition = mouseStates.getPosition();
            _tooltipParentWindowId = tooltipParentWindowId;

            _hoverStarted = false;
        }
    }
}

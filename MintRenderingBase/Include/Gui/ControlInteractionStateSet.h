#pragma once


#ifndef MINT_GUI_CONTROL_INTERACTION_STATESET
#define MINT_GUI_CONTROL_INTERACTION_STATESET


#include <MintRenderingBase/Include/Gui/ControlData.h>


namespace mint
{
    namespace Gui
    {
        struct MouseStates;


        class ControlInteractionStateSet
        {
        public:
            void                            setControlHovered(const ControlData& controlData) noexcept;
            const bool                      setControlPressed(const ControlData& controlData) noexcept;
            const bool                      setControlClicked(const ControlData& controlData) noexcept;
            void                            setControlFocused(const ControlData& controlData) noexcept;
            
        public:
            MINT_INLINE const bool          hasPressedControl() const noexcept { return _pressedControlId.isValid(); }
            MINT_INLINE const bool          hasClickedControl() const noexcept { return _clickedControlIdPerFrame.isValid(); }
            MINT_INLINE const bool          hasFocusedControl() const noexcept { return _focusedControlId.isValid(); }
            
        public:
            const bool                      isControlHovered(const ControlData& controlData) const noexcept;
            const bool                      isControlPressed(const ControlData& controlData) const noexcept;
            const bool                      isControlClicked(const ControlData& controlData) const noexcept;
            const bool                      isControlFocused(const ControlData& controlData) const noexcept;
            const bool                      isHoveringMoreThan(const uint64 durationMs) const noexcept;
            
        public:
            MINT_INLINE const ControlId&    getHoveredControlId() const noexcept { return _hoveredControlId; }
            MINT_INLINE const ControlId&    getPressedControlId() const noexcept { return _pressedControlId; }
            MINT_INLINE const ControlId&    getFocusedControlId() const noexcept { return _focusedControlId; }

        public:
            void                            resetPerFrameStates(const MouseStates& mouseStates) noexcept;
            void                            resetHover() noexcept;
            void                            resetHoverIf(const ControlData& controlData) noexcept;
            void                            resetPressIf(const ControlData& controlData) noexcept;
            
        public:
            MINT_INLINE void                setMouseInteractionDoneThisFrame() noexcept { _isMouseInteractionDoneThisFrame = true; }
            MINT_INLINE const bool          isMouseInteractionDoneThisFrame() const noexcept { return _isMouseInteractionDoneThisFrame; }

        public:
            MINT_INLINE const bool          needToShowTooltip() const noexcept { return _tooltipParentWindowId.isValid(); }
            MINT_INLINE const wchar_t*      getTooltipText() const noexcept { return _tooltipTextFinal; }
            const Float2                    getTooltipWindowPosition(const ControlData& tooltipParentWindow) const noexcept;
            MINT_INLINE const ControlId&    getTooltipParentWindowId() const noexcept { return _tooltipParentWindowId; }
            void                            setTooltipData(const MouseStates& mouseStates, const wchar_t* const tooltipText, const ControlId& tooltipParentWindowId) noexcept;

        private:
            bool                            _isMouseInteractionDoneThisFrame = false;
            ControlId                       _hoveredControlId;
            ControlId                       _focusedControlId;
            ControlId                       _pressedControlId;
            Float2                          _pressedControlInitialPosition;
            ControlId                       _clickedControlIdPerFrame;
            uint64                          _hoverStartTimeMs = 0;
            bool                            _hoverStarted = false;
            Float2                          _tooltipPosition;
            ControlId                       _tooltipParentWindowId;
            const wchar_t*                  _tooltipTextFinal = nullptr;
        };
    }
}


#endif // !MINT_GUI_CONTROL_INTERACTION_STATESET

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
            MINT_INLINE const bool          hasPressedControl() const noexcept { return _pressedControlID.isValid(); }
            MINT_INLINE const bool          hasClickedControl() const noexcept { return _clickedControlIDPerFrame.isValid(); }
            MINT_INLINE const bool          hasFocusedControl() const noexcept { return _focusedControlID.isValid(); }
            
        public:
            const bool                      isControlHovered(const ControlData& controlData) const noexcept;
            const bool                      isControlPressed(const ControlData& controlData) const noexcept;
            const bool                      isControlClicked(const ControlData& controlData) const noexcept;
            const bool                      isControlFocused(const ControlData& controlData) const noexcept;
            const bool                      isHoveringMoreThan(const uint64 durationMs) const noexcept;
            
        public:
            MINT_INLINE const ControlID&    getHoveredControlID() const noexcept { return _hoveredControlID; }
            MINT_INLINE const ControlID&    getPressedControlID() const noexcept { return _pressedControlID; }
            MINT_INLINE const ControlID&    getFocusedControlID() const noexcept { return _focusedControlID; }

        public:
            void                            resetPerFrameStates(const MouseStates& mouseStates) noexcept;
            void                            resetHover() noexcept;
            void                            resetHoverIf(const ControlData& controlData) noexcept;
            void                            resetPressIf(const ControlData& controlData) noexcept;
            
        public:
            MINT_INLINE void                setMouseInteractionDoneThisFrame() noexcept { _isMouseInteractionDoneThisFrame = true; }
            MINT_INLINE const bool          isMouseInteractionDoneThisFrame() const noexcept { return _isMouseInteractionDoneThisFrame; }

        public:
            MINT_INLINE const bool          needToShowTooltip() const noexcept { return _tooltipParentWindowID.isValid(); }
            MINT_INLINE const wchar_t*      getTooltipText() const noexcept { return _tooltipTextFinal; }
            const Float2                    getTooltipWindowPosition(const ControlData& tooltipParentWindow) const noexcept;
            MINT_INLINE const ControlID&    getTooltipParentWindowID() const noexcept { return _tooltipParentWindowID; }
            void                            setTooltipData(const MouseStates& mouseStates, const wchar_t* const tooltipText, const ControlID& tooltipParentWindowID) noexcept;

        private:
            bool                            _isMouseInteractionDoneThisFrame = false;
            ControlID                       _hoveredControlID;
            ControlID                       _focusedControlID;
            ControlID                       _pressedControlID;
            Float2                          _pressedControlInitialPosition;
            ControlID                       _clickedControlIDPerFrame;
            uint64                          _hoverStartTimeMs = 0;
            bool                            _hoverStarted = false;
            Float2                          _tooltipPosition;
            ControlID                       _tooltipParentWindowID;
            const wchar_t*                  _tooltipTextFinal = nullptr;
        };
    }
}


#endif // !MINT_GUI_CONTROL_INTERACTION_STATESET

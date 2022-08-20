#pragma once


#ifndef _MINT_GUI_THEME_H_
#define _MINT_GUI_THEME_H_


#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintRenderingBase/Include/GUI/ControlData.h>


namespace mint
{
    namespace Rendering
    {
        namespace GUI
        {
            struct HoverPressColorSet
            {
                HoverPressColorSet() = default;
                HoverPressColorSet(const Color& normal, const Color& hover, const Color& press) : _normalColor{ normal }, _hoveredColor{ hover }, _pressedColor{ press } { __noop; }
            
                const Color& chooseColorByInteractionState(const ControlData::InteractionState interactionState) const
                {
                    const bool isPressing = (interactionState == ControlData::InteractionState::Pressing);
                    const bool isHovering = (interactionState == ControlData::InteractionState::Hovering);
                    return (isPressing ? _pressedColor : (isHovering ? _hoveredColor : _normalColor));
                };

                Color   _normalColor = Color(0.28125f, 0.28125f, 0.28125f);
                Color   _hoveredColor = _normalColor.cloneScaleRGB(1.5f);
                Color   _pressedColor = Color(0.25f, 0.375f, 0.5f);
            };

            struct Theme
            {
                float   _roundnessInPixel = 8.0f;
                float   _windowInnerLineThickness = 1.0f;
                float   _systemButtonRadius = 7.0f;

                Rect    _outerResizingDistances = Rect(2.0f, 2.0f, 2.0f, 2.0f);
                Rect    _innerResizingDistances = Rect(4.0f, 4.0f, 4.0f, 4.0f);

                Rect    _defaultMargin = Rect(4.0f, 4.0f, 4.0f, 4.0f);
                Rect    _defaultPadding = Rect(8.0f, 8.0f, 4.0f, 4.0f);
                Rect    _titleBarPadding = Rect(8.0f, 6.0f, 4.0f, 4.0f);

                Color   _textColor = Color(0.875f, 0.875f, 0.875f);

                HoverPressColorSet  _hoverPressColorSet;
                HoverPressColorSet  _closeButtonColorSet = HoverPressColorSet(Color(1.0f, 0.25f, 0.25f), Color(1.0f, 0.375f, 0.375f), Color(1.0f, 0.5f, 0.5f));
                Color   _defaultLabelBackgroundColor = Color::kTransparent;

                Color   _windowBackgroundColor = _hoverPressColorSet._normalColor.cloneScaleRGB(2.0f / 3.0f).cloneScaleA(0.9375f);
                Color   _windowTitleBarFocusedColor = _hoverPressColorSet._normalColor.cloneScaleRGB(0.5f);
                Color   _windowTitleBarUnfocusedColor = _hoverPressColorSet._normalColor;

                Color   _shipCandidateTitleBarColor = _hoverPressColorSet._pressedColor;
                Color   _shipCandidateBackgroundColor = _shipCandidateTitleBarColor.cloneScaleRGB(1.25f);
            };
        }
    }
}


#endif // !_MINT_GUI_THEME_H_

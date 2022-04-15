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

                Color   _normalColor = Color(0.375f, 0.375f, 0.375f);
                Color   _hoveredColor = Color(0.625f, 0.625f, 0.625f);
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

                Color   _windowBackgroundColor = Color(0.125f, 0.125f, 0.125f, 0.875f);
                Color   _windowTitleBarFocusedColor = _windowBackgroundColor.cloneAddRGB(-0.0625f);
                Color   _windowTitleBarUnfocusedColor = _windowTitleBarFocusedColor.cloneScaleRGB(4.0f);
            };
        }
    }
}


#endif // !_MINT_GUI_THEME_H_

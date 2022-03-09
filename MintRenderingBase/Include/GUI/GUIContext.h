#pragma once


#ifndef _MINT_GUI_CONTEXT_H_
#define _MINT_GUI_CONTEXT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/BitVector.h>
#include <MintContainer/Include/String.h>
#include <MintContainer/Include/HashMap.h>

#include <MintMath/Include/Float2.h>
#include <MintMath/Include/Float4.h>
#include <MintMath/Include/Rect.h>

#include <MintRenderingBase/Include/GUI/ControlData.h>


namespace mint
{
    namespace Rendering
    {
        class GraphicDevice;


#pragma region Controls
        enum class InteractionState
        {
            None,
            Hovering,
            Pressing,
            Clicked,
        };

        struct ControlRenderingDesc
        {
            Float2  _position;
            Float2  _size;

            Rect    _margin;
            float   _borderThickness = 0.0f;
            Rect    _padding;
            const wchar_t*  _text = nullptr;
        };

        struct HoverPressColorSet
        {
            HoverPressColorSet() = default;
            HoverPressColorSet(const Color& normal, const Color& hover, const Color& press) : _normalColor{ normal }, _hoveredColor{ hover }, _pressedColor{ press } { __noop; }
            
            const Color& chooseColorByInteractionState(const InteractionState interactionState) const
            {
                const bool isPressing = interactionState == InteractionState::Pressing;
                const bool isHovering = interactionState == InteractionState::Hovering;
                return (isPressing ? _pressedColor : (isHovering ? _hoveredColor : _normalColor));
            };

            Color   _normalColor = Color(0.375f, 0.375f, 0.375f);
            Color   _hoveredColor = Color(0.625f, 0.625f, 0.625f);
            Color   _pressedColor = Color(0.5f, 0.75f, 1.0f);
        };

        struct Theme
        {
            float   _roundnessInPixel = 8.0f;
            float   _defaultBorderThickness = 0.0f;
            float   _windowInnerLineThickness = 1.0f;

            Rect    _defaultMargin = Rect(2.0f, 2.0f, 4.0f, 4.0f);
            Rect    _defaultPadding = Rect(2.0f, 2.0f, 2.0f, 2.0f);
            Rect    _titleBarPadding = Rect(8.0f, 6.0f, 4.0f, 4.0f);

            HoverPressColorSet  _hoverPressColorSet;
            HoverPressColorSet  _closeButtonColorSet = HoverPressColorSet(Color(1.0f, 0.25f, 0.25f), Color(1.0f, 0.375f, 0.375f), Color(1.0f, 0.5f, 0.5f));
            Color   _defaultLabelBackgroundColor = Color::kTransparent;
            Color   _windowInnerLineColor = Color(0.875f, 0.875f, 0.875f);
            Color   _focusedWindowColor = Color(0.125f, 0.125f, 0.125f);
            Color   _unfocusedWindowColor = _focusedWindowColor * 2.0f;

            Color   _textColor = _windowInnerLineColor;
        };

        struct LabelDesc
        {
            const wchar_t*          _text = nullptr;
            TextRenderDirectionHorz _directionHorz = TextRenderDirectionHorz::Centered;
            TextRenderDirectionVert _directionVert = TextRenderDirectionVert::Centered;

        public:
            void                    setBackgroundColor(const Color& color) { _useThemeColor = false; _color = color; }
            void                    setTextColor(const Color& color) { _useThemeTextColor = false; _textColor = color; }
            const Color&            getBackgroundColor(const Theme& theme) const { return (_useThemeColor ? theme._defaultLabelBackgroundColor : _color); }
            const Color&            getTextColor(const Theme& theme) const { return (_useThemeTextColor ? theme._textColor : _textColor); }

        private:
            bool                    _useThemeColor = true;
            Color                   _color = Color::kTransparent;
            bool                    _useThemeTextColor = true;
            Color                   _textColor;
        };

        struct ButtonDesc
        {
            const wchar_t*      _text = nullptr;
            bool                _isRoundButton = false;

            bool                _customizeColor = false;
            HoverPressColorSet  _customizedColorSet;
        };
#pragma endregion


        class GUIContext final
        {
            // 생성자 때문에 friend 선언
            friend GraphicDevice;

            struct ControlDesc
            {
                ControlRenderingDesc    _controlRenderingDesc;
                InteractionState        _interactionState;
            };

        private:
                                                GUIContext(GraphicDevice& graphicDevice);

        public:
                                                ~GUIContext();

        public:
            void                                initialize();
            Theme&                              accessTheme() { return _theme; }

        public:
            void                                processEvent() noexcept;
            void                                updateScreenSize(const Float2& newScreenSize);
            void                                render() noexcept;

        // Next control's ControlRenderingDesc
        public:
            void                                nextControlPosition(const Float2& position);
            void                                nextControlSize(const Float2& contentSize);

        // make-()
        // begin-() && end-()
        public:
            void                                makeLabel(const FileLine& fileLine, const LabelDesc& labelDesc);
            const bool                          makeButton(const FileLine& fileLine, const ButtonDesc& buttonDesc);

        private:
            void                                makeLabel_render(const ControlDesc& controlDesc, const LabelDesc& labelDesc);
            void                                makeButton_render(const ControlDesc& controlDesc, const ButtonDesc& buttonDesc);

        private:
            ControlData&                        accessControlData(const ControlID& controlID, const ControlType controlType);
            void                                fillControlDesc_controlRenderingDesc(const wchar_t* const text, ControlDesc& controlDesc);
            void                                fillControlDesc_interactionState(ControlDesc& controlDesc) const;

        private:
            void                                drawText(const ControlRenderingDesc& controlRenderingDesc, const Color& color, const FontRenderingOption& fontRenderingOption);
            Float4                              computeShapePosition(const ControlRenderingDesc& controlRenderingDesc) const;
            const float                         computeRoundness(const ControlRenderingDesc& controlRenderingDesc) const;
            const bool                          isMouseCursorInControl(const ControlRenderingDesc& controlRenderingDesc, const Float2& mouseCurosrPosition) const;

        private:
            GraphicDevice&                      _graphicDevice;
            ShapeRendererContext                _rendererContext;
            float                               _fontSize;
            Theme                               _theme;

        private:
            ControlRenderingDesc                _nextControlRenderingDesc;

            // Interaction
            Float2                              _mouseDownPosition;

            // Size, Position 등은 Control 마다 기록되어야 하는 State 이다.
            // Docking 정보도 저장되어야 한다.
            HashMap<ControlID, ControlData>     _controlDataMap;
        };
    }
}


#endif // !_MINT_GUI_CONTEXT_H_

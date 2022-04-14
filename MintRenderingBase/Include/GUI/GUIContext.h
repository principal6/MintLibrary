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

#include <MintPlatform/Include/IWindow.h>


namespace mint
{
    namespace Rendering
    {
        class GraphicDevice;


#pragma region Controls
        struct ControlRenderingDesc
        {
            Rect            _margin;
            Rect            _padding;
            const wchar_t*  _text = nullptr;
        };

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

        struct WindowDesc
        {
            const wchar_t*      _title = nullptr;
            Float2              _initialPosition;
            Float2              _initialSize;
        };
#pragma endregion


        class GUIContext final
        {
            // 생성자 때문에 friend 선언
            friend GraphicDevice;

            struct ControlDesc
            {
                ControlID               _controlID;
                ControlRenderingDesc    _renderingDesc;
            };

            struct NextControlDesc
            {
                bool                    _sameLine = false;
                Float2                  _position;
                Float2                  _size;
                ControlRenderingDesc    _renderingDesc;
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
            void                                nextControlSameLine();
            void                                nextControlPosition(const Float2& position);
            void                                nextControlSize(const Float2& contentSize);
            void                                nextControlMargin(const Rect& margin);
            void                                nextControlPadding(const Rect& padding);

        // make-()
        // begin-() && end-()
        public:
            void                                makeLabel(const FileLine& fileLine, const LabelDesc& labelDesc);
            const bool                          makeButton(const FileLine& fileLine, const ButtonDesc& buttonDesc);
            const bool                          beginWindow(const FileLine& fileLine, const WindowDesc& windowDesc);
            void                                endWindow();

        private:
            void                                makeLabel_render(const ControlDesc& controlDesc, const LabelDesc& labelDesc, const ControlData& controlData);
            void                                makeButton_render(const ControlDesc& controlDesc, const ButtonDesc& buttonDesc, const ControlData& controlData);
            void                                beginWindow_render(const ControlDesc& controlDesc, const ControlData& controlData, const ControlData& parentControlData);
            void                                renderControlCommon(const ControlData& controlData);

        private:
            ControlData&                        accessControlData(const ControlID& controlID) const;
            ControlData&                        accessControlData(const ControlID& controlID, const ControlType controlType);
            ControlData&                        accessStackParentControlData();
            void                                updateControlData(const wchar_t* const text, ControlDesc& controlDesc, ControlData& controlData, ControlData& parentControlData);
            void                                updateControlData_processResizing(const ControlData& controlData, const ControlData& parentControlData);
            void                                updateControlData_processDragging(const ControlData& controlData, const ControlData& parentControlData);
            void                                updateControlData_renderingData(const wchar_t* const text, ControlDesc& controlDesc, ControlData& controlData, ControlData& parentControlData);
            void                                updateControlData_interaction(ControlDesc& controlDesc, ControlData& controlData, ControlData& parentControlData);
            void                                updateControlData_interaction_focusing(ControlData& controlData);
            void                                updateControlData_interaction_resizing(ControlData& controlData);
            void                                updateControlData_interaction_dragging(ControlData& controlData, const ControlData& parentControlData);
            void                                updateControlData_resetNextControlDesc();

        private:
            void                                selectResizingCursorType(const ControlData::ResizingFlags& resizingFlags);

        private:
            void                                drawText(const ControlDesc& controlDesc, const Color& color, const FontRenderingOption& fontRenderingOption);
            void                                drawText(const Float2& position, const Float2& size, const wchar_t* const text, const Color& color, const FontRenderingOption& fontRenderingOption);
            Float4                              computeShapePosition(const ControlDesc& controlDesc) const;
            Float4                              computeShapePosition(const Float2& position, const Float2& size) const;
            const float                         computeRoundness(const ControlDesc& controlDesc) const;

        private:
            GraphicDevice&                      _graphicDevice;
            ShapeRendererContext                _rendererContext;
            float                               _fontSize;
            Theme                               _theme;
            Window::CursorType                  _currentCursor;

        private:
            NextControlDesc                     _nextControlDesc;

            // Interaction
            Float2                              _mousePressedPosition;

            // Size, Position 등은 Control 마다 기록되어야 하는 State 이다.
            // Docking 정보도 저장되어야 한다.
            HashMap<ControlID, ControlData>     _controlDataMap;
            Vector<ControlID>                   _controlStack;
            ControlID                           _rootControlID;

        private:
            class InteractionModule
            {
            public:
                const bool                  isInteracting() const;
                const bool                  isInteracting(const ControlData& controlData) const;
                virtual const bool          begin(const ControlData& controlData, const Float2& mousePressedPosition, const void* const customData) abstract;
                MINT_INLINE virtual void    end() { _controlID.invalidate(); }

            public:
                Float2                      computeRelativeMousePressedPosition() const;
                MINT_INLINE const Float2&   getMousePressedPosition() const { return _mousePressedPosition; }
                MINT_INLINE const Float2&   getInitialControlPosition() const { return _initialControlPosition; }

            protected:
                const bool                  beginInternal(const ControlData& controlData, const Float2& mousePressedPosition);

            protected:
                ControlID                   _controlID;
                Float2                      _mousePressedPosition = Float2::kNan;
                Float2                      _initialControlPosition = Float2::kNan;
            };

            class DraggingModule final : public InteractionModule 
            {
            public:
                virtual const bool  begin(const ControlData& controlData, const Float2& mousePressedPosition, const void* const customData = nullptr) override { return beginInternal(controlData, mousePressedPosition); }
            };

            class ResizingModule final : public InteractionModule
            {
            public:
                static void                         makeOuterAndInenrRects(const ControlData& controlData, const Theme& theme, Rect& outerRect, Rect& innerRect);
                static ControlData::ResizingFlags   makeResizingFlags(const Float2& mousePosition, const ControlData& controlData, const Rect& outerRect, const Rect& innerRect);

            public:
                virtual const bool          begin(const ControlData& controlData, const Float2& mousePressedPosition, const void* const customData) override;

            public:
                MINT_INLINE const Float2&   getInitialControlSize() const { return _initialControlSize; }
                MINT_INLINE const ControlData::ResizingFlags&   getResizingFlags() const { return _resizingFlags; }

            private:
                Float2                      _initialControlSize;
                ControlData::ResizingFlags  _resizingFlags;
            };

            class FocusingModule final : public InteractionModule
            {
            public:
                virtual const bool  begin(const ControlData& controlData, const Float2& mousePressedPosition, const void* const customData = nullptr) override { return beginInternal(controlData, mousePressedPosition); }
            };

            DraggingModule      _draggingModule;
            ResizingModule      _resizingModule;
            FocusingModule      _focusingModule;
        };
    }
}


#endif // !_MINT_GUI_CONTEXT_H_

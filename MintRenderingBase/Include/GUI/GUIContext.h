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
#include <MintRenderingBase/Include/GUI/GUIInteractionModules.h>
#include <MintRenderingBase/Include/GUI/GUITheme.h>

#include <MintPlatform/Include/IWindow.h>


namespace mint
{
    namespace Rendering
    {
        class GraphicDevice;


        namespace GUI
        {
#pragma region Controls
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
                // 생성자가 private 이라서 friend 선언
                friend GraphicDevice;

            private:
                                                GUIContext(GraphicDevice& graphicDevice);

            public:
                                                ~GUIContext();

            public:
                void                            initialize();

            public:
                void                            processEvent() noexcept;
                void                            updateScreenSize(const Float2& newScreenSize);
                void                            render() noexcept;

            // Next control's ControlRenderingDesc
            public:
                void                            nextControlSameLine();
                void                            nextControlPosition(const Float2& position);
                void                            nextControlSize(const Float2& contentSize);
                void                            nextControlMargin(const Rect& margin);
                void                            nextControlPadding(const Rect& padding);

            // Control creation #0 make-()
            public:
                void                            makeLabel(const FileLine& fileLine, const LabelDesc& labelDesc);
                bool                            makeButton(const FileLine& fileLine, const ButtonDesc& buttonDesc);

            // Control creation #1 begin-() && end-()
            public:
                bool                            beginWindow(const FileLine& fileLine, const WindowDesc& windowDesc);
                void                            endWindow();

            // Control rendering
            private:
                void                            makeLabel_render(const LabelDesc& labelDesc, const ControlData& controlData);
                void                            makeButton_render(const ButtonDesc& buttonDesc, const ControlData& controlData);
                void                            beginWindow_render(const ControlData& controlData);

            private:
                ControlData&                    accessControlData(const ControlID& controlID) const;
                ControlData&                    accessControlData(const ControlID& controlID, const ControlType controlType);
                ControlData&                    accessStackParentControlData();
                ControlID                       findAncestorWindowControl(const ControlID& controlID) const;

            private:
                void                            updateControlData(ControlData& controlData);
                void                            updateControlData_processResizing(const ControlData& controlData);
                void                            updateControlData_processDragging(const ControlData& controlData);
                void                            updateControlData_renderingData(ControlData& controlData);
                void                            updateControlData_interaction(ControlData& controlData);
                void                            updateControlData_interaction_focusing(ControlData& controlData);
                void                            updateControlData_interaction_resizing(ControlData& controlData);
                void                            updateControlData_interaction_dragging(ControlData& controlData);
                void                            updateControlData_resetNextControlDesc();

            private:
                void                            selectResizingCursorType(const ControlData::ResizingFlags& resizingFlags);

            // Internal rendering functions
            private:
                void                            drawText(const ControlID& controlID, const Color& color, const FontRenderingOption& fontRenderingOption);
                void                            drawText(const Float2& position, const Float2& size, const wchar_t* const text, const Color& color, const FontRenderingOption& fontRenderingOption);
        
            // Internal rendering helpers
            private:
                Float4                          computeShapePosition(const ControlID& controlID) const;
                Float4                          computeShapePosition(const Float2& position, const Float2& size) const;
                float                           computeRoundness(const ControlID& controlID) const;

            private:
                void                            debugRender_control(const ControlData& controlData);

            public:
                struct DebugSwitch
                {
                    union
                    {
                        uint8       _raw = 0;
                        struct
                        {
                            bool    _renderZoneOverlay : 1;
                            bool    _renderMousePoints : 1;
                            bool    _renderResizingArea : 1;
                        };
                    };
                };
                DebugSwitch                     _debugSwitch;
                Theme                           _theme;

            private:
                GraphicDevice&                  _graphicDevice;
                ShapeRendererContext            _rendererContext;
                float                           _fontSize;
                Window::CursorType              _currentCursor;

            private:
                struct NextControlDesc
                {
                    bool                        _sameLine = false;
                    Float2                      _position;
                    Float2                      _size;
                    Rect                        _margin;
                    Rect                        _padding;
                };
                NextControlDesc                 _nextControlDesc;

            private:
                // Size, Position 등은 Control 마다 기록되어야 하는 State 이다.
                HashMap<ControlID, ControlData> _controlDataMap;
                Vector<ControlID>               _controlStack;
                ControlID                       _rootControlID;
                Vector<ControlID>               _controlIDsOfCurrentFrame;
                bool                            _isInBeginWindow = false;

            private:
                // Interaction
                Float2                          _mousePressedPosition;
                DraggingModule                  _draggingModule;
                ResizingModule                  _resizingModule;
                FocusingModule                  _focusingModule;
            };
        }
    }
}


#endif // !_MINT_GUI_CONTEXT_H_

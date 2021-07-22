#pragma once


#ifndef MINT_GUI_CONTEXT_H
#define MINT_GUI_CONTEXT_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/ShapeFontRendererContext.h>
#include <MintRenderingBase/Include/Gui/GuiCommon.h>
#include <MintRenderingBase/Include/Gui/ControlData.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/IId.h>
#include <MintContainer/Include/HashMap.h>

#include <MintPlatform/Include/IWindow.h>

#include <MintMath/Include/Float2.h>
#include <MintMath/Include/Float4.h>
#include <MintMath/Include/Rect.h>


namespace mint
{
    namespace RenderingBase
    {
        class GraphicDevice;
    }

    namespace Window
    {
        class IWindow;
    }


    namespace Gui
    {
        class GuiContext;

        static constexpr float          kDefaultIntervalX = 5.0f;
        static constexpr float          kDefaultIntervalY = 5.0f;
        static constexpr float          kDefaultRoundnessInPixel = 8.0f;
        static constexpr float          kDefaultFocusedAlpha = 0.875f;
        static constexpr float          kDefaultOutOfFocusAlpha = 0.5f;
        static constexpr float          kDefaultRoundButtonRadius = 7.0f;
        static constexpr float          kFontScaleA = 1.0f;
        static constexpr float          kFontScaleB = 0.875f;
        static constexpr float          kFontScaleC = 0.8125f;
        static constexpr mint::Rect     kWindowInnerPadding = mint::Rect(4.0f);
        static constexpr float          kScrollBarThickness = 8.0f;
        static constexpr mint::Rect     kTitleBarInnerPadding = mint::Rect(12.0f, 6.0f, 6.0f, 6.0f);
        static constexpr mint::Float2   kTitleBarBaseSize = mint::Float2(0.0f, mint::RenderingBase::kDefaultFontSize + kTitleBarInnerPadding.vert());
        static constexpr mint::Float2   kMenuBarBaseSize = mint::Float2(0.0f, mint::RenderingBase::kDefaultFontSize + 8.0f);
        static constexpr float          kMenuBarItemTextSpace = 24.0f;
        static constexpr float          kMenuItemSpaceLeft = 16.0f;
        static constexpr float          kMenuItemSpaceRight = 48.0f;
        static constexpr float          kHalfBorderThickness = 5.0f;
        static constexpr float          kSliderTrackThicknes = 6.0f;
        static constexpr float          kSliderThumbRadius = 8.0f;
        static constexpr float          kDockingInteractionShort = 30.0f;
        static constexpr float          kDockingInteractionLong = 40.0f;
        static constexpr float          kDockingInteractionDisplayBorderThickness = 2.0f;
        static constexpr float          kDockingInteractionOffset = 5.0f;
        static constexpr mint::Float2   kCheckBoxSize = mint::Float2(16.0f, 16.0f);
        static constexpr float          kMouseWheelScrollScale = -8.0f;
        static constexpr float          kTextBoxBackSpaceStride = 48.0f;
        static constexpr uint32         kTextBoxMaxTextLength = 2048;


        enum class TextInputMode
        {
            General,
            NumberOnly,
        };

        struct CommonControlParam
        {
            CommonControlParam() = default;
            CommonControlParam(const mint::Float2& size) : _size{ size } { __noop; }
            mint::Float2        _size = mint::Float2::kZero;
            mint::Float2        _offset = mint::Float2::kZero;
        };

        struct WindowParam
        {
            CommonControlParam  _common                 = CommonControlParam(mint::Float2(180, 100));
            mint::Float2        _position               = mint::Float2(100, 100);
            ScrollBarType       _scrollBarType          = ScrollBarType::None;
            DockingMethod       _initialDockingMethod   = DockingMethod::COUNT;
            mint::Float2        _initialDockingSize     = mint::Float2(160);
        };

        // If no value is set, default values will be used properly
        struct LabelParam
        {
            mint::RenderingBase::Color  _backgroundColor    = mint::RenderingBase::Color::kTransparent;
            mint::RenderingBase::Color  _fontColor          = mint::RenderingBase::Color::kTransparent;
            CommonControlParam          _common             = CommonControlParam(mint::Float2::kZero);
            mint::Float2                _paddingForAutoSize = mint::Float2(24, 12);
            TextAlignmentHorz           _alignmentHorz      = TextAlignmentHorz::Center;
            TextAlignmentVert           _alignmentVert      = TextAlignmentVert::Middle;
        };

        struct SliderParam
        {
            //uint32            _stepCount      = 0; // If stepcount is 0, the value is treated as real number
            //float             _min            = 0.0f;
            //float             _max            = 1.0f;
            //float             _stride         = 0.1f; // Only applies when (_stepCount == 0)
            //bool              _isVertical     = false; // Horizontal if false
            CommonControlParam  _common         = CommonControlParam(mint::Float2(128.0f, 0.0f));
        };
        
        struct TextBoxParam
        {
            CommonControlParam          _common             = CommonControlParam(mint::Float2(128.0f, 0.0f));
            TextAlignmentHorz           _alignmentHorz      = TextAlignmentHorz::Left;
            mint::RenderingBase::Color  _backgroundColor    = mint::RenderingBase::Color::kWhite;
            mint::RenderingBase::Color  _fontColor          = mint::RenderingBase::Color::kBlack;
            float                       _roundnessInPixel   = kDefaultRoundnessInPixel;
            TextInputMode               _textInputMode      = TextInputMode::General;
        };

        struct ListViewParam
        {
            bool        _useScrollBar   = true;
        };

        struct ScrollBarTrackParam
        {
            CommonControlParam  _common             = CommonControlParam(mint::Float2(180.0f, 100.0f));
            mint::Float2        _positionInParent   = mint::Float2(100, 100);
        };


        class TaskWhenMouseUp
        {
        public:
            void                clear() noexcept;
            const bool          isSet() const noexcept;
            void                setUpdateDockDatum(const uint64 controlHashKey) noexcept;
            const uint64        getUpdateDockDatum() const noexcept;

        private:
            uint64              _controlHashKeyForUpdateDockDatum = 0;
        };


        class GuiContext final
        {
            friend mint::RenderingBase::GraphicDevice;

        private:
            enum class NamedColor
            {
                NormalState,
                HoverState,
                PressedState,

                WindowFocused,
                WindowOutOfFocus,
                Dock,
                ShownInDock,
                HighlightColor,

                TitleBarFocused,
                TitleBarOutOfFocus,

                TooltipBackground,

                ScrollBarTrack,
                ScrollBarThumb,

                LightFont,
                DarkFont,
                ShownInDockFont,

                COUNT
            };

        private:
            struct ControlStackData
            {
            public:
                                    ControlStackData() = default;
                                    ControlStackData(const ControlData& controlData);

            public:
                ControlType         _controlType;
                uint64              _hashKey;
            };

            struct NextControlStates
            {
            public:
                                                                NextControlStates();

            public:
                void                                            reset() noexcept;

            public:
                bool                                            _nextSameLine;
                mint::Float2                                    _nextDesiredControlSize;
                bool                                            _nextSizingForced;
                bool                                            _nextControlSizeNonContrainedToParent;
                bool                                            _nextNoInterval;
                bool                                            _nextNoAutoPositioned;
                mint::Float2                                    _nextControlPosition;
                const wchar_t*                                  _nextTooltipText;
            };

            struct MouseStates
            {
            public:
                void                                            resetPerFrame() noexcept;

            public:
                void                                            setPosition(const mint::Float2& position) noexcept;
                void                                            setButtonDownPosition(const mint::Float2& position) noexcept;
                void                                            setButtonUpPosition(const mint::Float2& position) noexcept;
                void                                            setButtonDown() noexcept;
                void                                            setButtonUp() noexcept;

            public:
                const mint::Float2&                             getPosition() const noexcept;
                const mint::Float2&                             getButtonDownPosition() const noexcept;
                const mint::Float2&                             getButtonUpPosition() const noexcept;
                const mint::Float2                              getMouseDragDelta() const noexcept;
                const bool                                      isButtonDown() const noexcept;
                const bool                                      isButtonDownThisFrame() const noexcept;
                const bool                                      isButtonDownUp() const noexcept;
                const bool                                      isCursor(const mint::Window::CursorType cursorType) const noexcept;

            private:
                mint::Float2                                    _mousePosition;
                mint::Float2                                    _mouseDownPosition;
                mint::Float2                                    _mouseUpPosition;
                bool                                            _isButtonDown = false;
                bool                                            _isButtonDownThisFrame = false;
                bool                                            _isButtonDownUp = false;

            public:
                mutable float                                   _mouseWheel;
                mutable mint::Window::CursorType                _cursorType = mint::Window::CursorType::Arrow; // per frame
            };

            struct TextBoxProcessInputResult
            {
                bool    _clearWcharInput = false;
                bool    _clearKeyCode = false;
            };

            class ControlInteractionStates
            {
            public:
                const bool                  setControlPressed(const ControlData& controlData) noexcept;
                MINT_INLINE void            setControlFocused(const uint64 controlHashKey) noexcept { _focusedControlHashKey = controlHashKey; }
                void                        setControlHovered(const uint64 controlHashKey) noexcept;
                const bool                  setControlClicked(const ControlData& controlData) noexcept;
                MINT_INLINE const bool      hasFocusedControl() const noexcept { return (0 != _focusedControlHashKey); }
                MINT_INLINE const bool      hasPressedControl() const noexcept { return (0 != _pressedControlHashKey); }
                MINT_INLINE const bool      hasClickedControl() const noexcept { return (0 != _clickedControlHashKeyPerFrame); }
                const bool                  isControlHovered(const ControlData& controlData) const noexcept;
                const bool                  isControlPressed(const ControlData& controlData) const noexcept;
                const bool                  isControlClicked(const ControlData& controlData) const noexcept;
                const bool                  isControlFocused(const ControlData& controlData) const noexcept;
                MINT_INLINE const uint64    getHoveredControlHashKey() const noexcept { return _hoveredControlHashKey; }
                MINT_INLINE const uint64    getFocusedControlHashKey() const noexcept { return _focusedControlHashKey; }
                MINT_INLINE const uint64    getPressedControlHashKey() const noexcept { return _pressedControlHashKey; }
                MINT_INLINE const uint64    getTooltipParentWindowHashKey() const noexcept { return _tooltipParentWindowHashKey; }
                void                        resetPerFrameStates(const MouseStates& mouseStates) noexcept;
                void                        resetHover() noexcept;
                void                        resetHoverIf(const ControlData& controlData) noexcept;
                void                        resetPressIf(const ControlData& controlData) noexcept;
                const bool                  isHoveringMoreThan(const uint64 durationMs) const noexcept;
                MINT_INLINE void            setMouseInteractionDoneThisFrame() noexcept { _isMouseInteractionDoneThisFrame = true; }
                MINT_INLINE const bool      isMouseInteractionDoneThisFrame() const noexcept { return _isMouseInteractionDoneThisFrame; }
                MINT_INLINE const bool      needToShowTooltip() const noexcept { return (0 != _tooltipParentWindowHashKey); }
                MINT_INLINE const wchar_t*  getTooltipText() const noexcept { return _tooltipTextFinal; }
                const mint::Float2          getTooltipWindowPosition(const ControlData& tooltipParentWindow) const noexcept;
                void                        setTooltipData(const MouseStates& mouseStates, const wchar_t* const tooltipText, const uint64 tooltipParentWindowHashKey) noexcept;

            private:
                bool                        _isMouseInteractionDoneThisFrame = false;
                uint64                      _hoveredControlHashKey = 0;
                uint64                      _focusedControlHashKey = 0;
                uint64                      _pressedControlHashKey = 0;
                mint::Float2                _pressedControlInitialPosition;
                uint64                      _clickedControlHashKeyPerFrame = 0;
                uint64                      _hoverStartTimeMs = 0;
                bool                        _hoverStarted = false;
                mint::Float2                _tooltipPosition;
                uint64                      _tooltipParentWindowHashKey = 0;
                const wchar_t*              _tooltipTextFinal = nullptr;
            };
        
        private:
                                                                GuiContext(mint::RenderingBase::GraphicDevice* const graphicDevice);

        public:
                                                                ~GuiContext();

        public:
            void                                                initialize(const char* const font);
            void                                                updateScreenSize(const mint::Float2& newScreenSize);

        public:
            void                                                processEvent(mint::Window::IWindow* const window) noexcept;

        private:
            const bool                                          shouldInteract(const mint::Float2& screenPosition, const ControlData& controlData) const noexcept;


#pragma region Next-states
        public:
            void                                                nextSameLine();
            void                                                nextControlSize(const mint::Float2& size, const bool force = false);
            void                                                nextNoInterval();
            void                                                nextNoAutoPositioned();
            void                                                nextControlSizeNonContrainedToParent();
            // Only works if NoAutoPositioned!
            void                                                nextControlPosition(const mint::Float2& position);
            void                                                nextTooltip(const wchar_t* const tooltipText);
#pragma endregion


#pragma region Controls
            // 
            // Button Window
            // Tooltip Label
            // ScrollBar Slider
            // Window docking system!!!
            // CheckBox
            // TextBox
            // ListView
            // Menu
            // SpinBox
            // TreeView
            // Group RadioButton
            // ComboBox
            // Splitter

        public:
            void                                                testWindow(VisibleState& inoutVisibleState);
            void                                                testDockedWindow(VisibleState& inoutVisibleState);

        public:
            // [Window | Control with ID]
            // \param title [Used as unique id for windows]
            const bool                                          beginWindow(const wchar_t* const title, const WindowParam& windowParam, VisibleState& inoutVisibleState);
            void                                                endWindow() { endControlInternal(ControlType::Window); }

        private:
            void                                                dockWindowOnceInitially(ControlData& windowControlData, const DockingMethod dockingMethod, const mint::Float2& initialDockingSize);
            void                                                updateWindowPositionByParentWindow(ControlData& windowControlData) noexcept;
            void                                                updateDockingWindowDisplay(ControlData& windowControlData) noexcept;
            const bool                                          needToProcessWindowControl(const ControlData& windowControlData) const noexcept;

        public:
            
            // A simple button control
            // \param text [Text to display on the button]
            // \returns true, if clicked
            const bool                                          beginButton(const wchar_t* const text);
            
            
            void                                                endButton() { endControlInternal(ControlType::Button); }

            // [CheckBox]
            // \return true, if toggle state has changed
            const bool                                          beginCheckBox(const wchar_t* const text, bool* const outIsChecked = nullptr);
            void                                                endCheckBox() { endControlInternal(ControlType::CheckBox); }

        public:
            // [Label]
            void                                                pushLabel(const wchar_t* const name, const wchar_t* const text, const LabelParam& labelParam = LabelParam());

        private:
            mint::Float4                                        calculateLabelTextPosition(const LabelParam& labelParam, const ControlData& labelControlData) const noexcept;
            mint::RenderingBase::FontRenderingOption            getLabelFontRenderingOption(const LabelParam& labelParam, const ControlData& labelControlData) const noexcept;

    #pragma region Controls - Slider
        public:
            // \return true, if value has been changed
            const bool                                          beginSlider(const wchar_t* const name, const SliderParam& sliderParam, float& outValue);
            void                                                endSlider() { endControlInternal(ControlType::Slider); }

        private:
            void                                                drawSliderTrack(const SliderParam& sliderParam, const ControlData& trackControlData, const mint::RenderingBase::Color& trackColor) noexcept;
            void                                                drawSliderThumb(const SliderParam& sliderParam, const ControlData& thumbControlData, const mint::RenderingBase::Color& thumbColor) noexcept;
    #pragma endregion

    #pragma region Controls - TextBox
        public:
            // \param name [Unique name to distinguish control]
            // \param textBoxParam [Various options]
            // \param outText [The content of the textbox]
            // \return true, if the content has changed
            const bool                                          beginTextBox(const wchar_t* const name, const TextBoxParam& textBoxParam, std::wstring& outText);
            void                                                endTextBox() { endControlInternal(ControlType::TextBox); }

        private:
            void                                                textBoxProcessInput(const bool wasControlFocused, const TextInputMode textInputMode, ControlData& controlData, mint::Float4& textRenderOffset, std::wstring& outText) noexcept;
        
        private:
            void                                                textBoxProcessInputMouse(ControlData& controlData, mint::Float4& textRenderOffset, std::wstring& outText, TextBoxProcessInputResult& result);
            void                                                textBoxProcessInputKeyDeleteBefore(ControlData& controlData, std::wstring& outText);
            void                                                textBoxProcessInputKeyDeleteAfter(ControlData& controlData, std::wstring& outText);
            void                                                textBoxProcessInputKeySelectAll(ControlData& controlData, std::wstring& outText);
            void                                                textBoxProcessInputKeyCopy(ControlData& controlData, std::wstring& outText);
            void                                                textBoxProcessInputKeyCut(ControlData& controlData, std::wstring& outText);
            void                                                textBoxProcessInputKeyPaste(const std::wstring& errorMessage, ControlData& controlData, std::wstring& outText);
            void                                                textBoxProcessInputCaretToPrev(ControlData& controlData);
            void                                                textBoxProcessInputCaretToNext(ControlData& controlData, const std::wstring& text);
            void                                                textBoxProcessInputCaretToHead(ControlData& controlData);
            void                                                textBoxProcessInputCaretToTail(ControlData& controlData, const std::wstring& text);
            void                                                textBoxRefreshCaret(const uint64 currentTimeMs, uint16& caretState, uint64& lastCaretBlinkTimeMs) noexcept;
            void                                                textBoxEraseSelection(ControlData& controlData, std::wstring& outText) noexcept;
            const bool                                          textBoxInsertWchar(const wchar_t input, uint16& caretAt, std::wstring& outText);
            const bool                                          textBoxInsertWstring(const std::wstring& input, uint16& caretAt, std::wstring& outText);
            void                                                textBoxUpdateSelection(const uint16 oldCaretAt, const uint16 caretAt, ControlData& controlData);
            const bool                                          textBoxIsValidInput(const wchar_t input, const uint16 caretAt, const TextInputMode textInputMode, const std::wstring& text) noexcept;
        
        private:
            void                                                textBoxUpdateTextDisplayOffset(const uint16 textLength, const float textWidthTillCaret, const float inputCandidateWidth, ControlData& controlData) noexcept;
            void                                                textBoxDrawTextWithInputCandidate(const TextBoxParam& textBoxParam, const mint::Float4& textRenderOffset, ControlData& textBoxControlData, std::wstring& outText) noexcept;
            void                                                textBoxDrawTextWithoutInputCandidate(const TextBoxParam& textBoxParam, const mint::Float4& textRenderOffset, ControlData& textBoxControlData, std::wstring& outText) noexcept;
            void                                                textBoxDrawSelection(const mint::Float4& textRenderOffset, ControlData& textBoxControlData, std::wstring& outText) noexcept;
    #pragma endregion

        public:
            // [ListView]
            const bool                                          beginListView(const wchar_t* const name, int16& outSelectedListItemIndex, const ListViewParam& listViewParam);
            void                                                endListView();

            // [ListItem]
            void                                                pushListItem(const wchar_t* const text);

            // [MenuBar]
            const bool                                          beginMenuBar(const wchar_t* const name);
            void                                                endMenuBar() { endControlInternal(ControlType::MenuBar); }

            // [MenuBarItem]
            const bool                                          beginMenuBarItem(const wchar_t* const text);
            void                                                endMenuBarItem() { endControlInternal(ControlType::MenuBarItem); }

            // [MenuItem]
            const bool                                          beginMenuItem(const wchar_t* const text);
            void                                                endMenuItem() { endControlInternal(ControlType::MenuItem); }


        private:
            // \return Size of titlebar
            mint::Float2                                        beginTitleBar(const wchar_t* const windowTitle, const mint::Float2& titleBarSize, const mint::Rect& innerPadding, VisibleState& inoutParentVisibleState);
            void                                                endTitleBar() { endControlInternal(ControlType::TitleBar); }

            const bool                                          pushRoundButton(const wchar_t* const windowTitle, const mint::RenderingBase::Color& color);

            // [Tooltip]
            // Unique control
            void                                                pushTooltipWindow(const wchar_t* const tooltipText, const mint::Float2& position);

            // [ScrollBar]
            void                                                pushScrollBar(const ScrollBarType scrollBarType);
            void                                                pushScrollBarVert() noexcept;
            void                                                pushScrollBarHorz() noexcept;

            ControlData&                                        pushScrollBarTrack(const ScrollBarType scrollBarType, const ScrollBarTrackParam& scrollBarTrackParam, mint::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext, bool& outHasExtraSize);
            void                                                pushScrollBarThumb(const ScrollBarType scrollBarType, const float visibleLength, const float totalLength, const ControlData& scrollBarTrack, mint::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext);

        private:
            void                                                processDock(const ControlData& controlData, mint::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext);
            void                                                endControlInternal(const ControlType controlType);
            void                                                setClipRectForMe(ControlData& controlData, const mint::Rect& clipRect);
            void                                                setClipRectForDocks(ControlData& controlData, const mint::Rect& clipRect);
            void                                                setClipRectForChildren(ControlData& controlData, const mint::Rect& clipRect);

        private:
            const ControlData&                                  getControlStackTopXXX() const noexcept;
            ControlData&                                        getControlStackTopXXX() noexcept;
            ControlData&                                        getControlData(const uint64 hashKey) noexcept;
            const ControlData&                                  getControlData(const uint64 hashKey) const noexcept;
            mint::Float4                                        getControlCenterPosition(const ControlData& controlData) const noexcept;
            mint::Float2                                        getControlPositionInParentSpace(const ControlData& controlData) const noexcept;
            const wchar_t*                                      generateControlKeyString(const wchar_t* const name, const ControlType controlType) const noexcept;
            const wchar_t*                                      generateControlKeyString(const ControlData& parentControlData, const wchar_t* const name, const ControlType controlType) const noexcept;
            const uint64                                        generateControlHashKeyXXX(const wchar_t* const text, const ControlType controlType) const noexcept;
            ControlData&                                        createOrGetControlData(const wchar_t* const text, const ControlType controlType, const wchar_t* const hashGenerationKeyOverride = nullptr) noexcept;
            const ControlData&                                  getParentWindowControlData() const noexcept;
            const ControlData&                                  getParentWindowControlData(const ControlData& controlData) const noexcept;
            const ControlData&                                  getParentWindowControlDataInternal(const uint64 hashKey) const noexcept;
#pragma endregion


        public:
            const bool                                          isThisControlPressed() const noexcept;
            const bool                                          isFocusedControlTextBox() const noexcept;

        private:
            void                                                setControlFocused(const ControlData& controlData) noexcept;
            void                                                setControlHovered(const ControlData& controlData) noexcept;
            void                                                setControlPressed(const ControlData& controlData) noexcept;
            void                                                setControlClicked(const ControlData& controlData) noexcept;


#pragma region Before drawing controls
        private:
            void                                                prepareControlData(ControlData& controlData, const PrepareControlDataParam& prepareControlDataParam) noexcept;
            void                                                calculateControlChildAt(ControlData& controlData) noexcept;

            const bool                                          processClickControl(ControlData& controlData, const mint::RenderingBase::Color& normalColor, const mint::RenderingBase::Color& hoverColor, const mint::RenderingBase::Color& pressedColor, mint::RenderingBase::Color& outBackgroundColor) noexcept;
            const bool                                          processFocusControl(ControlData& controlData, const mint::RenderingBase::Color& focusedColor, const mint::RenderingBase::Color& nonFocusedColor, mint::RenderingBase::Color& outBackgroundColor) noexcept;
            void                                                processShowOnlyControl(ControlData& controlData, mint::RenderingBase::Color& outBackgroundColor, const bool setMouseInteractionDone = true) noexcept;
            const bool                                          processScrollableControl(ControlData& controlData, const mint::RenderingBase::Color& normalColor, const mint::RenderingBase::Color& dragColor, mint::RenderingBase::Color& outBackgroundColor) noexcept;
            const bool                                          processToggleControl(ControlData& controlData, const mint::RenderingBase::Color& normalColor, const mint::RenderingBase::Color& hoverColor, const mint::RenderingBase::Color& toggledColor, mint::RenderingBase::Color& outBackgroundColor) noexcept;
            
            void                                                processControlInteractionInternal(ControlData& controlData, const bool setMouseInteractionDone = true) noexcept;

            void                                                processControlCommon(ControlData& controlData) noexcept;
            void                                                checkControlResizing(ControlData& controlData) noexcept;
            void                                                checkControlHoveringForTooltip(ControlData& controlData) noexcept;
            void                                                processControlResizingInternal(ControlData& controlData) noexcept;
            void                                                processControlDraggingInternal(ControlData& controlData) noexcept;
            void                                                processControlDockingInternal(ControlData& controlData) noexcept;

            void                                                dock(const uint64 dockedControlHashKey, const uint64 dockControlHashKey) noexcept;
            void                                                undock(const uint64 dockedControlHashKey) noexcept;
            void                                                updateDockDatum(const uint64 dockControlHashKey, const bool dontUpdateWidthArray = false) noexcept;

            const bool                                          isInteractingInternal(const ControlData& controlData) const noexcept;
            
            // These functions must be called after process- functions
            const bool                                          isControlBeingDragged(const ControlData& controlData) const noexcept;
            const bool                                          isControlBeingResized(const ControlData& controlData) const noexcept;

            // RendererContext 고를 때 사용
            const bool                                          isAncestorControlFocused(const ControlData& controlData) const noexcept;
            const bool                                          isAncestorControlPressed(const ControlData& controlData) const noexcept;
            const bool                                          isAncestorControlTargetRecursiveXXX(const uint64 hashKey, const uint64 targetHashKey) const noexcept;
            const bool                                          isAncestorControlFocusedInclusiveXXX(const ControlData& controlData) const noexcept;

            const bool                                          isAncestorControlInclusive(const ControlData& controlData, const uint64 ancestorCandidateHashKey) const noexcept;
            const bool                                          isAncestorControlRecursiveXXX(const uint64 currentControlHashKey, const uint64 ancestorCandidateHashKey) const noexcept;
            const bool                                          isDescendantControlInclusive(const ControlData& controlData, const uint64 descendantCandidateHashKey) const noexcept;
            const bool                                          isDescendantControlRecursiveXXX(const uint64 currentControlHashKey, const uint64 descendantCandidateHashKey) const noexcept;

            const bool                                          isParentControlRoot(const ControlData& controlData) const noexcept;

            // Focus, Out-of-focus 색 정할 때 사용
            const bool                                          needToColorFocused(const ControlData& controlData) const noexcept;
            const bool                                          isDescendantControlFocusedInclusive(const ControlData& controlData) const noexcept;
            const bool                                          isDescendantControlHoveredInclusive(const ControlData& controlData) const noexcept;
            const bool                                          isDescendantControlPressedInclusive(const ControlData& controlData) const noexcept;
            const bool                                          isDescendantControlPressed(const ControlData& controlData) const noexcept;
            const bool                                          isDescendantControlHovered(const ControlData& controlData) const noexcept;
            const ControlData&                                  getClosestFocusableAncestorControlInclusive(const ControlData& controlData) const noexcept;
            const bool                                          hasDockingAncestorControlInclusive(const ControlData& controlData) const noexcept;

            const mint::RenderingBase::Color&                   getNamedColor(const NamedColor namedColor) const noexcept;
            void                                                setNamedColor(const NamedColor namedColor, const mint::RenderingBase::Color& color) noexcept;

            const float                                         getMouseWheelScroll(const ControlData& scrollParentControlData) const noexcept;
            const float                                         calculateTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept;
            const uint32                                        calculateIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept;
#pragma endregion

        private:
            mint::RenderingBase::ShapeFontRendererContext&      getRendererContext(const ControlData& controlData) noexcept;
            mint::RenderingBase::ShapeFontRendererContext&      getRendererContext(const RendererContextLayer rendererContextLayer) noexcept;
            // TopMost 는 제외!!
            const RendererContextLayer                          getUpperRendererContextLayer(const ControlData& controlData) noexcept;
            void                                                render();
            void                                                resetPerFrameStates();

        private:
            mint::RenderingBase::GraphicDevice* const           _graphicDevice;

        private: // these are set externally
            float                                               _fontSize;
            uint32                                              _caretBlinkIntervalMs;
            mint::RenderingBase::ShapeFontRendererContext       _rendererContexts[getRendererContextLayerCount()];

        private: // screen size
            int8                                                _updateScreenSizeCounter;
            mint::Rect                                          _clipRectFullScreen;

        private:
            ControlData                                         _rootControlData;
            mint::Vector<ControlStackData>                      _controlStackPerFrame;

        private:
            mutable ControlInteractionStates                    _controlInteractionStates;

#pragma region Mouse Capture States
        private:
            mutable bool                                        _isDragBegun;
            mutable uint64                                      _draggedControlHashKey;
            mutable mint::Float2                                _draggedControlInitialPosition;
        
        private:
            mutable bool                                        _isResizeBegun;
            mutable uint64                                      _resizedControlHashKey;
            mutable mint::Float2                                _resizedControlInitialPosition;
            mutable mint::Float2                                _resizedControlInitialDisplaySize;
            mutable ResizingMethod                              _resizingMethod;
#pragma endregion
        
        private:
            mint::HashMap<uint64, ControlData>                  _controlIdMap;

        private:
            NextControlStates                                   _nextControlStates;
            MouseStates                                         _mouseStates;

#pragma region Key Character Input
        private:
            wchar_t                                             _wcharInput;
            wchar_t                                             _wcharInputCandiate;
            mint::Window::EventData::KeyCode                    _keyCode;
#pragma endregion

        private:
            TaskWhenMouseUp                                     _taskWhenMouseUp;

        private:
            mint::RenderingBase::Color                          _namedColors[static_cast<uint32>(NamedColor::COUNT)];
        };
    }
}


#include <MintRenderingBase/Include/Gui/GuiContext.inl>


#endif // !MINT_GUI_CONTEXT_H

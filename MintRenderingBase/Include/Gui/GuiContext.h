#pragma once


#ifndef MINT_GUI_CONTEXT_H
#define MINT_GUI_CONTEXT_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/String.h>
#include <MintContainer/Include/IId.h>
#include <MintContainer/Include/HashMap.h>

#include <MintMath/Include/Float2.h>
#include <MintMath/Include/Float4.h>
#include <MintMath/Include/Rect.h>

#include <MintPlatform/Include/IWindow.h>

#include <MintRenderingBase/Include/ShapeFontRendererContext.h>
#include <MintRenderingBase/Include/Gui/GuiCommon.h>
#include <MintRenderingBase/Include/Gui/ControlData.h>
#include <MintRenderingBase/Include/Gui/InputHelpers.h>


namespace mint
{
    namespace Rendering
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
        static constexpr Rect           kWindowInnerPadding = Rect(4.0f);
        static constexpr float          kScrollBarThickness = 8.0f;
        static constexpr Rect           kTitleBarInnerPadding = Rect(12.0f, 6.0f, 6.0f, 6.0f);
        static constexpr float          kTitleBarBaseThickness = Rendering::kDefaultFontSize + kTitleBarInnerPadding.vert();
        static constexpr Float2         kMenuBarBaseSize = Float2(0.0f, Rendering::kDefaultFontSize + 8.0f);
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
        static constexpr Float2         kCheckBoxSize = Float2(16.0f, 16.0f);
        static constexpr float          kMouseWheelScrollScale = -8.0f;
        static constexpr float          kTextBoxBackSpaceStride = 48.0f;
        static constexpr uint32         kTextBoxMaxTextLength = 2048;


        struct WindowParam
        {
            CommonControlParam  _common                 = CommonControlParam(Float2(180, 100));
            Float2              _position               = Float2(100, 100);
            ScrollBarType       _scrollBarType          = ScrollBarType::None;
            DockingMethod       _initialDockingMethod   = DockingMethod::COUNT;
            Float2              _initialDockingSize     = Float2(160);
        };

        // If no value is set, default values will be used properly
        struct LabelParam
        {
            CommonControlParam  _common                 = CommonControlParam(Float2::kNegativeOne, Rendering::Color::kTransparent, Rendering::Color::kTransparent);
            Float2              _paddingForAutoSize     = Float2(24, 12);
            TextAlignmentHorz   _alignmentHorz          = TextAlignmentHorz::Left;
            TextAlignmentVert   _alignmentVert          = TextAlignmentVert::Middle;
        };

        struct SliderParam
        {
            //uint32            _stepCount              = 0; // If stepcount is 0, the value is treated as real number
            //float             _min                    = 0.0f;
            //float             _max                    = 1.0f;
            //float             _stride                 = 0.1f; // Only applies when (_stepCount == 0)
            //bool              _isVertical             = false; // Horizontal if false
            CommonControlParam  _common                 = CommonControlParam(Float2(128.0f, 0.0f));
        };
        
        struct TextBoxParam
        {
            CommonControlParam  _common                 = CommonControlParam(Float2(128.0f, 0.0f));
            TextAlignmentHorz   _alignmentHorz          = TextAlignmentHorz::Left;
            float               _roundnessInPixel       = kDefaultRoundnessInPixel;
            TextInputMode       _textInputMode          = TextInputMode::General;
        };

        struct ListViewParam
        {
            bool                _useScrollBar           = true;
        };

        struct ScrollBarTrackParam
        {
            CommonControlParam  _common                 = CommonControlParam(Float2(180.0f, 100.0f));
            Float2              _positionInParent       = Float2(100, 100);
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
            friend Rendering::GraphicDevice;

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
                void                reset() noexcept;

            public:
                bool                _nextSameLine;
                Float2              _nextDesiredControlSize;
                bool                _nextSizingForced;
                bool                _nextControlSizeNonContrainedToParent;
                bool                _nextNoInterval;
                bool                _nextAutoPositionOff;
                Float2              _nextControlPosition;
                const wchar_t*      _nextTooltipText;
            };


            class ControlInteractionStates
            {
            public:
                void                        setControlHovered(const ControlData& controlData) noexcept;
                const bool                  setControlPressed(const ControlData& controlData) noexcept;
                const bool                  setControlClicked(const ControlData& controlData) noexcept;
                void                        setControlFocused(const ControlData& controlData) noexcept;
            
            public:
                MINT_INLINE const bool      hasPressedControl() const noexcept { return (0 != _pressedControlHashKey); }
                MINT_INLINE const bool      hasClickedControl() const noexcept { return (0 != _clickedControlHashKeyPerFrame); }
                MINT_INLINE const bool      hasFocusedControl() const noexcept { return (0 != _focusedControlHashKey); }
            
            public:
                const bool                  isControlHovered(const ControlData& controlData) const noexcept;
                const bool                  isControlPressed(const ControlData& controlData) const noexcept;
                const bool                  isControlClicked(const ControlData& controlData) const noexcept;
                const bool                  isControlFocused(const ControlData& controlData) const noexcept;
                const bool                  isHoveringMoreThan(const uint64 durationMs) const noexcept;
            
            public:
                MINT_INLINE const uint64    getHoveredControlHashKey() const noexcept { return _hoveredControlHashKey; }
                MINT_INLINE const uint64    getPressedControlHashKey() const noexcept { return _pressedControlHashKey; }
                MINT_INLINE const uint64    getFocusedControlHashKey() const noexcept { return _focusedControlHashKey; }

            public:
                void                        resetPerFrameStates(const MouseStates& mouseStates) noexcept;
                void                        resetHover() noexcept;
                void                        resetHoverIf(const ControlData& controlData) noexcept;
                void                        resetPressIf(const ControlData& controlData) noexcept;
            
            public:
                MINT_INLINE void            setMouseInteractionDoneThisFrame() noexcept { _isMouseInteractionDoneThisFrame = true; }
                MINT_INLINE const bool      isMouseInteractionDoneThisFrame() const noexcept { return _isMouseInteractionDoneThisFrame; }

            public:
                MINT_INLINE const bool      needToShowTooltip() const noexcept { return (0 != _tooltipParentWindowHashKey); }
                MINT_INLINE const wchar_t*  getTooltipText() const noexcept { return _tooltipTextFinal; }
                const Float2                getTooltipWindowPosition(const ControlData& tooltipParentWindow) const noexcept;
                MINT_INLINE const uint64    getTooltipParentWindowHashKey() const noexcept { return _tooltipParentWindowHashKey; }
                void                        setTooltipData(const MouseStates& mouseStates, const wchar_t* const tooltipText, const uint64 tooltipParentWindowHashKey) noexcept;

            private:
                bool                        _isMouseInteractionDoneThisFrame = false;
                uint64                      _hoveredControlHashKey = 0;
                uint64                      _focusedControlHashKey = 0;
                uint64                      _pressedControlHashKey = 0;
                Float2                      _pressedControlInitialPosition;
                uint64                      _clickedControlHashKeyPerFrame = 0;
                uint64                      _hoverStartTimeMs = 0;
                bool                        _hoverStarted = false;
                Float2                      _tooltipPosition;
                uint64                      _tooltipParentWindowHashKey = 0;
                const wchar_t*              _tooltipTextFinal = nullptr;
            };
        
        private:
                                                        GuiContext(Rendering::GraphicDevice* const graphicDevice);

        public:
                                                        ~GuiContext();

        public:
            void                                        initialize(const char* const font);
            void                                        updateScreenSize(const Float2& newScreenSize);

        public:
            void                                        processEvent(Window::IWindow* const window) noexcept;

        private:
            const bool                                  shouldInteract(const Float2& screenPosition, const ControlData& controlData) const noexcept;


#pragma region Next-states
        public:
            void                                        nextSameLine();
            void                                        nextControlSize(const Float2& size, const bool force = false);
            void                                        nextNoInterval();
            void                                        nextAutoPositionOff();
            void                                        nextControlSizeNonContrainedToParent();
            // Only works if NoAutoPositioned!
            void                                        nextControlPosition(const Float2& position);
            void                                        nextTooltip(const wchar_t* const tooltipText);
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

    #pragma region Controls - Presets
        public:
            void                                        makeTestWindow(VisibleState& inoutVisibleState);
            void                                        makeTestDockedWindow(VisibleState& inoutVisibleState);
            void                                        makeDebugControlDataViewer(VisibleState& inoutVisibleState);
            void                                        makeFromReflectionClass(const ReflectionData& reflectionData, const void* const reflectionClass);
    #pragma endregion

    #pragma region Controls - Window
        public:
            // [Window | Control with ID]
            // \param title [Used as unique id for windows]
            const bool                                  beginWindow(const wchar_t* const title, const WindowParam& windowParam, VisibleState& inoutVisibleState);
            void                                        endWindow() { endControlInternal(ControlType::Window); }

        private:
            void                                        dockWindowOnceInitially(ControlData& windowControlData, const DockingMethod dockingMethod, const Float2& initialDockingSize);
            void                                        updateWindowPositionByParentWindow(ControlData& windowControlData) noexcept;
            void                                        updateDockingWindowDisplay(ControlData& windowControlData) noexcept;
            const bool                                  needToProcessWindowControl(const ControlData& windowControlData) const noexcept;
    #pragma endregion

    #pragma region Controls - Button
        public:
            // A simple button control
            // \param text [Text to display on the button]
            // \returns true, if clicked
            const bool                                  beginButton(const wchar_t* const text);
            void                                        endButton() { endControlInternal(ControlType::Button); }
    #pragma endregion

    #pragma region Controls - CheckBox
        public:
            // [CheckBox]
            // \return true, if toggle state has changed
            const bool                                  beginCheckBox(const wchar_t* const text, bool* const outIsChecked = nullptr);
            void                                        endCheckBox() { endControlInternal(ControlType::CheckBox); }
    #pragma endregion

    #pragma region Controls - Label
        public:
            void                                        makeLabel(const wchar_t* const text, const LabelParam& labelParam = LabelParam());
            void                                        makeLabel(const wchar_t* const name, const wchar_t* const text, const LabelParam& labelParam = LabelParam());

        private:
            Float4                                      labelCalculateTextPosition(const LabelParam& labelParam, const ControlData& labelControlData) const noexcept;
            Rendering::FontRenderingOption              labelGetFontRenderingOption(const LabelParam& labelParam, const ControlData& labelControlData) const noexcept;
    #pragma endregion

    #pragma region Controls - Slider
        public:
            // \return true, if value has been changed
            const bool                                  beginSlider(const wchar_t* const name, const SliderParam& sliderParam, float& outValue);
            void                                        endSlider() { endControlInternal(ControlType::Slider); }

        private:
            void                                        sliderDrawTrack(const SliderParam& sliderParam, const ControlData& trackControlData, const Rendering::Color& trackColor) noexcept;
            void                                        sliderDrawThumb(const SliderParam& sliderParam, const ControlData& thumbControlData, const Rendering::Color& thumbColor) noexcept;
    #pragma endregion

    #pragma region Controls - TextBox
        public:
            // \param name [Unique name to distinguish control]
            // \param textBoxParam [Various options]
            // \param outText [The content of the textbox]
            // \return true, if the content has changed
            const bool                                  beginTextBox(const wchar_t* const name, const TextBoxParam& textBoxParam, StringW& outText);
            void                                        endTextBox() { endControlInternal(ControlType::TextBox); }

        private:
            void                                        textBoxProcessInput(const bool wasControlFocused, const TextInputMode textInputMode, ControlData& controlData, Float4& textRenderOffset, StringW& outText) noexcept;
    #pragma endregion

    #pragma region Controls - ValueSlider
        public:
            const bool                                  beginValueSlider(const wchar_t* const name, const CommonControlParam& commonControlParam, const float roundnessInPixel, const int32 decimalDigits, float& value);
            void                                        endValueSlider() { endControlInternal(ControlType::ValueSlider); }
            
            const bool                                  beginLabeledValueSlider(const wchar_t* const name, const wchar_t* const labelText, const LabelParam& labelParam, const CommonControlParam& commonControlParam, const float roundnessInPixel, const int32 decimalDigits, float& value);
            void                                        endLabeledValueSlider() { endControlInternal(ControlType::ValueSlider); }

        private:
            void                                        valueSliderProcessInput(const bool wasControlFocused, ControlData& controlData, Float4& textRenderOffset, float& value, StringW& outText) noexcept;
    #pragma endregion

    #pragma region Controls - List (ListView, ListItem)
        public:
            const bool                                  beginListView(const wchar_t* const name, int16& outSelectedListItemIndex, const ListViewParam& listViewParam);
            void                                        endListView();

            void                                        makeListItem(const wchar_t* const text);
    #pragma endregion

    #pragma region Controls - Menu (MenuBar, MenuBarItem, MenuItem)
        public:
            const bool                                  beginMenuBar(const wchar_t* const name);
            void                                        endMenuBar() { endControlInternal(ControlType::MenuBar); }

            const bool                                  beginMenuBarItem(const wchar_t* const text);
            void                                        endMenuBarItem() { endControlInternal(ControlType::MenuBarItem); }

            const bool                                  beginMenuItem(const wchar_t* const text);
            void                                        endMenuItem() { endControlInternal(ControlType::MenuItem); }
    #pragma endregion

    #pragma region Controls - Internal use
        private:
            // \return Size of titlebar
            Float2                                      beginTitleBar(const wchar_t* const windowTitle, const Float2& titleBarSize, const Rect& innerPadding, VisibleState& inoutParentVisibleState);
            void                                        endTitleBar() { endControlInternal(ControlType::TitleBar); }

            const bool                                  makeRoundButton(const wchar_t* const windowTitle, const Rendering::Color& color);

            // [Tooltip]
            // Unique control
            void                                        makeTooltipWindow(const wchar_t* const tooltipText, const Float2& position);

            // [ScrollBar]
            void                                        makeScrollBar(const ScrollBarType scrollBarType);
            void                                        makeScrollBarVert() noexcept;
            void                                        makeScrollBarHorz() noexcept;

            ControlData&                                makeScrollBarTrack(const ScrollBarType scrollBarType, const ScrollBarTrackParam& scrollBarTrackParam, Rendering::ShapeFontRendererContext& shapeFontRendererContext, bool& outHasExtraSize);
            void                                        makeScrollBarThumb(const ScrollBarType scrollBarType, const float visibleLength, const float totalLength, const ControlData& scrollBarTrack, Rendering::ShapeFontRendererContext& shapeFontRendererContext);
    #pragma endregion

        private:
            void                                        processDock(const ControlData& controlData, Rendering::ShapeFontRendererContext& shapeFontRendererContext);
            void                                        endControlInternal(const ControlType controlType);
            void                                        setClipRectForMe(ControlData& controlData, const Rect& clipRect);
            void                                        setClipRectForDocks(ControlData& controlData, const Rect& clipRect);
            void                                        setClipRectForChildren(ControlData& controlData, const Rect& clipRect);

        public:
            const float                                 getCurrentAvailableDisplaySizeX() const noexcept;
            const float                                 getCurrentSameLineIntervalX() const noexcept;

        private:
            const bool                                  isValidControlDataHashKey(const uint64 hashKey) const noexcept;
            const ControlData&                          getControlStackTopXXX() const noexcept;
            ControlData&                                getControlStackTopXXX() noexcept;
            ControlData&                                getControlData(const uint64 hashKey) noexcept;
            const ControlData&                          getControlData(const uint64 hashKey) const noexcept;
            Float2                                      getControlPositionInParentSpace(const ControlData& controlData) const noexcept;
            const wchar_t*                              generateControlKeyString(const wchar_t* const name, const ControlType controlType) const noexcept;
            const wchar_t*                              generateControlKeyString(const ControlData& parentControlData, const wchar_t* const name, const ControlType controlType) const noexcept;
            const uint64                                generateControlHashKeyXXX(const wchar_t* const text, const ControlType controlType) const noexcept;
            ControlData&                                createOrGetControlData(const wchar_t* const text, const ControlType controlType, const wchar_t* const hashGenerationKeyOverride = nullptr) noexcept;
            const ControlData&                          getParentWindowControlData() const noexcept;
            const ControlData&                          getParentWindowControlData(const ControlData& controlData) const noexcept;
            const ControlData&                          getParentWindowControlDataInternal(const uint64 hashKey) const noexcept;
#pragma endregion


        public:
            const bool                                  isThisControlPressed() const noexcept;
            const bool                                  isFocusedControlInputBox() const noexcept;

        private:
            void                                        setControlFocused(const ControlData& controlData) noexcept;
            void                                        setControlHovered(const ControlData& controlData) noexcept;
            void                                        setControlPressed(const ControlData& controlData) noexcept;
            void                                        setControlClicked(const ControlData& controlData) noexcept;


#pragma region Before drawing controls
        private:
            void                                        prepareControlData(ControlData& controlData, const PrepareControlDataParam& prepareControlDataParam) noexcept;
            void                                        calculateControlChildAt(ControlData& controlData) noexcept;

            const bool                                  processClickControl(ControlData& controlData, const Rendering::Color& normalColor, const Rendering::Color& hoverColor, const Rendering::Color& pressedColor, Rendering::Color& outBackgroundColor) noexcept;
            const bool                                  processFocusControl(ControlData& controlData, const Rendering::Color& focusedColor, const Rendering::Color& nonFocusedColor, Rendering::Color& outBackgroundColor) noexcept;
            void                                        processShowOnlyControl(ControlData& controlData, Rendering::Color& outBackgroundColor, const bool setMouseInteractionDone = true) noexcept;
            const bool                                  processScrollableControl(ControlData& controlData, const Rendering::Color& normalColor, const Rendering::Color& dragColor, Rendering::Color& outBackgroundColor) noexcept;
            const bool                                  processToggleControl(ControlData& controlData, const Rendering::Color& normalColor, const Rendering::Color& hoverColor, const Rendering::Color& toggledColor, Rendering::Color& outBackgroundColor) noexcept;
    
            void                                        processControlInteractionInternal(ControlData& controlData, const bool setMouseInteractionDone = true) noexcept;

            void                                        processControlCommon(ControlData& controlData) noexcept;
            void                                        checkControlResizing(ControlData& controlData) noexcept;
            void                                        checkControlHoveringForTooltip(ControlData& controlData) noexcept;
            void                                        processControlResizingInternal(ControlData& controlData) noexcept;
            void                                        processControlDraggingInternal(ControlData& controlData) noexcept;
            void                                        processControlDockingInternal(ControlData& controlData) noexcept;

            void                                        dock(const uint64 dockedControlHashKey, const uint64 dockControlHashKey) noexcept;
            void                                        undock(const uint64 dockedControlHashKey) noexcept;
            void                                        updateDockDatum(const uint64 dockControlHashKey, const bool dontUpdateWidthArray = false) noexcept;

            const bool                                  isInteractingInternal(const ControlData& controlData) const noexcept;
            
            // These functions must be called after process- functions
            const bool                                  isControlBeingDragged(const ControlData& controlData) const noexcept;
            const bool                                  isControlBeingResized(const ControlData& controlData) const noexcept;

            // RendererContext 고를 때 사용
            const bool                                  isAncestorControlFocused(const ControlData& controlData) const noexcept;
            const bool                                  isAncestorControlPressed(const ControlData& controlData) const noexcept;
            const bool                                  isAncestorControlTargetRecursiveXXX(const uint64 hashKey, const uint64 targetHashKey) const noexcept;
            const bool                                  isAncestorControlFocusedInclusiveXXX(const ControlData& controlData) const noexcept;

            const bool                                  isAncestorControlInclusive(const ControlData& controlData, const uint64 ancestorCandidateHashKey) const noexcept;
            const bool                                  isAncestorControlRecursiveXXX(const uint64 currentControlHashKey, const uint64 ancestorCandidateHashKey) const noexcept;
            const bool                                  isDescendantControlInclusive(const ControlData& controlData, const uint64 descendantCandidateHashKey) const noexcept;
            const bool                                  isDescendantControlRecursiveXXX(const uint64 currentControlHashKey, const uint64 descendantCandidateHashKey) const noexcept;

            const bool                                  isParentControlRoot(const ControlData& controlData) const noexcept;

            // Focus, Out-of-focus 색 정할 때 사용
            const bool                                  needToColorFocused(const ControlData& controlData) const noexcept;
            const bool                                  isDescendantControlFocusedInclusive(const ControlData& controlData) const noexcept;
            const bool                                  isDescendantControlHoveredInclusive(const ControlData& controlData) const noexcept;
            const bool                                  isDescendantControlPressedInclusive(const ControlData& controlData) const noexcept;
            const bool                                  isDescendantControlPressed(const ControlData& controlData) const noexcept;
            const bool                                  isDescendantControlHovered(const ControlData& controlData) const noexcept;
            const ControlData&                          getClosestFocusableAncestorControlInclusive(const ControlData& controlData) const noexcept;
            const bool                                  hasDockingAncestorControlInclusive(const ControlData& controlData) const noexcept;

            const Rendering::Color&                     getNamedColor(const NamedColor namedColor) const noexcept;
            void                                        setNamedColor(const NamedColor namedColor, const Rendering::Color& color) noexcept;

            const float                                 getMouseWheelScroll(const ControlData& scrollParentControlData) const noexcept;
            const float                                 calculateTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept;
            const uint32                                calculateIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept;
#pragma endregion

        private:
            Rendering::ShapeFontRendererContext&        getRendererContext(const ControlData& controlData) noexcept;
            Rendering::ShapeFontRendererContext&        getRendererContext(const RendererContextLayer rendererContextLayer) noexcept;
            // TopMost 는 제외!!
            const RendererContextLayer                  getUpperRendererContextLayer(const ControlData& controlData) noexcept;
            void                                        render();
            void                                        resetPerFrameStates();

        private:
            Rendering::GraphicDevice* const             _graphicDevice;

        private: // these are set externally
            float                                       _fontSize;
            uint32                                      _caretBlinkIntervalMs;
            Rendering::ShapeFontRendererContext         _rendererContexts[getRendererContextLayerCount()];

        private: // screen size
            int8                                        _updateScreenSizeCounter;
            Rect                                        _clipRectFullScreen;

        private:
            Rendering::Color                            _namedColors[static_cast<uint32>(NamedColor::COUNT)];

        private:
            ControlData                                 _rootControlData;
            Vector<ControlStackData>                    _controlStackPerFrame;
            uint64                                      _viewerTargetControlDataHashKey;

        private:
            HashMap<uint64, ControlData>                _controlIdMap;

        private:
            mutable ControlInteractionStates            _controlInteractionStates;
            NextControlStates                           _nextControlStates;

#pragma region Mouse
        private:
            MouseStates                                 _mouseStates;
        
        private:
            mutable bool                                _isDragBegun;
            mutable uint64                              _draggedControlHashKey;
            mutable Float2                              _draggedControlInitialPosition;
        
        private:
            mutable bool                                _isResizeBegun;
            mutable uint64                              _resizedControlHashKey;
            mutable Float2                              _resizedControlInitialPosition;
            mutable Float2                              _resizedControlInitialDisplaySize;
            mutable ResizingMethod                      _resizingMethod;

        private:
            TaskWhenMouseUp                             _taskWhenMouseUp;
#pragma endregion
        
#pragma region Keyboard
        private:
            wchar_t                                     _wcharInput;
            wchar_t                                     _wcharInputCandidate;
            Platform::KeyCode                           _keyCode;
#pragma endregion
        };
    }
}


#include <MintRenderingBase/Include/Gui/GuiContext.inl>


#endif // !MINT_GUI_CONTEXT_H

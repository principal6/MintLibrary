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

#include <MintRenderingBase/Include/GUI/GUICommon.h>
#include <MintRenderingBase/Include/GUI/ControlData.h>
#include <MintRenderingBase/Include/GUI/InputHelpers.h>
#include <MintRenderingBase/Include/GUI/ControlMetaStateSet.h>
#include <MintRenderingBase/Include/GUI/ControlInteractionStateSet.h>


namespace mint
{
    // Use MINT_FILE_LINE macro!
    struct FileLine
    {
        explicit FileLine(const char* const file, const int line) : _file{ file }, _line{ line } { __noop; }
        const char* const _file;
        const int         _line;
    };
    #define MINT_FILE_LINE() FileLine(__FILE__, __LINE__)


    namespace Rendering
    {
        class GraphicDevice;
    }


    namespace Window
    {
        class IWindow;
    }


    namespace GUI
    {
        class GUIContext;

        
        struct WindowParam
        {
            CommonControlParam  _common;
            Float2              _position               = Float2(100, 100);
            ScrollBarType       _scrollBarType          = ScrollBarType::None;
            DockZone            _initialDockZone        = DockZone::COUNT;
            Float2              _initialDockingSize     = Float2(160);
        };

        // If no value is set, default values will be used properly
        struct LabelParam
        {
            CommonControlParam  _common                 = CommonControlParam(Rendering::Color::kTransparent, Rendering::Color::kTransparent);
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
            CommonControlParam  _common;
        };
        
        struct TextBoxParam
        {
            CommonControlParam  _common;
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
            CommonControlParam  _common;
            Float2              _positionInParent       = Float2(100, 100);
        };


        class TaskWhenMouseUp
        {
        public:
            void                clear() noexcept;
            const bool          isSet() const noexcept;
            void                setUpdateDockZoneData(const ControlID& controlID) noexcept;
            const ControlID&    getUpdateDockZoneData() const noexcept;

        private:
            ControlID           _controlIDForUpdateDockZoneData;
        };


        // Dock ============================
        // ||           | Docked Control  ||
        // ||           |                 ||
        // ||           |                 ||
        // =================================

        class GUIContext final
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
                                    ControlStackData(const ControlType controlType, const ControlID controlID);

            public:
                ControlType         _controlType;
                ControlID           _id;
            };

        private:
                                                        GUIContext(Rendering::GraphicDevice& graphicDevice);

        public:
                                                        ~GUIContext();

        public:
            void                                        initialize();
            void                                        updateScreenSize(const Float2& newScreenSize);

        public:
            void                                        processEvent() noexcept;

        private:
            const bool                                  shouldInteract(const Float2& screenPosition, const ControlData& controlData) const noexcept;


#pragma region ControlMetaStateSet
        public:
            MINT_INLINE ControlMetaStateSet&            getControlMetaStateSet() noexcept { return _controlMetaStateSet; }
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
            void                                        makeFromReflectionClass(const FileLine& fileLine, const ReflectionData& reflectionData, const void* const reflectionClass);
    #pragma endregion

    #pragma region Controls - Window
        public:
            // [Window | Control with ID]
            // \param title [Used as unique id for windows]
            const bool                                  beginWindow(const FileLine& fileLine, const wchar_t* const title, const WindowParam& windowParam, VisibleState& inoutVisibleState);
            void                                        endWindow() { endControlInternal(ControlType::Window); }

        private:
            void                                        windowDockInitially(ControlData& windowControlData, const DockZone dockZone, const Float2& initialDockingSize);
            void                                        windowUpdatePositionByParentWindow(ControlData& windowControlData) noexcept;
            void                                        windowUpdateDockingWindowDisplay(ControlData& windowControlData) noexcept;
            const bool                                  windowNeedToProcessControl(const ControlData& windowControlData) const noexcept;
            void                                        windowSetClipRectForMe(ControlData& windowControlData) const noexcept;
            void                                        windowSetClipRectForChildren(ControlData& windowControlData) const noexcept;
            void                                        windowSetClipRectForDocks(ControlData& windowControlData) const noexcept;
    #pragma endregion

    #pragma region Controls - Button
        public:
            // A simple button control
            // \param text [Text to display on the button]
            // \returns true, if clicked
            const bool                                  beginButton(const FileLine& fileLine, const wchar_t* const text);
            void                                        endButton() { endControlInternal(ControlType::Button); }
    #pragma endregion

    #pragma region Controls - CheckBox
        public:
            // [CheckBox]
            // \return true, if toggle state has changed
            const bool                                  beginCheckBox(const FileLine& fileLine, const wchar_t* const text, bool* const outIsChecked = nullptr);
            void                                        endCheckBox() { endControlInternal(ControlType::CheckBox); }
    #pragma endregion

    #pragma region Controls - Label
        public:
            void                                        makeLabel(const FileLine& fileLine, const wchar_t* const text, const LabelParam& labelParam = LabelParam());

        private:
            Float4                                      labelComputeTextPosition(const LabelParam& labelParam, const ControlData& labelControlData) const noexcept;
            Rendering::FontRenderingOption              labelMakeFontRenderingOption(const LabelParam& labelParam, const ControlData& labelControlData) const noexcept;
    #pragma endregion

    #pragma region Controls - Slider
        public:
            // \return true, if value has been changed
            const bool                                  beginSlider(const FileLine& fileLine, const SliderParam& sliderParam, float& outValue);
            void                                        endSlider() { endControlInternal(ControlType::Slider); }

        private:
            void                                        sliderDrawTrack(const SliderParam& sliderParam, const ControlData& trackControlData, const Rendering::Color& trackColor) noexcept;
            void                                        sliderDrawThumb(const SliderParam& sliderParam, const ControlData& thumbControlData, const Rendering::Color& thumbColor) noexcept;
    #pragma endregion

    #pragma region Controls - TextBox
        public:
            // \param textBoxParam [Various options]
            // \param outText [The content of the textbox]
            // \return true, if the content has changed
            const bool                                  beginTextBox(const FileLine& fileLine, const TextBoxParam& textBoxParam, StringW& outText);
            void                                        endTextBox() { endControlInternal(ControlType::TextBox); }

        private:
            void                                        textBoxProcessInput(const bool wasControlFocused, const TextInputMode textInputMode, ControlData& controlData, Float4& textRenderOffset, StringW& outText) noexcept;
    #pragma endregion

    #pragma region Controls - ValueSlider
        public:
            const bool                                  beginValueSlider(const FileLine& fileLine, const CommonControlParam& commonControlParam, const float roundnessInPixel, const int32 decimalDigits, float& value);
            void                                        endValueSlider() { endControlInternal(ControlType::ValueSlider); }
            
            const bool                                  beginLabeledValueSlider(const FileLine& fileLine, const wchar_t* const labelText, const LabelParam& labelParam, const CommonControlParam& valueSliderParam, const float labelWidth, const float roundnessInPixel, const int32 decimalDigits, float& value);
            void                                        endLabeledValueSlider() { endControlInternal(ControlType::ValueSlider); }

        private:
            void                                        valueSliderProcessInput(const bool wasControlFocused, ControlData& controlData, Float4& textRenderOffset, float& value, StringW& outText) noexcept;
    #pragma endregion

    #pragma region Controls - List (ListView, ListItem)
        public:
            const bool                                  beginListView(const FileLine& fileLine, int16& outSelectedListItemIndex, const ListViewParam& listViewParam);
            void                                        endListView();

            void                                        makeListItem(const FileLine& fileLine, const wchar_t* const text);
    #pragma endregion

    #pragma region Controls - Menu (MenuBar, MenuBarItem, MenuItem)
        public:
            const bool                                  beginMenuBar(const FileLine& fileLine, const wchar_t* const name);
            void                                        endMenuBar() { endControlInternal(ControlType::MenuBar); }

            const bool                                  beginMenuBarItem(const FileLine& fileLine, const wchar_t* const text);
            void                                        endMenuBarItem() { endControlInternal(ControlType::MenuBarItem); }

            const bool                                  beginMenuItem(const FileLine& fileLine, const wchar_t* const text);
            void                                        endMenuItem() { endControlInternal(ControlType::MenuItem); }
    #pragma endregion

    #pragma region Controls - Internal use
        private:
            const bool                                  beginTitleBar(const ControlID parentControlID, const wchar_t* const windowTitle, const Float2& titleBarSize, const Rect& innerPadding, VisibleState& inoutParentVisibleState);
            void                                        endTitleBar() { endControlInternal(ControlType::TitleBar); }

            // [RoundButton]
            const bool                                  makeRoundButton(const ControlID parentControlID, const wchar_t* const identifier, const wchar_t* const windowTitle, const Rendering::Color& color);

            // [Tooltip]
            // Unique control
            void                                        makeTooltipWindow(const ControlID parentControlID, const wchar_t* const tooltipText, const Float2& position);

            // [ScrollBar]
            void                                        makeScrollBar(const ControlID parentControlID, const ScrollBarType scrollBarType);
            void                                        makeScrollBar_vert(const ControlID parentControlID) noexcept;
            void                                        makeScrollBar_horz(const ControlID parentControlID) noexcept;
            ControlData&                                _makeScrollBarTrack(const ControlID parentControlID, const ScrollBarType scrollBarType, const ScrollBarTrackParam& scrollBarTrackParam, Rendering::ShapeRendererContext& shapeFontRendererContext, bool& outHasExtraSize);
            void                                        _makeScrollBarThumb(const ControlID parentControlID, const ScrollBarType scrollBarType, const float visibleLength, const float totalLength, Rendering::ShapeRendererContext& shapeFontRendererContext);
    #pragma endregion

        private:
            void                                        processDock(const ControlData& controlData, Rendering::ShapeRendererContext& shapeFontRendererContext);
            const bool                                  beginControlInternal(const ControlType controlType, const ControlID controlID, const bool returnValue);
            void                                        endControlInternal(const ControlType controlType);

        private:
            const bool                                  isValidControl(const ControlID& id) const noexcept;
            const ControlID                             issueControlID(const ControlID parentControlID, const ControlType controlType, const wchar_t* const identifier, const wchar_t* const text) noexcept;
            const ControlID                             issueControlID(const FileLine& fileLine, const ControlType controlType, const wchar_t* const text) noexcept;
            const ControlID                             _createControlDataInternalXXX(const ControlID controlID, const ControlID parentControlID, const ControlType controlType, const wchar_t* const text) noexcept;
            const ControlID                             _generateControlIDXXX(const ControlID& parentControlID, const ControlType controlType, const wchar_t* const identifier) const noexcept;
            const ControlID                             _generateControlIDXXX(const FileLine& fileLine, const ControlType controlType) const noexcept;
            const ControlData&                          getControlStackTopXXX() const noexcept;
            const ControlData&                          getControlData(const ControlID& id) const noexcept;
            ControlData&                                accessControlData(const ControlID& id) noexcept;
            Float2                                      getControlPositionInParentSpace(const ControlData& controlData) const noexcept;
            const ControlData&                          getParentWindowControlData(const ControlData& controlData) const noexcept;
            const ControlData&                          getParentWindowControlDataInternal(const ControlID& id) const noexcept;
            const bool                                  isInControlInnerInteractionArea(const Float2& screenPosition, const ControlData& controlData) const noexcept;
#pragma endregion

        public:
            const float                                 getCurrentAvailableDisplaySizeX() const noexcept;
            const float                                 getCurrentSameLineIntervalX() const noexcept;
            const bool                                  isThisControlPressed() const noexcept;
            const ControlAccessData&                    getThisControlAccessData() const noexcept;
            const bool                                  isFocusedControlInputBox() const noexcept;

        private:
            void                                        setControlFocused(const ControlData& controlData) noexcept;
            void                                        setControlHovered(const ControlData& controlData) noexcept;
            void                                        setControlPressed(const ControlData& controlData) noexcept;
            void                                        setControlClicked(const ControlData& controlData) noexcept;


#pragma region Before drawing controls
        private:
            void                                        updateControlData(ControlData& controlData, const ControlData::UpdateParam& updateParam) noexcept;
            void                                        computeControlChildAt(ControlData& controlData) noexcept;

            const bool                                  processClickControl(ControlData& controlData, const Rendering::Color& normalColor, const Rendering::Color& hoverColor, const Rendering::Color& pressedColor, Rendering::Color& outBackgroundColor) noexcept;
            const bool                                  processFocusControl(ControlData& controlData, const Rendering::Color& focusedColor, const Rendering::Color& nonFocusedColor, Rendering::Color& outBackgroundColor) noexcept;
            void                                        processShowOnlyControl(ControlData& controlData, Rendering::Color& outBackgroundColor, const bool setMouseInteractionDone = true) noexcept;
            const bool                                  processScrollableControl(ControlData& controlData, const Rendering::Color& normalColor, const Rendering::Color& dragColor, Rendering::Color& outBackgroundColor) noexcept;
            const bool                                  processToggleControl(ControlData& controlData, const Rendering::Color& normalColor, const Rendering::Color& hoverColor, const Rendering::Color& toggledColor, Rendering::Color& outBackgroundColor) noexcept;
    
            void                                        processControlInteractionInternal(ControlData& controlData, const bool setMouseInteractionDone = true) noexcept;

            void                                        processControlCommon(ControlData& controlData) noexcept;
            void                                        processControlCommon_updateMouseCursorForResizing(ControlData& controlData) noexcept;
            void                                        processControlCommon_updateTooltipData(ControlData& controlData) noexcept;
            void                                        processControlCommon_resize(ControlData& controlData) noexcept;
            void                                        processControlCommon_drag(ControlData& controlData) noexcept;
            void                                        processControlCommon_dock(ControlData& controlData) noexcept;
            Rect                                        processControlCommon_dock_makeInteractionBoxRect(const DockZone dockZone, ControlData& parentControlData) const noexcept;
            Rect                                        processControlCommon_dock_makePreviewRect(const DockZone dockZone, const Rect& interactionBoxRect, ControlData& parentControlData) const noexcept;
            void                                        processControlCommon_dock_renderInteractionBox(const Rendering::Color& color, const Rect& interactionBoxRect, const ControlData& parentControlData) noexcept;
            void                                        processControlCommon_dock_renderPreviewBox(const Rendering::Color& color, const Rect& previewRect) noexcept;

            void                                        dock(const ControlID& dockedControlID, const ControlID& dockControlID) noexcept;
            void                                        undock(const ControlID& dockedControlID) noexcept;
            void                                        updateDockZoneData(const ControlID& dockControlID, const bool updateWidthArray = true) noexcept;

            const bool                                  isInteracting(const ControlData& controlData) const noexcept;
            
            // These functions must be called after process- functions
            const bool                                  isControlBeingDragged(const ControlData& controlData) const noexcept;
            const bool                                  isControlBeingResized(const ControlData& controlData) const noexcept;

            // RendererContext 고를 때 사용
            const bool                                  isAncestorControlInteractionState(const ControlData& controlData, const ControlInteractionState controlInteractionState) const noexcept;
            const bool                                  isAncestorControlInteractionState_recursive(const ControlID& id, const ControlID& targetID) const noexcept;

            const bool                                  isDescendantControlInclusive(const ControlData& controlData, const ControlID& descendantCandidateID) const noexcept;
            const bool                                  isDescendantControlRecursiveXXX(const ControlID& currentControlID, const ControlID& descendantCandidateID) const noexcept;

            const bool                                  isParentControlRoot(const ControlData& controlData) const noexcept;

            // Focus, Out-of-focus 색 정할 때 사용
            const bool                                  needToColorFocused(const ControlData& controlData) const noexcept;
            const bool                                  isThisOrDescendantControlInteractionState(const ControlData& controlData, const ControlInteractionState controlInteractionState) const noexcept;
            const bool                                  isDescendantControlInteractionState(const ControlData& controlData, const ControlInteractionState controlInteractionState) const noexcept;
            const ControlData&                          getClosestFocusableAncestorControlInclusive(const ControlData& controlData) const noexcept;
            const bool                                  hasDockingAncestorControlInclusive(const ControlData& controlData) const noexcept;

            const Rendering::Color&                     getNamedColor(const NamedColor namedColor) const noexcept;
            void                                        setNamedColor(const NamedColor namedColor, const Rendering::Color& color) noexcept;

            const float                                 getMouseWheelScroll(const ControlData& scrollParentControlData) const noexcept;
            const float                                 computeTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept;
            const uint32                                computeIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept;
#pragma endregion

        private:
            Rendering::ShapeRendererContext&            getRendererContext(const ControlData& controlData) noexcept;
            Rendering::ShapeRendererContext&            getRendererContext(const RendererContextLayer rendererContextLayer) noexcept;
            // TopMost 는 제외!!
            const RendererContextLayer                  getUpperRendererContextLayer(const ControlData& controlData) noexcept;
            void                                        render();
            void                                        resetPerFrameStates();

        private:
            Rendering::GraphicDevice&                   _graphicDevice;

        private: // these are set externally
            float                                       _fontSize;
            uint32                                      _caretBlinkIntervalMs;
            Rendering::ShapeRendererContext             _rendererContexts[getRendererContextLayerCount()];

        private: // screen size
            int8                                        _updateScreenSizeCounter;
            Rect                                        _clipRectFullScreen;

        private:
            Rendering::Color                            _namedColors[static_cast<uint32>(NamedColor::COUNT)];

        private:
            ControlData                                 _rootControlData;
            Vector<ControlStackData>                    _controlStack;
            ControlID                                   _viewerTargetControlID;

        private:
            HashMap<ControlID, ControlData>             _controlIDMap;

        private:
            mutable ControlInteractionStateSet          _controlInteractionStateSet;
            ControlMetaStateSet                         _controlMetaStateSet;

#pragma region Mouse
        private:
            MouseStates                                 _mouseStates;
        
        private:
            mutable bool                                _isDragBegun;
            mutable ControlID                           _draggedControlID;
            mutable Float2                              _draggedControlInitialPosition;
            mutable uint64                              _dragStartTimeMs;

        private:
            mutable bool                                _isResizeBegun;
            mutable ControlID                           _resizedControlID;
            mutable Float2                              _resizedControlInitialPosition;
            mutable Float2                              _resizedControlInitialSize;
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


#include <MintRenderingBase/Include/GUI/GUIContext.inl>


#endif // !_MINT_GUI_CONTEXT_H_

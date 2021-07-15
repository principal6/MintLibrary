#pragma once


#ifndef MINT_GUI_CONTEXT_H
#define MINT_GUI_CONTEXT_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/GuiCommon.h>

#include <MintRenderingBase/Include/ShapeFontRendererContext.h>

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
        static constexpr float          kControlDisplayMinWidth = 10.0f;
        static constexpr float          kControlDisplayMinHeight = 10.0f;
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

        enum class ViewportUsage
        {
            Parent,
            ParentDock,
            Child,
        };

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


        //             |        CASE 0       |        CASE 1       |
        //             |---------------------|---------------------|
        // hi00 i0 li0 |               ScrollBarType               |
        // hi01        |                MenuBarType                |
        // hi02 i1     | CaretAt             | ThumbAt             |
        // hi03        | CaretStete          |  ..                 |
        // hi04 i2 li1 | SelectionStart      | SelectedItemIndex   |
        // hi05        | SelectionLength     | IsToggled           |
        // hi06 i3     | TextDisplayOffset   | ItemSizeX           | == TitleBarSizeX
        // hi07        |  ..                 |  ..                 |
        // hi08 i4 li2 |                     | ItemSizeY           | == TitleBarSizeY
        // hi09        |                     |  ..                 |
        // hi10 i5     |                     |                     |
        // hi11        |                     |                     |
        // hi12 i6 li3 | InternalTimeMs      |                     |
        // hi13        |  ..                 |                     |
        // hi14 i7     |  ..                 |                     |
        // hi15        |  ..                 |                     |
        class ControlValue
        {
        public:
                                    ControlValue();
                                    ~ControlValue()                         = default;

        public:
            ControlValue&           operator=(const ControlValue& rhs)      = default;
            ControlValue&           operator=(ControlValue&& rhs) noexcept  = default;

        public:
            void                    enableScrollBar(const ScrollBarType scrollBarType) noexcept;
            void                    disableScrollBar(const ScrollBarType scrollBarType) noexcept;
            const bool              isScrollBarEnabled(const ScrollBarType scrollBarType) const noexcept;

        private:
            void                    setCurrentScrollBarType(const ScrollBarType scrollBarType) noexcept;
            const ScrollBarType&    getCurrentScrollBarType() const noexcept;
        
        public:
            void                    setCurrentMenuBarType(const MenuBarType menuBarType) noexcept;
            void                    setThumbAt(const float thumbAt) noexcept;
            void                    setSelectedItemIndex(const int16 itemIndex) noexcept;
            void                    resetSelectedItemIndex() noexcept;
            void                    setIsToggled(const bool isToggled) noexcept;
            void                    setItemSizeX(const float itemSizeX) noexcept;
            void                    setItemSizeY(const float itemSizeY) noexcept;
            void                    addItemSizeX(const float itemSizeX) noexcept;
            void                    addItemSizeY(const float itemSizeY) noexcept;
            void                    setInternalTimeMs(const uint64 internalTimeMs) noexcept;

        public:
            const MenuBarType&      getCurrentMenuBarType() const noexcept;
            const float             getThumbAt() const noexcept; // [Slider], [ScrollBar]
            int16&                  getSelectedItemIndex() noexcept; // [ListView]
            const bool&             getIsToggled() const noexcept; // [CheckBox]
            const float             getItemSizeX() const noexcept;
            const float             getItemSizeY() const noexcept;
            const mint::Float2      getItemSize() const noexcept;
            uint16&                 getCaretAt() noexcept;
            uint16&                 getCaretState() noexcept;
            uint16&                 getSelectionStart() noexcept;
            uint16&                 getSelectionLength() noexcept;
            float&                  getTextDisplayOffset() noexcept;
            uint64&                 getInternalTimeMs() noexcept;

        private:
            static constexpr uint32 kSize64 = 4;
            union
            {
                struct
                {
                    uint64  _lui[kSize64];
                };
                struct
                {
                    int64   _li[kSize64];
                };
                struct
                {
                    int32   _i[kSize64 * 2];
                };                
                struct
                {
                    float   _f[kSize64 * 2];
                };
                struct
                {
                    uint16  _hui[kSize64 * 4];
                };
                struct
                {
                    int16   _hi[kSize64 * 4];
                };
                struct
                {
                    int8    _c[kSize64 * 8];
                };
            };
        };
        

        struct ResizingMask
        {
        public:
                                    ResizingMask();
                                    ResizingMask(const ResizingMask& rhs)       = default;
                                    ResizingMask(ResizingMask&& rhs) noexcept   = default;
                                    ~ResizingMask()                             = default;

        public:
            ResizingMask&           operator=(const ResizingMask& rhs)          = default;
            ResizingMask&           operator=(ResizingMask&& rhs) noexcept      = default;

        public:
            static ResizingMask     fromDockingMethod(const DockingMethod dockingMethod) noexcept;

        public:
            void                    setAllTrue() noexcept;
            void                    setAllFalse() noexcept;
            const bool              isResizable() const noexcept;
            const bool              topLeft() const noexcept;
            const bool              topRight() const noexcept;
            const bool              bottomLeft() const noexcept;
            const bool              bottomRight() const noexcept;
            const bool              overlaps(const ResizingMask& rhs) const noexcept;

        public:
            union
            {
                uint8       _rawMask;
                struct
                {
                    bool    _top : 1;
                    bool    _left : 1;
                    bool    _right : 1;
                    bool    _bottom : 1;
                };
            };
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
            class DockDatum
            {
            public:
                                            DockDatum();
                                            ~DockDatum() = default;

            public:
                const bool                  hasDockedControls() const noexcept;

            public:
                const bool                  isRawDockSizeSet() const noexcept;
                void                        setRawDockSize(const mint::Float2& rawDockSize) noexcept;
                const mint::Float2&         getRawDockSizeXXX() const noexcept;
                void                        swapDockedControlsXXX(const int32 indexA, const int32 indexB) noexcept;
                const int32                 getDockedControlIndex(const uint64 dockedControlHashKey) const noexcept;
                const uint64                getDockedControlHashKey(const int32 dockedControlIndex) const noexcept;
                const float                 getDockedControlTitleBarOffset(const int32 dockedControlIndex) const noexcept;
                const int32                 getDockedControlIndexByMousePosition(const float relativeMousePositionX) const noexcept;

            public:
                mint::Vector<uint64>        _dockedControlHashArray;
                int32                       _dockedControlIndexShown;
                mint::Vector<float>         _dockedControlTitleBarOffsetArray; // TitleBar 렌더링 위치 계산에 사용
                mint::Vector<float>         _dockedControlTitleBarWidthArray; // TitleBar 순서 변경 시 마우스 Interaction 에 사용!

            private:
                mint::Float2                _rawDockSize;
            };

            struct DockingStateContext
            {
                mint::Float2                _displaySize;
                ResizingMask                _resizingMask;
            };

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


        public:
            struct PrepareControlDataParam
            {
                mint::Rect          _innerPadding;
                mint::Float2        _initialDisplaySize;
                ResizingMask        _initialResizingMask;
                mint::Float2        _offset                         = mint::Float2::kZero;
                mint::Float2        _desiredPositionInParent        = mint::Float2::kNan;
                mint::Float2        _deltaInteractionSize           = mint::Float2::kZero;
                mint::Float2        _deltaInteractionSizeByDock     = mint::Float2::kZero;
                mint::Float2        _displaySizeMin                 = mint::Float2(kControlDisplayMinWidth, kControlDisplayMinHeight);
                bool                _alwaysResetDisplaySize         = false;
                bool                _alwaysResetParent              = false;
                uint64              _parentHashKeyOverride          = 0;
                bool                _alwaysResetPosition            = true;
                bool                _ignoreMeForContentAreaSize     = false;
                bool                _noIntervalForNextSibling       = false;
                ViewportUsage       _viewportUsage                  = ViewportUsage::Child;
            };

            class ControlData
            {
            public:
                                                            ControlData();
                                                            ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType);
                                                            ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType, const mint::Float2& size);
            
            public:
                void                                        clearPerFrameData() noexcept;
                void                                        updatePerFrameWithParent(const bool isNewData, const PrepareControlDataParam& prepareControlDataParam, ControlData& parent) noexcept;

            public:
                const uint64                                getHashKey() const noexcept;
                const uint64                                getParentHashKey() const noexcept;
                const mint::Rect&                           getInnerPadding() const noexcept;
                mint::Float2                                getClientSize() const noexcept;
                const float                                 getTopOffsetToClientArea() const noexcept;
                const mint::Float2&                         getDisplaySizeMin() const noexcept;
                const float                                 getPureDisplayWidth() const noexcept;
                const float                                 getPureDisplayHeight() const noexcept;
                const mint::Float2&                         getInteractionSize() const noexcept;
                const mint::Float2&                         getNonDockInteractionSize() const noexcept;
                const mint::Float2&                         getContentAreaSize() const noexcept;
                const mint::Float2&                         getPreviousContentAreaSize() const noexcept;
                const mint::Float2&                         getChildAt() const noexcept;
                const mint::Float2&                         getNextChildOffset() const noexcept;
                const ControlType                           getControlType() const noexcept;
                const bool                                  isTypeOf(const ControlType controlType) const noexcept;
                const wchar_t*                              getText() const noexcept;
                const bool                                  isRootControl() const noexcept;
                const bool                                  visibleStateEquals(const VisibleState visibleState) const noexcept;
                const bool                                  isControlVisible() const noexcept;
                const mint::Rect&                           getClipRect() const noexcept;
                const mint::Rect&                           getClipRectForChildren() const noexcept;
                const mint::Rect&                           getClipRectForDocks() const noexcept;
                const mint::Vector<uint64>&                 getChildControlDataHashKeyArray() const noexcept;
                const mint::Vector<uint64>&                 getPreviousChildControlDataHashKeyArray() const noexcept;
                const uint16                                getPreviousChildControlCount() const noexcept;
                const uint16                                getPreviousMaxChildControlCount() const noexcept;
                void                                        prepareChildControlDataHashKeyArray() noexcept;
                const bool                                  hasChildWindow() const noexcept;
                DockDatum&                                  getDockDatum(const DockingMethod dockingMethod) noexcept;
                const DockDatum&                            getDockDatum(const DockingMethod dockingMethod) const noexcept;
                const bool                                  isShowingInDock(const ControlData& dockedControlData) const noexcept;
                void                                        setDockSize(const DockingMethod dockingMethod, const mint::Float2& dockSize) noexcept;
                const mint::Float2                          getDockSize(const DockingMethod dockingMethod) const noexcept;
                const mint::Float2                          getDockSizeIfHosting(const DockingMethod dockingMethod) const noexcept;
                const mint::Float2                          getDockOffsetSize() const noexcept;
                const mint::Float2                          getDockPosition(const DockingMethod dockingMethod) const noexcept;
                const float                                 getHorzDockSizeSum() const noexcept;
                const float                                 getVertDockSizeSum() const noexcept;
                const mint::Float2                          getMenuBarThickness() const noexcept;
                void                                        connectToDock(const uint64 dockControlHashKey) noexcept;
                void                                        disconnectFromDock() noexcept;
                const uint64                                getDockControlHashKey() const noexcept;
                const bool                                  isDocking() const noexcept;
                const bool                                  isDockHosting() const noexcept;
                const bool                                  isResizable() const noexcept;
                Rect                                        getControlRect() const noexcept;
                Rect                                        getControlPaddedRect() const noexcept;
            
            public:
                void                                        connectChildWindowIfNot(const ControlData& childWindowControlData) noexcept;
                void                                        disconnectChildWindow(const uint64 childWindowHashKey) noexcept;
                const mint::HashMap<uint64, bool>&          getChildWindowHashKeyMap() const noexcept;

            public:
                void                                        setVisibleState(const VisibleState visibleState) noexcept;
                void                                        swapDockingStateContext() noexcept;
            
            public:
                void                                        setParentHashKeyXXX(const uint64 parentHashKey) noexcept;
                void                                        setOffsetY_XXX(const float offsetY) noexcept;
                void                                        setClipRectXXX(const mint::Rect& clipRect) noexcept;
                void                                        setClipRectForChildrenXXX(const mint::Rect& clipRect) noexcept;
                void                                        setClipRectForDocksXXX(const mint::Rect& clipRect) noexcept;

            public:
                uint8                                       _updateCount;
                mint::Float2                                _displaySize;
                mint::Float2                                _position; // In screen space, at left-top corner
                mint::Float2                                _currentFrameDeltaPosition; // Used for dragging
                mint::Float2                                _childDisplayOffset; // Used for scrolling child controls (of Window control)
                bool                                        _isFocusable;
                bool                                        _isDraggable;
                bool                                        _isInteractableOutsideParent;
                ResizingMask                                _resizingMask;
                mint::Rect                                  _draggingConstraints; // MUST set all four values if want to limit dragging area
                uint64                                      _delegateHashKey; // Used for drag, resize and focus
                DockingControlType                          _dockingControlType;
                DockingMethod                               _lastDockingMethod;
                DockingMethod                               _lastDockingMethodCandidate;
                std::wstring                                _text;
                ControlValue                                _controlValue;

            private:
                uint64                                      _hashKey;
                uint64                                      _parentHashKey;
                mint::Rect                                  _innerPadding; // For child controls
                mint::Float2                                _displaySizeMin;
                mint::Float2                                _interactionSize; // _nonDockInteractionSize + dock size
                mint::Float2                                _nonDockInteractionSize; // Exluces dock area
                mint::Float2                                _contentAreaSize; // Could be smaller or larger than _displaySize
                mint::Float2                                _previousContentAreaSize;
                mint::Float2                                _childAt; // In screen space, Next child control will be positioned according to this
                mint::Float2                                _nextChildOffset; // Every new child sets this offset to calculate next _childAt
                ControlType                                 _controlType;
                VisibleState                                _visibleState;
                mint::Rect                                  _clipRect;
                mint::Rect                                  _clipRectForChildren; // Used by window
                mint::Rect                                  _clipRectForDocks;
                mint::Vector<uint64>                        _childControlDataHashKeyArray;
                mint::Vector<uint64>                        _previousChildControlDataHashKeyArray;
                uint16                                      _previousMaxChildControlCount;
                mint::HashMap<uint64, bool>                 _childWindowHashKeyMap;
                DockDatum                                   _dockData[static_cast<uint32>(DockingMethod::COUNT)];
                uint64                                      _dockControlHashKey;
                DockingStateContext                         _dokcingStateContext;
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
            void                                                textBoxProcessInput(const bool wasControlFocused, const TextInputMode textInputMode, GuiContext::ControlData& controlData, mint::Float4& textRenderOffset, std::wstring& outText) noexcept;
        
        private:
            void                                                textBoxProcessInputMouse(GuiContext::ControlData& controlData, mint::Float4& textRenderOffset, std::wstring& outText, TextBoxProcessInputResult& result);
            void                                                textBoxProcessInputKeyDeleteBefore(GuiContext::ControlData& controlData, std::wstring& outText);
            void                                                textBoxProcessInputKeyDeleteAfter(GuiContext::ControlData& controlData, std::wstring& outText);
            void                                                textBoxProcessInputKeySelectAll(GuiContext::ControlData& controlData, std::wstring& outText);
            void                                                textBoxProcessInputKeyCopy(GuiContext::ControlData& controlData, std::wstring& outText);
            void                                                textBoxProcessInputKeyCut(GuiContext::ControlData& controlData, std::wstring& outText);
            void                                                textBoxProcessInputKeyPaste(const std::wstring& errorMessage, GuiContext::ControlData& controlData, std::wstring& outText);
            void                                                textBoxProcessInputCaretToPrev(GuiContext::ControlData& controlData);
            void                                                textBoxProcessInputCaretToNext(GuiContext::ControlData& controlData, const std::wstring& text);
            void                                                textBoxProcessInputCaretToHead(GuiContext::ControlData& controlData);
            void                                                textBoxProcessInputCaretToTail(GuiContext::ControlData& controlData, const std::wstring& text);
            void                                                textBoxRefreshCaret(const uint64 currentTimeMs, uint16& caretState, uint64& lastCaretBlinkTimeMs) noexcept;
            void                                                textBoxEraseSelection(GuiContext::ControlData& controlData, std::wstring& outText) noexcept;
            const bool                                          textBoxInsertWchar(const wchar_t input, uint16& caretAt, std::wstring& outText);
            const bool                                          textBoxInsertWstring(const std::wstring& input, uint16& caretAt, std::wstring& outText);
            void                                                textBoxUpdateSelection(const uint16 oldCaretAt, const uint16 caretAt, GuiContext::ControlData& controlData);
            const bool                                          textBoxIsValidInput(const wchar_t input, const uint16 caretAt, const TextInputMode textInputMode, const std::wstring& text) noexcept;
        
        private:
            void                                                textBoxUpdateTextDisplayOffset(const uint16 textLength, const float textWidthTillCaret, const float inputCandidateWidth, GuiContext::ControlData& controlData) noexcept;
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
            mint::RenderingBase::ShapeFontRendererContext&      getRendererContextForChildControl(const ControlData& controlData) noexcept;

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
            const bool                                          isControlClicked() const noexcept;
            const bool                                          isControlPressed() const noexcept;
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
            void                                                resetHoverDataIfMe(const uint64 controlHashKey) noexcept;
            void                                                resetPressDataIfMe(const uint64 controlHashKey) noexcept;

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
            const bool                                          isControlHovered(const ControlData& controlData) const noexcept;
            const bool                                          isControlPressed(const ControlData& controlData) const noexcept;
            const bool                                          isControlClicked(const ControlData& controlData) const noexcept;
            const bool                                          isControlFocused(const ControlData& controlData) const noexcept;

            // RendererContext 고를 때 사용
            const bool                                          isAncestorControlFocused(const ControlData& controlData) const noexcept;
            const bool                                          isAncestorControlPressed(const ControlData& controlData) const noexcept;
            const bool                                          isAncestorControlTargetRecursiveXXX(const uint64 hashKey, const uint64 targetHashKey) const noexcept;
            const bool                                          isAncestorControlFocusedInclusiveXXX(const ControlData& controlData) const noexcept;

            const bool                                          isAncestorControlInclusive(const ControlData& controlData, const uint64 ancestorCandidateHashKey) const noexcept;
            const bool                                          isAncestorControlRecursiveXXX(const uint64 currentControlHashKey, const uint64 ancestorCandidateHashKey) const noexcept;
            const bool                                          isDescendantControlInclusive(const ControlData& controlData, const uint64 descendantCandidateHashKey) const noexcept;
            const bool                                          isDescendantControlRecursiveXXX(const uint64 currentControlHashKey, const uint64 descendantCandidateHashKey) const noexcept;

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
            void                                                render();
            void                                                resetPerFrameStates();

        private:
            mint::RenderingBase::GraphicDevice* const           _graphicDevice;

        private: // these are set externally
            float                                               _fontSize;
            uint32                                              _caretBlinkIntervalMs;
            mint::RenderingBase::ShapeFontRendererContext       _shapeFontRendererContextBackground;
            mint::RenderingBase::ShapeFontRendererContext       _shapeFontRendererContextForeground;
            mint::RenderingBase::ShapeFontRendererContext       _shapeFontRendererContextTopMost;

        private: // screen size
            int8                                                _updateScreenSizeCounter;
            mint::Rect                                          _clipRectFullScreen;

        private:
            ControlData                                         _rootControlData;
            mint::Vector<ControlStackData>                      _controlStackPerFrame;

        private:
            mutable bool                                        _isMouseInteractionDoneThisFrame;
            mutable uint64                                      _focusedControlHashKey;
            mutable uint64                                      _hoveredControlHashKey;
            mutable uint64                                      _pressedControlHashKey;
            mutable mint::Float2                                _pressedControlInitialPosition;
            mutable uint64                                      _clickedControlHashKeyPerFrame;
            uint64                                              _hoverStartTimeMs;
            bool                                                _hoverStarted;
        

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
            mint::Float2                                        _tooltipPosition;
            uint64                                              _tooltipParentWindowHashKey;
            const wchar_t*                                      _tooltipTextFinal;
            TaskWhenMouseUp                                     _taskWhenMouseUp;

        private:
            mint::RenderingBase::Color                          _namedColors[static_cast<uint32>(NamedColor::COUNT)];
        };
    }
}


#include <MintRenderingBase/Include/GuiContext.inl>


#endif // !MINT_GUI_CONTEXT_H

#pragma once


#ifndef MINT_GUI_CONTROL_DATA_H
#define MINT_GUI_CONTROL_DATA_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/HashMap.h>

#include <MintMath/Include/Rect.h>

#include <MintRenderingBase/Include/Gui/GuiCommon.h>


namespace mint
{
    namespace Gui
    {
        static constexpr float          kControlDisplayMinWidth = 10.0f;
        static constexpr float          kControlDisplayMinHeight = 10.0f;


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


        struct PrepareControlDataParam
        {
        public:
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
    }
}


#endif // !MINT_GUI_CONTROL_DATA_H

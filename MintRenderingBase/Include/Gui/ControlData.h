#pragma once


#ifndef MINT_GUI_CONTROL_DATA_H
#define MINT_GUI_CONTROL_DATA_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/String.h>
#include <MintContainer/Include/HashMap.h>

#include <MintMath/Include/Rect.h>

#include <MintRenderingBase/Include/Gui/GuiCommon.h>

#include <MintReflection/Include/Reflection.h>


namespace mint
{
    namespace Gui
    {
        static constexpr float          kControlDisplayMinWidth = 10.0f;
        static constexpr float          kControlDisplayMinHeight = 10.0f;


        class ControlValue
        {
        public:
            struct CommonData
            {
                MenuBarType     _menuBarType;
                void            enableScrollBar(const ScrollBarType scrollBarType) noexcept;
                void            disableScrollBar(const ScrollBarType scrollBarType) noexcept;
                const bool      isScrollBarEnabled(const ScrollBarType scrollBarType) const noexcept;

            private:
                ScrollBarType   _scrollBarType;
            };

            struct TextBoxData
            {
                uint16      _caretAt = 0;
                uint16      _caretState = 0;
                uint16      _selectionStart = 0;
                uint16      _selectionLength = 0;
                float       _textDisplayOffset = 0.0f;
                uint64      _lastCaretBlinkTimeMs = 0;
            };

            struct WindowData
            {
                float       _titleBarThickness;
            };

            struct ThumbData
            {
                float       _thumbAt = 0.0f;
            };

            struct ItemData
            {
                Float2                  _itemSize;
                MINT_INLINE void        select(const int16 itemIndex) noexcept { _selectedItemIndex = itemIndex; }
                MINT_INLINE void        deselect() noexcept { _selectedItemIndex = -1; }
                MINT_INLINE const bool  isSelected(const int16 itemIndex) const noexcept { return (_selectedItemIndex == itemIndex); }
                MINT_INLINE int16       getSelectedItemIndex() const noexcept { return _selectedItemIndex; }

            private:
                int16                   _selectedItemIndex = 0;
            };

            struct BooleanData
            {
                MINT_INLINE void        toggle() noexcept { _value = !_value; }
                MINT_INLINE void        set(const bool value) noexcept { _value = value; }
                MINT_INLINE bool&       get() noexcept { return _value; }

            private:
                bool                    _value = false;
            };

        public:
                                    ControlValue();
                                    ~ControlValue()                         = default;

        public:
            ControlValue&           operator=(const ControlValue& rhs)      = default;
            ControlValue&           operator=(ControlValue&& rhs) noexcept  = default;

        public:
            CommonData              _commonData;
            union
            {
                TextBoxData         _textBoxData;
                WindowData          _windowData;
                ThumbData           _thumbData;
                struct // 아래 두 개는 동시에 쓰이는 경우가 있기 때문에 이렇게 struct 로 묶어둬야 메모리 침범을 하지 않는다.
                {
                    ItemData        _itemData;
                    BooleanData     _booleanData;
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
            const bool          hasDockedControls() const noexcept;

        public:
            const bool          isRawDockSizeSet() const noexcept;
            void                setRawDockSize(const Float2& rawDockSize) noexcept;
            const Float2&       getRawDockSizeXXX() const noexcept;
            void                swapDockedControlsXXX(const int32 indexA, const int32 indexB) noexcept;
            const int32         getDockedControlIndex(const uint64 dockedControlHashKey) const noexcept;
            const uint64        getDockedControlHashKey(const int32 dockedControlIndex) const noexcept;
            const float         getDockedControlTitleBarOffset(const int32 dockedControlIndex) const noexcept;
            const int32         getDockedControlIndexByMousePosition(const float relativeMousePositionX) const noexcept;

        public:
            Vector<uint64>      _dockedControlHashArray;
            int32               _dockedControlIndexShown;
            Vector<float>       _dockedControlTitleBarOffsetArray; // TitleBar 렌더링 위치 계산에 사용
            Vector<float>       _dockedControlTitleBarWidthArray; // TitleBar 순서 변경 시 마우스 Interaction 에 사용!

        private:
            Float2              _rawDockSize;
        };


        struct DockingStateContext
        {
            Float2              _displaySize;
            ResizingMask        _resizingMask;
        };


        struct PrepareControlDataParam
        {
        public:
            Rect            _innerPadding;
            Float2          _autoCalculatedDisplaySize      = Float2::kZero;
            ResizingMask    _initialResizingMask;
            Float2          _offset                         = Float2::kZero;
            Float2          _desiredPositionInParent        = Float2::kNan;
            Float2          _deltaInteractionSize           = Float2::kZero;
            Float2          _deltaInteractionSizeByDock     = Float2::kZero;
            Float2          _displaySizeMin                 = Float2(kControlDisplayMinWidth, kControlDisplayMinHeight);
            bool            _alwaysResetDisplaySize         = false;
            bool            _alwaysResetParent              = false;
            uint64          _parentHashKeyOverride          = 0;
            bool            _alwaysResetPosition            = true;
            bool            _ignoreMeForContentAreaSize     = false;
            bool            _noIntervalForNextSibling       = false;
            ClipRectUsage   _clipRectUsage                  = ClipRectUsage::ParentsChild;
        };


        class ControlData
        {
        public:
            class DockRelatedData
            {
                friend class ControlData;

            public:
                                                DockRelatedData(const ControlType controlType);
                                                ~DockRelatedData() = default;

            public:
                DockingControlType              _dockingControlType;
                DockingMethod                   _lastDockingMethod;
                DockingMethod                   _lastDockingMethodCandidate;

            private:
                DockDatum                       _dockData[static_cast<uint32>(DockingMethod::COUNT)];
                uint64                          _dockControlHashKey;
                DockingStateContext             _dokcingStateContext;
            };

        public:
                                                REFLECTION_CLASS(ControlData);
                                                ControlData();
                                                ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType);
                                                ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType, const Float2& size);
    
        public:
            void                                clearPerFrameData() noexcept;
            void                                updatePerFrameWithParent(const bool isNewData, const PrepareControlDataParam& prepareControlDataParam, ControlData& parent) noexcept;

        public:
            const uint64                        getHashKey() const noexcept;
            const uint64                        getParentHashKey() const noexcept;
            const Rect&                         getInnerPadding() const noexcept;
            Float2                              getClientSize() const noexcept;
            const float                         getTopOffsetToClientArea() const noexcept;
            const Float2&                       getDisplaySizeMin() const noexcept;
            const float                         getPureDisplayWidth() const noexcept;
            const float                         getPureDisplayHeight() const noexcept;
            const Float2&                       getInteractionSize() const noexcept;
            const Float2&                       getNonDockInteractionSize() const noexcept;
            const Float2&                       getContentAreaSize() const noexcept;
            const Float2&                       getPreviousContentAreaSize() const noexcept;
            const Float2&                       getChildAt() const noexcept;
            const Float2&                       getNextChildOffset() const noexcept;
            const ControlType                   getControlType() const noexcept;
            const bool                          isTypeOf(const ControlType controlType) const noexcept;
            const bool                          isInputBoxType() const noexcept;
            const wchar_t*                      getText() const noexcept;
            const bool                          isRootControl() const noexcept;
            const bool                          updateVisibleState(const VisibleState visibleState) noexcept;
            const bool                          isControlVisible() const noexcept;
            const Rect&                         getClipRect() const noexcept;
            const Rect&                         getClipRectForChildren() const noexcept;
            const Rect&                         getClipRectForDocks() const noexcept;
            const Vector<uint64>&               getChildControlDataHashKeyArray() const noexcept;
            const Vector<uint64>&               getPreviousChildControlDataHashKeyArray() const noexcept;
            const uint16                        getPreviousChildControlCount() const noexcept;
            const uint16                        getPreviousMaxChildControlCount() const noexcept;
            void                                prepareChildControlDataHashKeyArray() noexcept;
            const bool                          hasChildWindow() const noexcept;
            DockDatum&                          getDockDatum(const DockingMethod dockingMethod) noexcept;
            const DockDatum&                    getDockDatum(const DockingMethod dockingMethod) const noexcept;
            const bool                          isShowingInDock(const ControlData& dockedControlData) const noexcept;
            void                                setDockSize(const DockingMethod dockingMethod, const Float2& dockSize) noexcept;
            const Float2                        getDockSize(const DockingMethod dockingMethod) const noexcept;
            const Float2                        getDockSizeIfHosting(const DockingMethod dockingMethod) const noexcept;
            const Float2                        getDockOffsetSize() const noexcept;
            const Float2                        getDockPosition(const DockingMethod dockingMethod) const noexcept;
            const float                         getHorzDockSizeSum() const noexcept;
            const float                         getVertDockSizeSum() const noexcept;
            const Float2                        getMenuBarThickness() const noexcept;
            void                                connectToDock(const uint64 dockControlHashKey) noexcept;
            void                                disconnectFromDock() noexcept;
            const uint64                        getDockControlHashKey() const noexcept;
            const bool                          isDocking() const noexcept;
            const bool                          isDockHosting() const noexcept;
            const bool                          isResizable() const noexcept;
            Rect                                getControlRect() const noexcept;
            Rect                                getControlPaddedRect() const noexcept;
            
        public:
            void                                connectChildWindowIfNot(const ControlData& childWindowControlData) noexcept;
            void                                disconnectChildWindow(const uint64 childWindowHashKey) noexcept;
            const HashMap<uint64, bool>&        getChildWindowHashKeyMap() const noexcept;

        public:
            void                                swapDockingStateContext() noexcept;
    
        public:
            void                                setParentHashKeyXXX(const uint64 parentHashKey) noexcept;
            void                                setOffsetY_XXX(const float offsetY) noexcept;
            void                                setClipRectXXX(const Rect& clipRect) noexcept;
            void                                setClipRectForChildrenXXX(const Rect& clipRect) noexcept;
            void                                setClipRectForDocksXXX(const Rect& clipRect) noexcept;

        public:
            const Float4                        getControlCenterPosition() const noexcept;
            const Float2                        getControlLeftCenterPosition() const noexcept;
            const Float2                        getControlRightCenterPosition() const noexcept;

        public:
            uint8                               _updateCount;
            Float2                              _displaySize;

            // In screen space, at left-top corner
            REFLECTION_MEMBER(Float2, _position);

            Float2                              _currentFrameDeltaPosition; // Used for dragging
            Float2                              _childDisplayOffset; // Used for scrolling child controls (of Window control)
            bool                                _isFocusable;
            bool                                _needDoubleClickToFocus;
            bool                                _isDraggable;
            bool                                _isInteractableOutsideParent;
            ResizingMask                        _resizingMask;
            Rect                                _draggingConstraints; // MUST set all four values if want to limit dragging area
            uint64                              _delegateHashKey; // Used for drag, resize and focus
            REFLECTION_MEMBER(StringW, _text);
            ControlValue                        _controlValue;
            RendererContextLayer                _rendererContextLayer;
            DockRelatedData                     _dockRelatedData;

        private:
            REFLECTION_MEMBER(uint64, _hashKey);
            uint64                              _parentHashKey;
            Rect                                _innerPadding; // For child controls
            Float2                              _displaySizeMin;
            Float2                              _interactionSize; // _nonDockInteractionSize + dock size
            Float2                              _nonDockInteractionSize; // Exluces dock area
            Float2                              _contentAreaSize; // Could be smaller or larger than _displaySize
            Float2                              _previousContentAreaSize;
            Float2                              _childAt; // In screen space, Next child control will be positioned according to this
            Float2                              _nextChildOffset; // Every new child sets this offset to calculate next _childAt
            ControlType                         _controlType;
            VisibleState                        _visibleState;
            Rect                                _clipRect;
            Rect                                _clipRectForChildren; // Used by window
            Rect                                _clipRectForDocks;
            Vector<uint64>                      _childControlDataHashKeyArray;
            Vector<uint64>                      _previousChildControlDataHashKeyArray;
            uint16                              _previousMaxChildControlCount;
            HashMap<uint64, bool>               _childWindowHashKeyMap;

        private:
            REFLECTION_BIND_BEGIN;
                REFLECTION_BIND(_position);
                REFLECTION_BIND(_text);
                REFLECTION_BIND(_hashKey);
            REFLECTION_BIND_END;
        };
    }
}


#endif // !MINT_GUI_CONTROL_DATA_H

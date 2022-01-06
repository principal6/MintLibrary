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
        class ControlData;
        class ControlMetaStateSet;


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
                float       _titleBarThickness = 0.0f;
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

        class ControlID
        {
        public:
                                        ControlID() : _hash{ 0 } {}
            explicit                    ControlID(const uint64 hash) : _hash{ hash } {}
                                        ~ControlID() = default;

        public:
            void                        reset() noexcept { _hash = 0; }
            MINT_INLINE const bool      isValid() const noexcept { return _hash != 0; }
            MINT_INLINE const uint64    getRawValue() const noexcept { return _hash; }

        public:
            MINT_INLINE const bool      operator==(const ControlID& rhs) const noexcept { return _hash == rhs._hash; }
            MINT_INLINE const bool      operator!=(const ControlID& rhs) const noexcept { return _hash != rhs._hash; }

        private:
            uint64                      _hash;
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
            const int32         getDockedControlIndex(const ControlID& dockedControlID) const noexcept;
            const ControlID     getDockedControlID(const int32 dockedControlIndex) const noexcept;
            const float         getDockedControlTitleBarOffset(const int32 dockedControlIndex) const noexcept;
            const int32         getDockedControlIndexByMousePosition(const float relativeMousePositionX) const noexcept;

        public:
            Vector<ControlID>   _dockedControlIDArray;
            int32               _dockedControlIndexShown;
            Vector<float>       _dockedControlTitleBarOffsetArray; // TitleBar 렌더링 위치 계산에 사용
            Vector<float>       _dockedControlTitleBarWidthArray; // TitleBar 순서 변경 시 마우스 Interaction 에 사용!

        private:
            Float2              _rawDockSize;
        };


        struct DockingStateContext
        {
            Float2              _size;
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
            Float2          _minSize                 = Float2(kControlDisplayMinWidth, kControlDisplayMinHeight);
            bool            _alwaysResetDisplaySize         = true;
            bool            _alwaysResetParent              = false;
            ControlID       _parentIdOverride               = ControlID();
            bool            _alwaysResetPosition            = true;
            bool            _ignoreMeForContentAreaSize     = false;
            ClipRectUsage   _clipRectUsage                  = ClipRectUsage::ParentsChild;
        };

        class ControlAccessData
        {
            friend ControlData;

        public:
                                                    ControlAccessData() = default;
                                                    ~ControlAccessData() = default;

        public:
            MINT_INLINE const ControlID&            getControlID() const noexcept { return _controlID; }
            MINT_INLINE const Vector<ControlID>&    getChildControlIDs() const noexcept { return _childControlIDs; }

        private:
            ControlID                               _controlID;
            Vector<ControlID>                       _childControlIDs;
        };


        class ControlData
        {
        public:
            class DockRelatedData
            {
                friend ControlData;

            public:
                                                DockRelatedData(const ControlType controlType);
                                                ~DockRelatedData() = default;

            public:
                DockingControlType              _dockingControlType;
                DockingMethod                   _lastDockingMethod;
                DockingMethod                   _lastDockingMethodCandidate;

            private:
                DockDatum                       _dockData[static_cast<uint32>(DockingMethod::COUNT)];
                ControlID                       _dockControlID;
                DockingStateContext             _dokcingStateContext;
            };

            struct Option
            {
                        Option();

                bool    _isFocusable                    : 1;
                bool    _needDoubleClickToFocus         : 1;
                bool    _isDraggable                    : 1;
                bool    _isInteractableOutsideParent    : 1;
            };

            struct PerFrameData
            {
                void                reset(const Rect& innerPadding);

                Float2              _deltaPosition; // Used for dragging
                Float2              _nextChildOffset; // Every new child sets this offset to calculate next _childAt
                Float2              _childAt; // In screen space, Next child control will be positioned according to this
                Float2              _contentAreaSize; // Could be smaller or larger than _size
                Vector<ControlID>   _childControlIDs;
            };

            // 한 프레임 지연된 데이터
            struct LastFrameData
            {
                                    LastFrameData();

                Float2              _contentAreaSize;
                Vector<ControlID>   _childControlIDs;
                uint16              _maxChildControlCount;
            };

        public:
                                                REFLECTION_CLASS(ControlData);
                                                ControlData();
                                                ControlData(const ControlID& id, const ControlID& parentId, const ControlType controlType);
                                                ControlData(const ControlID& id, const ControlID& parentId, const ControlType controlType, const Float2& size);
    
        public:
            void                                clearPerFrameData() noexcept;
            void                                updatePerFrame(const PrepareControlDataParam& prepareControlDataParam, ControlData& parentControlData, const ControlMetaStateSet& controlMetaStateSet, const float availableDisplaySizeX, const bool computeSize) noexcept;
            
        private:
            void                                updateSize(const PrepareControlDataParam& prepareControlDataParam, const ControlMetaStateSet& controlMetaStateSet, const float availableDisplaySizeX, const bool compute) noexcept;

        public:
            const ControlID&                    getId() const noexcept;
            void                                setParentId(const ControlID& parentId) noexcept;
            const ControlID&                    getParentId() const noexcept;
            const int16                         getLastAddedChildIndex() const noexcept;
            const Vector<ControlID>&            getChildControlIDs() const noexcept;
            const uint16                        getChildControlCount() const noexcept;
            const uint16                        getMaxChildControlCount() const noexcept;
            const bool                          hasChildWindow() const noexcept;
            void                                registerChildWindow(const ControlData& childWindowControlData) noexcept;
            const HashMap<ControlID, bool>&     getChildWindowIDMap() const noexcept;

        public:
            const ControlType                   getControlType() const noexcept;
            const bool                          isTypeOf(const ControlType controlType) const noexcept;
            const bool                          isInputBoxType() const noexcept;
            const bool                          isRootControl() const noexcept;

        public:
            void                                setOffsetY_XXX(const float offsetY) noexcept;
            const Rect&                         getInnerPadding() const noexcept;
            Float2                              getClientSize() const noexcept;
            const float                         getTopOffsetToClientArea() const noexcept;
            const Float2&                       getMinSize() const noexcept;
            const Float2&                       getInteractionSize() const noexcept;
            const Float2&                       getNonDockInteractionSize() const noexcept;
            const Float2&                       getContentAreaSize() const noexcept;
            const Float2&                       getChildAt() const noexcept;
            const float                         computeScrollDisplayWidth() const noexcept;
            const float                         computeScrollDisplayHeight() const noexcept;

        public:
            const wchar_t*                      getText() const noexcept;

        public:
            const bool                          updateVisibleState(const VisibleState visibleState) noexcept;
            const bool                          isControlVisible() const noexcept;

        public:
            void                                setAllClipRects(const Rect& clipRect) noexcept;
            void                                setClipRectForMe(const Rect& clipRect) noexcept;
            void                                setClipRectForChildren(const Rect& clipRect) noexcept;
            void                                setClipRectForDocks(const Rect& clipRect) noexcept;
            const Rect&                         getClipRectForMe() const noexcept;
            const Rect&                         getClipRectForChildren() const noexcept;
            const Rect&                         getClipRectForDocks() const noexcept;
            
        public:
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
            void                                connectToDock(const ControlID& dockControlID) noexcept;
            void                                disconnectFromDock() noexcept;
            const ControlID&                    getDockControlID() const noexcept;
            const bool                          isDocking() const noexcept;
            const bool                          isDockHosting() const noexcept;
            const bool                          isResizable() const noexcept;
            void                                swapDockingStateContext() noexcept;

        public:
            const Float2                        getMenuBarThickness() const noexcept;
            Rect                                getControlRect() const noexcept;
            Rect                                getControlPaddedRect() const noexcept;
            const Float4                        getControlCenterPosition() const noexcept;
            const Float2                        getControlLeftCenterPosition() const noexcept;
            const Float2                        getControlRightCenterPosition() const noexcept;

        public:
            uint8                               _updateCount;
            Float2                              _size;

            // In screen space, at left-top corner
            REFLECTION_MEMBER(Float2, _position);

            Option                              _option;
            PerFrameData                        _perFrameData;
            Float2                              _childDisplayOffset; // Used for scrolling child controls (of Window control)
            ResizingMask                        _resizingMask;
            Rect                                _positionConstraintsForDragging; // MUST set all four values if want to limit dragging area
            ControlID                           _delegateControlID; // Used for drag, resize and focus
            REFLECTION_MEMBER(StringW, _text);
            ControlValue                        _controlValue;
            RendererContextLayer                _rendererContextLayer;
            DockRelatedData                     _dockRelatedData;
            ControlAccessData                   _controlAccessData;

        private:
            ControlType                         _controlType;
            REFLECTION_MEMBER(ControlID, _id);
            ControlID                           _parentId;
            Rect                                _innerPadding; // For child controls
            Float2                              _minSize;
            Float2                              _interactionSize; // _nonDockInteractionSize + dock size
            Float2                              _nonDockInteractionSize; // Exluces dock area
            VisibleState                        _visibleState;
            Rect                                _clipRect;
            Rect                                _clipRectForChildren; // Used by window
            Rect                                _clipRectForDocks;
            HashMap<ControlID, bool>            _childWindowIDMap;
            LastFrameData                       _lastFrameData;

        private:
            REFLECTION_BIND_BEGIN;
                REFLECTION_BIND(_position);
                REFLECTION_BIND(_text);
                REFLECTION_BIND(_id);
            REFLECTION_BIND_END;
        };
    }
}


#endif // !MINT_GUI_CONTROL_DATA_H

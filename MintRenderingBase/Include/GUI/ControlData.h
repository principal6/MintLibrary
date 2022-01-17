#pragma once


#ifndef MINT_GUI_CONTROL_DATA_H
#define MINT_GUI_CONTROL_DATA_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/String.h>
#include <MintContainer/Include/HashMap.h>

#include <MintMath/Include/Rect.h>

#include <MintRenderingBase/Include/GUI/GUICommon.h>
#include <MintRenderingBase/Include/GUI/ControlValue.h>

#include <MintReflection/Include/Reflection.h>


namespace mint
{
    namespace GUI
    {
        class ControlData;
        class ControlMetaStateSet;


        static constexpr float          kControlDisplayMinWidth = 10.0f;
        static constexpr float          kControlDisplayMinHeight = 10.0f;


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
            static ResizingMask     fromDockZone(const DockZone dockZone) noexcept;

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


        class DockZoneData
        {
        public:
                                DockZoneData();
                                ~DockZoneData() = default;

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
            int32               _focusedDockedControlIndex;
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
            struct UpdateParam
            {
            public:
                Rect            _innerPadding;
                Float2          _autoComputedDisplaySize = Float2::kZero;
                ResizingMask    _initialResizingMask;
                Float2          _offset = Float2::kZero;
                Float2          _desiredPositionInParent = Float2::kNan;
                Float2          _deltaInteractionSize = Float2::kZero;
                Float2          _deltaInteractionSizeByDock = Float2::kZero;
                Float2          _minSize = Float2(kControlDisplayMinWidth, kControlDisplayMinHeight);
                bool            _alwaysResetDisplaySize = true;
                bool            _alwaysResetParent = false;
                ControlID       _parentIDOverride = ControlID();
                bool            _alwaysResetPosition = true;
                bool            _ignoreMeForContentAreaSize = false;
                ClipRectUsage   _clipRectUsage = ClipRectUsage::ParentsChild;
            };

            class DockContext
            {
                friend ControlData;

            public:
                                                DockContext(const ControlType controlType);
                                                ~DockContext() = default;

            public:
                const bool                      isDock() const noexcept;

            public:
                DockingControlType              _dockingControlType;
                DockZone                        _lastDockZone;
                DockZone                        _lastDockZoneCandidate;

            private:
                DockZoneData                    _dockZoneDatas[static_cast<uint32>(DockZone::COUNT)];
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
                void                reset(const float nextChildOffsetY, const Rect& innerPadding);

                Float2              _deltaPosition; // Used for dragging
                Float2              _nextChildOffset; // Every new child sets this offset to compute next _childAt
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

            struct ClipRects
            {
                Rect                _forMe;
                Rect                _forChildren; // Used by window
                Rect                _forDocks;
            };

        public:
                                                REFLECTION_CLASS(ControlData);
                                                ControlData();
                                                ControlData(const ControlID& id, const ControlID& parentID, const ControlType controlType);
                                                ControlData(const ControlID& id, const ControlID& parentID, const ControlType controlType, const Float2& size);
    
        public:
            void                                clearPerFrameData() noexcept;
            void                                updatePerFrame(const UpdateParam& updateParam, ControlData& parentControlData, const ControlMetaStateSet& controlMetaStateSet, const float availableDisplaySizeX, const bool computeSize) noexcept;
            const bool                          needInitialization() const noexcept;

        public:
            const ControlID&                    getID() const noexcept;
            void                                setParentID(const ControlID& parentID) noexcept;
            const ControlID&                    getParentID() const noexcept;
            const int16                         getLastAddedChildIndex() const noexcept;
            const Vector<ControlID>&            getChildControlIDs() const noexcept;
            const uint16                        getChildControlCount() const noexcept;
            const uint16                        getMaxChildControlCount() const noexcept;
            const bool                          hasChildWindow() const noexcept;

        public:
            const ControlType                   getControlType() const noexcept;
            const bool                          isTypeOf(const ControlType controlType) const noexcept;
            const bool                          isInputBoxType() const noexcept;
            const bool                          isRootControl() const noexcept;
            const bool                          isResizable() const noexcept;

        public:
            const Rect&                         getInnerPadding() const noexcept;
            const Float2&                       getMinSize() const noexcept;
            const Float2                        getResizeMinSize() const noexcept;
            const Float2&                       getInteractionSize() const noexcept;
            const Float2&                       getInnerInteractionSize() const noexcept;
            const Float2&                       getContentAreaSize() const noexcept;
            const Float2&                       getChildAt() const noexcept;
            Float2                              computeInnerDisplaySize() const noexcept;
            const float                         computeScrollDisplayWidth() const noexcept;
            const float                         computeScrollDisplayHeight() const noexcept;

        public:
            const bool                          updateVisibleState(const VisibleState visibleState) noexcept;
            const bool                          isControlVisible() const noexcept;

        public:
            void                                setAllClipRects(const Rect& clipRect) noexcept;
            void                                setClipRectForMe(const Rect& clipRect) noexcept;
            void                                setClipRectForChildren(const Rect& clipRect) noexcept;
            void                                setClipRectForDocks(const Rect& clipRect) noexcept;
            const ClipRects&                    getClipRects() const noexcept;
            
        public:
            DockZoneData&                       getDockZoneData(const DockZone dockZone) noexcept;
            const DockZoneData&                 getDockZoneData(const DockZone dockZone) const noexcept;
            const bool                          isFocusedDocker(const ControlData& dockerControlData) const noexcept;
            void                                setDockZoneSize(const DockZone dockZone, const Float2& dockSize) noexcept;
            const Float2                        getDockZoneSize(const DockZone dockZone) const noexcept;
            const Float2                        getDockZoneSizeCached(const DockZone dockZone) const noexcept;
            const float                         getHorzDockZoneSize() const noexcept;
            const float                         getVertDockZoneSize() const noexcept;
            const Float2                        getDockZonePosition(const DockZone dockZone) const noexcept;
            const Float2                        getDockZonePositionCached(const DockZone dockZone) const noexcept;
            const Float2                        getDockOffsetSize() const noexcept;
            void                                connectToDock(const ControlID& dockControlID) noexcept;
            void                                disconnectFromDock() noexcept;
            const ControlID&                    getDockControlID() const noexcept;
            const bool                          isDocking() const noexcept;
            const bool                          isDockHosting() const noexcept;
            void                                swapDockingStateContext() noexcept;

        public:
            const Float2                        getMenuBarThickness() const noexcept;
            Rect                                getRect() const noexcept;
            Rect                                getControlPaddedRect() const noexcept;
            const Float4                        getControlCenterPosition() const noexcept;
            const Float2                        getControlLeftCenterPosition() const noexcept;
            const Float2                        getControlRightCenterPosition() const noexcept;

        private:
            void                                updateSize(const UpdateParam& updateParam, const ControlMetaStateSet& controlMetaStateSet, const float availableDisplaySizeX, const bool compute) noexcept;
            const Float2                        getDockZonePositionInternal(const DockZone dockZone, const Float2& dockSize) const noexcept;

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
            ControlID                           _titleBarControlID; // Used for window ...
            REFLECTION_MEMBER(StringW, _text);
            ControlValue                        _controlValue;
            RendererContextLayer                _rendererContextLayer;
            DockContext                         _dockContext;
            ControlAccessData                   _controlAccessData;

        private:
            ControlType                         _controlType;
            REFLECTION_MEMBER(ControlID, _id);
            ControlID                           _parentID;
            Rect                                _innerPadding; // For child controls
            Float2                              _minSize;
            Float2                              _interactionSize; // _nonDockInteractionSize + dock size
            Float2                              _nonDockInteractionSize; // Exluces dock area
            VisibleState                        _visibleState;
            ClipRects                           _clipRects;
            LastFrameData                       _lastFrameData;
            bool                                _hasChildWindow;

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

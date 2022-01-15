#pragma once


#include <stdafx.h>
#include <MintRenderingBase/Include/GUI/ControlData.h>

#include <MintContainer/Include/String.hpp>
#include <MintContainer/Include/Hash.hpp>

#include <MintRenderingBase/Include/GUI/ControlMetaStateSet.h>


namespace mint
{
#pragma region ControlID
    template <>
    MINT_INLINE const uint64 computeHash(const GUI::ControlID& controlID) noexcept
    {
        return computeHash(controlID.getRawValue());
    }
#pragma endregion


    namespace GUI
    {
#pragma region ControlValue
        inline ControlValue::ControlValue()
            : _commonData{}
            , _textBoxData{}
        {
            __noop;
        }

        MINT_INLINE void GUI::ControlValue::CommonData::enableScrollBar(const ScrollBarType scrollBarType) noexcept
        {
            if (_scrollBarType != ScrollBarType::Both && scrollBarType != ScrollBarType::None)
            {
                if (_scrollBarType == ScrollBarType::Horz && scrollBarType != ScrollBarType::Horz)
                {
                    _scrollBarType = ScrollBarType::Both;
                }
                else if (_scrollBarType == ScrollBarType::Vert && scrollBarType != ScrollBarType::Vert)
                {
                    _scrollBarType = ScrollBarType::Both;
                }
                else
                {
                    _scrollBarType = scrollBarType;
                }
            }
        }

        MINT_INLINE void ControlValue::CommonData::disableScrollBar(const ScrollBarType scrollBarType) noexcept
        {
            if (_scrollBarType != ScrollBarType::None && scrollBarType != ScrollBarType::None)
            {
                if (scrollBarType == ScrollBarType::Both)
                {
                    _scrollBarType = ScrollBarType::None;
                }
                else if (scrollBarType == ScrollBarType::Vert)
                {
                    if (_scrollBarType == ScrollBarType::Vert)
                    {
                        _scrollBarType = ScrollBarType::None;
                    }
                    else
                    {
                        _scrollBarType = ScrollBarType::Horz;
                    }
                }
                else if (scrollBarType == ScrollBarType::Horz)
                {
                    if (_scrollBarType == ScrollBarType::Horz)
                    {
                        _scrollBarType = ScrollBarType::None;
                    }
                    else
                    {
                        _scrollBarType = ScrollBarType::Vert;
                    }
                }
            }
        }

        MINT_INLINE const bool ControlValue::CommonData::isScrollBarEnabled(const ScrollBarType scrollBarType) const noexcept
        {
            MINT_ASSERT("김장원", scrollBarType != ScrollBarType::None, "잘못된 질문입니다.");

            if (_scrollBarType == ScrollBarType::Both)
            {
                return true;
            }
            return (_scrollBarType == scrollBarType);
        }
#pragma endregion


#pragma region ResizingMask
        inline ResizingMask::ResizingMask()
            : _rawMask{ 0 }
        {
            __noop;
        }

        inline ResizingMask ResizingMask::fromDockZone(const DockZone dockZone) noexcept
        {
            ResizingMask result;
            result.setAllFalse();

            switch (dockZone)
            {
            case GUI::DockZone::LeftSide:
                result._right = true;
                break;
            case GUI::DockZone::RightSide:
                result._left = true;
                break;
            case GUI::DockZone::TopSide:
                result._bottom = true;
                break;
            case GUI::DockZone::BottomSide:
                result._top = true;
                break;
            case GUI::DockZone::COUNT:
                break;
            default:
                break;
            }

            return result;
        }

        MINT_INLINE void ResizingMask::setAllTrue() noexcept
        {
            _rawMask = 0xFF;
        }

        MINT_INLINE void ResizingMask::setAllFalse() noexcept
        {
            _rawMask = 0;
        }

        MINT_INLINE const bool ResizingMask::isResizable() const noexcept
        {
            return (_rawMask != 0);
        }

        MINT_INLINE const bool ResizingMask::topLeft() const noexcept
        {
            return (_top && _left);
        }

        MINT_INLINE const bool ResizingMask::topRight() const noexcept
        {
            return (_top && _right);
        }

        MINT_INLINE const bool ResizingMask::bottomLeft() const noexcept
        {
            return (_bottom && _left);
        }

        MINT_INLINE const bool ResizingMask::bottomRight() const noexcept
        {
            return (_bottom && _right);
        }

        MINT_INLINE const bool ResizingMask::overlaps(const ResizingMask& rhs) const noexcept
        {
            return ((_rawMask & rhs._rawMask) != 0);
        }
#pragma endregion


#pragma region DockZoneData
        inline DockZoneData::DockZoneData()
            : _focusedDockedControlIndex{ 0 }
        {
            __noop;
        }

        MINT_INLINE const bool DockZoneData::hasDockedControls() const noexcept
        {
            return !_dockedControlIDArray.empty();
        }

        MINT_INLINE const bool DockZoneData::isRawDockSizeSet() const noexcept
        {
            return _rawDockSize != Float2::kZero;
        }

        MINT_INLINE void DockZoneData::setRawDockSize(const Float2& rawDockSize) noexcept
        {
            _rawDockSize = rawDockSize;
        }

        MINT_INLINE const Float2& DockZoneData::getRawDockSizeXXX() const noexcept
        {
            return _rawDockSize;
        }

        MINT_INLINE void DockZoneData::swapDockedControlsXXX(const int32 indexA, const int32 indexB) noexcept
        {
            std::swap(_dockedControlIDArray[indexA], _dockedControlIDArray[indexB]);
        }

        MINT_INLINE const int32 DockZoneData::getDockedControlIndex(const ControlID& dockedControlID) const noexcept
        {
            const int32 dockedControlCount = static_cast<int32>(_dockedControlIDArray.size());
            for (int32 dockedControlIndex = 0; dockedControlIndex < dockedControlCount; ++dockedControlIndex)
            {
                if (_dockedControlIDArray[dockedControlIndex] == dockedControlID)
                {
                    return dockedControlIndex;
                }
            }
            return -1;
        }

        MINT_INLINE const ControlID DockZoneData::getDockedControlID(const int32 dockedControlIndex) const noexcept
        {
            return _dockedControlIDArray[dockedControlIndex];
        }

        MINT_INLINE const float DockZoneData::getDockedControlTitleBarOffset(const int32 dockedControlIndex) const noexcept
        {
            return _dockedControlTitleBarOffsetArray[dockedControlIndex];
        }

        MINT_INLINE const int32 DockZoneData::getDockedControlIndexByMousePosition(const float relativeMousePositionX) const noexcept
        {
            int32 resultIndex = -1;
            float widthSum = 0.0f;
            const int32 dockedControlCount = static_cast<int32>(_dockedControlTitleBarWidthArray.size());
            for (int32 dockedControlIndex = 0; dockedControlIndex < dockedControlCount; ++dockedControlIndex)
            {
                if (widthSum <= relativeMousePositionX && relativeMousePositionX <= widthSum + _dockedControlTitleBarWidthArray[dockedControlIndex])
                {
                    resultIndex = dockedControlIndex;
                }

                widthSum += _dockedControlTitleBarWidthArray[dockedControlIndex];
            }

            return resultIndex;
        }
#pragma endregion


#pragma region ControlData - DockContext
        inline ControlData::DockContext::DockContext(const ControlType controlType)
            : _dockingControlType{ (controlType == ControlType::ROOT) ? DockingControlType::Dock : DockingControlType::None }
            , _lastDockZone{ DockZone::COUNT }
            , _lastDockZoneCandidate{ DockZone::COUNT }
            , _dockControlID{ 0 }
        {
            __noop;
        }
#pragma endregion

#pragma region ControlData - Option
        inline ControlData::Option::Option()
            : _isFocusable{ false }
            , _needDoubleClickToFocus{ false }
            , _isDraggable{ false }
            , _isInteractableOutsideParent{ false }
        {
            __noop;
        }
#pragma endregion
        
#pragma region ControlData - PerFrameData
        inline void ControlData::PerFrameData::reset(const float nextChildOffsetY, const Rect& innerPadding)
        {
            _deltaPosition.setZero();
            _nextChildOffset.setZero();
            _nextChildOffset._y = innerPadding.top() + nextChildOffsetY;
            _contentAreaSize.setZero();
            _childControlIDs.clear();
            _childAt.set(innerPadding.left(), innerPadding.top());
        }
#pragma endregion
        
#pragma region ControlData - LastFrameData
        inline ControlData::LastFrameData::LastFrameData()
            : _maxChildControlCount{ 0 }
        {
            __noop;
        }
#pragma endregion

#pragma region ControlData
        inline ControlData::ControlData()
            : ControlData(ControlID(), ControlID(), ControlType::ROOT)
        {
            initializeReflection();
        }

        inline ControlData::ControlData(const ControlID& id, const ControlID& parentID, const ControlType controlType)
            : ControlData(id, parentID, controlType, Float2::kNan)
        {
            __noop;
        }

        inline ControlData::ControlData(const ControlID& id, const ControlID& parentID, const ControlType controlType, const Float2& size)
            : _updateCount{ 0 }
            , _interactionSize{ size }
            , _nonDockInteractionSize{ size }
            , _size{ size }
            , _minSize{ kControlDisplayMinWidth, kControlDisplayMinHeight }
            , _delegateControlID{ 0 }
            , _dockContext{ controlType }
            , _rendererContextLayer{ RendererContextLayer::Background }
            , _id{ id }
            , _parentID{ parentID }
            , _controlType{ controlType }
            , _visibleState{ VisibleState::Visible }
            , _hasChildWindow{ false }
        {
            _positionConstraintsForDragging.setNan();

            _controlAccessData._controlID = _id;
        }

        MINT_INLINE void ControlData::clearPerFrameData() noexcept
        {
            _lastFrameData._contentAreaSize = _perFrameData._contentAreaSize;
            _lastFrameData._childControlIDs = _perFrameData._childControlIDs;
            _lastFrameData._maxChildControlCount = max(_lastFrameData._maxChildControlCount, static_cast<uint16>(_lastFrameData._childControlIDs.size()));

            _controlAccessData._childControlIDs = _lastFrameData._childControlIDs;

            const float nextChildOffsetY = isTypeOf(ControlType::Window) ? _controlValue._windowData._titleBarThickness : 0.0f;
            _perFrameData.reset(nextChildOffsetY, _innerPadding);
        }
        
        MINT_INLINE void ControlData::updatePerFrame(const UpdateControlDataParam& updateControlDataParam, ControlData& parentControlData, const ControlMetaStateSet& controlMetaStateSet, const float availableDisplaySizeX, const bool computeSize) noexcept
        {
            clearPerFrameData();

            parentControlData._perFrameData._childControlIDs.push_back(_id);
            if (isTypeOf(ControlType::Window))
            {
                parentControlData._hasChildWindow = true;
            }

            updateSize(updateControlDataParam, controlMetaStateSet, availableDisplaySizeX, computeSize);

            // Drag constraints 적용! (Dragging 이 아닐 때도 Constraint 가 적용되어야 함.. 예를 들면 resizing 중에!)
            if (_option._isDraggable == true && _positionConstraintsForDragging.isNan() == false)
            {
                _position._x = min(max(_positionConstraintsForDragging.left(), _position._x), _positionConstraintsForDragging.right());
                _position._y = min(max(_positionConstraintsForDragging.top(), _position._y), _positionConstraintsForDragging.bottom());
            }

            switch (updateControlDataParam._clipRectUsage)
            {
            case GUI::ClipRectUsage::ParentsOwn:
                setClipRectForMe(parentControlData.getClipRects()._forMe);
                break;
            case GUI::ClipRectUsage::ParentsChild:
                setClipRectForMe(parentControlData.getClipRects()._forChildren);
                break;
            case GUI::ClipRectUsage::ParentsDock:
                setClipRectForMe(parentControlData.getClipRects()._forDocks);
                break;
            default:
                break;
            }
        }

        MINT_INLINE void ControlData::updateSize(const UpdateControlDataParam& updateControlDataParam, const ControlMetaStateSet& controlMetaStateSet, const float availableDisplaySizeX, const bool compute) noexcept
        {
            Float2 desiredSize = controlMetaStateSet.getNextDesiredSize();
            if (compute)
            {
                const float maxDisplaySizeX = availableDisplaySizeX;
                if (desiredSize._x <= 0.0f)
                {
                    desiredSize._x = updateControlDataParam._autoComputedDisplaySize._x;
                }
                if (desiredSize._y <= 0.0f)
                {
                    desiredSize._y = updateControlDataParam._autoComputedDisplaySize._y;
                }

                if (controlMetaStateSet.getNextUseSizeConstraintToParent() == false)
                {
                    _size = desiredSize;
                }
                else
                {
                    if (controlMetaStateSet.getNextSizeForced() == false)
                    {
                        desiredSize._x = min(maxDisplaySizeX, desiredSize._x);
                    }

                    _size = desiredSize;
                }
            }

            _minSize = updateControlDataParam._minSize;
            _innerPadding = updateControlDataParam._innerPadding;
            _interactionSize = _size + updateControlDataParam._deltaInteractionSize;
            _nonDockInteractionSize = _interactionSize + updateControlDataParam._deltaInteractionSizeByDock;
        }

        MINT_INLINE const bool ControlData::needInitialization() const noexcept
        {
            return _updateCount <= 1;
        }

        MINT_INLINE const ControlID& ControlData::getID() const noexcept
        {
            return _id;
        }

        MINT_INLINE void ControlData::setParentID(const ControlID& parentID) noexcept
        {
            _parentID = parentID;
        }

        MINT_INLINE const ControlID& ControlData::getParentID() const noexcept
        {
            return _parentID;
        }

        MINT_INLINE const int16 ControlData::getLastAddedChildIndex() const noexcept
        {
            return static_cast<int16>(_perFrameData._childControlIDs.size() - 1);
        }

        MINT_INLINE const Vector<ControlID>& ControlData::getChildControlIDs() const noexcept
        {
            return _lastFrameData._childControlIDs;
        }

        MINT_INLINE const uint16 ControlData::getChildControlCount() const noexcept
        {
            return static_cast<uint16>(_lastFrameData._childControlIDs.size());
        }
        
        MINT_INLINE const uint16 ControlData::getMaxChildControlCount() const noexcept
        {
            return _lastFrameData._maxChildControlCount;
        }

        MINT_INLINE const bool ControlData::hasChildWindow() const noexcept
        {
            return _hasChildWindow;
        }

        MINT_INLINE const Rect& ControlData::getInnerPadding() const noexcept
        {
            return _innerPadding;
        }

        MINT_INLINE Float2 ControlData::computeInnerDisplaySize() const noexcept
        {
            Float2 result = Float2(_size._x - _innerPadding.horz(), _size._y - _innerPadding.vert());
            if (isTypeOf(ControlType::Window))
            {
                result._y -= _controlValue._windowData._titleBarThickness;
                result._y -= getMenuBarThickness()._y;
            }
            return result;
        }

        MINT_INLINE const Float2 ControlData::getResizeMinSize() const noexcept
        {
            return _minSize + Float2(getHorzDockZoneSize(), getVertDockZoneSize());
        }

        MINT_INLINE const Float2& ControlData::getInteractionSize() const noexcept
        {
            return _interactionSize;
        }

        MINT_INLINE const Float2& ControlData::getInnerInteractionSize() const noexcept
        {
            return isDockHosting() ? _nonDockInteractionSize : _interactionSize;
        }

        MINT_INLINE const Float2& ControlData::getContentAreaSize() const noexcept
        {
            return _lastFrameData._contentAreaSize;
        }

        MINT_INLINE const Float2& ControlData::getChildAt() const noexcept
        {
            return _perFrameData._childAt;
        }

        MINT_INLINE const float ControlData::computeScrollDisplayWidth() const noexcept
        {
            return max(0.0f,
                _size._x - getHorzDockZoneSize() - _innerPadding.horz()
                - ((_controlValue._commonData.isScrollBarEnabled(ScrollBarType::Vert) == true) ? kScrollBarThickness * 2.0f : 0.0f)
                - getMenuBarThickness()._x
            );
        }

        MINT_INLINE const float ControlData::computeScrollDisplayHeight() const noexcept
        {
            const float titleBarHeight = (_controlType == GUI::ControlType::Window) ? _controlValue._windowData._titleBarThickness : 0.0f;
            return max(0.0f,
                _size._y - getVertDockZoneSize() - titleBarHeight - _innerPadding.vert()
                - ((_controlValue._commonData.isScrollBarEnabled(ScrollBarType::Horz) == true) ? kScrollBarThickness * 2.0f : 0.0f)
                - getMenuBarThickness()._y
            );
        }

        MINT_INLINE const ControlType ControlData::getControlType() const noexcept
        {
            return _controlType;
        }

        MINT_INLINE const bool ControlData::isTypeOf(const ControlType controlType) const noexcept
        {
            return (controlType == _controlType);
        }

        MINT_INLINE const bool ControlData::isInputBoxType() const noexcept
        {
            return (ControlType::TextBox == _controlType) || (ControlType::ValueSlider == _controlType);
        }

        MINT_INLINE const bool ControlData::isRootControl() const noexcept
        {
            return _controlType == ControlType::ROOT;
        }

        MINT_INLINE const bool ControlData::isResizable() const noexcept
        {
            return _resizingMask.isResizable();
        }

        MINT_INLINE const bool ControlData::updateVisibleState(const VisibleState visibleState) noexcept
        {
            if (visibleState != _visibleState)
            {
                _visibleState = visibleState;
                return true;
            }
            return false;
        }

        MINT_INLINE const bool ControlData::isControlVisible() const noexcept
        {
            return (_visibleState != VisibleState::Invisible);
        }

        MINT_INLINE const ControlData::ClipRects& ControlData::getClipRects() const noexcept
        {
            return _clipRects;
        }

        MINT_INLINE DockZoneData& ControlData::getDockZoneData(const DockZone dockZone) noexcept
        {
            return _dockContext._dockZoneDatas[static_cast<uint32>(dockZone)];
        }

        MINT_INLINE const DockZoneData& ControlData::getDockZoneData(const DockZone dockZone) const noexcept
        {
            return _dockContext._dockZoneDatas[static_cast<uint32>(dockZone)];
        }

        MINT_INLINE const bool ControlData::isFocusedDocker(const ControlData& dockerControlData) const noexcept
        {
            const DockZoneData& dockZoneData = getDockZoneData(dockerControlData._dockContext._lastDockZone);
            return dockZoneData.getDockedControlIndex(dockerControlData.getID()) == dockZoneData._focusedDockedControlIndex;
        }

        MINT_INLINE void ControlData::setDockZoneSize(const DockZone dockZone, const Float2& dockSize) noexcept
        {
            getDockZoneData(dockZone).setRawDockSize(dockSize);
        }

        MINT_INLINE const Float2 ControlData::getDockZoneSize(const DockZone dockZone) const noexcept
        {
            const DockZoneData& dockZoneData = getDockZoneData(dockZone);
            if (dockZoneData.hasDockedControls() == false)
            {
                return Float2::kZero;
            }

            const DockZoneData& dockZoneDataTopSide = getDockZoneData(DockZone::TopSide);
            const DockZoneData& dockZoneDataBottomSide = getDockZoneData(DockZone::BottomSide);
            const Float2 innerDisplaySize = computeInnerDisplaySize();
            Float2 resultDockSize = dockZoneData.getRawDockSizeXXX();
            switch (dockZone)
            {
            case GUI::DockZone::LeftSide:
            case GUI::DockZone::RightSide:
                resultDockSize._y = innerDisplaySize._y;
                if (dockZoneDataTopSide.hasDockedControls() == true)
                {
                    resultDockSize._y -= dockZoneDataTopSide.getRawDockSizeXXX()._y;
                }
                if (dockZoneDataBottomSide.hasDockedControls() == true)
                {
                    resultDockSize._y -= dockZoneDataBottomSide.getRawDockSizeXXX()._y;
                }
                break;
            case GUI::DockZone::TopSide:
            case GUI::DockZone::BottomSide:
                resultDockSize._x = innerDisplaySize._x;
                break;
            case GUI::DockZone::COUNT:
                break;
            default:
                break;
            }
            return resultDockSize;
        }

        MINT_INLINE const float ControlData::getHorzDockZoneSize() const noexcept
        {
            return getDockZoneSize(DockZone::LeftSide)._x + getDockZoneSize(DockZone::RightSide)._x;
        }

        MINT_INLINE const float ControlData::getVertDockZoneSize() const noexcept
        {
            return getDockZoneSize(DockZone::TopSide)._y + getDockZoneSize(DockZone::BottomSide)._y;
        }

        MINT_INLINE const Float2 ControlData::getDockZonePosition(const DockZone dockZone) const noexcept
        {
            const Float2& dockSize = getDockZoneSize(dockZone);
            const Float2& offset = getDockOffsetSize();

            Float2 resultDockPosition;
            switch (dockZone)
            {
            case GUI::DockZone::LeftSide:
                resultDockPosition = _position + offset;
                resultDockPosition._y += getDockZoneSize(DockZone::TopSide)._y;
                break;
            case GUI::DockZone::RightSide:
                resultDockPosition = Float2(_position._x + _size._x - dockSize._x, _position._y) + offset;
                resultDockPosition._y += getDockZoneSize(DockZone::TopSide)._y;
                break;
            case GUI::DockZone::TopSide:
                resultDockPosition = Float2(_position._x, _position._y) + offset;
                break;
            case GUI::DockZone::BottomSide:
                resultDockPosition = Float2(_position._x, _position._y + _size._y - dockSize._y);
                break;
            case GUI::DockZone::COUNT:
            default:
                break;
            }
            return resultDockPosition;
        }

        MINT_INLINE const Float2 ControlData::getDockOffsetSize() const noexcept
        {
            return Float2(0.0f, ((_controlType == ControlType::Window) ? _controlValue._windowData._titleBarThickness + _innerPadding.top() : 0.0f) + getMenuBarThickness()._y);
        }

        MINT_INLINE const Float2 ControlData::getMenuBarThickness() const noexcept
        {
            Float2 result;
            const MenuBarType currentMenuBarType = _controlValue._commonData._menuBarType;
            if (currentMenuBarType == MenuBarType::Top || currentMenuBarType == MenuBarType::Bottom)
            {
                result._y = kMenuBarBaseSize._y;
            }
            return result;
        }

        MINT_INLINE void ControlData::connectToDock(const ControlID& dockControlID) noexcept
        {
            _dockContext._dockControlID = dockControlID;
        }

        MINT_INLINE void ControlData::disconnectFromDock() noexcept
        {
            _dockContext._dockControlID.reset();
        }

        MINT_INLINE const ControlID& ControlData::getDockControlID() const noexcept
        {
            return _dockContext._dockControlID;
        }

        MINT_INLINE const bool ControlData::isDocking() const noexcept
        {
            return _dockContext._dockControlID.isValid();
        }

        MINT_INLINE const bool ControlData::isDockHosting() const noexcept
        {
            for (DockZone dockZoneIter = static_cast<DockZone>(0); dockZoneIter != DockZone::COUNT; dockZoneIter = static_cast<DockZone>(static_cast<uint32>(dockZoneIter) + 1))
            {
                const DockZoneData& dockZoneData = getDockZoneData(dockZoneIter);
                if (dockZoneData.hasDockedControls() == true)
                {
                    return true;
                }
            }
            return false;
        }

        MINT_INLINE Rect ControlData::getRect() const noexcept
        {
            return Rect::fromPositionSize(_position, _size);
        }

        MINT_INLINE Rect ControlData::getControlPaddedRect() const noexcept
        {
            return Rect
            (
                _position._x + _innerPadding.left(),
                _position._x + _size._x - _innerPadding.horz(),
                _position._y + _innerPadding.top(),
                _position._y + _size._y - _innerPadding.vert()
            );
        }

        MINT_INLINE void ControlData::swapDockingStateContext() noexcept
        {
            std::swap(_size, _dockContext._dokcingStateContext._size);
            std::swap(_resizingMask, _dockContext._dokcingStateContext._resizingMask);
        }

        MINT_INLINE void ControlData::setAllClipRects(const Rect& clipRect) noexcept
        {
            setClipRectForMe(clipRect);
            setClipRectForChildren(clipRect);
            setClipRectForDocks(clipRect);
        }

        MINT_INLINE void ControlData::setClipRectForMe(const Rect& clipRect) noexcept
        {
            _clipRects._forMe = clipRect;
        }

        MINT_INLINE void ControlData::setClipRectForChildren(const Rect& clipRect) noexcept
        {
            _clipRects._forChildren = clipRect;
        }

        MINT_INLINE void ControlData::setClipRectForDocks(const Rect& clipRect) noexcept
        {
            _clipRects._forDocks = clipRect;
        }

        MINT_INLINE const Float4 ControlData::getControlCenterPosition() const noexcept
        {
            return Float4(_position._x + _size._x * 0.5f, _position._y + _size._y * 0.5f, 0.0f, 1.0f);
        }

        MINT_INLINE const Float2 ControlData::getControlLeftCenterPosition() const noexcept
        {
            return Float2(_position._x, _position._y + _size._y * 0.5f);
        }

        MINT_INLINE const Float2 ControlData::getControlRightCenterPosition() const noexcept
        {
            return Float2(_position._x + _size._x, _position._y + _size._y * 0.5f);
        }
#pragma endregion
    }
}

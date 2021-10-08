#pragma once


#include <stdafx.h>
#include <MintRenderingBase/Include/Gui/ControlData.h>

#include <MintContainer/Include/String.hpp>
#include <MintContainer/Include/Hash.hpp>


namespace mint
{
#pragma region ControlId
    template <>
    MINT_INLINE const uint64 computeHash(const Gui::ControlId& controlId) noexcept
    {
        return computeHash(controlId.getRawValue());
    }
#pragma endregion


    namespace Gui
    {
#pragma region ControlValue
        inline ControlValue::ControlValue()
        {
            __noop;
        }

        MINT_INLINE void Gui::ControlValue::CommonData::enableScrollBar(const ScrollBarType scrollBarType) noexcept
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

        inline ResizingMask ResizingMask::fromDockingMethod(const DockingMethod dockingMethod) noexcept
        {
            ResizingMask result;
            result.setAllFalse();

            switch (dockingMethod)
            {
            case Gui::DockingMethod::LeftSide:
                result._right = true;
                break;
            case Gui::DockingMethod::RightSide:
                result._left = true;
                break;
            case Gui::DockingMethod::TopSide:
                result._bottom = true;
                break;
            case Gui::DockingMethod::BottomSide:
                result._top = true;
                break;
            case Gui::DockingMethod::COUNT:
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


#pragma region DockDatum
        inline DockDatum::DockDatum()
            : _dockedControlIndexShown{ 0 }
        {
            __noop;
        }

        MINT_INLINE const bool DockDatum::hasDockedControls() const noexcept
        {
            return !_dockedControlIdArray.empty();
        }

        MINT_INLINE const bool DockDatum::isRawDockSizeSet() const noexcept
        {
            return _rawDockSize != Float2::kZero;
        }

        MINT_INLINE void DockDatum::setRawDockSize(const Float2& rawDockSize) noexcept
        {
            _rawDockSize = rawDockSize;
        }

        MINT_INLINE const Float2& DockDatum::getRawDockSizeXXX() const noexcept
        {
            return _rawDockSize;
        }

        MINT_INLINE void DockDatum::swapDockedControlsXXX(const int32 indexA, const int32 indexB) noexcept
        {
            std::swap(_dockedControlIdArray[indexA], _dockedControlIdArray[indexB]);
        }

        MINT_INLINE const int32 DockDatum::getDockedControlIndex(const ControlId& dockedControlId) const noexcept
        {
            const int32 dockedControlCount = static_cast<int32>(_dockedControlIdArray.size());
            for (int32 dockedControlIndex = 0; dockedControlIndex < dockedControlCount; ++dockedControlIndex)
            {
                if (_dockedControlIdArray[dockedControlIndex] == dockedControlId)
                {
                    return dockedControlIndex;
                }
            }
            return -1;
        }

        MINT_INLINE const ControlId DockDatum::getDockedControlId(const int32 dockedControlIndex) const noexcept
        {
            return _dockedControlIdArray[dockedControlIndex];
        }

        MINT_INLINE const float DockDatum::getDockedControlTitleBarOffset(const int32 dockedControlIndex) const noexcept
        {
            return _dockedControlTitleBarOffsetArray[dockedControlIndex];
        }

        MINT_INLINE const int32 DockDatum::getDockedControlIndexByMousePosition(const float relativeMousePositionX) const noexcept
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


#pragma region ControlData - DockRelatedData
        inline ControlData::DockRelatedData::DockRelatedData(const ControlType controlType)
            : _dockingControlType{ (controlType == ControlType::ROOT) ? DockingControlType::Dock : DockingControlType::None }
            , _lastDockingMethod{ DockingMethod::COUNT }
            , _lastDockingMethodCandidate{ DockingMethod::COUNT }
            , _dockControlId{ 0 }
        {
            __noop;
        }
#pragma endregion

#pragma region ControlData
        inline ControlData::ControlData()
            : ControlData(ControlId(), ControlId(), ControlType::ROOT)
        {
            initializeReflection();
        }

        inline ControlData::ControlData(const ControlId& id, const ControlId& parentId, const ControlType controlType)
            : ControlData(id, parentId, controlType, Float2::kNan)
        {
            __noop;
        }

        inline ControlData::ControlData(const ControlId& id, const ControlId& parentId, const ControlType controlType, const Float2& size)
            : _updateCount{ 0 }
            , _interactionSize{ size }
            , _nonDockInteractionSize{ size }
            , _isFocusable{ false }
            , _needDoubleClickToFocus{ false }
            , _isDraggable{ false }
            , _isInteractableOutsideParent{ false }
            , _displaySize{ size }
            , _displaySizeMin{ kControlDisplayMinWidth, kControlDisplayMinHeight }
            , _childAt{ _innerPadding.left(), _innerPadding.top() }
            , _delegateControlId{ 0 }
            , _dockRelatedData{ controlType }
            , _rendererContextLayer{ RendererContextLayer::Background }
            , _id{ id }
            , _parentId{ parentId }
            , _controlType{ controlType }
            , _visibleState{ VisibleState::Visible }
            , _previousMaxChildControlCount{ 0 }
        {
            _draggingConstraints.setNan();
        }

        MINT_INLINE void ControlData::clearPerFrameData() noexcept
        {
            _nextChildOffset.setZero();
            _previousContentAreaSize = _contentAreaSize;
            _contentAreaSize.setZero();
            _currentFrameDeltaPosition.setZero();
        }

        MINT_INLINE void ControlData::updatePerFrameWithParent(const bool isNewData, const PrepareControlDataParam& prepareControlDataParam, ControlData& parentControlData) noexcept
        {
            clearPerFrameData();

            prepareChildControlIds();

            parentControlData._childControlIds.push_back(_id);
            parentControlData.connectChildWindowIfNot(*this);

            _displaySizeMin = prepareControlDataParam._displaySizeMin;
            _innerPadding = prepareControlDataParam._innerPadding;
            _interactionSize = _displaySize + prepareControlDataParam._deltaInteractionSize;
            _nonDockInteractionSize = _interactionSize + prepareControlDataParam._deltaInteractionSizeByDock;

            // Drag constraints 적용! (Dragging 이 아닐 때도 Constraint 가 적용되어야 함.. 예를 들면 resizing 중에!)
            if (_isDraggable == true && _draggingConstraints.isNan() == false)
            {
                _position._x = min(max(_draggingConstraints.left(), _position._x), _draggingConstraints.right());
                _position._y = min(max(_draggingConstraints.top(), _position._y), _draggingConstraints.bottom());
            }

            switch (prepareControlDataParam._clipRectUsage)
            {
            case Gui::ClipRectUsage::ParentsOwn:
                setClipRectXXX(parentControlData.getClipRect());
                break;
            case Gui::ClipRectUsage::ParentsChild:
                setClipRectXXX(parentControlData.getClipRectForChildren());
                break;
            case Gui::ClipRectUsage::ParentsDock:
                setClipRectXXX(parentControlData.getClipRectForDocks());
                break;
            default:
                break;
            }
        }

        MINT_INLINE const ControlId& ControlData::getId() const noexcept
        {
            return _id;
        }

        MINT_INLINE const ControlId& ControlData::getParentId() const noexcept
        {
            return _parentId;
        }

        MINT_INLINE const Rect& ControlData::getInnerPadding() const noexcept
        {
            return _innerPadding;
        }

        MINT_INLINE Float2 ControlData::getClientSize() const noexcept
        {
            Float2 result = Float2(_displaySize._x - _innerPadding.horz(), _displaySize._y - _innerPadding.vert());
            if (_controlType == ControlType::Window)
            {
                result._y -= kTitleBarBaseThickness;
            }
            return result;
        }

        MINT_INLINE const float ControlData::getTopOffsetToClientArea() const noexcept
        {
            return ((_controlType == ControlType::Window) ? kTitleBarBaseThickness : 0.0f) + getMenuBarThickness()._y;
        }

        MINT_INLINE const Float2& ControlData::getDisplaySizeMin() const noexcept
        {
            return _displaySizeMin;
        }

        MINT_INLINE const float ControlData::getPureDisplayWidth() const noexcept
        {
            const Float2& menuBarThicknes = getMenuBarThickness();
            return max(
                0.0f,
                _displaySize._x - getHorzDockSizeSum() - _innerPadding.horz()
                - ((_controlValue._commonData.isScrollBarEnabled(ScrollBarType::Vert) == true) ? kScrollBarThickness * 2.0f : 0.0f)
                - menuBarThicknes._x
            );
        }

        MINT_INLINE const float ControlData::getPureDisplayHeight() const noexcept
        {
            const float titleBarHeight = (_controlType == Gui::ControlType::Window) ? kTitleBarBaseThickness : 0.0f;
            const Float2& menuBarThicknes = getMenuBarThickness();
            return max(
                0.0f,
                _displaySize._y - getVertDockSizeSum() - titleBarHeight - _innerPadding.vert()
                - ((_controlValue._commonData.isScrollBarEnabled(ScrollBarType::Horz) == true) ? kScrollBarThickness * 2.0f : 0.0f)
                - menuBarThicknes._y
            );
        }

        MINT_INLINE const Float2& ControlData::getInteractionSize() const noexcept
        {
            return _interactionSize;
        }

        MINT_INLINE const Float2& ControlData::getNonDockInteractionSize() const noexcept
        {
            return _nonDockInteractionSize;
        }

        MINT_INLINE const Float2& ControlData::getContentAreaSize() const noexcept
        {
            return _contentAreaSize;
        }

        MINT_INLINE const Float2& ControlData::getPreviousContentAreaSize() const noexcept
        {
            return _previousContentAreaSize;
        }

        MINT_INLINE const Float2& ControlData::getChildAt() const noexcept
        {
            return _childAt;
        }

        MINT_INLINE  const Float2& ControlData::getNextChildOffset() const noexcept
        {
            return _nextChildOffset;
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

        MINT_INLINE const wchar_t* ControlData::getText() const noexcept
        {
            return _text.c_str();
        }

        MINT_INLINE const bool ControlData::isRootControl() const noexcept
        {
            return _controlType == ControlType::ROOT;
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

        MINT_INLINE const Rect& ControlData::getClipRect() const noexcept
        {
            return _clipRect;
        }

        MINT_INLINE const Rect& ControlData::getClipRectForChildren() const noexcept
        {
            return _clipRectForChildren;
        }

        MINT_INLINE const Rect& ControlData::getClipRectForDocks() const noexcept
        {
            return _clipRectForDocks;
        }

        MINT_INLINE const Vector<ControlId>& ControlData::getChildControlIds() const noexcept
        {
            return _childControlIds;
        }

        MINT_INLINE const Vector<ControlId>& ControlData::getPreviousChildControlIds() const noexcept
        {
            return _previousChildControlIds;
        }

        MINT_INLINE const uint16 ControlData::getPreviousChildControlCount() const noexcept
        {
            return static_cast<uint16>(_previousChildControlIds.size());
        }

        MINT_INLINE const uint16 ControlData::getPreviousMaxChildControlCount() const noexcept
        {
            return _previousMaxChildControlCount;
        }

        MINT_INLINE void ControlData::prepareChildControlIds() noexcept
        {
            std::swap(_childControlIds, _previousChildControlIds);
            _previousMaxChildControlCount = max(_previousMaxChildControlCount, static_cast<uint16>(_previousChildControlIds.size()));
            _childControlIds.clear();
        }

        MINT_INLINE const bool ControlData::hasChildWindow() const noexcept
        {
            return !_childWindowIdMap.empty();
        }

        MINT_INLINE DockDatum& ControlData::getDockDatum(const DockingMethod dockingMethod) noexcept
        {
            return _dockRelatedData._dockData[static_cast<uint32>(dockingMethod)];
        }

        MINT_INLINE const DockDatum& ControlData::getDockDatum(const DockingMethod dockingMethod) const noexcept
        {
            return _dockRelatedData._dockData[static_cast<uint32>(dockingMethod)];
        }

        MINT_INLINE const bool ControlData::isShowingInDock(const ControlData& dockedControlData) const noexcept
        {
            const DockDatum& dockDatum = getDockDatum(dockedControlData._dockRelatedData._lastDockingMethod);
            return dockDatum.getDockedControlIndex(dockedControlData.getId()) == dockDatum._dockedControlIndexShown;
        }

        MINT_INLINE void ControlData::setDockSize(const DockingMethod dockingMethod, const Float2& dockSize) noexcept
        {
            getDockDatum(dockingMethod).setRawDockSize(dockSize);
        }

        MINT_INLINE const Float2 ControlData::getDockSize(const DockingMethod dockingMethod) const noexcept
        {
            const Float2& menuBarThickness = getMenuBarThickness();
            const DockDatum& dockDatumTopSide = getDockDatum(DockingMethod::TopSide);
            const DockDatum& dockDatumBottomSide = getDockDatum(DockingMethod::BottomSide);
            const DockDatum& dockDatum = getDockDatum(dockingMethod);
            Float2 resultDockSize = dockDatum.getRawDockSizeXXX();
            switch (dockingMethod)
            {
            case Gui::DockingMethod::LeftSide:
            case Gui::DockingMethod::RightSide:
                resultDockSize._y = getClientSize()._y;
                if (dockDatumTopSide.hasDockedControls() == true)
                {
                    resultDockSize._y -= dockDatumTopSide.getRawDockSizeXXX()._y;
                }
                if (dockDatumBottomSide.hasDockedControls() == true)
                {
                    resultDockSize._y -= dockDatumBottomSide.getRawDockSizeXXX()._y;
                }

                resultDockSize._y -= menuBarThickness._y;
                break;
            case Gui::DockingMethod::TopSide:
            case Gui::DockingMethod::BottomSide:
                resultDockSize._x = getClientSize()._x;
                break;
            case Gui::DockingMethod::COUNT:
                break;
            default:
                break;
            }
            return resultDockSize;
        }

        MINT_INLINE const Float2 ControlData::getDockSizeIfHosting(const DockingMethod dockingMethod) const noexcept
        {
            const DockDatum& dockDatum = getDockDatum(dockingMethod);
            return (dockDatum.hasDockedControls() == true) ? getDockSize(dockingMethod) : Float2::kZero;
        }

        MINT_INLINE const Float2 ControlData::getDockOffsetSize() const noexcept
        {
            return Float2(0.0f, ((_controlType == ControlType::Window) ? kTitleBarBaseThickness + _innerPadding.top() : 0.0f) + getMenuBarThickness()._y);
        }

        MINT_INLINE const Float2 ControlData::getDockPosition(const DockingMethod dockingMethod) const noexcept
        {
            const DockDatum& dockDatumTopSide = getDockDatum(DockingMethod::TopSide);
            const Float2& dockSize = getDockSize(dockingMethod);
            const Float2& offset = getDockOffsetSize();

            Float2 resultDockPosition;
            switch (dockingMethod)
            {
            case Gui::DockingMethod::LeftSide:
                resultDockPosition = _position + offset;
                resultDockPosition._y += getDockSizeIfHosting(DockingMethod::TopSide)._y;
                break;
            case Gui::DockingMethod::RightSide:
                resultDockPosition = Float2(_position._x + _displaySize._x - dockSize._x, _position._y) + offset;
                resultDockPosition._y += getDockSizeIfHosting(DockingMethod::TopSide)._y;
                break;
            case Gui::DockingMethod::TopSide:
                resultDockPosition = Float2(_position._x, _position._y) + offset;
                break;
            case Gui::DockingMethod::BottomSide:
                resultDockPosition = Float2(_position._x, _position._y + _displaySize._y - dockSize._y);
                break;
            case Gui::DockingMethod::COUNT:
            default:
                break;
            }
            return resultDockPosition;
        }

        MINT_INLINE const float ControlData::getHorzDockSizeSum() const noexcept
        {
            return getDockSizeIfHosting(DockingMethod::LeftSide)._x + getDockSizeIfHosting(DockingMethod::RightSide)._x;
        }

        MINT_INLINE const float ControlData::getVertDockSizeSum() const noexcept
        {
            return getDockSizeIfHosting(DockingMethod::TopSide)._y + getDockSizeIfHosting(DockingMethod::BottomSide)._y;
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

        MINT_INLINE void ControlData::connectToDock(const ControlId& dockControlId) noexcept
        {
            _dockRelatedData._dockControlId = dockControlId;
        }

        MINT_INLINE void ControlData::disconnectFromDock() noexcept
        {
            _dockRelatedData._dockControlId.reset();
        }

        MINT_INLINE const ControlId& ControlData::getDockControlId() const noexcept
        {
            return _dockRelatedData._dockControlId;
        }

        MINT_INLINE const bool ControlData::isDocking() const noexcept
        {
            return _dockRelatedData._dockControlId.isValid();
        }

        MINT_INLINE const bool ControlData::isDockHosting() const noexcept
        {
            for (DockingMethod dockingMethodIter = static_cast<DockingMethod>(0); dockingMethodIter != DockingMethod::COUNT; dockingMethodIter = static_cast<DockingMethod>(static_cast<uint32>(dockingMethodIter) + 1))
            {
                const DockDatum& dockDatum = getDockDatum(dockingMethodIter);
                if (dockDatum.hasDockedControls() == true)
                {
                    return true;
                }
            }
            return false;
        }

        MINT_INLINE const bool ControlData::isResizable() const noexcept
        {
            return _resizingMask.isResizable();
        }

        MINT_INLINE Rect ControlData::getControlRect() const noexcept
        {
            return Rect::fromPositionSize(_position, _displaySize);
        }

        MINT_INLINE Rect ControlData::getControlPaddedRect() const noexcept
        {
            return Rect
            (
                _position._x + _innerPadding.left(),
                _position._x + _displaySize._x - _innerPadding.horz(),
                _position._y + _innerPadding.top(),
                _position._y + _displaySize._y - _innerPadding.vert()
            );
        }

        MINT_INLINE void ControlData::connectChildWindowIfNot(const ControlData& childWindowControlData) noexcept
        {
            if (childWindowControlData._controlType == ControlType::Window && _childWindowIdMap.find(childWindowControlData._id).isValid() == false)
            {
                _childWindowIdMap.insert(childWindowControlData._id, true);
            }
        }

        MINT_INLINE void ControlData::disconnectChildWindow(const ControlId& childWindowId) noexcept
        {
            _childWindowIdMap.erase(childWindowId);
        }

        MINT_INLINE const HashMap<ControlId, bool>& ControlData::getChildWindowIdMap() const noexcept
        {
            return _childWindowIdMap;
        }

        MINT_INLINE void ControlData::swapDockingStateContext() noexcept
        {
            std::swap(_displaySize, _dockRelatedData._dokcingStateContext._displaySize);
            std::swap(_resizingMask, _dockRelatedData._dokcingStateContext._resizingMask);
        }

        MINT_INLINE void ControlData::setParentIdXXX(const ControlId& parentId) noexcept
        {
            _parentId = parentId;
        }

        MINT_INLINE void ControlData::setOffsetY_XXX(const float offsetY) noexcept
        {
            _nextChildOffset._y = offsetY;
        }

        MINT_INLINE void ControlData::setClipRectXXX(const Rect& clipRect) noexcept
        {
            _clipRect = clipRect;
        }

        MINT_INLINE void ControlData::setClipRectForChildrenXXX(const Rect& clipRect) noexcept
        {
            _clipRectForChildren = clipRect;
        }

        MINT_INLINE void ControlData::setClipRectForDocksXXX(const Rect& clipRect) noexcept
        {
            _clipRectForDocks = clipRect;
        }

        MINT_INLINE const Float4 ControlData::getControlCenterPosition() const noexcept
        {
            return Float4(_position._x + _displaySize._x * 0.5f, _position._y + _displaySize._y * 0.5f, 0.0f, 1.0f);
        }

        MINT_INLINE const Float2 ControlData::getControlLeftCenterPosition() const noexcept
        {
            return Float2(_position._x, _position._y + _displaySize._y * 0.5f);
        }

        MINT_INLINE const Float2 ControlData::getControlRightCenterPosition() const noexcept
        {
            return Float2(_position._x + _displaySize._x, _position._y + _displaySize._y * 0.5f);
        }
#pragma endregion
    }
}

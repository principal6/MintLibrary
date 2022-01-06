#pragma once


#include <stdafx.h>
#include <MintRenderingBase/Include/Gui/ControlData.h>

#include <MintContainer/Include/String.hpp>
#include <MintContainer/Include/Hash.hpp>

#include <MintRenderingBase/Include/Gui/ControlMetaStateSet.h>


namespace mint
{
#pragma region ControlID
    template <>
    MINT_INLINE const uint64 computeHash(const Gui::ControlID& controlID) noexcept
    {
        return computeHash(controlID.getRawValue());
    }
#pragma endregion


    namespace Gui
    {
#pragma region ControlValue
        inline ControlValue::ControlValue()
            : _commonData{}
            , _textBoxData{}
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
            return !_dockedControlIDArray.empty();
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
            std::swap(_dockedControlIDArray[indexA], _dockedControlIDArray[indexB]);
        }

        MINT_INLINE const int32 DockDatum::getDockedControlIndex(const ControlID& dockedControlID) const noexcept
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

        MINT_INLINE const ControlID DockDatum::getDockedControlID(const int32 dockedControlIndex) const noexcept
        {
            return _dockedControlIDArray[dockedControlIndex];
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
            _nextChildOffset._y = nextChildOffsetY;
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

        inline ControlData::ControlData(const ControlID& id, const ControlID& parentId, const ControlType controlType)
            : ControlData(id, parentId, controlType, Float2::kNan)
        {
            __noop;
        }

        inline ControlData::ControlData(const ControlID& id, const ControlID& parentId, const ControlType controlType, const Float2& size)
            : _updateCount{ 0 }
            , _interactionSize{ size }
            , _nonDockInteractionSize{ size }
            , _size{ size }
            , _minSize{ kControlDisplayMinWidth, kControlDisplayMinHeight }
            , _delegateControlID{ 0 }
            , _dockRelatedData{ controlType }
            , _rendererContextLayer{ RendererContextLayer::Background }
            , _id{ id }
            , _parentId{ parentId }
            , _controlType{ controlType }
            , _visibleState{ VisibleState::Visible }
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
        
        MINT_INLINE void ControlData::updatePerFrame(const PrepareControlDataParam& prepareControlDataParam, ControlData& parentControlData, const ControlMetaStateSet& controlMetaStateSet, const float availableDisplaySizeX, const bool computeSize) noexcept
        {
            clearPerFrameData();

            parentControlData._perFrameData._childControlIDs.push_back(_id);
            parentControlData.registerChildWindow(*this);

            updateSize(prepareControlDataParam, controlMetaStateSet, availableDisplaySizeX, computeSize);

            // Drag constraints 적용! (Dragging 이 아닐 때도 Constraint 가 적용되어야 함.. 예를 들면 resizing 중에!)
            if (_option._isDraggable == true && _positionConstraintsForDragging.isNan() == false)
            {
                _position._x = min(max(_positionConstraintsForDragging.left(), _position._x), _positionConstraintsForDragging.right());
                _position._y = min(max(_positionConstraintsForDragging.top(), _position._y), _positionConstraintsForDragging.bottom());
            }

            switch (prepareControlDataParam._clipRectUsage)
            {
            case Gui::ClipRectUsage::ParentsOwn:
                setClipRectForMe(parentControlData.getClipRectForMe());
                break;
            case Gui::ClipRectUsage::ParentsChild:
                setClipRectForMe(parentControlData.getClipRectForChildren());
                break;
            case Gui::ClipRectUsage::ParentsDock:
                setClipRectForMe(parentControlData.getClipRectForDocks());
                break;
            default:
                break;
            }
        }

        MINT_INLINE void ControlData::updateSize(const PrepareControlDataParam& prepareControlDataParam, const ControlMetaStateSet& controlMetaStateSet, const float availableDisplaySizeX, const bool compute) noexcept
        {
            Float2 desiredSize = controlMetaStateSet.getNextDesiredSize();
            if (compute)
            {
                const float maxDisplaySizeX = availableDisplaySizeX;
                if (desiredSize._x <= 0.0f)
                {
                    desiredSize._x = prepareControlDataParam._autoCalculatedDisplaySize._x;
                }
                if (desiredSize._y <= 0.0f)
                {
                    desiredSize._y = prepareControlDataParam._autoCalculatedDisplaySize._y;
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

            _minSize = prepareControlDataParam._minSize;
            _innerPadding = prepareControlDataParam._innerPadding;
            _interactionSize = _size + prepareControlDataParam._deltaInteractionSize;
            _nonDockInteractionSize = _interactionSize + prepareControlDataParam._deltaInteractionSizeByDock;
        }

        MINT_INLINE const ControlID& ControlData::getId() const noexcept
        {
            return _id;
        }

        MINT_INLINE void ControlData::setParentId(const ControlID& parentId) noexcept
        {
            _parentId = parentId;
        }

        MINT_INLINE const ControlID& ControlData::getParentId() const noexcept
        {
            return _parentId;
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
            return _childWindowIDMap.empty() == false;
        }

        MINT_INLINE void ControlData::registerChildWindow(const ControlData& childWindowControlData) noexcept
        {
            if (childWindowControlData._controlType == ControlType::Window && _childWindowIDMap.find(childWindowControlData._id).isValid() == false)
            {
                _childWindowIDMap.insert(childWindowControlData._id, true);
            }
        }

        MINT_INLINE const HashMap<ControlID, bool>& ControlData::getChildWindowIDMap() const noexcept
        {
            return _childWindowIDMap;
        }

        MINT_INLINE const Rect& ControlData::getInnerPadding() const noexcept
        {
            return _innerPadding;
        }

        MINT_INLINE Float2 ControlData::getClientSize() const noexcept
        {
            Float2 result = Float2(_size._x - _innerPadding.horz(), _size._y - _innerPadding.vert());
            if (isTypeOf(ControlType::Window))
            {
                result._y -= kTitleBarBaseThickness;
            }
            return result;
        }

        MINT_INLINE const float ControlData::getTopOffsetToClientArea() const noexcept
        {
            return ((_controlType == ControlType::Window) ? kTitleBarBaseThickness : 0.0f) + getMenuBarThickness()._y;
        }

        MINT_INLINE const Float2& ControlData::getMinSize() const noexcept
        {
            return _minSize;
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
            return _lastFrameData._contentAreaSize;
        }

        MINT_INLINE const Float2& ControlData::getChildAt() const noexcept
        {
            return _perFrameData._childAt;
        }

        MINT_INLINE const float ControlData::computeScrollDisplayWidth() const noexcept
        {
            const Float2& menuBarThicknes = getMenuBarThickness();
            return max(
                0.0f,
                _size._x - getHorzDockSizeSum() - _innerPadding.horz()
                - ((_controlValue._commonData.isScrollBarEnabled(ScrollBarType::Vert) == true) ? kScrollBarThickness * 2.0f : 0.0f)
                - menuBarThicknes._x
            );
        }

        MINT_INLINE const float ControlData::computeScrollDisplayHeight() const noexcept
        {
            const float titleBarHeight = (_controlType == Gui::ControlType::Window) ? kTitleBarBaseThickness : 0.0f;
            const Float2& menuBarThicknes = getMenuBarThickness();
            return max(
                0.0f,
                _size._y - getVertDockSizeSum() - titleBarHeight - _innerPadding.vert()
                - ((_controlValue._commonData.isScrollBarEnabled(ScrollBarType::Horz) == true) ? kScrollBarThickness * 2.0f : 0.0f)
                - menuBarThicknes._y
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

        MINT_INLINE const Rect& ControlData::getClipRectForMe() const noexcept
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
                resultDockPosition = Float2(_position._x + _size._x - dockSize._x, _position._y) + offset;
                resultDockPosition._y += getDockSizeIfHosting(DockingMethod::TopSide)._y;
                break;
            case Gui::DockingMethod::TopSide:
                resultDockPosition = Float2(_position._x, _position._y) + offset;
                break;
            case Gui::DockingMethod::BottomSide:
                resultDockPosition = Float2(_position._x, _position._y + _size._y - dockSize._y);
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

        MINT_INLINE void ControlData::connectToDock(const ControlID& dockControlID) noexcept
        {
            _dockRelatedData._dockControlID = dockControlID;
        }

        MINT_INLINE void ControlData::disconnectFromDock() noexcept
        {
            _dockRelatedData._dockControlID.reset();
        }

        MINT_INLINE const ControlID& ControlData::getDockControlID() const noexcept
        {
            return _dockRelatedData._dockControlID;
        }

        MINT_INLINE const bool ControlData::isDocking() const noexcept
        {
            return _dockRelatedData._dockControlID.isValid();
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
            std::swap(_size, _dockRelatedData._dokcingStateContext._size);
            std::swap(_resizingMask, _dockRelatedData._dokcingStateContext._resizingMask);
        }

        MINT_INLINE void ControlData::setAllClipRects(const Rect& clipRect) noexcept
        {
            setClipRectForMe(clipRect);
            setClipRectForChildren(clipRect);
            setClipRectForDocks(clipRect);
        }

        MINT_INLINE void ControlData::setClipRectForMe(const Rect& clipRect) noexcept
        {
            _clipRect = clipRect;
        }

        MINT_INLINE void ControlData::setClipRectForChildren(const Rect& clipRect) noexcept
        {
            _clipRectForChildren = clipRect;
        }

        MINT_INLINE void ControlData::setClipRectForDocks(const Rect& clipRect) noexcept
        {
            _clipRectForDocks = clipRect;
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

#include <stdafx.h>
#include <MintRenderingBase/Include/Gui/ControlData.h>


namespace mint
{
    namespace Gui
    {
#pragma region ControlValue
        inline ControlValue::ControlValue()
        {
            __noop;
        }

        MINT_INLINE void mint::Gui::ControlValue::CommonData::enableScrollBar(const ScrollBarType scrollBarType) noexcept
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
            case mint::Gui::DockingMethod::LeftSide:
                result._right = true;
                break;
            case mint::Gui::DockingMethod::RightSide:
                result._left = true;
                break;
            case mint::Gui::DockingMethod::TopSide:
                result._bottom = true;
                break;
            case mint::Gui::DockingMethod::BottomSide:
                result._top = true;
                break;
            case mint::Gui::DockingMethod::COUNT:
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
            return !_dockedControlHashArray.empty();
        }

        MINT_INLINE const bool DockDatum::isRawDockSizeSet() const noexcept
        {
            return _rawDockSize != mint::Float2::kZero;
        }

        MINT_INLINE void DockDatum::setRawDockSize(const mint::Float2& rawDockSize) noexcept
        {
            _rawDockSize = rawDockSize;
        }

        MINT_INLINE const mint::Float2& DockDatum::getRawDockSizeXXX() const noexcept
        {
            return _rawDockSize;
        }

        MINT_INLINE void DockDatum::swapDockedControlsXXX(const int32 indexA, const int32 indexB) noexcept
        {
            std::swap(_dockedControlHashArray[indexA], _dockedControlHashArray[indexB]);
        }

        MINT_INLINE const int32 DockDatum::getDockedControlIndex(const uint64 dockedControlHashKey) const noexcept
        {
            const int32 dockedControlCount = static_cast<int32>(_dockedControlHashArray.size());
            for (int32 dockedControlIndex = 0; dockedControlIndex < dockedControlCount; ++dockedControlIndex)
            {
                if (_dockedControlHashArray[dockedControlIndex] == dockedControlHashKey)
                {
                    return dockedControlIndex;
                }
            }
            return -1;
        }

        MINT_INLINE const uint64 DockDatum::getDockedControlHashKey(const int32 dockedControlIndex) const noexcept
        {
            return _dockedControlHashArray[dockedControlIndex];
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


#pragma region ControlData
        inline ControlData::ControlData()
            : ControlData(0, 0, ControlType::ROOT)
        {
            __noop;
        }

        inline ControlData::ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType)
            : ControlData(hashKey, parentHashKey, controlType, mint::Float2::kNan)
        {
            __noop;
        }

        inline ControlData::ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType, const mint::Float2& size)
            : _updateCount{ 0 }
            , _interactionSize{ size }
            , _nonDockInteractionSize{ size }
            , _isFocusable{ false }
            , _isDraggable{ false }
            , _isInteractableOutsideParent{ false }
            , _displaySize{ size }
            , _displaySizeMin{ kControlDisplayMinWidth, kControlDisplayMinHeight }
            , _childAt{ _innerPadding.left(), _innerPadding.top() }
            , _delegateHashKey{ 0 }
            , _dockingControlType{ DockingControlType::None }
            , _lastDockingMethod{ DockingMethod::COUNT }
            , _lastDockingMethodCandidate{ DockingMethod::COUNT }
            , _hashKey{ hashKey }
            , _parentHashKey{ parentHashKey }
            , _controlType{ controlType }
            , _visibleState{ VisibleState::Visible }
            , _previousMaxChildControlCount{ 0 }
            , _dockControlHashKey{ 0 }
        {
            _draggingConstraints.setNan();

            if (controlType == ControlType::ROOT)
            {
                _dockingControlType = DockingControlType::Dock;
            }
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

            prepareChildControlDataHashKeyArray();

            parentControlData._childControlDataHashKeyArray.push_back(_hashKey);
            parentControlData.connectChildWindowIfNot(*this);

            _displaySizeMin = prepareControlDataParam._displaySizeMin;
            _innerPadding = prepareControlDataParam._innerPadding;
            _interactionSize = _displaySize + prepareControlDataParam._deltaInteractionSize;
            _nonDockInteractionSize = _interactionSize + prepareControlDataParam._deltaInteractionSizeByDock;

            // Drag constraints 적용! (Dragging 이 아닐 때도 Constraint 가 적용되어야 함.. 예를 들면 resizing 중에!)
            if (_isDraggable == true && _draggingConstraints.isNan() == false)
            {
                _position._x = mint::min(mint::max(_draggingConstraints.left(), _position._x), _draggingConstraints.right());
                _position._y = mint::min(mint::max(_draggingConstraints.top(), _position._y), _draggingConstraints.bottom());
            }

            switch (prepareControlDataParam._viewportUsage)
            {
            case mint::Gui::ViewportUsage::Parent:
                setClipRectXXX(parentControlData.getClipRect());
                break;
            case mint::Gui::ViewportUsage::ParentDock:
                setClipRectXXX(parentControlData.getClipRectForDocks());
                break;
            case mint::Gui::ViewportUsage::Child:
                setClipRectXXX(parentControlData.getClipRectForChildren());
                break;
            default:
                break;
            }
        }

        MINT_INLINE const uint64 ControlData::getHashKey() const noexcept
        {
            return _hashKey;
        }

        MINT_INLINE const uint64 ControlData::getParentHashKey() const noexcept
        {
            return _parentHashKey;
        }

        MINT_INLINE const mint::Rect& ControlData::getInnerPadding() const noexcept
        {
            return _innerPadding;
        }

        MINT_INLINE mint::Float2 ControlData::getClientSize() const noexcept
        {
            mint::Float2 result = mint::Float2(_displaySize._x - _innerPadding.horz(), _displaySize._y - _innerPadding.vert());
            if (_controlType == ControlType::Window)
            {
                result._y -= kTitleBarBaseSize._y;
            }
            return result;
        }

        MINT_INLINE const float ControlData::getTopOffsetToClientArea() const noexcept
        {
            return ((_controlType == ControlType::Window) ? kTitleBarBaseSize._y : 0.0f) + getMenuBarThickness()._y;
        }

        MINT_INLINE const mint::Float2& ControlData::getDisplaySizeMin() const noexcept
        {
            return _displaySizeMin;
        }

        MINT_INLINE const float ControlData::getPureDisplayWidth() const noexcept
        {
            const mint::Float2& menuBarThicknes = getMenuBarThickness();
            return mint::max(
                0.0f,
                _displaySize._x - getHorzDockSizeSum() - _innerPadding.horz()
                - ((_controlValue._commonData.isScrollBarEnabled(ScrollBarType::Vert) == true) ? kScrollBarThickness * 2.0f : 0.0f)
                - menuBarThicknes._x
            );
        }

        MINT_INLINE const float ControlData::getPureDisplayHeight() const noexcept
        {
            const float titleBarHeight = (_controlType == mint::Gui::ControlType::Window) ? kTitleBarBaseSize._y : 0.0f;
            const mint::Float2& menuBarThicknes = getMenuBarThickness();
            return mint::max(
                0.0f,
                _displaySize._y - getVertDockSizeSum() - titleBarHeight - _innerPadding.vert()
                - ((_controlValue._commonData.isScrollBarEnabled(ScrollBarType::Horz) == true) ? kScrollBarThickness * 2.0f : 0.0f)
                - menuBarThicknes._y
            );
        }

        MINT_INLINE const mint::Float2& ControlData::getInteractionSize() const noexcept
        {
            return _interactionSize;
        }

        MINT_INLINE const mint::Float2& ControlData::getNonDockInteractionSize() const noexcept
        {
            return _nonDockInteractionSize;
        }

        MINT_INLINE const mint::Float2& ControlData::getContentAreaSize() const noexcept
        {
            return _contentAreaSize;
        }

        MINT_INLINE const mint::Float2& ControlData::getPreviousContentAreaSize() const noexcept
        {
            return _previousContentAreaSize;
        }

        MINT_INLINE const mint::Float2& ControlData::getChildAt() const noexcept
        {
            return _childAt;
        }

        MINT_INLINE  const mint::Float2& ControlData::getNextChildOffset() const noexcept
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

        MINT_INLINE const wchar_t* ControlData::getText() const noexcept
        {
            return _text.c_str();
        }

        MINT_INLINE const bool ControlData::isRootControl() const noexcept
        {
            return _controlType == ControlType::ROOT;
        }

        MINT_INLINE const bool ControlData::visibleStateEquals(const VisibleState visibleState) const noexcept
        {
            return visibleState == _visibleState;
        }

        MINT_INLINE const bool ControlData::isControlVisible() const noexcept
        {
            return (_visibleState != VisibleState::Invisible);
        }

        MINT_INLINE const mint::Rect& ControlData::getClipRect() const noexcept
        {
            return _clipRect;
        }

        MINT_INLINE const mint::Rect& ControlData::getClipRectForChildren() const noexcept
        {
            return _clipRectForChildren;
        }

        MINT_INLINE const mint::Rect& ControlData::getClipRectForDocks() const noexcept
        {
            return _clipRectForDocks;
        }

        MINT_INLINE const mint::Vector<uint64>& ControlData::getChildControlDataHashKeyArray() const noexcept
        {
            return _childControlDataHashKeyArray;
        }

        MINT_INLINE const mint::Vector<uint64>& ControlData::getPreviousChildControlDataHashKeyArray() const noexcept
        {
            return _previousChildControlDataHashKeyArray;
        }

        MINT_INLINE const uint16 ControlData::getPreviousChildControlCount() const noexcept
        {
            return static_cast<uint16>(_previousChildControlDataHashKeyArray.size());
        }

        MINT_INLINE const uint16 ControlData::getPreviousMaxChildControlCount() const noexcept
        {
            return _previousMaxChildControlCount;
        }

        MINT_INLINE void ControlData::prepareChildControlDataHashKeyArray() noexcept
        {
            std::swap(_childControlDataHashKeyArray, _previousChildControlDataHashKeyArray);
            _previousMaxChildControlCount = mint::max(_previousMaxChildControlCount, static_cast<uint16>(_previousChildControlDataHashKeyArray.size()));
            _childControlDataHashKeyArray.clear();
        }

        MINT_INLINE const bool ControlData::hasChildWindow() const noexcept
        {
            return !_childWindowHashKeyMap.empty();
        }

        MINT_INLINE DockDatum& ControlData::getDockDatum(const DockingMethod dockingMethod) noexcept
        {
            return _dockData[static_cast<uint32>(dockingMethod)];
        }

        MINT_INLINE const DockDatum& ControlData::getDockDatum(const DockingMethod dockingMethod) const noexcept
        {
            return _dockData[static_cast<uint32>(dockingMethod)];
        }

        MINT_INLINE const bool ControlData::isShowingInDock(const ControlData& dockedControlData) const noexcept
        {
            const DockDatum& dockDatum = getDockDatum(dockedControlData._lastDockingMethod);
            return dockDatum.getDockedControlIndex(dockedControlData.getHashKey()) == dockDatum._dockedControlIndexShown;
        }

        MINT_INLINE void ControlData::setDockSize(const DockingMethod dockingMethod, const mint::Float2& dockSize) noexcept
        {
            getDockDatum(dockingMethod).setRawDockSize(dockSize);
        }

        MINT_INLINE const mint::Float2 ControlData::getDockSize(const DockingMethod dockingMethod) const noexcept
        {
            const mint::Float2& menuBarThickness = getMenuBarThickness();
            const DockDatum& dockDatumTopSide = getDockDatum(DockingMethod::TopSide);
            const DockDatum& dockDatumBottomSide = getDockDatum(DockingMethod::BottomSide);
            const DockDatum& dockDatum = getDockDatum(dockingMethod);
            mint::Float2 resultDockSize = dockDatum.getRawDockSizeXXX();
            switch (dockingMethod)
            {
            case mint::Gui::DockingMethod::LeftSide:
            case mint::Gui::DockingMethod::RightSide:
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
            case mint::Gui::DockingMethod::TopSide:
            case mint::Gui::DockingMethod::BottomSide:
                resultDockSize._x = getClientSize()._x;
                break;
            case mint::Gui::DockingMethod::COUNT:
                break;
            default:
                break;
            }
            return resultDockSize;
        }

        MINT_INLINE const mint::Float2 ControlData::getDockSizeIfHosting(const DockingMethod dockingMethod) const noexcept
        {
            const DockDatum& dockDatum = getDockDatum(dockingMethod);
            return (dockDatum.hasDockedControls() == true) ? getDockSize(dockingMethod) : mint::Float2::kZero;
        }

        MINT_INLINE const mint::Float2 ControlData::getDockOffsetSize() const noexcept
        {
            return mint::Float2(0.0f, ((_controlType == ControlType::Window) ? kTitleBarBaseSize._y + _innerPadding.top() : 0.0f) + getMenuBarThickness()._y);
        }

        MINT_INLINE const mint::Float2 ControlData::getDockPosition(const DockingMethod dockingMethod) const noexcept
        {
            const DockDatum& dockDatumTopSide = getDockDatum(DockingMethod::TopSide);
            const mint::Float2& dockSize = getDockSize(dockingMethod);
            const mint::Float2& offset = getDockOffsetSize();

            mint::Float2 resultDockPosition;
            switch (dockingMethod)
            {
            case mint::Gui::DockingMethod::LeftSide:
                resultDockPosition = _position + offset;
                resultDockPosition._y += getDockSizeIfHosting(DockingMethod::TopSide)._y;
                break;
            case mint::Gui::DockingMethod::RightSide:
                resultDockPosition = mint::Float2(_position._x + _displaySize._x - dockSize._x, _position._y) + offset;
                resultDockPosition._y += getDockSizeIfHosting(DockingMethod::TopSide)._y;
                break;
            case mint::Gui::DockingMethod::TopSide:
                resultDockPosition = mint::Float2(_position._x, _position._y) + offset;
                break;
            case mint::Gui::DockingMethod::BottomSide:
                resultDockPosition = mint::Float2(_position._x, _position._y + _displaySize._y - dockSize._y);
                break;
            case mint::Gui::DockingMethod::COUNT:
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

        MINT_INLINE const mint::Float2 ControlData::getMenuBarThickness() const noexcept
        {
            mint::Float2 result;
            const MenuBarType currentMenuBarType = _controlValue._commonData._menuBarType;
            if (currentMenuBarType == MenuBarType::Top || currentMenuBarType == MenuBarType::Bottom)
            {
                result._y = kMenuBarBaseSize._y;
            }
            return result;
        }

        MINT_INLINE void ControlData::connectToDock(const uint64 dockControlHashKey) noexcept
        {
            _dockControlHashKey = dockControlHashKey;
        }

        MINT_INLINE void ControlData::disconnectFromDock() noexcept
        {
            _dockControlHashKey = 0;
        }

        MINT_INLINE const uint64 ControlData::getDockControlHashKey() const noexcept
        {
            return _dockControlHashKey;
        }

        MINT_INLINE const bool ControlData::isDocking() const noexcept
        {
            return (_dockControlHashKey != 0);
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
            if (childWindowControlData._controlType == ControlType::Window && _childWindowHashKeyMap.find(childWindowControlData._hashKey).isValid() == false)
            {
                _childWindowHashKeyMap.insert(childWindowControlData._hashKey, true);
            }
        }

        MINT_INLINE void ControlData::disconnectChildWindow(const uint64 childWindowHashKey) noexcept
        {
            _childWindowHashKeyMap.erase(childWindowHashKey);
        }

        MINT_INLINE const mint::HashMap<uint64, bool>& ControlData::getChildWindowHashKeyMap() const noexcept
        {
            return _childWindowHashKeyMap;
        }

        MINT_INLINE void ControlData::setVisibleState(const VisibleState visibleState) noexcept
        {
            _visibleState = visibleState;
        }

        MINT_INLINE void ControlData::swapDockingStateContext() noexcept
        {
            std::swap(_displaySize, _dokcingStateContext._displaySize);
            std::swap(_resizingMask, _dokcingStateContext._resizingMask);
        }

        MINT_INLINE void ControlData::setParentHashKeyXXX(const uint64 parentHashKey) noexcept
        {
            _parentHashKey = parentHashKey;
        }

        MINT_INLINE void ControlData::setOffsetY_XXX(const float offsetY) noexcept
        {
            _nextChildOffset._y = offsetY;
        }

        MINT_INLINE void ControlData::setClipRectXXX(const mint::Rect& clipRect) noexcept
        {
            _clipRect = clipRect;
        }

        MINT_INLINE void ControlData::setClipRectForChildrenXXX(const mint::Rect& clipRect) noexcept
        {
            _clipRectForChildren = clipRect;
        }

        MINT_INLINE void ControlData::setClipRectForDocksXXX(const mint::Rect& clipRect) noexcept
        {
            _clipRectForDocks = clipRect;
        }
#pragma endregion
    }
}

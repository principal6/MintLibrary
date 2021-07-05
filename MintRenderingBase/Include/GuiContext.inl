#include "GuiContext.h"
#pragma once


namespace mint
{
    namespace Gui
    {
        MINT_INLINE void TaskWhenMouseUp::clear() noexcept
        {
            _controlHashKeyForUpdateDockDatum = 0;
        }

        MINT_INLINE const bool TaskWhenMouseUp::isSet() const noexcept
        {
            return _controlHashKeyForUpdateDockDatum != 0;
        }

        MINT_INLINE void TaskWhenMouseUp::setUpdateDockDatum(const uint64 controlHashKey) noexcept
        {
            _controlHashKeyForUpdateDockDatum = controlHashKey;
        }

        MINT_INLINE const uint64 TaskWhenMouseUp::getUpdateDockDatum() const noexcept
        {
            return _controlHashKeyForUpdateDockDatum;
        }


        inline ControlValue::ControlValue()
            : _i{}
        {
            __noop;
        }

        MINT_INLINE void ControlValue::enableScrollBar(const ScrollBarType scrollBarType) noexcept
        {
            const ScrollBarType currentScrollBarType = getCurrentScrollBarType();
            if (currentScrollBarType != ScrollBarType::Both && scrollBarType != ScrollBarType::None)
            {
                if (currentScrollBarType == ScrollBarType::Horz && scrollBarType != ScrollBarType::Horz)
                {
                    setCurrentScrollBarType(ScrollBarType::Both);
                }
                else if (currentScrollBarType == ScrollBarType::Vert && scrollBarType != ScrollBarType::Vert)
                {
                    setCurrentScrollBarType(ScrollBarType::Both);
                }
                else
                {
                    setCurrentScrollBarType(scrollBarType);
                }
            }
        }

        MINT_INLINE void ControlValue::disableScrollBar(const ScrollBarType scrollBarType) noexcept
        {
            const ScrollBarType currentScrollBarType = getCurrentScrollBarType();
            if (currentScrollBarType != ScrollBarType::None && scrollBarType != ScrollBarType::None)
            {
                if (scrollBarType == ScrollBarType::Both)
                {
                    setCurrentScrollBarType(ScrollBarType::None);
                }
                else if (scrollBarType == ScrollBarType::Vert)
                {
                    if (currentScrollBarType == ScrollBarType::Vert)
                    {
                        setCurrentScrollBarType(ScrollBarType::None);
                    }
                    else
                    {
                        setCurrentScrollBarType(ScrollBarType::Horz);
                    }
                }
                else if (scrollBarType == ScrollBarType::Horz)
                {
                    if (currentScrollBarType == ScrollBarType::Horz)
                    {
                        setCurrentScrollBarType(ScrollBarType::None);
                    }
                    else
                    {
                        setCurrentScrollBarType(ScrollBarType::Vert);
                    }
                }
            }
        }

        MINT_INLINE const bool ControlValue::isScrollBarEnabled(const ScrollBarType scrollBarType) const noexcept
        {
            MINT_ASSERT("김장원", scrollBarType != ScrollBarType::None, "잘못된 질문입니다.");

            const ScrollBarType currentScrollBarType = getCurrentScrollBarType();
            if (currentScrollBarType == ScrollBarType::Both)
            {
                return true;
            }
            return (currentScrollBarType == scrollBarType);
        }

        MINT_INLINE void ControlValue::setCurrentScrollBarType(const ScrollBarType scrollBarType) noexcept
        {
            _hi[0] = static_cast<int16>(scrollBarType);
        }

        MINT_INLINE const ScrollBarType& ControlValue::getCurrentScrollBarType() const noexcept
        {
            return *reinterpret_cast<const ScrollBarType*>(&_hi[0]);
        }

        MINT_INLINE void ControlValue::setCurrentMenuBarType(const MenuBarType menuBarType) noexcept
        {
            _hi[1] = static_cast<int16>(menuBarType);
        }

        MINT_INLINE void ControlValue::setThumbAt(const float thumbAt) noexcept
        {
            _f[1] = thumbAt;
        }

        MINT_INLINE void ControlValue::setSelectedItemIndex(const int16 itemIndex) noexcept
        {
            _hi[4] = itemIndex;
        }

        MINT_INLINE void ControlValue::setIsToggled(const bool isToggled) noexcept
        {
            //_hi[5] = (isToggled == true) ? 1 : 0;
            _c[5 * 2] = (isToggled == true) ? 1 : 0;
        }

        MINT_INLINE void ControlValue::setItemSizeX(const float itemSizeX) noexcept
        {
            _f[3] = itemSizeX;
        }

        MINT_INLINE void ControlValue::setItemSizeY(const float itemSizeY) noexcept
        {
            _f[4] = itemSizeY;
        }

        MINT_INLINE void ControlValue::addItemSizeX(const float itemSizeX) noexcept
        {
            _f[3] += itemSizeX;
        }

        MINT_INLINE void ControlValue::addItemSizeY(const float itemSizeY) noexcept
        {
            _f[4] += itemSizeY;
        }

        MINT_INLINE void ControlValue::setInternalTimeMs(const uint64 internalTimeMs) noexcept
        {
            _lui[3] = internalTimeMs;
        }

        MINT_INLINE const MenuBarType& ControlValue::getCurrentMenuBarType() const noexcept
        {
            return *reinterpret_cast<const MenuBarType*>(&_hi[1]);
        }

        MINT_INLINE const float ControlValue::getThumbAt() const noexcept
        {
            return _f[1];
        }

        MINT_INLINE int16& ControlValue::getSelectedItemIndex() noexcept
        {
            return _hi[4];
        }

        MINT_INLINE const bool& ControlValue::getIsToggled() const noexcept
        {
            return *reinterpret_cast<const bool*>(&_c[5 * 2]); // (_hi[5] != 0);
        }

        MINT_INLINE const float ControlValue::getItemSizeX() const noexcept
        {
            return _f[3];
        }

        MINT_INLINE const float ControlValue::getItemSizeY() const noexcept
        {
            return _f[4];
        }

        MINT_INLINE const mint::Float2 ControlValue::getItemSize() const noexcept
        {
            return mint::Float2(getItemSizeX(), getItemSizeY());
        }

        MINT_INLINE uint16& ControlValue::getCaretAt() noexcept
        {
            return _hui[2];
        }

        MINT_INLINE uint16& ControlValue::getCaretState() noexcept
        {
            return _hui[3];
        }

        MINT_INLINE uint16& ControlValue::getSelectionStart() noexcept
        {
            return _hui[4];
        }

        MINT_INLINE uint16& ControlValue::getSelectionLength() noexcept
        {
            return _hui[5];
        }

        MINT_INLINE float& ControlValue::getTextDisplayOffset() noexcept
        {
            return _f[3];
        }

        MINT_INLINE uint64& ControlValue::getInternalTimeMs() noexcept
        {
            return _lui[3];
        }


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


        inline GuiContext::DockDatum::DockDatum()
            : _dockedControlIndexShown{ 0 }
        {
            __noop;
        }

        MINT_INLINE const bool GuiContext::DockDatum::hasDockedControls() const noexcept
        {
            return !_dockedControlHashArray.empty();
        }

        MINT_INLINE const bool GuiContext::DockDatum::isRawDockSizeSet() const noexcept
        {
            return _rawDockSize != mint::Float2::kZero;
        }

        MINT_INLINE void GuiContext::DockDatum::setRawDockSize(const mint::Float2& rawDockSize) noexcept
        {
            _rawDockSize = rawDockSize;
        }

        MINT_INLINE const mint::Float2& GuiContext::DockDatum::getRawDockSizeXXX() const noexcept
        {
            return _rawDockSize;
        }

        MINT_INLINE void GuiContext::DockDatum::swapDockedControlsXXX(const int32 indexA, const int32 indexB) noexcept
        {
            std::swap(_dockedControlHashArray[indexA], _dockedControlHashArray[indexB]);
        }

        MINT_INLINE const int32 GuiContext::DockDatum::getDockedControlIndex(const uint64 dockedControlHashKey) const noexcept
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

        MINT_INLINE const uint64 GuiContext::DockDatum::getDockedControlHashKey(const int32 dockedControlIndex) const noexcept
        {
            return _dockedControlHashArray[dockedControlIndex];
        }

        MINT_INLINE const float GuiContext::DockDatum::getDockedControlTitleBarOffset(const int32 dockedControlIndex) const noexcept
        {
            return _dockedControlTitleBarOffsetArray[dockedControlIndex];
        }

        MINT_INLINE const int32 GuiContext::DockDatum::getDockedControlIndexByMousePosition(const float relativeMousePositionX) const noexcept
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


        inline GuiContext::ControlData::ControlData()
            : ControlData(0, 0, ControlType::ROOT)
        {
            __noop;
        }

        inline GuiContext::ControlData::ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType)
            : ControlData(hashKey, parentHashKey, controlType, mint::Float2::kNan)
        {
            __noop;
        }
        
        inline GuiContext::ControlData::ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType, const mint::Float2& size)
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

        MINT_INLINE void GuiContext::ControlData::clearPerFrameData() noexcept
        {
            _nextChildOffset.setZero();
            _previousContentAreaSize = _contentAreaSize;
            _contentAreaSize.setZero();
            _currentFrameDeltaPosition.setZero();
        }

        MINT_INLINE void GuiContext::ControlData::updatePerFrameWithParent(const bool isNewData, const PrepareControlDataParam& prepareControlDataParam, ControlData& parentControlData) noexcept
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

        MINT_INLINE const uint64 GuiContext::ControlData::getHashKey() const noexcept
        {
            return _hashKey;
        }

        MINT_INLINE const uint64 GuiContext::ControlData::getParentHashKey() const noexcept
        {
            return _parentHashKey;
        }

        MINT_INLINE const mint::Rect& GuiContext::ControlData::getInnerPadding() const noexcept
        {
            return _innerPadding;
        }

        MINT_INLINE mint::Float2 GuiContext::ControlData::getClientSize() const noexcept
        {
            mint::Float2 result = mint::Float2(_displaySize._x - _innerPadding.horz(), _displaySize._y - _innerPadding.vert());
            if (_controlType == ControlType::Window)
            {
                result._y -= kTitleBarBaseSize._y;
            }
            return result;
        }

        MINT_INLINE const float GuiContext::ControlData::getTopOffsetToClientArea() const noexcept
        {
            return ((_controlType == ControlType::Window) ? kTitleBarBaseSize._y : 0.0f) + getMenuBarThickness()._y;
        }

        MINT_INLINE const mint::Float2& GuiContext::ControlData::getDisplaySizeMin() const noexcept
        {
            return _displaySizeMin;
        }

        MINT_INLINE const float GuiContext::ControlData::getPureDisplayWidth() const noexcept
        {
            const mint::Float2& menuBarThicknes = getMenuBarThickness();
            return mint::max(
                0.0f,
                _displaySize._x - getHorzDockSizeSum() - _innerPadding.horz()
                 - ((_controlValue.isScrollBarEnabled(ScrollBarType::Vert) == true) ? kScrollBarThickness * 2.0f : 0.0f)
                 - menuBarThicknes._x
            );
        }

        MINT_INLINE const float GuiContext::ControlData::getPureDisplayHeight() const noexcept
        {
            const float titleBarHeight = (_controlType == mint::Gui::ControlType::Window) ? kTitleBarBaseSize._y : 0.0f;
            const mint::Float2& menuBarThicknes = getMenuBarThickness();
            return mint::max(
                0.0f,
                _displaySize._y - getVertDockSizeSum() - titleBarHeight - _innerPadding.vert()
                 - ((_controlValue.isScrollBarEnabled(ScrollBarType::Horz) == true) ? kScrollBarThickness * 2.0f : 0.0f)
                 - menuBarThicknes._y
            );
        }

        MINT_INLINE const mint::Float2& GuiContext::ControlData::getInteractionSize() const noexcept
        {
            return _interactionSize;
        }

        MINT_INLINE const mint::Float2& GuiContext::ControlData::getNonDockInteractionSize() const noexcept
        {
            return _nonDockInteractionSize;
        }

        MINT_INLINE const mint::Float2& GuiContext::ControlData::getContentAreaSize() const noexcept
        {
            return _contentAreaSize;
        }

        MINT_INLINE const mint::Float2& GuiContext::ControlData::getPreviousContentAreaSize() const noexcept
        {
            return _previousContentAreaSize;
        }

        MINT_INLINE const mint::Float2& GuiContext::ControlData::getChildAt() const noexcept
        {
            return _childAt;
        }

        MINT_INLINE  const mint::Float2& GuiContext::ControlData::getNextChildOffset() const noexcept
        {
            return _nextChildOffset;
        }

        MINT_INLINE const ControlType GuiContext::ControlData::getControlType() const noexcept
        {
            return _controlType;
        }

        MINT_INLINE const bool GuiContext::ControlData::isTypeOf(const ControlType controlType) const noexcept
        {
            return (controlType == _controlType);
        }

        MINT_INLINE const wchar_t* GuiContext::ControlData::getText() const noexcept
        {
            return _text.c_str();
        }

        MINT_INLINE const bool GuiContext::ControlData::isRootControl() const noexcept
        {
            return _controlType == ControlType::ROOT;
        }

        MINT_INLINE const bool GuiContext::ControlData::visibleStateEquals(const VisibleState visibleState) const noexcept
        {
            return visibleState == _visibleState;
        }

        MINT_INLINE const bool GuiContext::ControlData::isControlVisible() const noexcept
        {
            return (_visibleState != VisibleState::Invisible);
        }

        MINT_INLINE const mint::Rect& GuiContext::ControlData::getClipRect() const noexcept
        {
            return _clipRect;
        }

        MINT_INLINE const mint::Rect& GuiContext::ControlData::getClipRectForChildren() const noexcept
        {
            return _clipRectForChildren;
        }

        MINT_INLINE const mint::Rect& GuiContext::ControlData::getClipRectForDocks() const noexcept
        {
            return _clipRectForDocks;
        }

        MINT_INLINE const mint::Vector<uint64>& GuiContext::ControlData::getChildControlDataHashKeyArray() const noexcept
        {
            return _childControlDataHashKeyArray;
        }

        MINT_INLINE const mint::Vector<uint64>& GuiContext::ControlData::getPreviousChildControlDataHashKeyArray() const noexcept
        {
            return _previousChildControlDataHashKeyArray;
        }

        MINT_INLINE const uint16 GuiContext::ControlData::getPreviousChildControlCount() const noexcept
        {
            return static_cast<uint16>(_previousChildControlDataHashKeyArray.size());
        }

        MINT_INLINE const uint16 GuiContext::ControlData::getPreviousMaxChildControlCount() const noexcept
        {
            return _previousMaxChildControlCount;
        }

        MINT_INLINE void GuiContext::ControlData::prepareChildControlDataHashKeyArray() noexcept
        {
            std::swap(_childControlDataHashKeyArray, _previousChildControlDataHashKeyArray);
            _previousMaxChildControlCount = mint::max(_previousMaxChildControlCount, static_cast<uint16>(_previousChildControlDataHashKeyArray.size()));
            _childControlDataHashKeyArray.clear();
        }

        MINT_INLINE const bool GuiContext::ControlData::hasChildWindow() const noexcept
        {
            return !_childWindowHashKeyMap.empty();
        }

        MINT_INLINE GuiContext::DockDatum& GuiContext::ControlData::getDockDatum(const DockingMethod dockingMethod) noexcept
        {
            return _dockData[static_cast<uint32>(dockingMethod)];
        }

        MINT_INLINE const GuiContext::DockDatum& GuiContext::ControlData::getDockDatum(const DockingMethod dockingMethod) const noexcept
        {
            return _dockData[static_cast<uint32>(dockingMethod)];
        }

        MINT_INLINE const bool GuiContext::ControlData::isShowingInDock(const ControlData& dockedControlData) const noexcept
        {
            const DockDatum& dockDatum = getDockDatum(dockedControlData._lastDockingMethod);            
            return dockDatum.getDockedControlIndex(dockedControlData.getHashKey()) == dockDatum._dockedControlIndexShown;
        }

        MINT_INLINE void GuiContext::ControlData::setDockSize(const DockingMethod dockingMethod, const mint::Float2& dockSize) noexcept
        {
            getDockDatum(dockingMethod).setRawDockSize(dockSize);
        }

        MINT_INLINE const mint::Float2 GuiContext::ControlData::getDockSize(const DockingMethod dockingMethod) const noexcept
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

        MINT_INLINE const mint::Float2 GuiContext::ControlData::getDockSizeIfHosting(const DockingMethod dockingMethod) const noexcept
        {
            const DockDatum& dockDatum = getDockDatum(dockingMethod);
            return (dockDatum.hasDockedControls() == true) ? getDockSize(dockingMethod) : mint::Float2::kZero;
        }

        MINT_INLINE const mint::Float2 GuiContext::ControlData::getDockOffsetSize() const noexcept
        {
            return mint::Float2(0.0f, ((_controlType == ControlType::Window) ? kTitleBarBaseSize._y + _innerPadding.top() : 0.0f) + getMenuBarThickness()._y);
        }

        MINT_INLINE const mint::Float2 GuiContext::ControlData::getDockPosition(const DockingMethod dockingMethod) const noexcept
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

        MINT_INLINE const float GuiContext::ControlData::getHorzDockSizeSum() const noexcept
        {
            return getDockSizeIfHosting(DockingMethod::LeftSide)._x + getDockSizeIfHosting(DockingMethod::RightSide)._x;
        }

        MINT_INLINE const float GuiContext::ControlData::getVertDockSizeSum() const noexcept
        {
            return getDockSizeIfHosting(DockingMethod::TopSide)._y + getDockSizeIfHosting(DockingMethod::BottomSide)._y;
        }

        MINT_INLINE const mint::Float2 GuiContext::ControlData::getMenuBarThickness() const noexcept
        {
            mint::Float2 result;
            const MenuBarType currentMenuBarType = _controlValue.getCurrentMenuBarType();
            if (currentMenuBarType == MenuBarType::Top || currentMenuBarType == MenuBarType::Bottom)
            {
                result._y = kMenuBarBaseSize._y;
            }
            return result;
        }

        MINT_INLINE void GuiContext::ControlData::connectToDock(const uint64 dockControlHashKey) noexcept
        {
            _dockControlHashKey = dockControlHashKey;
        }

        MINT_INLINE void GuiContext::ControlData::disconnectFromDock() noexcept
        {
            _dockControlHashKey = 0;
        }

        MINT_INLINE const uint64 GuiContext::ControlData::getDockControlHashKey() const noexcept
        {
            return _dockControlHashKey;
        }

        MINT_INLINE const bool GuiContext::ControlData::isDocking() const noexcept
        {
            return (_dockControlHashKey != 0);
        }

        MINT_INLINE const bool GuiContext::ControlData::isDockHosting() const noexcept
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

        MINT_INLINE const bool GuiContext::ControlData::isResizable() const noexcept
        {
            return _resizingMask.isResizable();
        }

        MINT_INLINE Rect GuiContext::ControlData::getControlRect() const noexcept
        {
            return Rect::fromPositionSize(_position, _displaySize);
        }

        MINT_INLINE Rect GuiContext::ControlData::getControlPaddedRect() const noexcept
        {
            return Rect
            (
                _position._x + _innerPadding.left(),
                _position._x + _displaySize._x - _innerPadding.horz(),
                _position._y + _innerPadding.top(), 
                _position._y + _displaySize._y - _innerPadding.vert()
            );
        }

        MINT_INLINE void GuiContext::ControlData::connectChildWindowIfNot(const ControlData& childWindowControlData) noexcept
        {
            if (childWindowControlData._controlType == ControlType::Window && _childWindowHashKeyMap.find(childWindowControlData._hashKey).isValid() == false)
            {
                _childWindowHashKeyMap.insert(childWindowControlData._hashKey, true);
            }
        }

        MINT_INLINE void GuiContext::ControlData::disconnectChildWindow(const uint64 childWindowHashKey) noexcept
        {
            _childWindowHashKeyMap.erase(childWindowHashKey);
        }

        MINT_INLINE const mint::HashMap<uint64, bool>& GuiContext::ControlData::getChildWindowHashKeyMap() const noexcept
        {
            return _childWindowHashKeyMap;
        }

        MINT_INLINE void GuiContext::ControlData::setVisibleState(const VisibleState visibleState) noexcept
        {
            _visibleState = visibleState;
        }

        MINT_INLINE void GuiContext::ControlData::swapDockingStateContext() noexcept
        {
            std::swap(_displaySize, _dokcingStateContext._displaySize);
            std::swap(_resizingMask, _dokcingStateContext._resizingMask);
        }

        MINT_INLINE void GuiContext::ControlData::setParentHashKeyXXX(const uint64 parentHashKey) noexcept
        {
            _parentHashKey = parentHashKey;
        }

        MINT_INLINE void GuiContext::ControlData::setOffsetY_XXX(const float offsetY) noexcept
        {
            _nextChildOffset._y = offsetY;
        }

        MINT_INLINE void GuiContext::ControlData::setClipRectXXX(const mint::Rect& clipRect) noexcept
        {
            _clipRect = clipRect;
        }

        MINT_INLINE void GuiContext::ControlData::setClipRectForChildrenXXX(const mint::Rect& clipRect) noexcept
        {
            _clipRectForChildren = clipRect;
        }

        MINT_INLINE void GuiContext::ControlData::setClipRectForDocksXXX(const mint::Rect& clipRect) noexcept
        {
            _clipRectForDocks = clipRect;
        }


        inline GuiContext::ControlStackData::ControlStackData(const ControlData& controlData)
            : _controlType{ controlData.getControlType() }
            , _hashKey{ controlData.getHashKey() }
        {
            __noop;
        }


        inline GuiContext::NextControlStates::NextControlStates()
        {
            reset();
        }

        MINT_INLINE void GuiContext::NextControlStates::reset() noexcept
        {
            _nextSameLine = false;
            _nextDesiredControlSize.setZero();
            _nextSizingForced = false;
            _nextControlSizeNonContrainedToParent = false;
            _nextNoInterval = false;
            _nextNoAutoPositioned = false;
            _nextControlPosition.setZero();
            _nextTooltipText = nullptr;
        }


        MINT_INLINE void GuiContext::MouseStates::resetPerFrame() noexcept
        {
            _isButtonDownUp = false;
            _isButtonDownThisFrame = false;
        }

        MINT_INLINE void GuiContext::MouseStates::setPosition(const mint::Float2& position) noexcept
        {
            _mousePosition = position;
        }

        MINT_INLINE void GuiContext::MouseStates::setButtonDownPosition(const mint::Float2& position) noexcept
        {
            _mouseDownPosition = position;
        }

        MINT_INLINE void GuiContext::MouseStates::setButtonUpPosition(const mint::Float2& position) noexcept
        {
            _mouseUpPosition = position;
        }

        MINT_INLINE void GuiContext::MouseStates::setButtonDown() noexcept
        {
            _isButtonDown = true;
            _isButtonDownThisFrame = true;
        }

        MINT_INLINE void GuiContext::MouseStates::setButtonUp() noexcept
        {
            if (_isButtonDown == true)
            {
                _isButtonDownUp = true;
            }
            _isButtonDown = false;
        }

        MINT_INLINE const mint::Float2& GuiContext::MouseStates::getPosition() const noexcept
        {
            return _mousePosition;
        }

        MINT_INLINE const mint::Float2& GuiContext::MouseStates::getButtonDownPosition() const noexcept
        {
            return _mouseDownPosition;
        }

        MINT_INLINE const mint::Float2& GuiContext::MouseStates::getButtonUpPosition() const noexcept
        {
            return _mouseUpPosition;
        }

        MINT_INLINE const mint::Float2 GuiContext::MouseStates::getMouseDragDelta() const noexcept
        {
            return _mousePosition - _mouseDownPosition;
        }

        MINT_INLINE const bool GuiContext::MouseStates::isButtonDown() const noexcept
        {
            return _isButtonDown;
        }

        MINT_INLINE const bool GuiContext::MouseStates::isButtonDownThisFrame() const noexcept
        {
            return _isButtonDownThisFrame;
        }

        MINT_INLINE const bool GuiContext::MouseStates::isButtonDownUp() const noexcept
        {
            return _isButtonDownUp;
        }

        MINT_INLINE const bool GuiContext::MouseStates::isCursor(const mint::Window::CursorType cursorType) const noexcept
        {
            return _cursorType == cursorType;
        }


        MINT_INLINE void GuiContext::nextSameLine()
        {
            _nextControlStates._nextSameLine = true;
        }

        MINT_INLINE void GuiContext::nextControlSize(const mint::Float2& size, const bool force)
        {
            _nextControlStates._nextDesiredControlSize = size;
            _nextControlStates._nextSizingForced = force;
        }

        MINT_INLINE void GuiContext::nextNoInterval()
        {
            _nextControlStates._nextNoInterval = true;
        }

        MINT_INLINE void GuiContext::nextNoAutoPositioned()
        {
            _nextControlStates._nextNoAutoPositioned = true;
        }

        MINT_INLINE void GuiContext::nextControlSizeNonContrainedToParent()
        {
            _nextControlStates._nextControlSizeNonContrainedToParent = true;
        }

        MINT_INLINE void GuiContext::nextControlPosition(const mint::Float2& position)
        {
            _nextControlStates._nextControlPosition = position;
        }

        MINT_INLINE void GuiContext::nextTooltip(const wchar_t* const tooltipText)
        {
            _nextControlStates._nextTooltipText = tooltipText;
        }

        MINT_INLINE const GuiContext::ControlData& GuiContext::getControlStackTopXXX() const noexcept
        {
            if (_controlStackPerFrame.empty() == false)
            {
                auto found = _controlIdMap.find(_controlStackPerFrame.back()._hashKey);
                if (found.isValid() == true)
                {
                    return *found._value;
                }
            }
            return _rootControlData;
        }

        MINT_INLINE GuiContext::ControlData& GuiContext::getControlStackTopXXX() noexcept
        {
            if (_controlStackPerFrame.empty() == false)
            {
                auto found = _controlIdMap.find(_controlStackPerFrame.back()._hashKey);
                if (found.isValid() == true)
                {
                    return *found._value;
                }
            }
            return _rootControlData;
        }

        MINT_INLINE GuiContext::ControlData& GuiContext::getControlData(const uint64 hashKey) noexcept
        {
            auto found = _controlIdMap.find(hashKey);
            if (found.isValid() == true)
            {
                return *found._value;
            }
            return _rootControlData;
        }

        MINT_INLINE const GuiContext::ControlData& GuiContext::getControlData(const uint64 hashKey) const noexcept
        {
            auto found = _controlIdMap.find(hashKey);
            if (found.isValid() == true)
            {
                return *found._value;
            }

            //MINT_ASSERT("김장원", false, "hashKey 가 존재하지 않는 ControlData 입니다!!!");
            return _rootControlData;
        }
        
        MINT_INLINE mint::Float4 GuiContext::getControlCenterPosition(const ControlData& controlData) const noexcept
        {
            return mint::Float4(controlData._position._x + controlData._displaySize._x * 0.5f, controlData._position._y + controlData._displaySize._y * 0.5f, 0.0f, 1.0f);
        }

        MINT_INLINE mint::Float2 GuiContext::getControlPositionInParentSpace(const ControlData& controlData) const noexcept
        {
            return controlData._position - getControlData(controlData.getParentHashKey())._position;
        }

        MINT_INLINE const bool GuiContext::isControlHovered(const ControlData& controlData) const noexcept
        {
            return (_hoveredControlHashKey == controlData.getHashKey());
        }

        MINT_INLINE const bool GuiContext::isControlPressed(const ControlData& controlData) const noexcept
        {
            return (_pressedControlHashKey == controlData.getHashKey());
        }

        MINT_INLINE const bool GuiContext::isControlClicked(const ControlData& controlData) const noexcept
        {
            return (_clickedControlHashKeyPerFrame == controlData.getHashKey());
        }

        MINT_INLINE const bool GuiContext::isControlFocused(const ControlData& controlData) const noexcept
        {
            return (_focusedControlHashKey == ((0 != controlData._delegateHashKey) ? controlData._delegateHashKey : controlData.getHashKey()));
        }

        MINT_INLINE const mint::RenderingBase::Color& GuiContext::getNamedColor(const NamedColor namedColor) const noexcept
        {
            return _namedColors[static_cast<uint32>(namedColor)];
        }

        MINT_INLINE void GuiContext::setNamedColor(const NamedColor namedColor, const mint::RenderingBase::Color& color) noexcept
        {
            _namedColors[static_cast<uint32>(namedColor)] = color;
        }

        MINT_INLINE const float GuiContext::calculateTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept
        {
            return _shapeFontRendererContextTopMost.calculateTextWidth(wideText, textLength);
        }

        MINT_INLINE const uint32 GuiContext::calculateIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept
        {
            return _shapeFontRendererContextTopMost.calculateIndexFromPositionInText(wideText, textLength, positionInText);
        }
    }
}

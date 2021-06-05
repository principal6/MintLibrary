#include "GuiContext.h"
#pragma once


namespace fs
{
    namespace Gui
    {
        FS_INLINE void TaskWhenMouseUp::clear() noexcept
        {
            _controlHashKeyForUpdateDockDatum = 0;
        }

        FS_INLINE const bool TaskWhenMouseUp::isSet() const noexcept
        {
            return _controlHashKeyForUpdateDockDatum != 0;
        }

        FS_INLINE void TaskWhenMouseUp::setUpdateDockDatum(const uint64 controlHashKey) noexcept
        {
            _controlHashKeyForUpdateDockDatum = controlHashKey;
        }

        FS_INLINE const uint64 TaskWhenMouseUp::getUpdateDockDatum() const noexcept
        {
            return _controlHashKeyForUpdateDockDatum;
        }


        inline ControlValue::ControlValue()
            : _i{}
        {
            __noop;
        }

        FS_INLINE void ControlValue::enableScrollBar(const ScrollBarType scrollBarType) noexcept
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

        FS_INLINE void ControlValue::disableScrollBar(const ScrollBarType scrollBarType) noexcept
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

        FS_INLINE const bool ControlValue::isScrollBarEnabled(const ScrollBarType scrollBarType) const noexcept
        {
            FS_ASSERT("김장원", scrollBarType != ScrollBarType::None, "잘못된 질문입니다.");

            const ScrollBarType currentScrollBarType = getCurrentScrollBarType();
            if (currentScrollBarType == ScrollBarType::Both)
            {
                return true;
            }
            return (currentScrollBarType == scrollBarType);
        }

        FS_INLINE void ControlValue::setCurrentScrollBarType(const ScrollBarType scrollBarType) noexcept
        {
            _hi[0] = static_cast<int16>(scrollBarType);
        }

        FS_INLINE const ScrollBarType& ControlValue::getCurrentScrollBarType() const noexcept
        {
            return *reinterpret_cast<const ScrollBarType*>(&_hi[0]);
        }

        FS_INLINE void ControlValue::setCurrentMenuBarType(const MenuBarType menuBarType) noexcept
        {
            _hi[1] = static_cast<int16>(menuBarType);
        }

        FS_INLINE void ControlValue::setThumbAt(const float thumbAt) noexcept
        {
            _f[1] = thumbAt;
        }

        FS_INLINE void ControlValue::setSelectedItemIndex(const int16 itemIndex) noexcept
        {
            _hi[4] = itemIndex;
        }

        FS_INLINE void ControlValue::setIsToggled(const bool isToggled) noexcept
        {
            //_hi[5] = (isToggled == true) ? 1 : 0;
            _c[5 * 2] = (isToggled == true) ? 1 : 0;
        }

        FS_INLINE void ControlValue::setItemSizeX(const float itemSizeX) noexcept
        {
            _f[3] = itemSizeX;
        }

        FS_INLINE void ControlValue::setItemSizeY(const float itemSizeY) noexcept
        {
            _f[4] = itemSizeY;
        }

        FS_INLINE void ControlValue::addItemSizeX(const float itemSizeX) noexcept
        {
            _f[3] += itemSizeX;
        }

        FS_INLINE void ControlValue::addItemSizeY(const float itemSizeY) noexcept
        {
            _f[4] += itemSizeY;
        }

        FS_INLINE void ControlValue::setInternalTimeMs(const uint64 internalTimeMs) noexcept
        {
            _lui[3] = internalTimeMs;
        }

        FS_INLINE const MenuBarType& ControlValue::getCurrentMenuBarType() const noexcept
        {
            return *reinterpret_cast<const MenuBarType*>(&_hi[1]);
        }

        FS_INLINE const float ControlValue::getThumbAt() const noexcept
        {
            return _f[1];
        }

        FS_INLINE int16& ControlValue::getSelectedItemIndex() noexcept
        {
            return _hi[4];
        }

        FS_INLINE const bool& ControlValue::getIsToggled() const noexcept
        {
            return *reinterpret_cast<const bool*>(&_c[5 * 2]); // (_hi[5] != 0);
        }

        FS_INLINE const float ControlValue::getItemSizeX() const noexcept
        {
            return _f[3];
        }

        FS_INLINE const float ControlValue::getItemSizeY() const noexcept
        {
            return _f[4];
        }

        FS_INLINE const fs::Float2 ControlValue::getItemSize() const noexcept
        {
            return fs::Float2(getItemSizeX(), getItemSizeY());
        }

        FS_INLINE uint16& ControlValue::getCaretAt() noexcept
        {
            return _hui[2];
        }

        FS_INLINE uint16& ControlValue::getCaretState() noexcept
        {
            return _hui[3];
        }

        FS_INLINE uint16& ControlValue::getSelectionStart() noexcept
        {
            return _hui[4];
        }

        FS_INLINE uint16& ControlValue::getSelectionLength() noexcept
        {
            return _hui[5];
        }

        FS_INLINE float& ControlValue::getTextDisplayOffset() noexcept
        {
            return _f[3];
        }

        FS_INLINE uint64& ControlValue::getInternalTimeMs() noexcept
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
            case fs::Gui::DockingMethod::LeftSide:
                result._right = true;
                break;
            case fs::Gui::DockingMethod::RightSide:
                result._left = true;
                break;
            case fs::Gui::DockingMethod::TopSide:
                result._bottom = true;
                break;
            case fs::Gui::DockingMethod::BottomSide:
                result._top = true;
                break;
            case fs::Gui::DockingMethod::COUNT:
                break;
            default:
                break;
            }

            return result;
        }

        FS_INLINE void ResizingMask::setAllTrue() noexcept
        {
            _rawMask = 0xFF;
        }

        FS_INLINE void ResizingMask::setAllFalse() noexcept
        {
            _rawMask = 0;
        }

        FS_INLINE const bool ResizingMask::isResizable() const noexcept
        {
            return (_rawMask != 0);
        }

        FS_INLINE const bool ResizingMask::topLeft() const noexcept
        {
            return (_top && _left);
        }

        FS_INLINE const bool ResizingMask::topRight() const noexcept
        {
            return (_top && _right);
        }

        FS_INLINE const bool ResizingMask::bottomLeft() const noexcept
        {
            return (_bottom && _left);
        }

        FS_INLINE const bool ResizingMask::bottomRight() const noexcept
        {
            return (_bottom && _right);
        }

        FS_INLINE const bool ResizingMask::overlaps(const ResizingMask& rhs) const noexcept
        {
            return ((_rawMask & rhs._rawMask) != 0);
        }


        inline GuiContext::DockDatum::DockDatum()
            : _dockedControlIndexShown{ 0 }
        {
            __noop;
        }

        FS_INLINE const bool GuiContext::DockDatum::hasDockedControls() const noexcept
        {
            return !_dockedControlHashArray.empty();
        }

        FS_INLINE const bool GuiContext::DockDatum::isRawDockSizeSet() const noexcept
        {
            return _rawDockSize != fs::Float2::kZero;
        }

        FS_INLINE void GuiContext::DockDatum::setRawDockSize(const fs::Float2& rawDockSize) noexcept
        {
            _rawDockSize = rawDockSize;
        }

        FS_INLINE const fs::Float2& GuiContext::DockDatum::getRawDockSizeXXX() const noexcept
        {
            return _rawDockSize;
        }

        FS_INLINE void GuiContext::DockDatum::swapDockedControlsXXX(const int32 indexA, const int32 indexB) noexcept
        {
            std::swap(_dockedControlHashArray[indexA], _dockedControlHashArray[indexB]);
        }

        FS_INLINE const int32 GuiContext::DockDatum::getDockedControlIndex(const uint64 dockedControlHashKey) const noexcept
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

        FS_INLINE const uint64 GuiContext::DockDatum::getDockedControlHashKey(const int32 dockedControlIndex) const noexcept
        {
            return _dockedControlHashArray[dockedControlIndex];
        }

        FS_INLINE const float GuiContext::DockDatum::getDockedControlTitleBarOffset(const int32 dockedControlIndex) const noexcept
        {
            return _dockedControlTitleBarOffsetArray[dockedControlIndex];
        }

        FS_INLINE const int32 GuiContext::DockDatum::getDockedControlIndexByMousePosition(const float relativeMousePositionX) const noexcept
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
            : ControlData(hashKey, parentHashKey, controlType, fs::Float2::kNan)
        {
            __noop;
        }
        
        inline GuiContext::ControlData::ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType, const fs::Float2& size)
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
            , _viewportIndex{ 0 }
            , _viewportIndexForChildren{ 0 }
            , _viewportIndexForDocks{ 0 }
            , _previousMaxChildControlCount{ 0 }
            , _dockControlHashKey{ 0 }
        {
            _draggingConstraints.setNan();

            if (controlType == ControlType::ROOT)
            {
                _dockingControlType = DockingControlType::Dock;
            }
        }

        FS_INLINE void GuiContext::ControlData::clearPerFrameData() noexcept
        {
            _nextChildOffset.setZero();
            _previousContentAreaSize = _contentAreaSize;
            _contentAreaSize.setZero();
            _currentFrameDeltaPosition.setZero();
        }

        FS_INLINE void GuiContext::ControlData::updatePerFrameWithParent(const bool isNewData, const PrepareControlDataParam& prepareControlDataParam, ControlData& parentControlData) noexcept
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
                _position._x = fs::min(fs::max(_draggingConstraints.left(), _position._x), _draggingConstraints.right());
                _position._y = fs::min(fs::max(_draggingConstraints.top(), _position._y), _draggingConstraints.bottom());
            }

            switch (prepareControlDataParam._viewportUsage)
            {
            case fs::Gui::ViewportUsage::Parent:
                setViewportIndexXXX(parentControlData.getViewportIndex());
                break;
            case fs::Gui::ViewportUsage::ParentDock:
                setViewportIndexXXX(parentControlData.getViewportIndexForDocks());
                break;
            case fs::Gui::ViewportUsage::Child:
                setViewportIndexXXX(parentControlData.getViewportIndexForChildren());
                break;
            default:
                break;
            }
        }

        FS_INLINE const uint64 GuiContext::ControlData::getHashKey() const noexcept
        {
            return _hashKey;
        }

        FS_INLINE const uint64 GuiContext::ControlData::getParentHashKey() const noexcept
        {
            return _parentHashKey;
        }

        FS_INLINE const fs::Rect& GuiContext::ControlData::getInnerPadding() const noexcept
        {
            return _innerPadding;
        }

        FS_INLINE fs::Float2 GuiContext::ControlData::getClientSize() const noexcept
        {
            fs::Float2 result = fs::Float2(_displaySize._x - _innerPadding.horz(), _displaySize._y - _innerPadding.vert());
            if (_controlType == ControlType::Window)
            {
                result._y -= kTitleBarBaseSize._y;
            }
            return result;
        }

        FS_INLINE const float GuiContext::ControlData::getTopOffsetToClientArea() const noexcept
        {
            return ((_controlType == ControlType::Window) ? kTitleBarBaseSize._y : 0.0f) + getMenuBarThickness()._y;
        }

        FS_INLINE const fs::Float2& GuiContext::ControlData::getDisplaySizeMin() const noexcept
        {
            return _displaySizeMin;
        }

        FS_INLINE const float GuiContext::ControlData::getPureDisplayWidth() const noexcept
        {
            const fs::Float2& menuBarThicknes = getMenuBarThickness();
            return fs::max(
                0.0f,
                _displaySize._x - getHorzDockSizeSum() - _innerPadding.horz()
                 - ((_controlValue.isScrollBarEnabled(ScrollBarType::Vert) == true) ? kScrollBarThickness * 2.0f : 0.0f)
                 - menuBarThicknes._x
            );
        }

        FS_INLINE const float GuiContext::ControlData::getPureDisplayHeight() const noexcept
        {
            const float titleBarHeight = (_controlType == fs::Gui::ControlType::Window) ? kTitleBarBaseSize._y : 0.0f;
            const fs::Float2& menuBarThicknes = getMenuBarThickness();
            return fs::max(
                0.0f,
                _displaySize._y - getVertDockSizeSum() - titleBarHeight - _innerPadding.vert()
                 - ((_controlValue.isScrollBarEnabled(ScrollBarType::Horz) == true) ? kScrollBarThickness * 2.0f : 0.0f)
                 - menuBarThicknes._y
            );
        }

        FS_INLINE const fs::Float2& GuiContext::ControlData::getInteractionSize() const noexcept
        {
            return _interactionSize;
        }

        FS_INLINE const fs::Float2& GuiContext::ControlData::getNonDockInteractionSize() const noexcept
        {
            return _nonDockInteractionSize;
        }

        FS_INLINE const fs::Float2& GuiContext::ControlData::getContentAreaSize() const noexcept
        {
            return _contentAreaSize;
        }

        FS_INLINE const fs::Float2& GuiContext::ControlData::getPreviousContentAreaSize() const noexcept
        {
            return _previousContentAreaSize;
        }

        FS_INLINE const fs::Float2& GuiContext::ControlData::getChildAt() const noexcept
        {
            return _childAt;
        }

        FS_INLINE  const fs::Float2& GuiContext::ControlData::getNextChildOffset() const noexcept
        {
            return _nextChildOffset;
        }

        FS_INLINE const ControlType GuiContext::ControlData::getControlType() const noexcept
        {
            return _controlType;
        }

        FS_INLINE const bool GuiContext::ControlData::isTypeOf(const ControlType controlType) const noexcept
        {
            return (controlType == _controlType);
        }

        FS_INLINE const wchar_t* GuiContext::ControlData::getText() const noexcept
        {
            return _text.c_str();
        }

        FS_INLINE const bool GuiContext::ControlData::isRootControl() const noexcept
        {
            return _controlType == ControlType::ROOT;
        }

        FS_INLINE const bool GuiContext::ControlData::isVisibleState(const VisibleState visibleState) const noexcept
        {
            return visibleState == _visibleState;
        }

        FS_INLINE const bool GuiContext::ControlData::isControlVisible() const noexcept
        {
            return (_visibleState != VisibleState::Invisible);
        }

        FS_INLINE const uint32 GuiContext::ControlData::getViewportIndex() const noexcept
        {
            return _viewportIndex;
        }

        FS_INLINE const uint32 GuiContext::ControlData::getViewportIndexForChildren() const noexcept
        {
            return _viewportIndexForChildren;
        }

        FS_INLINE const uint32 GuiContext::ControlData::getViewportIndexForDocks() const noexcept
        {
            return _viewportIndexForDocks;
        }

        FS_INLINE const fs::Vector<uint64>& GuiContext::ControlData::getChildControlDataHashKeyArray() const noexcept
        {
            return _childControlDataHashKeyArray;
        }

        FS_INLINE const fs::Vector<uint64>& GuiContext::ControlData::getPreviousChildControlDataHashKeyArray() const noexcept
        {
            return _previousChildControlDataHashKeyArray;
        }

        FS_INLINE const uint16 GuiContext::ControlData::getPreviousChildControlCount() const noexcept
        {
            return static_cast<uint16>(_previousChildControlDataHashKeyArray.size());
        }

        FS_INLINE const uint16 GuiContext::ControlData::getPreviousMaxChildControlCount() const noexcept
        {
            return _previousMaxChildControlCount;
        }

        FS_INLINE void GuiContext::ControlData::prepareChildControlDataHashKeyArray() noexcept
        {
            std::swap(_childControlDataHashKeyArray, _previousChildControlDataHashKeyArray);
            _previousMaxChildControlCount = fs::max(_previousMaxChildControlCount, static_cast<uint16>(_previousChildControlDataHashKeyArray.size()));
            _childControlDataHashKeyArray.clear();
        }

        FS_INLINE const bool GuiContext::ControlData::hasChildWindow() const noexcept
        {
            return !_childWindowHashKeyMap.empty();
        }

        FS_INLINE GuiContext::DockDatum& GuiContext::ControlData::getDockDatum(const DockingMethod dockingMethod) noexcept
        {
            return _dockData[static_cast<uint32>(dockingMethod)];
        }

        FS_INLINE const GuiContext::DockDatum& GuiContext::ControlData::getDockDatum(const DockingMethod dockingMethod) const noexcept
        {
            return _dockData[static_cast<uint32>(dockingMethod)];
        }

        FS_INLINE const bool GuiContext::ControlData::isShowingInDock(const ControlData& dockedControlData) const noexcept
        {
            const DockDatum& dockDatum = getDockDatum(dockedControlData._lastDockingMethod);            
            return dockDatum.getDockedControlIndex(dockedControlData.getHashKey()) == dockDatum._dockedControlIndexShown;
        }

        FS_INLINE void GuiContext::ControlData::setDockSize(const DockingMethod dockingMethod, const fs::Float2& dockSize) noexcept
        {
            getDockDatum(dockingMethod).setRawDockSize(dockSize);
        }

        FS_INLINE const fs::Float2 GuiContext::ControlData::getDockSize(const DockingMethod dockingMethod) const noexcept
        {
            const fs::Float2& menuBarThickness = getMenuBarThickness();
            const DockDatum& dockDatumTopSide = getDockDatum(DockingMethod::TopSide);
            const DockDatum& dockDatumBottomSide = getDockDatum(DockingMethod::BottomSide);
            const DockDatum& dockDatum = getDockDatum(dockingMethod);
            fs::Float2 resultDockSize = dockDatum.getRawDockSizeXXX();
            switch (dockingMethod)
            {
            case fs::Gui::DockingMethod::LeftSide:
            case fs::Gui::DockingMethod::RightSide:
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
            case fs::Gui::DockingMethod::TopSide:
            case fs::Gui::DockingMethod::BottomSide:
                resultDockSize._x = getClientSize()._x;
                break;
            case fs::Gui::DockingMethod::COUNT:
                break;
            default:
                break;
            }
            return resultDockSize;
        }

        FS_INLINE const fs::Float2 GuiContext::ControlData::getDockSizeIfHosting(const DockingMethod dockingMethod) const noexcept
        {
            const DockDatum& dockDatum = getDockDatum(dockingMethod);
            return (dockDatum.hasDockedControls() == true) ? getDockSize(dockingMethod) : fs::Float2::kZero;
        }

        FS_INLINE const fs::Float2 GuiContext::ControlData::getDockOffsetSize() const noexcept
        {
            return fs::Float2(0.0f, ((_controlType == ControlType::Window) ? kTitleBarBaseSize._y + _innerPadding.top() : 0.0f) + getMenuBarThickness()._y);
        }

        FS_INLINE const fs::Float2 GuiContext::ControlData::getDockPosition(const DockingMethod dockingMethod) const noexcept
        {
            const DockDatum& dockDatumTopSide = getDockDatum(DockingMethod::TopSide);
            const fs::Float2& dockSize = getDockSize(dockingMethod);
            const fs::Float2& offset = getDockOffsetSize();

            fs::Float2 resultDockPosition;
            switch (dockingMethod)
            {
            case fs::Gui::DockingMethod::LeftSide:
                resultDockPosition = _position + offset;
                resultDockPosition._y += getDockSizeIfHosting(DockingMethod::TopSide)._y;
                break;
            case fs::Gui::DockingMethod::RightSide:
                resultDockPosition = fs::Float2(_position._x + _displaySize._x - dockSize._x, _position._y) + offset;
                resultDockPosition._y += getDockSizeIfHosting(DockingMethod::TopSide)._y;
                break;
            case fs::Gui::DockingMethod::TopSide:
                resultDockPosition = fs::Float2(_position._x, _position._y) + offset;
                break;
            case fs::Gui::DockingMethod::BottomSide:
                resultDockPosition = fs::Float2(_position._x, _position._y + _displaySize._y - dockSize._y);
                break;
            case fs::Gui::DockingMethod::COUNT:
            default:
                break;
            }
            return resultDockPosition;
        }

        FS_INLINE const float GuiContext::ControlData::getHorzDockSizeSum() const noexcept
        {
            return getDockSizeIfHosting(DockingMethod::LeftSide)._x + getDockSizeIfHosting(DockingMethod::RightSide)._x;
        }

        FS_INLINE const float GuiContext::ControlData::getVertDockSizeSum() const noexcept
        {
            return getDockSizeIfHosting(DockingMethod::TopSide)._y + getDockSizeIfHosting(DockingMethod::BottomSide)._y;
        }

        FS_INLINE const fs::Float2 GuiContext::ControlData::getMenuBarThickness() const noexcept
        {
            fs::Float2 result;
            const MenuBarType currentMenuBarType = _controlValue.getCurrentMenuBarType();
            if (currentMenuBarType == MenuBarType::Top || currentMenuBarType == MenuBarType::Bottom)
            {
                result._y = kMenuBarBaseSize._y;
            }
            return result;
        }

        FS_INLINE void GuiContext::ControlData::connectToDock(const uint64 dockControlHashKey) noexcept
        {
            _dockControlHashKey = dockControlHashKey;
        }

        FS_INLINE void GuiContext::ControlData::disconnectFromDock() noexcept
        {
            _dockControlHashKey = 0;
        }

        FS_INLINE const uint64 GuiContext::ControlData::getDockControlHashKey() const noexcept
        {
            return _dockControlHashKey;
        }

        FS_INLINE const bool GuiContext::ControlData::isDocking() const noexcept
        {
            return (_dockControlHashKey != 0);
        }

        FS_INLINE const bool GuiContext::ControlData::isDockHosting() const noexcept
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

        FS_INLINE const bool GuiContext::ControlData::isResizable() const noexcept
        {
            return _resizingMask.isResizable();
        }

        FS_INLINE Rect GuiContext::ControlData::getControlRect() const noexcept
        {
            return Rect::fromPositionSize(_position, _displaySize);
        }

        FS_INLINE Rect GuiContext::ControlData::getControlPaddedRect() const noexcept
        {
            return Rect
            (
                _position._x + _innerPadding.left(),
                _position._x + _displaySize._x - _innerPadding.horz(),
                _position._y + _innerPadding.top(), 
                _position._y + _displaySize._y - _innerPadding.vert()
            );
        }

        FS_INLINE void GuiContext::ControlData::connectChildWindowIfNot(const ControlData& childWindowControlData) noexcept
        {
            if (childWindowControlData._controlType == ControlType::Window && _childWindowHashKeyMap.find(childWindowControlData._hashKey).isValid() == false)
            {
                _childWindowHashKeyMap.insert(childWindowControlData._hashKey, true);
            }
        }

        FS_INLINE void GuiContext::ControlData::disconnectChildWindow(const uint64 childWindowHashKey) noexcept
        {
            _childWindowHashKeyMap.erase(childWindowHashKey);
        }

        FS_INLINE const fs::HashMap<uint64, bool>& GuiContext::ControlData::getChildWindowHashKeyMap() const noexcept
        {
            return _childWindowHashKeyMap;
        }

        FS_INLINE void GuiContext::ControlData::setVisibleState(const VisibleState visibleState) noexcept
        {
            _visibleState = visibleState;
        }

        FS_INLINE void GuiContext::ControlData::swapDockingStateContext() noexcept
        {
            std::swap(_displaySize, _dokcingStateContext._displaySize);
            std::swap(_resizingMask, _dokcingStateContext._resizingMask);
        }

        FS_INLINE void GuiContext::ControlData::setParentHashKeyXXX(const uint64 parentHashKey) noexcept
        {
            _parentHashKey = parentHashKey;
        }

        FS_INLINE void GuiContext::ControlData::setOffsetY_XXX(const float offsetY) noexcept
        {
            _nextChildOffset._y = offsetY;
        }

        FS_INLINE void GuiContext::ControlData::setViewportIndexXXX(const uint32 viewportIndex) noexcept
        {
            _viewportIndex = viewportIndex;
        }

        FS_INLINE void GuiContext::ControlData::setViewportIndexForChildrenXXX(const uint32 viewportIndex) noexcept
        {
            _viewportIndexForChildren = viewportIndex;
        }

        FS_INLINE void GuiContext::ControlData::setViewportIndexForDocksXXX(const uint32 viewportIndex) noexcept
        {
            _viewportIndexForDocks = viewportIndex;
        }


        inline GuiContext::ControlStackData::ControlStackData(const ControlData& controlData)
            : _controlType{ controlData.getControlType() }
            , _hashKey{ controlData.getHashKey() }
        {
            __noop;
        }


        FS_INLINE void GuiContext::nextSameLine()
        {
            _nextSameLine = true;
        }

        FS_INLINE void GuiContext::nextControlSize(const fs::Float2& size, const bool force)
        {
            _nextDesiredControlSize = size;
            _nextSizingForced = force;
        }

        FS_INLINE void GuiContext::nextNoInterval()
        {
            _nextNoInterval = true;
        }

        FS_INLINE void GuiContext::nextNoAutoPositioned()
        {
            _nextNoAutoPositioned = true;
        }

        FS_INLINE void GuiContext::nextControlSizeNonContrainedToParent()
        {
            _nextControlSizeNonContrainedToParent = true;
        }

        FS_INLINE void GuiContext::nextControlPosition(const fs::Float2& position)
        {
            _nextControlPosition = position;
        }

        FS_INLINE void GuiContext::nextTooltip(const wchar_t* const tooltipText)
        {
            _nextTooltipText = tooltipText;
        }

        FS_INLINE void GuiContext::resetNextStates()
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

        FS_INLINE const GuiContext::ControlData& GuiContext::getControlStackTopXXX() const noexcept
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

        FS_INLINE GuiContext::ControlData& GuiContext::getControlStackTopXXX() noexcept
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

        FS_INLINE GuiContext::ControlData& GuiContext::getControlData(const uint64 hashKey) noexcept
        {
            auto found = _controlIdMap.find(hashKey);
            if (found.isValid() == true)
            {
                return *found._value;
            }
            return _rootControlData;
        }

        FS_INLINE const GuiContext::ControlData& GuiContext::getControlData(const uint64 hashKey) const noexcept
        {
            auto found = _controlIdMap.find(hashKey);
            if (found.isValid() == true)
            {
                return *found._value;
            }

            //FS_ASSERT("김장원", false, "hashKey 가 존재하지 않는 ControlData 입니다!!!");
            return _rootControlData;
        }
        
        FS_INLINE fs::Float4 GuiContext::getControlCenterPosition(const ControlData& controlData) const noexcept
        {
            return fs::Float4(controlData._position._x + controlData._displaySize._x * 0.5f, controlData._position._y + controlData._displaySize._y * 0.5f, 0.0f, 1.0f);
        }

        FS_INLINE fs::Float2 GuiContext::getControlPositionInParentSpace(const ControlData& controlData) const noexcept
        {
            return controlData._position - getControlData(controlData.getParentHashKey())._position;
        }

        FS_INLINE const bool GuiContext::isControlHovered(const ControlData& controlData) const noexcept
        {
            return (_hoveredControlHashKey == controlData.getHashKey());
        }

        FS_INLINE const bool GuiContext::isControlPressed(const ControlData& controlData) const noexcept
        {
            return (_pressedControlHashKey == controlData.getHashKey());
        }

        FS_INLINE const bool GuiContext::isControlClicked(const ControlData& controlData) const noexcept
        {
            return (_clickedControlHashKeyPerFrame == controlData.getHashKey());
        }

        FS_INLINE const bool GuiContext::isControlFocused(const ControlData& controlData) const noexcept
        {
            return (_focusedControlHashKey == ((0 != controlData._delegateHashKey) ? controlData._delegateHashKey : controlData.getHashKey()));
        }

        FS_INLINE const fs::RenderingBase::Color& GuiContext::getNamedColor(const NamedColor namedColor) const noexcept
        {
            return _namedColors[static_cast<uint32>(namedColor)];
        }

        FS_INLINE fs::RenderingBase::Color& GuiContext::getNamedColor(const NamedColor namedColor) noexcept
        {
            return _namedColors[static_cast<uint32>(namedColor)];
        }

        FS_INLINE const float GuiContext::getMouseWheelScroll(const ControlData& scrollParentControlData) const noexcept
        {
            float result = 0.0f;
            if (0.0f != _mouseWheel && isInControlInteractionArea(_mousePosition, scrollParentControlData) == true)
            {
                result = _mouseWheel * kMouseWheelScrollScale;
                _mouseWheel = 0.0f;
            }
            return result;
        }
        
        FS_INLINE const float GuiContext::calculateTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept
        {
            return _shapeFontRendererContextTopMost.calculateTextWidth(wideText, textLength);
        }

        FS_INLINE const uint32 GuiContext::calculateIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept
        {
            return _shapeFontRendererContextTopMost.calculateIndexFromPositionInText(wideText, textLength, positionInText);
        }
    }
}

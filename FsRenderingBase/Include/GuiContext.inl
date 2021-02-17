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

		FS_INLINE void ControlValue::setScrollBarType(const ScrollBarType scrollBarType) noexcept
		{
			_i[0] = static_cast<int32>(scrollBarType);
		}

		FS_INLINE void ControlValue::setThumbAt(const float thumbAt) noexcept
		{
			_f[0] = thumbAt;
		}

		FS_INLINE void ControlValue::setIsToggled(const bool isToggled) noexcept
		{
			_i[0] = (isToggled == true) ? 1 : 0;
		}


		FS_INLINE const ScrollBarType& ControlValue::getScrollBarType() const noexcept
		{
			return *reinterpret_cast<const ScrollBarType*>(&_i);
		}

		FS_INLINE const float ControlValue::getThumbAt() const noexcept
		{
			return _f[0];
		}

		FS_INLINE const bool ControlValue::getIsToggled() const noexcept
		{
			return (_i[0] == 1);
		}

		FS_INLINE uint16& ControlValue::getCaretAt() noexcept
		{
			return _hui[0];
		}

		FS_INLINE uint16& ControlValue::getCaretState() noexcept
		{
			return _hui[1];
		}

		FS_INLINE uint16& ControlValue::getSelectionStart() noexcept
		{
			return _hui[2];
		}

		FS_INLINE uint16& ControlValue::getSelectionLength() noexcept
		{
			return _hui[3];
		}

		FS_INLINE float& ControlValue::getTextDisplayOffset() noexcept
		{
			return _f[2];
		}

		FS_INLINE uint64& ControlValue::getLastCaretBlinkTimeMs() noexcept
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
			, _controlState{ ControlState::Visible }
			, _viewportIndex{ 0 }
			, _viewportIndexForChildren{ 0 }
			, _viewportIndexForDocks{ 0 }
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
			_childControlDataArray.clear();
			_deltaPosition.setZero();
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
			if (_controlType == ControlType::Window)
			{
				return fs::Float2(_displaySize._x, _displaySize._y - kTitleBarBaseSize._y);
			}
			return _displaySize;
		}

		FS_INLINE const fs::Float2& GuiContext::ControlData::getDisplaySizeMin() const noexcept
		{
			return _displaySizeMin;
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

		FS_INLINE const wchar_t* GuiContext::ControlData::getText() const noexcept
		{
			return _text.c_str();
		}

		FS_INLINE const bool GuiContext::ControlData::isRootControl() const noexcept
		{
			return _controlType == ControlType::ROOT;
		}

		FS_INLINE const bool GuiContext::ControlData::isControlState(const ControlState controlState) const noexcept
		{
			return controlState == _controlState;
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

		FS_INLINE const std::vector<GuiContext::ControlData>& GuiContext::ControlData::getChildControlDataArray() const noexcept
		{
			return _childControlDataArray;
		}

		FS_INLINE const std::vector<GuiContext::ControlData>& GuiContext::ControlData::getPreviousChildControlDataArray() const noexcept
		{
			return _previousChildControlDataArray;
		}

		FS_INLINE void GuiContext::ControlData::prepareChildControlDataArray() noexcept
		{
			std::swap(_childControlDataArray, _previousChildControlDataArray);
			_childControlDataArray.clear();
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
			const DockDatum& dockDatumTopSide = getDockDatum(DockingMethod::TopSide);
			const DockDatum& dockDatumBottomSide = getDockDatum(DockingMethod::BottomSide);
			const DockDatum& dockDatum = getDockDatum(dockingMethod);
			fs::Float2 resultDockSize = dockDatum.getRawDockSizeXXX();
			switch (dockingMethod)
			{
			case fs::Gui::DockingMethod::LeftSide:
			case fs::Gui::DockingMethod::RightSide:
				resultDockSize._y = _displaySize._y - ((_controlType == ControlType::Window) ? kTitleBarBaseSize._y + _innerPadding.top() + _innerPadding.bottom() : 0.0f);
				if (dockDatumTopSide.hasDockedControls() == true)
				{
					resultDockSize._y -= dockDatumTopSide.getRawDockSizeXXX()._y;
				}
				if (dockDatumBottomSide.hasDockedControls() == true)
				{
					resultDockSize._y -= dockDatumBottomSide.getRawDockSizeXXX()._y;
				}
				break;
			case fs::Gui::DockingMethod::TopSide:
			case fs::Gui::DockingMethod::BottomSide:
				resultDockSize._x = _displaySize._x - ((_controlType == ControlType::Window) ? _innerPadding.left() + _innerPadding.right() : 0.0f);
				break;
			case fs::Gui::DockingMethod::COUNT:
				break;
			default:
				break;
			}
			return resultDockSize;
		}

		FS_INLINE const fs::Float2 GuiContext::ControlData::getDockOffsetSize() const noexcept
		{
			return fs::Float2(0.0f, (_controlType == ControlType::Window) ? kTitleBarBaseSize._y + _innerPadding.top() : 0.0f);
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
				if (dockDatumTopSide.hasDockedControls() == true)
				{
					const fs::Float2& dockSizeTopSide = getDockSize(DockingMethod::TopSide);
					resultDockPosition._y += dockSizeTopSide._y;
				}
				break;
			case fs::Gui::DockingMethod::RightSide:
				resultDockPosition = fs::Float2(_position._x + _displaySize._x - dockSize._x, _position._y) + offset;
				if (dockDatumTopSide.hasDockedControls() == true)
				{
					const fs::Float2& dockSizeTopSide = getDockSize(DockingMethod::TopSide);
					resultDockPosition._y += dockSizeTopSide._y;
				}
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

		inline const float GuiContext::ControlData::getHorzDockSizeSum() const noexcept
		{
			float sum = 0.0f;
			{
				const fs::Float2& dockSize = getDockSize(DockingMethod::LeftSide);
				const DockDatum& dockDatum = getDockDatum(DockingMethod::LeftSide);
				if (dockDatum._dockedControlHashArray.empty() == false)
				{
					sum += dockSize._x;
				}
			}
			{
				const fs::Float2& dockSize = getDockSize(DockingMethod::RightSide);
				const DockDatum& dockDatum = getDockDatum(DockingMethod::RightSide);
				if (dockDatum._dockedControlHashArray.empty() == false)
				{
					sum += dockSize._x;
				}
			}
			return sum;
		}

		inline const float GuiContext::ControlData::getVertDockSizeSum() const noexcept
		{
			float sum = 0.0f;
			{
				const fs::Float2& dockSize = getDockSize(DockingMethod::TopSide);
				const DockDatum& dockDatum = getDockDatum(DockingMethod::TopSide);
				if (dockDatum._dockedControlHashArray.empty() == false)
				{
					sum += dockSize._y;
				}
			}
			{
				const fs::Float2& dockSize = getDockSize(DockingMethod::BottomSide);
				const DockDatum& dockDatum = getDockDatum(DockingMethod::BottomSide);
				if (dockDatum._dockedControlHashArray.empty() == false)
				{
					sum += dockSize._y;
				}
			}
			return sum;
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

		FS_INLINE const bool GuiContext::ControlData::hasChildWindowConnected(const uint64 childWindowHashKey) const noexcept
		{
			return _childWindowHashKeyMap.find(childWindowHashKey) != _childWindowHashKeyMap.end();
		}

		FS_INLINE void GuiContext::ControlData::connectChildWindow(const uint64 childWindowHashKey) noexcept
		{
			_childWindowHashKeyMap.insert(std::make_pair(childWindowHashKey, true));
		}

		FS_INLINE void GuiContext::ControlData::disconnectChildWindow(const uint64 childWindowHashKey) noexcept
		{
			_childWindowHashKeyMap.erase(childWindowHashKey);
		}

		FS_INLINE const std::unordered_map<uint64, bool>& GuiContext::ControlData::getChildWindowHashKeyMap() const noexcept
		{
			return _childWindowHashKeyMap;
		}

		FS_INLINE void GuiContext::ControlData::setControlState(const ControlState controlState) noexcept
		{
			_controlState = controlState;
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
			_nextControlSize = size;
			_nextSizingForced = force;
		}

		FS_INLINE void GuiContext::nextNoAutoPositioned()
		{
			_nextNoAutoPositioned = true;
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
			_nextControlSize.setZero();
			_nextSizingForced = false;
			_nextNoAutoPositioned = false;
			_nextControlPosition.setZero();
			_nextTooltipText = nullptr;
		}

		FS_INLINE const GuiContext::ControlData& GuiContext::getControlDataStackTopXXX() const noexcept
		{
			if (_controlStackPerFrame.empty() == false)
			{
				auto found = _controlIdMap.find(_controlStackPerFrame.back()._hashKey);
				if (found != _controlIdMap.end())
				{
					return found->second;
				}
			}
			return _rootControlData;
		}

		FS_INLINE GuiContext::ControlData& GuiContext::getControlDataStackTopXXX() noexcept
		{
			if (_controlStackPerFrame.empty() == false)
			{
				auto found = _controlIdMap.find(_controlStackPerFrame.back()._hashKey);
				if (found != _controlIdMap.end())
				{
					return found->second;
				}
			}
			return _rootControlData;
		}

		FS_INLINE GuiContext::ControlData& GuiContext::getControlData(const uint64 hashKey) noexcept
		{
			auto found = _controlIdMap.find(hashKey);
			if (found != _controlIdMap.end())
			{
				return found->second;
			}
			return _rootControlData;
		}

		FS_INLINE const GuiContext::ControlData& GuiContext::getControlData(const uint64 hashKey) const noexcept
		{
			auto found = _controlIdMap.find(hashKey);
			if (found != _controlIdMap.end())
			{
				return found->second;
			}

			//FS_ASSERT("�����", false, "hashKey �� �������� �ʴ� ControlData �Դϴ�!!!");
			return _rootControlData;
		}
		
		FS_INLINE fs::Float4 GuiContext::getControlCenterPosition(const ControlData& controlData) const noexcept
		{
			return fs::Float4(controlData._position._x + controlData._displaySize._x * 0.5f, controlData._position._y + controlData._displaySize._y * 0.5f, 0.0f, 1.0f);
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

		FS_INLINE const fs::SimpleRendering::Color& GuiContext::getNamedColor(const NamedColor namedColor) const noexcept
		{
			return _namedColors[static_cast<uint32>(namedColor)];
		}

		FS_INLINE fs::SimpleRendering::Color& GuiContext::getNamedColor(const NamedColor namedColor) noexcept
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
	}
}
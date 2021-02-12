#include "GuiContext.h"
#pragma once


namespace fs
{
	namespace Gui
	{
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
			: _interactionSize{ size }
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

		FS_INLINE const Rect& GuiContext::ControlData::getInnerPadding() const noexcept
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

		FS_INLINE void GuiContext::ControlData::setDockSize(const DockingMethod dockingMethod, const fs::Float2& dockSize) noexcept
		{
			getDockDatum(dockingMethod).setRawDockSize(dockSize);
		}

		FS_INLINE const fs::Float2 GuiContext::ControlData::getDockSize(const DockingMethod dockingMethod) const noexcept
		{
			const DockDatum& dockDatum = getDockDatum(dockingMethod);
			fs::Float2 resultDockSize = dockDatum.getRawDockSizeXXX();
			switch (dockingMethod)
			{
			case fs::Gui::DockingMethod::LeftSide:
			case fs::Gui::DockingMethod::RightSide:
				resultDockSize._y = _displaySize._y - ((_controlType == ControlType::Window) ? kTitleBarBaseSize._y + _innerPadding.top() + _innerPadding.bottom() : 0.0f);
				break;
			case fs::Gui::DockingMethod::COUNT:
				break;
			default:
				break;
			}
			return resultDockSize;
		}

		FS_INLINE const fs::Float2 GuiContext::ControlData::getDockOffsetSize(const DockingMethod dockingMethod) const noexcept
		{
			return fs::Float2(0.0f, (_controlType == ControlType::Window) ? kTitleBarBaseSize._y + _innerPadding.top() : 0.0f);
		}

		FS_INLINE const fs::Float2 GuiContext::ControlData::getDockPosition(const DockingMethod dockingMethod) const noexcept
		{
			const fs::Float2& dockSize = getDockSize(dockingMethod);
			const fs::Float2& offset = getDockOffsetSize(dockingMethod);
			if (dockingMethod == DockingMethod::RightSide)
			{
				return fs::Float2(_position._x + _displaySize._x - dockSize._x, _position._y) + offset;
			}
			return _position + offset;
		}

		inline const fs::Float2 GuiContext::ControlData::getHorzDockSizeSum() const noexcept
		{
			fs::Float2 sum = fs::Float2::kZero;
			for (DockingMethod dockingMethodIter = static_cast<DockingMethod>(0); dockingMethodIter != DockingMethod::COUNT; dockingMethodIter = static_cast<DockingMethod>(static_cast<uint32>(dockingMethodIter) + 1))
			{
				const fs::Float2& dockSize = getDockSize(dockingMethodIter);
				const DockDatum& dockDatum = getDockDatum(dockingMethodIter);
				if (dockDatum._dockedControlHashArray.empty() == false)
				{
					sum._x += dockSize._x;
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

			//FS_ASSERT("김장원", false, "hashKey 가 존재하지 않는 ControlData 입니다!!!");
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
	}
}

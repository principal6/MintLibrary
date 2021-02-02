#include "GuiContext.h"
#pragma once


namespace fs
{
	namespace Gui
	{
		inline GuiContext::ControlData::ControlData()
			: ControlData(0, 0, ControlType::ROOT)
		{
			__noop;
		}

		inline GuiContext::ControlData::ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType)
			: ControlData(hashKey, parentHashKey, controlType, fs::Float2::kZero)
		{
			__noop;
		}
		
		inline GuiContext::ControlData::ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType, const fs::Float2& size)
			: _interactionSize{ size }
			, _isFocusable{ false }
			, _isResizable{ false }
			, _isDraggable{ false }
			, _hashKey{ hashKey }
			, _parentHashKey{ parentHashKey }
			, _displaySize{ size }
			, _displaySizeMin{ kControlDisplayMinWidth, kControlDisplayMinHeight }
			, _childAt{ _innerPadding.left(), _innerPadding.top() }
			, _delegateHashKey{ 0 }
			, _dockingType{ DockingType::None }
			, _controlType{ controlType }
			, _controlState{ ControlState::Visible }
			, _viewportIndex{ 0 }
			, _childViewportIndex{ 0 }
			, _hasChildWindow{ false }
			, _previousHasChildWindow{ false }
			, _depth{ 1.0f }
		{
			_draggingConstraints.setNan();

			if (controlType == ControlType::ROOT)
			{
				_dockingType = DockingType::Dock;
			}
		}

		FS_INLINE void GuiContext::ControlData::clearPerFrameData() noexcept
		{
			_nextChildOffset.setZero();
			_previousClientSize = _clientSize;
			_clientSize.setZero();
			_childControlDataArray.clear();
			_deltaPosition.setZero();
			_previousHasChildWindow = _hasChildWindow;
			_hasChildWindow = false;
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

		FS_INLINE const fs::Float2& GuiContext::ControlData::getDisplaySize() const noexcept
		{
			return _displaySize;
		}

		FS_INLINE const fs::Float2& GuiContext::ControlData::getDisplaySizeMin() const noexcept
		{
			return _displaySizeMin;
		}

		FS_INLINE const fs::Float2& GuiContext::ControlData::getClientSize() const noexcept
		{
			return _clientSize;
		}

		FS_INLINE const fs::Float2& GuiContext::ControlData::getPreviousClientSize() const noexcept
		{
			return _previousClientSize;
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

		FS_INLINE const uint32 GuiContext::ControlData::getChildViewportIndex() const noexcept
		{
			return _childViewportIndex;
		}

		FS_INLINE const std::vector<GuiContext::ControlData>& GuiContext::ControlData::getChildControlDataArray() const noexcept
		{
			return _childControlDataArray;
		}

		FS_INLINE const bool& GuiContext::ControlData::hasChildWindowInternalXXX() const noexcept
		{
			return _hasChildWindow;
		}

		FS_INLINE const bool& GuiContext::ControlData::hasChildWindow() const noexcept
		{
			return _previousHasChildWindow;
		}

		FS_INLINE const float GuiContext::ControlData::getDepth() const noexcept
		{
			return _depth;
		}

		FS_INLINE void GuiContext::ControlData::setControlState(const ControlState controlState) noexcept
		{
			_controlState = controlState;
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

		FS_INLINE void GuiContext::ControlData::setChildViewportIndexXXX(const uint32 viewportIndex) noexcept
		{
			_childViewportIndex = viewportIndex;
		}

		FS_INLINE void GuiContext::ControlData::setDepthXXX(const float depth) noexcept
		{
			_depth = depth;
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
			return fs::Float4(controlData._position._x + controlData.getDisplaySize()._x * 0.5f, controlData._position._y + controlData.getDisplaySize()._y * 0.5f, controlData.getDepth(), 1.0f);
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

#include "GuiContext.h"
#pragma once


namespace fs
{
	namespace Gui
	{
		inline GuiContext::ControlData::ControlData()
			: ControlData(fs::Float4())
		{
			__noop;
		}

		inline GuiContext::ControlData::ControlData(const fs::Float4& innerPadding)
			: _hashKey{ 0 }
			, _parentHashKey{ 0 }
			, _innerPadding{ innerPadding }
			, _childAt{ _innerPadding._x, _innerPadding._z }
			, _isFocusable{ false }
			, _controlTypeForDebug{ ControlType::ROOT }
			, _controlState{ ControlState::Visible }
		{
			__noop;
		}


		inline GuiContext::ControlStackData::ControlStackData(const ControlType controlType, const uint64 hashKey)
			: _controlType{ controlType }
			, _hashKey{ hashKey }
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

		FS_INLINE void GuiContext::resetNextStates()
		{
			_nextSameLine = false;
			_nextControlSize = fs::Float2(0.0f);
			_nextSizingForced = false;
			_nextNoAutoPositioned = false;
		}

		FS_INLINE const GuiContext::ControlData& GuiContext::getStackTopControlData() noexcept
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

			FS_ASSERT("김장원", false, "hashKey 가 존재하지 않는 ControlData 입니다!!!");
			return _rootControlData;
		}
		
		FS_INLINE fs::Float3 GuiContext::getControlCenterPosition(const ControlData& controlData) const noexcept
		{
			return fs::Float3(controlData._position._x + controlData._displaySize._x * 0.5f, controlData._position._y + controlData._displaySize._y * 0.5f, 0.0f);
		}

	}
}

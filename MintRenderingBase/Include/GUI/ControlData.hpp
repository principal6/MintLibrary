#pragma once


#ifndef _MINT_RENDERING_BASE_GUI_CONTROL_DATA_HPP_
#define _MINT_RENDERING_BASE_GUI_CONTROL_DATA_HPP_


#include <MintRenderingBase/Include/GUI/ControlData.h>

#include <MintContainer/Include/Hash.hpp>
#include <MintContainer/Include/StackString.hpp>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/StringReference.hpp>


namespace mint
{
	template<>
	inline uint64 Hasher<Rendering::GUI::ControlID>::operator()(const Rendering::GUI::ControlID& value) const noexcept
	{
		return ComputeHash(value.GetRawID());
	}


	namespace Rendering
	{
		namespace GUI
		{
			ControlData::ControlData(const ControlID& ID, const ControlType type)
				: _text{ nullptr }
				, _mouseInteractionState{ MouseInteractionState::None }
				, _perTypeData{}
				, _resizableMinSize{ Float2::kOne }
				, _ID{ ID }
				, _type{ type }
				, _accessCount{ 0 }
			{
				__noop;
			}

			ControlID ControlData::GenerateID(const ControlType type, const wchar_t* const text, const ControlID& parentControlID)
			{
				StackStringW<512> key;
				StackStringW<512> conv;
				key.Append(text);
				key.Append(L"_");
				StringUtil::ToString(static_cast<uint32>(type), conv);
				key.Append(conv);
				key.Append(L"_");
				StringUtil::ToString(parentControlID.GetRawID(), conv);
				key.Append(conv);
				return ControlID(key.ComputeHash());
			}

			inline void ControlData::UpdateZones()
			{
				ComputeTitleBarZone(_zones._titleBarZone);
				ComputeMenuBarZone(_zones._menuBarZone);

				ComputeContentZone(_zones._contentZone);
				ComputeContentZone(_zones._visibleContentZone);
			}

			inline Float2 ControlData::ComputeRelativePosition(const ControlData& parentControlData) const
			{
				return _absolutePosition - parentControlData._absolutePosition;
			}

			inline void ControlData::ComputeTitleBarZone(Rect& titleBarZone)
			{
				titleBarZone = Rect(Float2::kZero, Float2(_size._x, 0.0f));
				if (_type == ControlType::Window)
				{
					titleBarZone.Bottom() += (_perTypeData._windowData._titleBarHeight);
				}
			}

			inline void ControlData::ComputeMenuBarZone(Rect& menuBarZone)
			{
				menuBarZone = Rect(_zones._titleBarZone.Position() + Float2(0.0f, _zones._titleBarZone.Height()), Float2(_size._x, 0.0f));
				if (_type == ControlType::Window)
				{
					menuBarZone.Bottom() += _perTypeData._windowData._menuBarHeight;
				}
			}

			inline void ControlData::ComputeContentZone(Rect& contentZone)
			{
				contentZone = Rect(_zones._menuBarZone.Position() + Float2(0.0f, _zones._menuBarZone.Height()), _size);
			}
		}
	}
}


#endif // !_MINT_RENDERING_BASE_GUI_CONTROL_DATA_HPP_

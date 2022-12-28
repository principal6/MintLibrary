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
		return computeHash(value.getRawID());
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

			ControlID ControlData::generateID(const FileLine& fileLine, const ControlType type, const wchar_t* const text, const ControlID& parentControlID)
			{
				StackStringA<512> file = fileLine._file;
				StackStringW<512> key;
				StackStringW<512> conv;
				StringUtil::convertStackStringAToStackStringW(file, key);
				key.append(L"_");
				StringUtil::toString(fileLine._line, conv);
				key.append(conv);
				key.append(L"_");
				key.append(text);
				key.append(L"_");
				StringUtil::toString(static_cast<uint32>(type), conv);
				key.append(conv);
				key.append(L"_");
				StringUtil::toString(parentControlID.getRawID(), conv);
				key.append(conv);
				return ControlID(key.computeHash());
			}

			inline void ControlData::updateZones()
			{
				computeTitleBarZone(_zones._titleBarZone);
				computeMenuBarZone(_zones._menuBarZone);

				computeContentZone(_zones._contentZone);
				computeContentZone(_zones._visibleContentZone);
			}

			inline Float2 ControlData::computeRelativePosition(const ControlData& parentControlData) const
			{
				return _absolutePosition - parentControlData._absolutePosition;
			}

			inline void ControlData::computeTitleBarZone(Rect& titleBarZone)
			{
				titleBarZone = Rect(Float2::kZero, Float2(_size._x, 0.0f));
				if (_type == ControlType::Window)
				{
					titleBarZone.bottom() += (_perTypeData._windowData._titleBarHeight);
				}
			}

			inline void ControlData::computeMenuBarZone(Rect& menuBarZone)
			{
				menuBarZone = Rect(_zones._titleBarZone.position() + Float2(0.0f, _zones._titleBarZone.height()), Float2(_size._x, 0.0f));
				if (_type == ControlType::Window)
				{
					menuBarZone.bottom() += _perTypeData._windowData._menuBarHeight;
				}
			}

			inline void ControlData::computeContentZone(Rect& contentZone)
			{
				contentZone = Rect(_zones._menuBarZone.position() + Float2(0.0f, _zones._menuBarZone.height()), _size);
			}
		}
	}
}


#endif // !_MINT_RENDERING_BASE_GUI_CONTROL_DATA_HPP_

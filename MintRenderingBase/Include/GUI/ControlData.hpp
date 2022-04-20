#pragma once


#ifndef _MINT_GUI_CONTROL_DATA_HPP_
#define _MINT_GUI_CONTROL_DATA_HPP_


#include <MintRenderingBase/Include/GUI/ControlData.h>

#include <MintContainer/Include/Hash.hpp>
#include <MintContainer/Include/ScopeString.hpp>
#include <MintContainer/Include/StringUtil.hpp>


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
                , _interactionState{ InteractionState::None }
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
                ScopeStringA<512> file = fileLine._file;
                ScopeStringW<512> key;
                StringUtil::convertScopeStringAToScopeStringW(file, key);
                key.append(L"_");
                key.append(StringUtil::convertToStringW(fileLine._line).c_str());
                key.append(L"_");
                key.append(text);
                key.append(L"_");
                key.append(StringUtil::convertToStringW(static_cast<uint32>(type)).c_str());
                key.append(L"_");
                key.append(StringUtil::convertToStringW(parentControlID.getRawID()).c_str());
                return ControlID(key.computeHash());
            }

            inline void ControlData::updateZones()
            {
                computeTitleBarZone(_zones._titleBarZone);

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

            inline void ControlData::computeContentZone(Rect& contentZone)
            {
                contentZone = Rect(Float2::kZero, _size);
                if (_type == ControlType::Window)
                {
                    contentZone.top() += (_perTypeData._windowData._titleBarHeight + _perTypeData._windowData._menuBarHeight);
                }
            }
        }
    }
}


#endif // !_MINT_GUI_CONTROL_DATA_HPP_

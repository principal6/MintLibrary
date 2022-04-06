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
    inline const uint64 Hasher<Rendering::ControlID>::operator()(const Rendering::ControlID& value) const noexcept
    {
        return computeHash(value.getRawID());
    }


    namespace Rendering
    {
        ControlData::ControlData(const ControlID& ID, const ControlType type)
            : _interactionState{ InteractionState::None }
            , _perTypeData{}
            , _ID{ ID }
            , _type{ type }
            , _accessCount{ 0 }
        {
            __noop;
        }

        const ControlID ControlData::generateID(const FileLine& fileLine, const ControlType type, const wchar_t* const text, const ControlID& parentControlID)
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
            computeContentZone();
            computeTitleBarZone();
            computeVisibleContentZone();
        }

        inline void ControlData::computeContentZone()
        {
            _zones._contentZone = Rect(Float2::kZero, _size);
            if (_type == ControlType::Window)
            {
                _zones._contentZone.top() += (_perTypeData._windowData._titleBarHeight + _perTypeData._windowData._menuBarHeight);
            }
        }

        inline void ControlData::computeVisibleContentZone()
        {
            _zones._visibleContentZone = Rect(Float2::kZero, _size);
            if (_type == ControlType::Window)
            {
                _zones._visibleContentZone.top() += (_perTypeData._windowData._titleBarHeight + _perTypeData._windowData._menuBarHeight);
            }
        }

        inline void ControlData::computeTitleBarZone()
        {
            _zones._titleBarZone = Rect(Float2::kZero, Float2(_size._x, 0.0f));
            if (_type == ControlType::Window)
            {
                _zones._titleBarZone.bottom() += (_perTypeData._windowData._titleBarHeight);
            }
        }

#pragma region Dragging
        inline const bool ControlData::Dragging::isDragging() const
        {
            return _absolutePressedPosition.isNan() == false;
        }

        inline void ControlData::Dragging::beginDragging(const ControlData& controlData, const Float2& absoluteMousePressedPosition)
        {
            if (isDragging())
            {
                return;
            }

            _absolutePressedPosition = controlData._absolutePosition;
            _absoluteMousePressedPosition = absoluteMousePressedPosition;
        }

        inline void ControlData::Dragging::endDragging()
        {
            _absolutePressedPosition.setNan();
        }

        inline Float2 ControlData::Dragging::computeRelativeMousePressedPosition() const
        {
            return _absoluteMousePressedPosition - _absolutePressedPosition;
        }
#pragma endregion
    }
}


#endif // !_MINT_GUI_CONTROL_DATA_HPP_

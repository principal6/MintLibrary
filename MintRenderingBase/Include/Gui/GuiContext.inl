#include "GuiContext.h"
#pragma once


namespace mint
{
    namespace Gui
    {
#pragma region TaskWhenMouseUp
        MINT_INLINE void TaskWhenMouseUp::clear() noexcept
        {
            _controlIDForUpdateDockZoneData.reset();
        }

        MINT_INLINE const bool TaskWhenMouseUp::isSet() const noexcept
        {
            return _controlIDForUpdateDockZoneData.isValid();
        }

        MINT_INLINE void TaskWhenMouseUp::setUpdateDockZoneData(const ControlID& controlID) noexcept
        {
            _controlIDForUpdateDockZoneData = controlID;
        }

        MINT_INLINE const ControlID& TaskWhenMouseUp::getUpdateDockZoneData() const noexcept
        {
            return _controlIDForUpdateDockZoneData;
        }
#pragma endregion


#pragma region GuiContext - ControlStackData
        inline GuiContext::ControlStackData::ControlStackData(const ControlData& controlData)
            : _controlType{ controlData.getControlType() }
            , _id{ controlData.getID() }
        {
            __noop;
        }
#pragma endregion


        MINT_INLINE const bool GuiContext::isValidControl(const ControlID& id) const noexcept
        {
            const auto found = _controlIDMap.find(_controlStackPerFrame.back()._id);
            return found.isValid();
        }

        MINT_INLINE const ControlData& GuiContext::getControlStackTopXXX() const noexcept
        {
            if (_controlStackPerFrame.empty() == false)
            {
                auto found = _controlIDMap.find(_controlStackPerFrame.back()._id);
                if (found.isValid() == true)
                {
                    return *found._value;
                }
            }
            return _rootControlData;
        }

        MINT_INLINE ControlData& GuiContext::accessControlStackTopXXX() noexcept
        {
            return const_cast<ControlData&>(getControlStackTopXXX());
        }

        MINT_INLINE const ControlData& GuiContext::getControlData(const ControlID& id) const noexcept
        {
            auto found = _controlIDMap.find(id);
            if (found.isValid() == true)
            {
                return *found._value;
            }
            return _rootControlData;
        }

        MINT_INLINE ControlData& GuiContext::accessControlData(const ControlID& id) noexcept
        {
            return const_cast<ControlData&>(getControlData(id));
        }

        MINT_INLINE Float2 GuiContext::getControlPositionInParentSpace(const ControlData& controlData) const noexcept
        {
            return controlData._position - getControlData(controlData.getParentID())._position;
        }

        MINT_INLINE const Rendering::Color& GuiContext::getNamedColor(const NamedColor namedColor) const noexcept
        {
            return _namedColors[static_cast<uint32>(namedColor)];
        }

        MINT_INLINE void GuiContext::setNamedColor(const NamedColor namedColor, const Rendering::Color& color) noexcept
        {
            _namedColors[static_cast<uint32>(namedColor)] = color;
        }

        MINT_INLINE const float GuiContext::computeTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept
        {
            return _rendererContexts[0].computeTextWidth(wideText, textLength);
        }

        MINT_INLINE const uint32 GuiContext::computeIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept
        {
            return _rendererContexts[0].computeIndexFromPositionInText(wideText, textLength, positionInText);
        }
    }
}

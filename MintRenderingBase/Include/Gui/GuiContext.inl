#include "GuiContext.h"
#pragma once


namespace mint
{
    namespace Gui
    {
#pragma region TaskWhenMouseUp
        MINT_INLINE void TaskWhenMouseUp::clear() noexcept
        {
            _controlIdForUpdateDockDatum.reset();
        }

        MINT_INLINE const bool TaskWhenMouseUp::isSet() const noexcept
        {
            return _controlIdForUpdateDockDatum.isValid();
        }

        MINT_INLINE void TaskWhenMouseUp::setUpdateDockDatum(const ControlId& controlId) noexcept
        {
            _controlIdForUpdateDockDatum = controlId;
        }

        MINT_INLINE const ControlId& TaskWhenMouseUp::getUpdateDockDatum() const noexcept
        {
            return _controlIdForUpdateDockDatum;
        }
#pragma endregion


#pragma region GuiContext - ControlStackData
        inline GuiContext::ControlStackData::ControlStackData(const ControlData& controlData)
            : _controlType{ controlData.getControlType() }
            , _id{ controlData.getId() }
        {
            __noop;
        }
#pragma endregion


        MINT_INLINE const bool GuiContext::isValidControl(const ControlId& id) const noexcept
        {
            const auto found = _controlIdMap.find(_controlStackPerFrame.back()._id);
            return found.isValid();
        }

        MINT_INLINE const ControlData& GuiContext::getControlStackTopXXX() const noexcept
        {
            if (_controlStackPerFrame.empty() == false)
            {
                auto found = _controlIdMap.find(_controlStackPerFrame.back()._id);
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

        MINT_INLINE const ControlData& GuiContext::getControlData(const ControlId& id) const noexcept
        {
            auto found = _controlIdMap.find(id);
            if (found.isValid() == true)
            {
                return *found._value;
            }
            return _rootControlData;
        }

        MINT_INLINE ControlData& GuiContext::accessControlData(const ControlId& id) noexcept
        {
            return const_cast<ControlData&>(getControlData(id));
        }

        MINT_INLINE Float2 GuiContext::getControlPositionInParentSpace(const ControlData& controlData) const noexcept
        {
            return controlData._position - getControlData(controlData.getParentId())._position;
        }

        MINT_INLINE const Rendering::Color& GuiContext::getNamedColor(const NamedColor namedColor) const noexcept
        {
            return _namedColors[static_cast<uint32>(namedColor)];
        }

        MINT_INLINE void GuiContext::setNamedColor(const NamedColor namedColor, const Rendering::Color& color) noexcept
        {
            _namedColors[static_cast<uint32>(namedColor)] = color;
        }

        MINT_INLINE const float GuiContext::calculateTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept
        {
            return _rendererContexts[0].calculateTextWidth(wideText, textLength);
        }

        MINT_INLINE const uint32 GuiContext::calculateIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept
        {
            return _rendererContexts[0].calculateIndexFromPositionInText(wideText, textLength, positionInText);
        }
    }
}

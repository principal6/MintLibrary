#include "GuiContext.h"
#pragma once


namespace mint
{
    namespace Gui
    {
        MINT_INLINE void TaskWhenMouseUp::clear() noexcept
        {
            _controlHashKeyForUpdateDockDatum = 0;
        }

        MINT_INLINE const bool TaskWhenMouseUp::isSet() const noexcept
        {
            return _controlHashKeyForUpdateDockDatum != 0;
        }

        MINT_INLINE void TaskWhenMouseUp::setUpdateDockDatum(const uint64 controlHashKey) noexcept
        {
            _controlHashKeyForUpdateDockDatum = controlHashKey;
        }

        MINT_INLINE const uint64 TaskWhenMouseUp::getUpdateDockDatum() const noexcept
        {
            return _controlHashKeyForUpdateDockDatum;
        }


        inline GuiContext::ControlStackData::ControlStackData(const ControlData& controlData)
            : _controlType{ controlData.getControlType() }
            , _hashKey{ controlData.getHashKey() }
        {
            __noop;
        }


        inline GuiContext::NextControlStates::NextControlStates()
        {
            reset();
        }

        MINT_INLINE void GuiContext::NextControlStates::reset() noexcept
        {
            _nextSameLine = false;
            _nextDesiredControlSize.setZero();
            _nextSizingForced = false;
            _nextControlSizeNonContrainedToParent = false;
            _nextNoInterval = false;
            _nextNoAutoPositioned = false;
            _nextControlPosition.setZero();
            _nextTooltipText = nullptr;
        }


        MINT_INLINE void GuiContext::nextSameLine()
        {
            _nextControlStates._nextSameLine = true;
        }

        MINT_INLINE void GuiContext::nextControlSize(const mint::Float2& size, const bool force)
        {
            _nextControlStates._nextDesiredControlSize = size;
            _nextControlStates._nextSizingForced = force;
        }

        MINT_INLINE void GuiContext::nextNoInterval()
        {
            _nextControlStates._nextNoInterval = true;
        }

        MINT_INLINE void GuiContext::nextNoAutoPositioned()
        {
            _nextControlStates._nextNoAutoPositioned = true;
        }

        MINT_INLINE void GuiContext::nextControlSizeNonContrainedToParent()
        {
            _nextControlStates._nextControlSizeNonContrainedToParent = true;
        }

        MINT_INLINE void GuiContext::nextControlPosition(const mint::Float2& position)
        {
            _nextControlStates._nextControlPosition = position;
        }

        MINT_INLINE void GuiContext::nextTooltip(const wchar_t* const tooltipText)
        {
            _nextControlStates._nextTooltipText = tooltipText;
        }

        MINT_INLINE const ControlData& GuiContext::getControlStackTopXXX() const noexcept
        {
            if (_controlStackPerFrame.empty() == false)
            {
                auto found = _controlIdMap.find(_controlStackPerFrame.back()._hashKey);
                if (found.isValid() == true)
                {
                    return *found._value;
                }
            }
            return _rootControlData;
        }

        MINT_INLINE ControlData& GuiContext::getControlStackTopXXX() noexcept
        {
            if (_controlStackPerFrame.empty() == false)
            {
                auto found = _controlIdMap.find(_controlStackPerFrame.back()._hashKey);
                if (found.isValid() == true)
                {
                    return *found._value;
                }
            }
            return _rootControlData;
        }

        MINT_INLINE ControlData& GuiContext::getControlData(const uint64 hashKey) noexcept
        {
            auto found = _controlIdMap.find(hashKey);
            if (found.isValid() == true)
            {
                return *found._value;
            }
            return _rootControlData;
        }

        MINT_INLINE const ControlData& GuiContext::getControlData(const uint64 hashKey) const noexcept
        {
            auto found = _controlIdMap.find(hashKey);
            if (found.isValid() == true)
            {
                return *found._value;
            }

            //MINT_ASSERT("김장원", false, "hashKey 가 존재하지 않는 ControlData 입니다!!!");
            return _rootControlData;
        }
        
        MINT_INLINE mint::Float2 GuiContext::getControlPositionInParentSpace(const ControlData& controlData) const noexcept
        {
            return controlData._position - getControlData(controlData.getParentHashKey())._position;
        }

        MINT_INLINE const mint::RenderingBase::Color& GuiContext::getNamedColor(const NamedColor namedColor) const noexcept
        {
            return _namedColors[static_cast<uint32>(namedColor)];
        }

        MINT_INLINE void GuiContext::setNamedColor(const NamedColor namedColor, const mint::RenderingBase::Color& color) noexcept
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

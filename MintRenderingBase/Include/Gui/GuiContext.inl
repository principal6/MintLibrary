#include "GuiContext.h"
#pragma once


namespace mint
{
    namespace Gui
    {
#pragma region TaskWhenMouseUp
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
#pragma endregion


#pragma region GuiContext - ControlStackData
        inline GuiContext::ControlStackData::ControlStackData(const ControlData& controlData)
            : _controlType{ controlData.getControlType() }
            , _hashKey{ controlData.getHashKey() }
        {
            __noop;
        }
#pragma endregion


#pragma region GuiContext - ControlMetaStateSet
        inline GuiContext::ControlMetaStateSet::ControlMetaStateSet()
        {
            reset();
        }

        inline GuiContext::ControlMetaStateSet::~ControlMetaStateSet()
        {
            __noop;
        }

        MINT_INLINE void GuiContext::ControlMetaStateSet::reset() noexcept
        {
            _sameLine = false;
            _nextDesiredSize.setZero();
            _nextSizeForced = false;
            _nextDesiredPosition.setZero();
            _nextTooltipText = nullptr;

            _nextUseInterval = true;
            _nextUseAutoPosition = true;
            _nextUseSizeContraintToParent = true;
        }

        MINT_INLINE void GuiContext::ControlMetaStateSet::nextSameLine() noexcept
        {
            _sameLine = true;
        }

        MINT_INLINE void GuiContext::ControlMetaStateSet::nextSize(const Float2& size, const bool force) noexcept
        {
            _nextDesiredSize = size;
            _nextSizeForced = force;
        }

        MINT_INLINE void GuiContext::ControlMetaStateSet::nextPosition(const Float2& position) noexcept
        {
            _nextDesiredPosition = position;
        }

        MINT_INLINE void GuiContext::ControlMetaStateSet::nextTooltip(const wchar_t* const tooltipText) noexcept
        {
            _nextTooltipText = tooltipText;
        }

        MINT_INLINE void GuiContext::ControlMetaStateSet::nextOffInterval() noexcept
        {
            _nextUseInterval = false;
        }

        MINT_INLINE void GuiContext::ControlMetaStateSet::nextOffAutoPosition() noexcept
        {
            _nextUseAutoPosition = false;
        }

        MINT_INLINE void GuiContext::ControlMetaStateSet::nextOffSizeContraintToParent() noexcept
        {
            _nextUseSizeContraintToParent = false;
        }

        MINT_INLINE const bool GuiContext::ControlMetaStateSet::getNextSameLine() const noexcept
        {
            return _sameLine;
        }

        MINT_INLINE const Float2& GuiContext::ControlMetaStateSet::getNextDesiredSize() const noexcept
        {
            return _nextDesiredSize;
        }

        MINT_INLINE const bool GuiContext::ControlMetaStateSet::getNextSizeForced() const noexcept
        {
            return _nextSizeForced;
        }

        inline const Float2& GuiContext::ControlMetaStateSet::getNextDesiredPosition() const noexcept
        {
            return _nextDesiredPosition;
        }

        inline const wchar_t* GuiContext::ControlMetaStateSet::getNextTooltipText() const noexcept
        {
            return _nextTooltipText;
        }

        inline const bool GuiContext::ControlMetaStateSet::getNextUseInterval() const noexcept
        {
            return _nextUseInterval;
        }

        inline const bool GuiContext::ControlMetaStateSet::getNextUseAutoPosition() const noexcept
        {
            return _nextUseAutoPosition;
        }

        inline const bool GuiContext::ControlMetaStateSet::getNextUseSizeConstraintToParent() const noexcept
        {
            return _nextUseSizeContraintToParent;
        }
#pragma endregion


        MINT_INLINE const bool GuiContext::isValidControlDataHashKey(const uint64 hashKey) const noexcept
        {
            const auto found = _controlIdMap.find(_controlStackPerFrame.back()._hashKey);
            return found.isValid();
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
        
        MINT_INLINE Float2 GuiContext::getControlPositionInParentSpace(const ControlData& controlData) const noexcept
        {
            return controlData._position - getControlData(controlData.getParentHashKey())._position;
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

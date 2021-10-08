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


#pragma region ControlMetaStateSet
        inline ControlMetaStateSet::ControlMetaStateSet()
        {
            resetPerFrame();
        }

        inline ControlMetaStateSet::~ControlMetaStateSet()
        {
            __noop;
        }

        MINT_INLINE void ControlMetaStateSet::resetPerFrame() noexcept
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

        MINT_INLINE void ControlMetaStateSet::pushSize(const Float2& size, const bool force) noexcept
        {
            _stackDesiredSize.push_back(size);
            _stackSizeForced.push_back(force);
        }

        MINT_INLINE void ControlMetaStateSet::popSize() noexcept
        {
            _stackDesiredSize.pop_back();
            _stackSizeForced.pop_back();
        }

        MINT_INLINE void ControlMetaStateSet::nextSameLine() noexcept
        {
            _sameLine = true;
        }

        MINT_INLINE void ControlMetaStateSet::nextSize(const Float2& size, const bool force) noexcept
        {
            _nextDesiredSize = size;
            _nextSizeForced = force;
        }

        MINT_INLINE void ControlMetaStateSet::nextPosition(const Float2& position) noexcept
        {
            _nextDesiredPosition = position;
        }

        MINT_INLINE void ControlMetaStateSet::nextTooltip(const wchar_t* const tooltipText) noexcept
        {
            _nextTooltipText = tooltipText;
        }

        MINT_INLINE void ControlMetaStateSet::nextOffInterval() noexcept
        {
            _nextUseInterval = false;
        }

        MINT_INLINE void ControlMetaStateSet::nextOffAutoPosition() noexcept
        {
            _nextUseAutoPosition = false;
        }

        MINT_INLINE void ControlMetaStateSet::nextOffSizeContraintToParent() noexcept
        {
            _nextUseSizeContraintToParent = false;
        }

        MINT_INLINE const bool ControlMetaStateSet::getNextSameLine() const noexcept
        {
            return _sameLine;
        }

        MINT_INLINE const Float2 ControlMetaStateSet::getNextDesiredSize() const noexcept
        {
            Float2 result;
            if (_stackDesiredSize.empty() == false)
            {
                result = _stackDesiredSize.back();
            }
            else
            {
                result = _nextDesiredSize;
                _nextDesiredSize.setZero();
            }
            return result;
        }

        MINT_INLINE const bool ControlMetaStateSet::getNextSizeForced() const noexcept
        {
            bool result;
            if (_stackSizeForced.isEmpty() == false)
            {
                result = _stackSizeForced.last();
            }
            else
            {
                result = _nextSizeForced;
                _nextSizeForced = false;
            }
            return result;
        }

        inline const Float2& ControlMetaStateSet::getNextDesiredPosition() const noexcept
        {
            return _nextDesiredPosition;
        }

        inline const wchar_t* ControlMetaStateSet::getNextTooltipText() const noexcept
        {
            return _nextTooltipText;
        }

        inline const bool ControlMetaStateSet::getNextUseInterval() const noexcept
        {
            return _nextUseInterval;
        }

        inline const bool ControlMetaStateSet::getNextUseAutoPosition() const noexcept
        {
            return _nextUseAutoPosition;
        }

        inline const bool ControlMetaStateSet::getNextUseSizeConstraintToParent() const noexcept
        {
            return _nextUseSizeContraintToParent;
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


        MINT_INLINE const bool GuiContext::isValidControlId(const ControlId& id) const noexcept
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

        MINT_INLINE ControlData& GuiContext::getControlStackTopXXX() noexcept
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

        MINT_INLINE ControlData& GuiContext::getControlData(const ControlId& id) noexcept
        {
            auto found = _controlIdMap.find(id);
            if (found.isValid() == true)
            {
                return *found._value;
            }
            return _rootControlData;
        }

        MINT_INLINE const ControlData& GuiContext::getControlData(const ControlId& id) const noexcept
        {
            auto found = _controlIdMap.find(id);
            if (found.isValid() == true)
            {
                return *found._value;
            }

            //MINT_ASSERT("김장원", false, "id 가 존재하지 않는 ControlData 입니다!!!");
            return _rootControlData;
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

#pragma once


namespace mint
{
    namespace Gui
    {
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
    }
}

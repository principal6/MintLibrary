#pragma once


#ifndef MINT_GUI_CONTROL_METASTATE_SET
#define MINT_GUI_CONTROL_METASTATE_SET


#include <MintMath/Include/Float2.h>
#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/BitVector.h>


namespace mint
{
    namespace Gui
    {
        class ControlMetaStateSet
        {
        public:
                                        ControlMetaStateSet();
                                        ~ControlMetaStateSet();

        public:
            void                        resetPerFrame() noexcept;

        public:
            void                        pushSize(const Float2& size, const bool force = false) noexcept;
            void                        popSize() noexcept;

        public:
            void                        nextSameLine() noexcept;
            void                        nextSize(const Float2& size, const bool force = false) noexcept;
            void                        nextPosition(const Float2& position) noexcept;
            void                        nextTooltip(const wchar_t* const tooltipText) noexcept;

        public: // 아래는 기본값이 true 라서 Off 함수만 있음...
            void                        nextOffInterval() noexcept;
            void                        nextOffAutoPosition() noexcept;
            void                        nextOffSizeContraintToParent() noexcept;

        public:
            const bool                  getNextSameLine() const noexcept;
            const Float2                getNextDesiredSize() const noexcept;
            const bool                  getNextSizeForced() const noexcept;
            const Float2&               getNextDesiredPosition() const noexcept;
            const wchar_t*              getNextTooltipText() const noexcept;

        public:
            const bool                  getNextUseInterval() const noexcept;
            const bool                  getNextUseAutoPosition() const noexcept;
            const bool                  getNextUseSizeConstraintToParent() const noexcept;

        private:
            Vector<Float2>              _stackDesiredSize;
            BitVector                   _stackSizeForced;

        private:
            bool                        _sameLine;
            mutable Float2              _nextDesiredSize;
            mutable bool                _nextSizeForced;
            Float2                      _nextDesiredPosition;
            const wchar_t*              _nextTooltipText;

        private:
            bool                        _nextUseInterval;
            bool                        _nextUseAutoPosition;
            bool                        _nextUseSizeContraintToParent;
        };
    }
}


#include <MintRenderingBase/Include/Gui/ControlMetaStateSet.inl>


#endif // !MINT_GUI_CONTROL_METASTATE_SET

#pragma once


namespace mint
{
    namespace GUI
    {
        inline ControlValue::ControlValue()
            : _commonData{}
            , _textBoxData{}
        {
            __noop;
        }

        MINT_INLINE void GUI::ControlValue::CommonData::enableScrollBar(const ScrollBarType scrollBarType) noexcept
        {
            if (_scrollBarType != ScrollBarType::Both && scrollBarType != ScrollBarType::None)
            {
                if (_scrollBarType == ScrollBarType::Horz && scrollBarType != ScrollBarType::Horz)
                {
                    _scrollBarType = ScrollBarType::Both;
                }
                else if (_scrollBarType == ScrollBarType::Vert && scrollBarType != ScrollBarType::Vert)
                {
                    _scrollBarType = ScrollBarType::Both;
                }
                else
                {
                    _scrollBarType = scrollBarType;
                }
            }
        }

        MINT_INLINE void ControlValue::CommonData::disableScrollBar(const ScrollBarType scrollBarType) noexcept
        {
            if (_scrollBarType != ScrollBarType::None && scrollBarType != ScrollBarType::None)
            {
                if (scrollBarType == ScrollBarType::Both)
                {
                    _scrollBarType = ScrollBarType::None;
                }
                else if (scrollBarType == ScrollBarType::Vert)
                {
                    if (_scrollBarType == ScrollBarType::Vert)
                    {
                        _scrollBarType = ScrollBarType::None;
                    }
                    else
                    {
                        _scrollBarType = ScrollBarType::Horz;
                    }
                }
                else if (scrollBarType == ScrollBarType::Horz)
                {
                    if (_scrollBarType == ScrollBarType::Horz)
                    {
                        _scrollBarType = ScrollBarType::None;
                    }
                    else
                    {
                        _scrollBarType = ScrollBarType::Vert;
                    }
                }
            }
        }

        MINT_INLINE const bool ControlValue::CommonData::isScrollBarEnabled(const ScrollBarType scrollBarType) const noexcept
        {
            MINT_ASSERT(scrollBarType != ScrollBarType::None, "잘못된 질문입니다.");

            if (_scrollBarType == ScrollBarType::Both)
            {
                return true;
            }
            return (_scrollBarType == scrollBarType);
        }
    }
}

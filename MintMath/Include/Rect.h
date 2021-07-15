#pragma once


#ifndef MINT_RECT_H
#define MINT_RECT_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintMath/Include/Float4.h>


namespace mint
{
    class Rect
    {
    public:
        static constexpr Rect           fromPositionSize(const mint::Float2& positionFromLeftTop, const mint::Float2& size);
        static constexpr Rect           fromLongs(const long left, const long right, const long top, const long bottom);

    public:
        constexpr                       Rect();
        constexpr                       Rect(const float uniformValue);
        constexpr                       Rect(const mint::Float2& positionFromLeftTop, const mint::Float2& size);
        constexpr                       Rect(const float left, const float right, const float top, const float bottom);
        constexpr                       Rect(const Rect& rhs)           = default;
        constexpr                       Rect(Rect&& rhs) noexcept       = default;

    public:
        Rect&                           operator=(const Rect& rhs)      = default;
        Rect&                           operator=(Rect&& rhs) noexcept  = default;
        
    public:
        constexpr const bool            operator==(const Rect& rhs) const noexcept;
        constexpr const bool            operator!=(const Rect& rhs) const noexcept;

    public:
        constexpr const float           left() const noexcept;
        constexpr const float           right() const noexcept;
        constexpr const float           top() const noexcept;
        constexpr const float           bottom() const noexcept;
        constexpr const float           horz() const noexcept;
        constexpr const float           vert() const noexcept;

        float&                          left() noexcept;
        float&                          right() noexcept;
        float&                          top() noexcept;
        float&                          bottom() noexcept;

        constexpr void                  left(const float s) noexcept;
        constexpr void                  right(const float s) noexcept;
        constexpr void                  top(const float s) noexcept;
        constexpr void                  bottom(const float s) noexcept;

        constexpr const mint::Float2    center() const noexcept;
        constexpr const mint::Float2    size() const noexcept;
        constexpr const mint::Float2    position() const noexcept;
        constexpr void                  position(const mint::Float2& position) noexcept;

    public:
        void                            clipBy(const Rect& outerRect) noexcept;
        constexpr const bool            contains(const mint::Float2& position) const noexcept;
        constexpr const bool            contains(const Rect& rhs) const noexcept;
        const bool                      isNan() const noexcept;
        void                            setNan() noexcept;

    private:
        mint::Float4                    _raw;
    };
}


#include <MintMath/Include/Rect.inl>


#endif // !MINT_RECT_H

#pragma once


#ifndef FS_RECT_H
#define FS_RECT_H


#include <FsMath/Include/Float4.h>


namespace fs
{
    class Rect
    {
    public:
        static constexpr Rect           fromPositionSize(const fs::Float2& positionFromLeftTop, const fs::Float2& size);
        static constexpr Rect           fromLongs(const long left, const long right, const long top, const long bottom);

    public:
        constexpr                       Rect();
        constexpr                       Rect(const float uniformValue);
        constexpr                       Rect(const fs::Float2& positionFromLeftTop, const fs::Float2& size);
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

        constexpr void                  left(const float s) noexcept;
        constexpr void                  right(const float s) noexcept;
        constexpr void                  top(const float s) noexcept;
        constexpr void                  bottom(const float s) noexcept;

        constexpr const fs::Float2      center() const noexcept;
        constexpr const fs::Float2      size() const noexcept;
        constexpr const fs::Float2      position() const noexcept;
        constexpr void                  position(const fs::Float2& position) noexcept;

    public:
        constexpr const bool            contains(const fs::Float2& position) const noexcept;
        constexpr const bool            contains(const Rect& rhs) const noexcept;
        const bool                      isNan() const noexcept;
        void                            setNan() noexcept;

    private:
        fs::Float4                      _raw;
    };
}


#include <FsMath/Include/Rect.inl>


#endif // !FS_RECT_H

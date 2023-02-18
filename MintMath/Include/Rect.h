#pragma once


#ifndef _MINT_MATH_RECT_H_
#define _MINT_MATH_RECT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintMath/Include/Float4.h>


namespace mint
{
	class Rect
	{
	public:
		static constexpr Rect fromPositionSize(const Float2& positionFromLeftTop, const Float2& size);
		static constexpr Rect fromLongs(const long left, const long right, const long top, const long bottom);

	public:
		constexpr Rect();
		constexpr Rect(const float uniformValue);
		constexpr Rect(const Float2& positionFromLeftTop, const Float2& size);
		constexpr Rect(const float left, const float right, const float top, const float bottom);
		constexpr Rect(const Rect& rhs) = default;
		constexpr Rect(Rect&& rhs) noexcept = default;

	public:
		Rect& operator=(const Rect& rhs) = default;
		Rect& operator=(Rect&& rhs) noexcept = default;

	public:
		constexpr bool operator==(const Rect& rhs) const noexcept;
		constexpr bool operator!=(const Rect& rhs) const noexcept;

	public:
		constexpr float left() const noexcept;
		constexpr float right() const noexcept;
		constexpr float top() const noexcept;
		constexpr float bottom() const noexcept;
		constexpr float horz() const noexcept;
		constexpr float vert() const noexcept;
		constexpr float width() const noexcept;
		constexpr float height() const noexcept;

		float& left() noexcept;
		float& right() noexcept;
		float& top() noexcept;
		float& bottom() noexcept;

		constexpr void left(const float s) noexcept;
		constexpr void right(const float s) noexcept;
		constexpr void top(const float s) noexcept;
		constexpr void bottom(const float s) noexcept;

		constexpr Float2 center() const noexcept;
		constexpr Float2 size() const noexcept;
		constexpr Float2 position() const noexcept;
		constexpr void position(const Float2& position) noexcept;

	public:
		void clipBy(const Rect& outerRect) noexcept;
		void moveBy(const Float2& offset) noexcept;
		void expandByQuantity(const Rect& quantity) noexcept;
		void expand(const Rect& rhs) noexcept;
		void expandRightBottom(const Rect& rhs) noexcept;
		void shrinkByQuantity(const Rect& quantity) noexcept;
		constexpr Float2 bound(const Float2& position) const noexcept;
		constexpr bool Contains(const Float2& position) const noexcept;
		constexpr bool Contains(const Rect& rhs) const noexcept;

	public:
		bool isNan() const noexcept;
		void setNan() noexcept;

	private:
		constexpr float boundHorz(const float x) const noexcept;
		constexpr float boundVert(const float y) const noexcept;
		void validate() noexcept;

	private:
		Float4 _raw;
	};
}


#include <MintMath/Include/Rect.inl>


#endif // !_MINT_MATH_RECT_H_

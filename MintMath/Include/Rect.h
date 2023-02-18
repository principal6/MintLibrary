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
		static constexpr Rect FromPositionSize(const Float2& positionFromLeftTop, const Float2& size);
		static constexpr Rect FromLongs(const long left, const long right, const long top, const long bottom);

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
		constexpr float Left() const noexcept;
		constexpr float Right() const noexcept;
		constexpr float Top() const noexcept;
		constexpr float Bottom() const noexcept;
		constexpr float Horz() const noexcept;
		constexpr float Vert() const noexcept;
		constexpr float Width() const noexcept;
		constexpr float Height() const noexcept;

		float& Left() noexcept;
		float& Right() noexcept;
		float& Top() noexcept;
		float& Bottom() noexcept;

		constexpr void Left(const float s) noexcept;
		constexpr void Right(const float s) noexcept;
		constexpr void Top(const float s) noexcept;
		constexpr void Bottom(const float s) noexcept;

		constexpr Float2 Center() const noexcept;
		constexpr Float2 Size() const noexcept;
		constexpr Float2 Position() const noexcept;
		constexpr void Position(const Float2& position) noexcept;

	public:
		void ClipBy(const Rect& outerRect) noexcept;
		void MoveBy(const Float2& offset) noexcept;
		void ExpandByQuantity(const Rect& quantity) noexcept;
		void Expand(const Rect& rhs) noexcept;
		void ExpandRightBottom(const Rect& rhs) noexcept;
		void ShrinkByQuantity(const Rect& quantity) noexcept;
		constexpr Float2 Bound(const Float2& position) const noexcept;
		constexpr bool Contains(const Float2& position) const noexcept;
		constexpr bool Contains(const Rect& rhs) const noexcept;

	public:
		bool IsNAN() const noexcept;
		void SetNAN() noexcept;

	private:
		constexpr float BoundHorz(const float x) const noexcept;
		constexpr float BoundVert(const float y) const noexcept;
		void Validate() noexcept;

	private:
		Float4 _raw;
	};
}


#include <MintMath/Include/Rect.inl>


#endif // !_MINT_MATH_RECT_H_

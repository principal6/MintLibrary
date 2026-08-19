#pragma once


#ifndef _MINT_MATH_SCREEN_SPACE_COORDINATES_H_
#define _MINT_MATH_SCREEN_SPACE_COORDINATES_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintMath/Include/Float2.h>


namespace mint
{
	// 화면 좌상단이 원점이고 +X 는 오른쪽, +Y 는 아래인 픽셀 좌표.
	//
	// 엔진 내부의 2D 공간은 화면 중앙이 원점이고 +X 는 오른쪽, +Y 는 위다.
	// (WorldSpace 2D 카메라와 ScreenSpace 투영 행렬이 모두 이 규칙을 쓴다.)
	// 이 클래스는 그 두 공간의 경계에서만 존재하며, 변환은 오직 여기서 한 번만 일어난다.
	// 지오메트리, UV, Vertex winding 은 어떤 경우에도 내부 공간 기준으로 고정이다.
	class ScreenSpaceCoordinates
	{
	public:
		static ScreenSpaceCoordinates FromInternal(const Float2& internalPosition, const Float2& screenSize) noexcept;

	public:
		constexpr explicit ScreenSpaceCoordinates(const float x, const float y);
		constexpr explicit ScreenSpaceCoordinates(const Float2& pixels);
		constexpr ScreenSpaceCoordinates(const ScreenSpaceCoordinates& rhs) = default;
		constexpr ScreenSpaceCoordinates(ScreenSpaceCoordinates&& rhs) noexcept = default;

	public:
		ScreenSpaceCoordinates& operator=(const ScreenSpaceCoordinates& rhs) = default;
		ScreenSpaceCoordinates& operator=(ScreenSpaceCoordinates&& rhs) noexcept = default;

	public:
		// 화면 중앙이 원점이고 +Y 가 위인 내부 2D 좌표로 변환한다.
		Float2 ToInternal(const Float2& screenSize) const noexcept;
		constexpr const Float2& GetPixels() const noexcept { return _pixels; }

	private:
		Float2 _pixels;
	};
}


#include <MintMath/Include/ScreenSpaceCoordinates.inl>


#endif // !_MINT_MATH_SCREEN_SPACE_COORDINATES_H_

#pragma once


#ifndef _MINT_PLATFORM_COMMON_H_
#define _MINT_PLATFORM_COMMON_H_


#if defined(_WIN64)
#define MINT_PLATFORM_WINDOWS
#endif // defined(_WIN64)


namespace mint
{
	enum class MouseButton : int32
	{
		Left,
		Middle,
		Right,

		COUNT,
	};
	MINT_INLINE constexpr uint32 GetMouseButtonCount() noexcept
	{
		return static_cast<uint32>(MouseButton::COUNT);
	}

	enum class KeyCode : uint64
	{
		NONE,
		Escape,
		Enter,
		Up,
		Down,
		Left,
		Right,
		Home,
		End,
		Shift,
		Control,
		Alt,
		SpaceBar,
		Insert,
		Delete,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		Num0,
		Num1,
		Num2,
		Num3,
		Num4,
		Num5,
		Num6,
		Num7,
		Num8,
		Num9,

		COUNT
	};
	MINT_INLINE constexpr uint32 GetKeyCodeCount() noexcept
	{
		return static_cast<uint32>(KeyCode::COUNT);
	}
	MINT_INLINE constexpr bool IsKeyCodeAlnum(const KeyCode keyCode) noexcept
	{
		return (KeyCode::A <= keyCode && keyCode <= KeyCode::Num9);
	}
}


#endif // !_MINT_PLATFORM_COMMON_H_

#pragma once


#ifndef _MINT_PLATFORM_COMMON_H_
#define _MINT_PLATFORM_COMMON_H_


namespace mint
{
	namespace Platform
	{
		enum class PlatformType
		{
			INVALID,

			Windows,
			// MacOs
			// Linux
			// ...
		};

		enum class MouseButton : int32
		{
			Left,
			Middle,
			Right,

			COUNT,
		};
		MINT_INLINE constexpr uint32 getMouseButtonCount() noexcept
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
			Delete,
			Home,
			End,
			Shift,
			Control,
			Alt,
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
		MINT_INLINE constexpr uint32 getKeyCodeCount() noexcept
		{
			return static_cast<uint32>(KeyCode::COUNT);
		}
		MINT_INLINE constexpr bool isKeyCodeAlnum(const KeyCode keyCode) noexcept
		{
			return (KeyCode::A <= keyCode && keyCode <= KeyCode::Num9);
		}
	}
}


#endif // !_MINT_PLATFORM_COMMON_H_

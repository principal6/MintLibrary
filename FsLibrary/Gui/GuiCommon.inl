#include "GuiCommon.h"
#pragma once


namespace fs
{
	namespace Gui
	{
		inline constexpr Rect::Rect()
			: Rect(0.0f, 0.0f, 0.0f, 0.0f)
		{
			__noop;
		}

		inline constexpr Rect::Rect(const float uniformPadding)
			: Rect(uniformPadding, uniformPadding, uniformPadding, uniformPadding)
		{
			__noop;
		}

		inline constexpr Rect::Rect(const float left, const float right, const float top, const float bottom)
			: _raw{ left, right, top, bottom }
		{
			__noop;
		}

		FS_INLINE constexpr const float Rect::left() const noexcept
		{
			return _raw._x;
		}

		FS_INLINE constexpr const float Rect::right() const noexcept
		{
			return _raw._y;
		}

		FS_INLINE constexpr const float Rect::top() const noexcept
		{
			return _raw._z;
		}

		FS_INLINE constexpr const float Rect::bottom() const noexcept
		{
			return _raw._w;
		}

		FS_INLINE constexpr void Rect::left(const float s) noexcept
		{
			_raw._x = s;
		}

		FS_INLINE constexpr void Rect::right(const float s) noexcept
		{
			_raw._y = s;
		}

		FS_INLINE constexpr void Rect::top(const float s) noexcept
		{
			_raw._z = s;
		}

		FS_INLINE constexpr void Rect::bottom(const float s) noexcept
		{
			_raw._w = s;
		}

		FS_INLINE const bool Rect::isNan() const noexcept
		{
			return _raw.isNan();
		}
		
		FS_INLINE void Rect::setNan() noexcept
		{
			_raw.setNan();
		}
	}
}

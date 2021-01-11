#pragma once


#ifndef FS_INT2_H
#define FS_INT2_H


#include <FsMath/Shared/CommonDefinitions.h>


namespace fs
{
	// pair of int32
	class Int2 final
	{
	public:
						Int2();
		explicit		Int2(const int32 s);
		explicit		Int2(const int32 x, const int32 y);
						Int2(const Int2& rhs)				= default;
						Int2(Int2&& rhs) noexcept			= default;
						~Int2()								= default;

	public:
		Int2&			operator=(const Int2& rhs)			= default;
		Int2&			operator=(Int2&& rhs) noexcept		= default;
		
	public:
		Int2&			operator+=(const Int2& rhs) noexcept;
		Int2&			operator-=(const Int2& rhs) noexcept;
		Int2&			operator*=(const int32 s) noexcept;
		Int2&			operator/=(const int32 s) noexcept;

	public:
		Int2			operator+(const Int2& rhs) const noexcept;
		Int2			operator-(const Int2& rhs) const noexcept;
		Int2			operator*(const int32 s) const noexcept;
		Int2			operator/(const int32 s) const noexcept;

	public:
		int32&			operator[](const uint32 index) noexcept;
		const int32&	operator[](const uint32 index) const noexcept;

	public:
		const bool		operator==(const Int2& rhs) const noexcept;
		const bool		operator!=(const Int2& rhs) const noexcept;

	public:
		void			set(const int32 x, const int32 y) noexcept;

	public:
		int32			_x;
		int32			_y;
	};
}


#endif // !FS_INT2_H

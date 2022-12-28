#include <MintMath/Include/Int2.h>


namespace mint
{
	const Int2 Int2::kZero = Int2(0);

	Int2::Int2()
		: Int2(0)
	{
		__noop;
	}

	Int2::Int2(const int32 s)
		: Int2(s, s)
	{
		__noop;
	}

	Int2::Int2(const int32 x, const int32 y)
		: _x{ x }
		, _y{ y }
	{
		__noop;
	}

	Int2& Int2::operator+=(const Int2& rhs) noexcept
	{
		_x += rhs._x;
		_y += rhs._y;
		return *this;
	}

	Int2& Int2::operator-=(const Int2& rhs) noexcept
	{
		_x -= rhs._x;
		_y -= rhs._y;
		return *this;
	}

	Int2& Int2::operator*=(const int32 s) noexcept
	{
		_x *= s;
		_y *= s;
		return *this;
	}

	Int2& Int2::operator/=(const int32 s) noexcept
	{
		_x /= s;
		_y /= s;
		return *this;
	}

	Int2 Int2::operator+(const Int2& rhs) const noexcept
	{
		return Int2(_x + rhs._x, _y + rhs._y);
	}

	Int2 Int2::operator-(const Int2& rhs) const noexcept
	{
		return Int2(_x - rhs._x, _y - rhs._y);
	}

	Int2 Int2::operator*(const int32 s) const noexcept
	{
		return Int2(_x * s, _y * s);
	}

	Int2 Int2::operator/(const int32 s) const noexcept
	{
		return Int2(_x / s, _y / s);
	}

	int32& Int2::operator[](const uint32 index) noexcept
	{
		MINT_ASSERT(index < 2, "범위를 벗어난 접근입니다.");
		return (&_x)[index];
	}

	const int32& Int2::operator[](const uint32 index) const noexcept
	{
		MINT_ASSERT(index < 2, "범위를 벗어난 접근입니다.");
		return (&_x)[index];
	}

	bool Int2::operator==(const Int2& rhs) const noexcept
	{
		return (_x == rhs._x && _y == rhs._y);
	}

	bool Int2::operator!=(const Int2& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	void Int2::set(const int32 x, const int32 y) noexcept
	{
		_x = x;
		_y = y;
	}
}

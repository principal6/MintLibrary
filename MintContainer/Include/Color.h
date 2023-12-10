#pragma once


#ifndef _MINT_CONTAINER_COLOR_H_
#define _MINT_CONTAINER_COLOR_H_


#include <MintMath/Include/Float3.h>
#include <MintMath/Include/Float4.h>


namespace mint
{
	MINT_INLINE constexpr float ConvertByteToNormalizedFloat(const byte v) { return static_cast<float>(v) / 255.0f; }
	MINT_INLINE constexpr byte ConvertNormalizedFloatToByte(const float v) { return static_cast<byte>(v * 255.99f); }

	class ByteColor
	{
	public:
		static float ComputeAlphaFromColorKey(const ByteColor& color, const ByteColor& colorKey)
		{
			const Float3 a = Float3(color.RAsFloat(), color.GAsFloat(), color.BAsFloat());
			const Float3 b = Float3(colorKey.RAsFloat(), colorKey.GAsFloat(), colorKey.BAsFloat());
			const float distance = (b - a).Length();
			return Min(distance, 1.0f);
		}

	public:
		constexpr ByteColor() : ByteColor(255) { __noop; }
		constexpr ByteColor(byte c) : ByteColor(c, c, c, c) { __noop; }
		constexpr ByteColor(byte r, byte g, byte b) : ByteColor(r, g, b, 255) { __noop; }
		constexpr ByteColor(byte r, byte g, byte b, byte a) : _c{ r, g, b, a } { __noop; }

		MINT_INLINE byte& R() noexcept { return _c[0]; }
		MINT_INLINE byte& G() noexcept { return _c[1]; }
		MINT_INLINE byte& B() noexcept { return _c[2]; }
		MINT_INLINE byte& A() noexcept { return _c[3]; }

		MINT_INLINE constexpr const byte& R() const noexcept { return _c[0]; }
		MINT_INLINE constexpr const byte& G() const noexcept { return _c[1]; }
		MINT_INLINE constexpr const byte& B() const noexcept { return _c[2]; }
		MINT_INLINE constexpr const byte& A() const noexcept { return _c[3]; }

		MINT_INLINE constexpr float RAsFloat() const noexcept { return ConvertByteToNormalizedFloat(_c[0]); }
		MINT_INLINE constexpr float GAsFloat() const noexcept { return ConvertByteToNormalizedFloat(_c[1]); }
		MINT_INLINE constexpr float BAsFloat() const noexcept { return ConvertByteToNormalizedFloat(_c[2]); }
		MINT_INLINE constexpr float AAsFloat() const noexcept { return ConvertByteToNormalizedFloat(_c[3]); }

		MINT_INLINE constexpr void R(byte value) noexcept { _c[0] = value; }
		MINT_INLINE constexpr void G(byte value) noexcept { _c[1] = value; }
		MINT_INLINE constexpr void A(byte value) noexcept { _c[2] = value; }
		MINT_INLINE constexpr void B(byte value) noexcept { _c[3] = value; }

	private:
		byte _c[4];
	};

	class Color
	{
	public:
		static const Color kTransparent;
		static const Color kWhite;
		static const Color kBlack;
		static const Color kRed;
		static const Color kGreen;
		static const Color kBlue;
		static const Color kCyan;
		static const Color kMagenta;
		static const Color kYellow;

	public:
		constexpr Color() : Color(255, 255, 255) { __noop; }
		constexpr Color(const ByteColor& byteColor) : _raw{ byteColor.RAsFloat(), byteColor.GAsFloat(), byteColor.BAsFloat(), byteColor.AAsFloat() } { __noop; }
		constexpr Color(const float r, const float g, const float b, const float a) : _raw{ Math::Saturate(r), Math::Saturate(g), Math::Saturate(b), Math::Saturate(a) } { __noop; }
		constexpr Color(const int32 r, const int32 g, const int32 b, const int32 a) : Color(ConvertByteToNormalizedFloat(r), ConvertByteToNormalizedFloat(g), ConvertByteToNormalizedFloat(b), ConvertByteToNormalizedFloat(a)) { __noop; }
		constexpr Color(const float r, const float g, const float b) : Color(r, g, b, 1.0f) { __noop; }
		constexpr Color(const int32 r, const int32 g, const int32 b) : Color(r, g, b, 255) { __noop; }
		constexpr Color(const int32 c) : Color(c, c, c, 255) { __noop; }
		constexpr explicit Color(const Float3& rgb) : Color(rgb._x, rgb._y, rgb._z, 1.0f) { __noop; }
		constexpr explicit Color(const Float4& rgba) : Color(rgba._x, rgba._y, rgba._z, rgba._w) { __noop; }

	public:
		operator Float4& () noexcept { return _raw; }
		operator const Float4& () const noexcept { return _raw; }
		operator const float* () const noexcept { return _raw._c; }
		Color operator*(const Color& rhs) const noexcept { return Color(_raw._x * rhs._raw._x, _raw._y * rhs._raw._y, _raw._z * rhs._raw._z, _raw._w * rhs._raw._w); }
		Color operator*(const float s) const noexcept { return Color(_raw * s); }
		Color operator/(const float s) const { return Color(_raw / s); }
		Color operator+(const Color& rhs) const { return Color(_raw + rhs._raw); }
		Color operator-(const Color& rhs) const { return Color(_raw - rhs._raw); }
		bool operator==(const Color& rhs) const noexcept { return _raw == rhs._raw; }

	public:
		MINT_INLINE constexpr float R() const noexcept { return _raw._x; }
		MINT_INLINE constexpr float G() const noexcept { return _raw._y; }
		MINT_INLINE constexpr float B() const noexcept { return _raw._z; }
		MINT_INLINE constexpr float A() const noexcept { return _raw._w; }

		MINT_INLINE constexpr byte RAsByte() const noexcept { return ConvertNormalizedFloatToByte(_raw._x); }
		MINT_INLINE constexpr byte GAsByte() const noexcept { return ConvertNormalizedFloatToByte(_raw._y); }
		MINT_INLINE constexpr byte BAsByte() const noexcept { return ConvertNormalizedFloatToByte(_raw._z); }
		MINT_INLINE constexpr byte AAsByte() const noexcept { return ConvertNormalizedFloatToByte(_raw._w); }

		MINT_INLINE constexpr void R(const int32 value) noexcept { _raw._x = ConvertByteToNormalizedFloat(value); }
		MINT_INLINE constexpr void G(const int32 value) noexcept { _raw._y = ConvertByteToNormalizedFloat(value); }
		MINT_INLINE constexpr void A(const int32 value) noexcept { _raw._w = ConvertByteToNormalizedFloat(value); }
		MINT_INLINE constexpr void B(const int32 value) noexcept { _raw._z = ConvertByteToNormalizedFloat(value); }

		MINT_INLINE constexpr void R(const float value) noexcept { _raw._x = value; }
		MINT_INLINE constexpr void G(const float value) noexcept { _raw._y = value; }
		MINT_INLINE constexpr void B(const float value) noexcept { _raw._z = value; }
		MINT_INLINE constexpr void A(const float value) noexcept { _raw._w = value; }

	public:
		MINT_INLINE void Set(const Color& rhs) noexcept { _raw._x = rhs._raw._x; _raw._y = rhs._raw._y; _raw._z = rhs._raw._z; }
		MINT_INLINE void ScaleR(const float s) noexcept { _raw._x *= s; }
		MINT_INLINE void ScaleG(const float s) noexcept { _raw._y *= s; }
		MINT_INLINE void ScaleB(const float s) noexcept { _raw._z *= s; }
		MINT_INLINE void ScaleA(const float s) noexcept { _raw._w *= s; }
		MINT_INLINE Color CloneAddRGB(const float s) const noexcept { return Color(Math::Saturate(_raw._x + s), Math::Saturate(_raw._y + s), Math::Saturate(_raw._z + s), _raw._w); }
		MINT_INLINE Color CloneAddRGB(const int32 s) const noexcept { return CloneAddRGB(ConvertByteToNormalizedFloat(s)); }
		MINT_INLINE Color CloneScaleRGB(const float s) const noexcept { return Color(_raw._x * s, _raw._y * s, _raw._z * s, _raw._w); }
		MINT_INLINE Color CloneScaleA(const float s) const noexcept { return Color(_raw._x, _raw._y, _raw._z, _raw._w * s); }
		MINT_INLINE bool IsTransparent() const noexcept { return (_raw._w <= 0.0f); }

	public:
		// Rec. 601
		MINT_INLINE constexpr float ToLuma() const noexcept { return _raw._x * 0.299f + _raw._y * 0.587f + _raw._z * 0.114f; }

	private:
		Float4 _raw;
	};
}


#endif // !_MINT_CONTAINER_COLOR_H_

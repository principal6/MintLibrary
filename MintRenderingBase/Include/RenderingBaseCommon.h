#pragma once


#ifndef _MINT_RENDERING_BASE_RENDERING_BASE_COMMON_H_
#define _MINT_RENDERING_BASE_RENDERING_BASE_COMMON_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <d3d11.h>
#include <wrl.h>

#include <MintContainer/Include/Vector.h>

#include <MintMath/Include/Float3.h>
#include <MintMath/Include/Float4.h>
#include <MintMath/Include/Int2.h>


namespace mint
{
	namespace Rendering
	{
		enum class TextRenderDirectionHorz
		{
			Leftward,
			Centered,
			Rightward
		};

		enum class TextRenderDirectionVert
		{
			Upward,
			Centered,
			Downward
		};

		static constexpr int32 kDefaultFontSize = 16;

		using IndexElementType = uint16;

		// Triangle face
		struct Face
		{
			static constexpr uint8 kVertexCountPerFace = 3;

			IndexElementType _vertexIndexArray[kVertexCountPerFace];
		};

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

		class ByteColorImage
		{
		public:
			void SetSize(const Int2& size);
			const Int2& GetSize() const { return _size; }
			int32 GetWidth() const { return _size._x; }
			int32 GetHeight() const { return _size._y; }

			void Fill(const ByteColor& byteColor);
			void SetPixel(const Int2& at, const ByteColor& pixel);
			void SetPixel(const int32 x, const int32 y, const ByteColor& pixel);
			void SetPixel(const int32 index, const ByteColor& pixel);

			void SetPixelWithAlphaBlending(const Int2& at, const ByteColor& pixel, const float alpha);

			uint32 GetPixelCount() const;
			const ByteColor& GetPixel(const Int2& at) const;
			const ByteColor& GetPixel(const int32 x, const int32 y) const;
			const ByteColor& GetPixel(const int32 index) const;

			uint32 GetByteCount() const;
			const byte* GetBytes() const;

		private:
			Int2 _size;
			Vector<ByteColor> _pixels;
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
			constexpr Color(const float r, const float g, const float b, const float a) : _raw{ Math::Saturate(r), Math::Saturate(g), Math::Saturate(b), Math::Saturate(a) } { __noop; }
			constexpr Color(const int32 r, const int32 g, const int32 b, const int32 a) : Color(ConvertByteToNormalizedFloat(r), ConvertByteToNormalizedFloat(g), ConvertByteToNormalizedFloat(b), ConvertByteToNormalizedFloat(a)) { __noop; }
			constexpr Color(const float r, const float g, const float b) : Color(r, g, b, 1.0f) { __noop; }
			constexpr Color(const int32 r, const int32 g, const int32 b) : Color(r, g, b, 255) { __noop; }
			constexpr Color(const int32 c) : Color(c, c, c, 255) { __noop; }
			constexpr Color(const Float3& rgb) : Color(rgb._x, rgb._y, rgb._z, 1.0f) { __noop; }
			constexpr explicit Color(const Float4& float4) : Color(float4._x, float4._y, float4._z, float4._w) { __noop; }

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

		class ColorImage
		{
		public:
			struct AdjacentPixels
			{
				Color _left;
				Color _top;
				Color _right;
				Color _bottom;
			};

			struct CoAdjacentPixels
			{
				Color _topLeft;
				Color _topRight;
				Color _bottomRight;
				Color _bottomLeft;
			};

			template <typename T>
			struct Sample3x3
			{
				MINT_INLINE T NW() const noexcept { return _m[0][0]; }
				MINT_INLINE T N() const noexcept { return _m[0][1]; }
				MINT_INLINE T NE() const noexcept { return _m[0][2]; }

				MINT_INLINE T W() const noexcept { return _m[1][0]; }
				MINT_INLINE T M() const noexcept { return _m[1][1]; }
				MINT_INLINE T E() const noexcept { return _m[1][2]; }

				MINT_INLINE T SW() const noexcept { return _m[2][0]; }
				MINT_INLINE T S() const noexcept { return _m[2][1]; }
				MINT_INLINE T SE() const noexcept { return _m[2][2]; }

				MINT_INLINE T MaxAdjacent() const noexcept { return Max(Max(Max(N(), S()), E()), W()); }
				MINT_INLINE T MinAdjacent() const noexcept { return Min(Min(Min(N(), S()), E()), W()); }

				MINT_INLINE T SumAdjacent() const noexcept { return N() + S() + E() + W(); }
				MINT_INLINE T SumCoAdjacent() const noexcept { return NW() + NE() + SW() + SE(); }

				T _m[3][3];
			};

			struct Sample3x3Luma : public Sample3x3<float>
			{
				Sample3x3Luma(const Sample3x3<Color>& colorSample3x3)
				{
					for (uint32 y = 0; y < 3; ++y)
					{
						for (uint32 x = 0; x < 3; ++x)
						{
							_m[y][x] = colorSample3x3._m[y][x].ToLuma();
						}
					}
				}
			};

		public:
			ColorImage() = default;
			ColorImage(const ColorImage& rhs);
			ColorImage(ColorImage&& rhs) noexcept;
			~ColorImage() = default;

		public:
			ColorImage& operator=(const ColorImage& rhs);
			ColorImage& operator=(ColorImage&& rhs) noexcept;

		public:
			void SetSize(const Int2& size) noexcept;
			const Int2& GetSize() const noexcept;
			int32 GetWidth() const noexcept { return _size._x; }
			int32 GetHeight() const noexcept { return _size._y; }

		public:
			void Fill(const Color& color) noexcept;
			void FillRect(const Int2& position, const Int2& size, const Color& color) noexcept;
			void FillCircle(const Int2& center, const int32 radius, const Color& color) noexcept;
			void SetPixel(const int32 index, const Color& color) noexcept;
			void SetPixel(const Int2& at, const Color& color) noexcept;
			uint32 GetPixelCount() const noexcept;
			const Color& GetPixel(const Int2& at) const noexcept;
			const Color& GetPixel(const int32 index) const noexcept;
			Color GetSubPixel(const Float2& at) const noexcept;
			void GetAdjacentPixels(const Int2& at, ColorImage::AdjacentPixels& outAdjacentPixels) const noexcept;
			void GetCoAdjacentPixels(const Int2& at, ColorImage::CoAdjacentPixels& outCoAdjacentPixels) const noexcept;
			void Sample3x3_(const Int2& at, ColorImage::Sample3x3<Color>& outSample3x3) const noexcept;

		public:
			void BuildPixelRGBAArray(Vector<byte>& outBytes) const noexcept;

		private:
			int32 ConvertXYToIndex(const uint32 x, const uint32 y) const noexcept;
			const Color& GetColorFromXy(const uint32 x, const uint32 y) const noexcept;

		private:
			Int2 _size;
			Vector<Color> _colors;
		};


		class ByteColorImageAtlas
		{
		public:
			ByteColorImageAtlas();
			~ByteColorImageAtlas() = default;

		public:
			void SetInterPadding(const Int2& interPadding) { _interPadding = interPadding; }
			const Int2& GetInterPadding() const { return _interPadding; }

			void SetWidth(const uint32 width) { _width = width; }
			int32 GetWidth() const { return _width; }
			int32 GetHeight() const { return _height; }

			void ClearByteColorImages();
			int32 PushByteColorImage(ByteColorImage&& byteColorImage);
			int32 PushByteColorImage(const ByteColorImage& byteColorImage);

		public:
			bool BakeRGBABytes();
			const Vector<byte>& GetRGBABytes() const { return _rgbaBytes; }
			ByteColor GetPixel(const Int2& positionInAtlas) const;

		public:
			Int2 ComputePositionInAtlas(const int32 byteColorImageIndex, const Int2& positionInByteColorImage) const;

		private:
			Int2 PushColorImage_ComputeByteColorImagePosition(const ByteColorImage& byteColorImage) const;

		private:
			Int2 _interPadding;
			int32 _width;
			int32 _height;
			Vector<Int2> _byteColorImagePositions;
			Vector<Int2> _byteColorImageSizes;

		private:
			Vector<ByteColorImage> _byteColorImages;
			Vector<byte> _rgbaBytes;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_RENDERING_BASE_COMMON_H_

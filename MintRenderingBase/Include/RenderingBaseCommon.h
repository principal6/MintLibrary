#pragma once


#ifndef _MINT_RENDERING_BASE_RENDERING_BASE_COMMON_H_
#define _MINT_RENDERING_BASE_RENDERING_BASE_COMMON_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <d3d11.h>
#include <wrl.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/Color.h>

#include <MintMath/Include/Float3.h>
#include <MintMath/Include/Float4.h>
#include <MintMath/Include/Int2.h>


namespace mint
{
	namespace Rendering
	{
		struct VS_INPUT_SHAPE;

		using IndexElementType = uint16;
	}
}

namespace mint
{
	namespace Rendering
	{
		enum class CoordinateSpace : uint8
		{
			World,
			Screen,
		};

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

		struct Shape
		{
			Vector<VS_INPUT_SHAPE> _vertices;
			Vector<IndexElementType> _indices;
		};

		// Triangle face
		struct Face
		{
			static constexpr uint8 kVertexCountPerFace = 3;

			IndexElementType _vertexIndexArray[kVertexCountPerFace];
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

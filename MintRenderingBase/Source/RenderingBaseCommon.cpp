#include <MintRenderingBase/Include/RenderingBaseCommon.h>

#include <MintContainer/Include/Vector.hpp>


namespace mint
{
	namespace Rendering
	{
		const Color Color::kTransparent = Color(0, 0, 0, 0);
		const Color Color::kWhite = Color(255, 255, 255, 255);
		const Color Color::kBlack = Color(0, 0, 0, 255);
		const Color Color::kRed = Color(255, 0, 0, 255);
		const Color Color::kGreen = Color(0, 255, 0, 255);
		const Color Color::kBlue = Color(0, 0, 255, 255);
		const Color Color::kCyan = Color(0, 255, 255, 255);
		const Color Color::kMagenta = Color(255, 0, 255, 255);
		const Color Color::kYellow = Color(255, 255, 0, 255);


		MINT_INLINE int32 computeIndexFromXY(const uint32 width, const uint32 x, const uint32 y)
		{
			return static_cast<int32>((width * y) + x);
		}


#pragma region ByteColorImage
		void ByteColorImage::SetSize(const Int2& size)
		{
			_size = size;
			_pixels.Resize(_size._x * _size._y);
		}

		void ByteColorImage::setPixel(const Int2& at, const ByteColor& pixel)
		{
			setPixel(at._x, at._y, pixel);
		}

		void ByteColorImage::setPixel(const int32 x, const int32 y, const ByteColor& pixel)
		{
			setPixel(computeIndexFromXY(_size._x, x, y), pixel);
		}

		void ByteColorImage::setPixel(const int32 index, const ByteColor& pixel)
		{
			_pixels[index] = pixel;
		}

		void ByteColorImage::setPixelWithAlphaBlending(const Int2& at, const ByteColor& pixel, const float alpha)
		{
			const int32 index = computeIndexFromXY(_size._x, at._x, at._y);
			const ByteColor source = _pixels[index];
			const ByteColor& destination = pixel;
			_pixels[index].r() = Min(static_cast<byte>((1.0f - alpha) * source.r()) + static_cast<byte>(alpha * destination.r()), 255);
			_pixels[index].g() = Min(static_cast<byte>((1.0f - alpha) * source.g()) + static_cast<byte>(alpha * destination.g()), 255);
			_pixels[index].b() = Min(static_cast<byte>((1.0f - alpha) * source.b()) + static_cast<byte>(alpha * destination.b()), 255);
			_pixels[index].a() = 255;
		}

		uint32 ByteColorImage::getPixelCount() const
		{
			return _pixels.Size();
		}

		const ByteColor& ByteColorImage::getPixel(const Int2& at) const
		{
			return getPixel(at._x, at._y);
		}

		const ByteColor& ByteColorImage::getPixel(const int32 x, const int32 y) const
		{
			return getPixel(computeIndexFromXY(_size._x, x, y));
		}

		const ByteColor& ByteColorImage::getPixel(const int32 index) const
		{
			return _pixels[index];
		}

		uint32 ByteColorImage::getByteCount() const
		{
			return getPixelCount() * 4;
		}

		const byte* ByteColorImage::getBytes() const
		{
			if (_pixels.IsEmpty())
			{
				return nullptr;
			}
			return reinterpret_cast<const byte*>(_pixels.Data());
		}
#pragma endregion


#pragma region ByteColorImageAtlas
		ByteColorImageAtlas::ByteColorImageAtlas()
			: _interPadding{ 1, 1 }
			, _width{ 512 }
			, _height{ 0 }
		{
			__noop;
		}

		void ByteColorImageAtlas::clearByteColorImages()
		{
			_byteColorImagePositions.Clear();
			_byteColorImageSizes.Clear();
			_byteColorImages.Clear();
			_height = 0;
		}

		int32 ByteColorImageAtlas::pushByteColorImage(ByteColorImage&& byteColorImage)
		{
			if (byteColorImage.getWidth() > _width)
			{
				MINT_LOG_ERROR("ColorImage 의 Width 가 Atlas 의 Width 보다 큽니다! Atlas 의 Width 를 먼저 충분히 크게 늘려주세요!!");
				return -1;
			}

			const Int2 byteColorImagePosition = pushColorImage_computeByteColorImagePosition(byteColorImage);
			const Int2 byteColorImageSize = byteColorImage.GetSize();
			_byteColorImagePositions.PushBack(byteColorImagePosition);
			_byteColorImageSizes.PushBack(byteColorImageSize);
			_byteColorImages.PushBack(std::move(byteColorImage));
			_height = Max(_height, byteColorImagePosition._y + byteColorImageSize._y);
			return static_cast<int32>(_byteColorImages.Size() - 1);
		}

		int32 ByteColorImageAtlas::pushByteColorImage(const ByteColorImage& byteColorImage)
		{
			if (byteColorImage.getWidth() > _width)
			{
				MINT_LOG_ERROR("ColorImage 의 Width 가 Atlas 의 Width 보다 큽니다! Atlas 의 Width 를 먼저 충분히 크게 늘려주세요!!");
				return -1;
			}

			const Int2 byteColorImagePosition = pushColorImage_computeByteColorImagePosition(byteColorImage);
			_byteColorImagePositions.PushBack(byteColorImagePosition);
			_byteColorImageSizes.PushBack(byteColorImage.GetSize());
			_byteColorImages.PushBack(byteColorImage);
			_height = Max(_height, byteColorImagePosition._y + byteColorImage.getHeight());
			return static_cast<int32>(_byteColorImages.Size() - 1);
		}

		bool ByteColorImageAtlas::bakeRGBABytes()
		{
			if (_byteColorImages.IsEmpty())
			{
				return false;
			}

			static constexpr uint32 kByteCountPerPixel = 4;
			const uint32 atlasPixelCount = static_cast<uint32>(_width * _height);
			_rgbaBytes.Clear();
			_rgbaBytes.Resize(atlasPixelCount * kByteCountPerPixel);
			const uint32 colorImageCount = _byteColorImages.Size();
			for (uint32 colorImageIndex = 0; colorImageIndex < colorImageCount; ++colorImageIndex)
			{
				const ByteColorImage& byteColorImage = _byteColorImages[colorImageIndex];
				const Int2& byteColorImagePosition = _byteColorImagePositions[colorImageIndex];
				const uint32 byteColorImageHeight = byteColorImage.getHeight();
				const uint32 byteColorImageWidth = byteColorImage.getWidth();
				for (uint32 localY = 0; localY < byteColorImageHeight; ++localY)
				{
					for (uint32 localX = 0; localX < byteColorImageWidth; ++localX)
					{
						const int32 yInAtlas = byteColorImagePosition._y + localY;
						const int32 xInAtlas = byteColorImagePosition._x + localX;
						const int32 byteIndexInAtlas = (_width * yInAtlas + xInAtlas) * kByteCountPerPixel;
						const ByteColor& pixel = byteColorImage.getPixel(localX, localY);
						_rgbaBytes[byteIndexInAtlas + 0] = pixel.r();
						_rgbaBytes[byteIndexInAtlas + 1] = pixel.g();
						_rgbaBytes[byteIndexInAtlas + 2] = pixel.b();
						_rgbaBytes[byteIndexInAtlas + 3] = pixel.a();
					}
				}
			}
			_byteColorImages.Clear();
			return true;
		}

		ByteColor ByteColorImageAtlas::getPixel(const Int2& positionInAtlas) const
		{
			const int32 indexBase = positionInAtlas._y * getWidth() * 4 + positionInAtlas._x * 4;
			const byte r = getRGBABytes()[indexBase + 0];
			const byte g = getRGBABytes()[indexBase + 1];
			const byte b = getRGBABytes()[indexBase + 2];
			const byte a = getRGBABytes()[indexBase + 3];
			return ByteColor(r, g, b, a);
		}

		Int2 ByteColorImageAtlas::computePositionInAtlas(const int32 byteColorImageIndex, const Int2& positionInByteColorImage) const
		{
			if (static_cast<uint32>(byteColorImageIndex) >= _byteColorImagePositions.Size())
			{
				return Int2::kZero;
			}

			return _byteColorImagePositions[byteColorImageIndex] + positionInByteColorImage;
		}

		Int2 ByteColorImageAtlas::pushColorImage_computeByteColorImagePosition(const ByteColorImage& byteColorImage) const
		{
			if (_byteColorImages.IsEmpty())
			{
				return Int2::kZero;
			}
			const int32 sameLinePositionX = _byteColorImagePositions.Back()._x + _byteColorImages.Back().getWidth() + _interPadding._x;
			const int32 sameLinePositionY = _byteColorImagePositions.Back()._y;
			const bool needsNewLine = sameLinePositionX + byteColorImage.getWidth() > _width;
			if (needsNewLine)
			{
				const int32 newLinePositionX = 0;
				const int32 newLinePositionY = _height + _interPadding._y;
				return Int2(newLinePositionX, newLinePositionY);
			}
			return Int2(sameLinePositionX, sameLinePositionY);
		}
#pragma endregion


#pragma region ColorImage
		ColorImage::ColorImage(const ColorImage& rhs)
			: _size{ rhs._size }
			, _colors{ rhs._colors }
		{
			__noop;
		}

		ColorImage::ColorImage(ColorImage&& rhs) noexcept
			: _size{ std::move(rhs._size) }
			, _colors{ std::move(rhs._colors) }
		{
			__noop;
		}

		ColorImage& ColorImage::operator=(const ColorImage& rhs)
		{
			if (this != &rhs)
			{
				_size = rhs._size;
				_colors = rhs._colors;
			}
			return *this;
		}

		ColorImage& ColorImage::operator=(ColorImage&& rhs) noexcept
		{
			if (this != &rhs)
			{
				_size = std::move(rhs._size);
				_colors = std::move(rhs._colors);
			}
			return *this;
		}

		void ColorImage::SetSize(const Int2& size) noexcept
		{
			_size = size;
			_colors.Resize(_size._x * _size._y);
		}

		const Int2& ColorImage::GetSize() const noexcept
		{
			return _size;
		}

		void ColorImage::fill(const Color& color) noexcept
		{
			const uint32 colorCount = _colors.Size();
			for (uint32 colorIndex = 0; colorIndex < colorCount; ++colorIndex)
			{
				Color& colorEntry = _colors[colorIndex];
				colorEntry = color;
			}
		}

		void ColorImage::fillRect(const Int2& position, const Int2& size, const Color& color) noexcept
		{
			if (size._x <= 0 || size._y <= 0)
			{
				return;
			}

			const int32 beginX = Max(position._x, 0);
			const int32 beginY = Max(position._y, 0);
			if (_size._x <= beginX || _size._y <= beginY)
			{
				return;
			}

			const int32 endX = Min(position._x + size._x, _size._x);
			const int32 endY = Min(position._y + size._y, _size._y);
			if (endX < 0 || endY < 0)
			{
				return;
			}

			for (int32 y = beginY; y < endY; ++y)
			{
				const int32 base = _size._x * y;
				for (int32 x = beginX; x < endX; ++x)
				{
					_colors[base + x] = color;
				}
			}
		}

		void ColorImage::fillCircle(const Int2& center, const int32 radius, const Color& color) noexcept
		{
			const int32 twoRadii = radius * 2;
			const int32 left = center._x - radius;
			const int32 right = left + twoRadii;
			const int32 top = center._y - radius;
			const int32 bottom = top + twoRadii;
			const float radiusF = static_cast<float>(radius);
			for (int32 y = top; y < bottom; ++y)
			{
				for (int32 x = left; x < right; ++x)
				{
					const Int2 currentPosition{ x, y };
					const Float2 diff = Float2(currentPosition - center);
					if (diff.length() <= radiusF)
					{
						setPixel(currentPosition, color);
					}
				}
			}
		}

		void ColorImage::setPixel(const int32 index, const Color& color) noexcept
		{
			_colors[index] = color;
		}

		void ColorImage::setPixel(const Int2& at, const Color& color) noexcept
		{
			const int32 index = convertXyToIndex(at._x, at._y);
			_colors[index] = color;
		}

		uint32 ColorImage::getPixelCount() const noexcept
		{
			return _colors.Size();
		}

		const Color& ColorImage::getPixel(const Int2& at) const noexcept
		{
			const int32 index = convertXyToIndex(at._x, at._y);
			return _colors[index];
		}

		const Color& ColorImage::getPixel(const int32 index) const noexcept
		{
			return _colors[index];
		}

		Color ColorImage::getSubPixel(const Float2& at) const noexcept
		{
			static constexpr float kSubPixelEpsilon = 0.01f;

			const float floorX = std::floor(at._x);
			const float deltaX = at._x - floorX;

			const float floorY = std::floor(at._y);
			const float deltaY = at._y - floorY;

			if (deltaX < kSubPixelEpsilon && deltaY < kSubPixelEpsilon)
			{
				return getPixel(Int2(static_cast<int32>(at._x), static_cast<int32>(at._y)));
			}

			const int32 y = static_cast<int32>(floorY);
			const int32 x = static_cast<int32>(floorX);
			if (deltaX < kSubPixelEpsilon) // Only vertical
			{
				const int32 yPrime = y + static_cast<int32>(std::ceil(deltaY));

				Color a = getPixel(Int2(x, y));
				Color b = getPixel(Int2(x, yPrime));

				return a * (1.0f - deltaY) + b * deltaY;
			}
			else if (deltaY < kSubPixelEpsilon) // Only horizontal
			{
				const int32 xPrime = x + static_cast<int32>(std::ceil(deltaX));

				Color a = getPixel(Int2(x, y));
				Color b = getPixel(Int2(xPrime, y));

				return a * (1.0f - deltaX) + b * deltaX;
			}
			else // Both
			{
				const int32 xPrime = x + static_cast<int32>(std::ceil(deltaX));
				const int32 yPrime = y + static_cast<int32>(std::ceil(deltaY));

				Color a0 = getPixel(Int2(x, y));
				Color b0 = getPixel(Int2(xPrime, y));
				Color r0 = a0 * (1.0f - deltaX) + b0 * deltaX;

				Color a1 = getPixel(Int2(x, yPrime));
				Color b1 = getPixel(Int2(xPrime, yPrime));
				Color r1 = a1 * (1.0f - deltaX) + b1 * deltaX;

				return r0 * (1.0f - deltaY) + r1 * deltaY;
			}
		}

		void ColorImage::getAdjacentPixels(const Int2& at, ColorImage::AdjacentPixels& outAdjacentPixels) const noexcept
		{
			outAdjacentPixels._top = (at._y <= 0) ? Color::kTransparent : getColorFromXy(at._x, at._y - 1);
			outAdjacentPixels._bottom = (_size._y - 1 <= at._y) ? Color::kTransparent : getColorFromXy(at._x, at._y + 1);
			outAdjacentPixels._left = (at._x <= 0) ? Color::kTransparent : getColorFromXy(at._x - 1, at._y);
			outAdjacentPixels._right = (_size._x - 1 <= at._x) ? Color::kTransparent : getColorFromXy(at._x + 1, at._y);
		}

		void ColorImage::getCoAdjacentPixels(const Int2& at, ColorImage::CoAdjacentPixels& outCoAdjacentPixels) const noexcept
		{
			if (at._x <= 0)
			{
				outCoAdjacentPixels._topLeft = Color::kTransparent;
				outCoAdjacentPixels._bottomLeft = Color::kTransparent;
			}
			else
			{
				outCoAdjacentPixels._topLeft = (at._y <= 0) ? Color::kTransparent : getColorFromXy(at._x - 1, at._y - 1);
				outCoAdjacentPixels._bottomLeft = (_size._y - 1 <= at._y) ? Color::kTransparent : getColorFromXy(at._x - 1, at._y + 1);
			}

			if (_size._x - 1 <= at._x)
			{
				outCoAdjacentPixels._topRight = Color::kTransparent;
				outCoAdjacentPixels._bottomRight = Color::kTransparent;
			}
			else
			{
				outCoAdjacentPixels._topRight = (at._y <= 0) ? Color::kTransparent : getColorFromXy(at._x + 1, at._y - 1);
				outCoAdjacentPixels._bottomRight = (_size._y - 1 <= at._y) ? Color::kTransparent : getColorFromXy(at._x + 1, at._y + 1);
			}
		}

		void ColorImage::sample3x3(const Int2& at, ColorImage::Sample3x3<Color>& outSample3x3) const noexcept
		{
			Color(&pixels)[3][3] = outSample3x3._m;

			pixels[0][0] = getColorFromXy(at._x - 1, at._y - 1);
			pixels[0][1] = getColorFromXy(at._x, at._y - 1);
			pixels[0][2] = getColorFromXy(at._x + 1, at._y - 1);

			pixels[1][0] = getColorFromXy(at._x - 1, at._y);
			pixels[1][1] = getColorFromXy(at._x, at._y);
			pixels[1][2] = getColorFromXy(at._x + 1, at._y);

			pixels[2][0] = getColorFromXy(at._x - 1, at._y + 1);
			pixels[2][1] = getColorFromXy(at._x, at._y + 1);
			pixels[2][2] = getColorFromXy(at._x + 1, at._y + 1);
		}

		void ColorImage::buildPixelRgbaArray(Vector<byte>& outBytes) const noexcept
		{
			static constexpr uint32 kByteCountPerPixel = 4;
			const uint32 pixelCount = static_cast<uint32>(_colors.Size());
			outBytes.Resize(pixelCount * kByteCountPerPixel);
			for (uint32 pixelIndex = 0; pixelIndex < pixelCount; ++pixelIndex)
			{
				const Color& color = _colors[pixelIndex];
				outBytes[pixelIndex * kByteCountPerPixel + 0] = color.rAsByte();
				outBytes[pixelIndex * kByteCountPerPixel + 1] = color.gAsByte();
				outBytes[pixelIndex * kByteCountPerPixel + 2] = color.bAsByte();
				outBytes[pixelIndex * kByteCountPerPixel + 3] = color.aAsByte();
			}
		}

		int32 ColorImage::convertXyToIndex(const uint32 x, const uint32 y) const noexcept
		{
			return Min(static_cast<int32>((_size._x * y) + x), static_cast<int32>(_colors.Size() - 1));
		}

		const Color& ColorImage::getColorFromXy(const uint32 x, const uint32 y) const noexcept
		{
			return (x < static_cast<uint32>(_size._x) && y < static_cast<uint32>(_size._y)) ? _colors[(_size._x * y) + x] : Color::kTransparent;
		}
#pragma endregion
	}
}

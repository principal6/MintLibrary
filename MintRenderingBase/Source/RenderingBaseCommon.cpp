#include <stdafx.h>
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

        const bool Color::isTransparent() const noexcept
        {
            return (_raw._w <= 0.0f);
        }

        constexpr float Color::toLuma() const noexcept
        {
            return _raw._x * 0.299f + _raw._y * 0.587f + _raw._z * 0.114f;
        }

        
        void ColorImage::setSize(const mint::Int2& size) noexcept
        {
            _size = size;
            _colorArray.resize(_size._x * _size._y);
        }

        const mint::Int2& ColorImage::getSize() const noexcept
        {
            return _size;
        }

        void ColorImage::fill(const Color& color) noexcept
        {
            const uint32 colorCount = _colorArray.size();
            for (uint32 colorIndex = 0; colorIndex < colorCount; ++colorIndex)
            {
                Color& colorEntry = _colorArray[colorIndex];
                colorEntry = color;
            }
        }

        void ColorImage::fillRect(const mint::Int2& position, const mint::Int2& size, const Color& color) noexcept
        {
            if (size._x <= 0 || size._y <= 0)
            {
                return;
            }
            
            const int32 beginX = mint::max(position._x, 0);
            const int32 beginY = mint::max(position._y, 0);
            if (_size._x <= beginX || _size._y <= beginY)
            {
                return;
            }

            const int32 endX = mint::min(position._x + size._x, _size._x);
            const int32 endY = mint::min(position._y + size._y, _size._y);
            if (endX < 0 || endY < 0)
            {
                return;
            }

            for (int32 y = beginY; y < endY; ++y)
            {
                const int32 base = _size._x * y;
                for (int32 x = beginX; x < endX; ++x)
                {
                    _colorArray[base + x] = color;
                }
            }
        }

        void ColorImage::fillCircle(const mint::Int2& center, const int32 radius, const Color& color) noexcept
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
                    const mint::Int2 currentPosition{ x, y };
                    const mint::Float2 diff = mint::Float2(currentPosition - center);
                    if (diff.length() <= radiusF)
                    {
                        setPixel(currentPosition, color);
                    }
                }
            }
        }

        void ColorImage::setPixel(const mint::Int2& at, const Color& color) noexcept
        {
            const int32 index = convertXyToIndex(at._x, at._y);
            _colorArray[index] = color;
        }

        const Color& ColorImage::getPixel(const mint::Int2& at) const noexcept
        {
            const int32 index = convertXyToIndex(at._x, at._y);
            return _colorArray[index];
        }

        const Color ColorImage::getSubPixel(const mint::Float2& at) const noexcept
        {
            static constexpr float kSubPixelEpsilon = 0.01f;

            const float floorX = std::floor(at._x);
            const float deltaX = at._x - floorX;

            const float floorY = std::floor(at._y);
            const float deltaY = at._y - floorY;

            if (deltaX < kSubPixelEpsilon && deltaY < kSubPixelEpsilon)
            {
                return getPixel(mint::Int2(static_cast<int32>(at._x), static_cast<int32>(at._y)));
            }

            const int32 y = static_cast<int32>(floorY);
            const int32 x = static_cast<int32>(floorX);
            if (deltaX < kSubPixelEpsilon) // Only vertical
            {   
                const int32 yPrime = y + static_cast<int32>(std::ceil(deltaY));

                Color a = getPixel(mint::Int2(x, y));
                Color b = getPixel(mint::Int2(x, yPrime));

                return a * (1.0f - deltaY) + b * deltaY;
            }
            else if (deltaY < kSubPixelEpsilon) // Only horizontal
            {
                const int32 xPrime = x + static_cast<int32>(std::ceil(deltaX));

                Color a = getPixel(mint::Int2(x, y));
                Color b = getPixel(mint::Int2(xPrime, y));

                return a * (1.0f - deltaX) + b * deltaX;
            }
            else // Both
            {
                const int32 xPrime = x + static_cast<int32>(std::ceil(deltaX));
                const int32 yPrime = y + static_cast<int32>(std::ceil(deltaY));

                Color a0 = getPixel(mint::Int2(x, y));
                Color b0 = getPixel(mint::Int2(xPrime, y));
                Color r0 = a0 * (1.0f - deltaX) + b0 * deltaX;

                Color a1 = getPixel(mint::Int2(x, yPrime));
                Color b1 = getPixel(mint::Int2(xPrime, yPrime));
                Color r1 = a1 * (1.0f - deltaX) + b1 * deltaX;

                return r0 * (1.0f - deltaY) + r1 * deltaY;
            }
        }

        void ColorImage::getAdjacentPixels(const mint::Int2& at, ColorImage::AdjacentPixels& outAdjacentPixels) const noexcept
        {
            outAdjacentPixels._top      = (at._y <= 0) ? Color::kTransparent : getColorFromXy(at._x, at._y - 1);
            outAdjacentPixels._bottom   = (_size._y - 1 <= at._y) ? Color::kTransparent : getColorFromXy(at._x, at._y + 1);
            outAdjacentPixels._left     = (at._x <= 0) ? Color::kTransparent : getColorFromXy(at._x - 1, at._y);
            outAdjacentPixels._right    = (_size._x - 1 <= at._x) ? Color::kTransparent : getColorFromXy(at._x + 1, at._y);
        }

        void ColorImage::getCoAdjacentPixels(const mint::Int2& at, ColorImage::CoAdjacentPixels& outCoAdjacentPixels) const noexcept
        {
            if (at._x <= 0)
            {
                outCoAdjacentPixels._topLeft        = Color::kTransparent;
                outCoAdjacentPixels._bottomLeft     = Color::kTransparent;
            }
            else
            {
                outCoAdjacentPixels._topLeft        = (at._y <= 0) ? Color::kTransparent : getColorFromXy(at._x - 1, at._y - 1);
                outCoAdjacentPixels._bottomLeft     = (_size._y - 1 <= at._y) ? Color::kTransparent : getColorFromXy(at._x - 1, at._y + 1);
            }

            if (_size._x - 1 <= at._x)
            {
                outCoAdjacentPixels._topRight       = Color::kTransparent;
                outCoAdjacentPixels._bottomRight    = Color::kTransparent;
            }
            else
            {
                outCoAdjacentPixels._topRight       = (at._y <= 0) ? Color::kTransparent : getColorFromXy(at._x + 1, at._y - 1);
                outCoAdjacentPixels._bottomRight    = (_size._y - 1 <= at._y) ? Color::kTransparent : getColorFromXy(at._x + 1, at._y + 1);
            }
        }

        const byte* ColorImage::buildPixelRgbaArray() noexcept
        {
            static constexpr uint32 kByteCountPerPixel = 4;
            const uint32 pixelCount = static_cast<uint32>(_colorArray.size());
            _byteArray.resize(pixelCount * kByteCountPerPixel);
            for (uint32 pixelIndex = 0; pixelIndex < pixelCount; ++pixelIndex)
            {
                const Color& color = _colorArray[pixelIndex];
                _byteArray[pixelIndex * kByteCountPerPixel + 0] = color.rAsByte();
                _byteArray[pixelIndex * kByteCountPerPixel + 1] = color.gAsByte();
                _byteArray[pixelIndex * kByteCountPerPixel + 2] = color.bAsByte();
                _byteArray[pixelIndex * kByteCountPerPixel + 3] = color.aAsByte();
            }
            return _byteArray.data();
        }

        const int32 ColorImage::convertXyToIndex(const uint32 x, const uint32 y) const noexcept
        {
            return mint::min(static_cast<int32>((_size._x * y) + x), static_cast<int32>(_colorArray.size() - 1));
        }

        const Color& ColorImage::getColorFromXy(const uint32 x, const uint32 y) const noexcept
        {
            return (x < static_cast<uint32>(_size._x) && y < static_cast<uint32>(_size._y)) ? _colorArray[(_size._x * y) + x] : Color::kTransparent;
        }
    }
}

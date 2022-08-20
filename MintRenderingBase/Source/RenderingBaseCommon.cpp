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

        
        void ColorImage::setSize(const Int2& size) noexcept
        {
            _size = size;
            _colorArray.resize(_size._x * _size._y);
        }

        const Int2& ColorImage::getSize() const noexcept
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

        void ColorImage::fillRect(const Int2& position, const Int2& size, const Color& color) noexcept
        {
            if (size._x <= 0 || size._y <= 0)
            {
                return;
            }
            
            const int32 beginX = max(position._x, 0);
            const int32 beginY = max(position._y, 0);
            if (_size._x <= beginX || _size._y <= beginY)
            {
                return;
            }

            const int32 endX = min(position._x + size._x, _size._x);
            const int32 endY = min(position._y + size._y, _size._y);
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

        void ColorImage::setPixel(const Int2& at, const Color& color) noexcept
        {
            const int32 index = convertXyToIndex(at._x, at._y);
            _colorArray[index] = color;
        }

        const Color& ColorImage::getPixel(const Int2& at) const noexcept
        {
            const int32 index = convertXyToIndex(at._x, at._y);
            return _colorArray[index];
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
            outAdjacentPixels._top      = (at._y <= 0) ? Color::kTransparent : getColorFromXy(at._x, at._y - 1);
            outAdjacentPixels._bottom   = (_size._y - 1 <= at._y) ? Color::kTransparent : getColorFromXy(at._x, at._y + 1);
            outAdjacentPixels._left     = (at._x <= 0) ? Color::kTransparent : getColorFromXy(at._x - 1, at._y);
            outAdjacentPixels._right    = (_size._x - 1 <= at._x) ? Color::kTransparent : getColorFromXy(at._x + 1, at._y);
        }

        void ColorImage::getCoAdjacentPixels(const Int2& at, ColorImage::CoAdjacentPixels& outCoAdjacentPixels) const noexcept
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

        void ColorImage::sample3x3(const Int2& at, ColorImage::Sample3x3<Color>& outSample3x3) const noexcept
        {
            Color(&pixels)[3][3] = outSample3x3._m;

            pixels[0][0] = getColorFromXy(at._x - 1, at._y - 1);
            pixels[0][1] = getColorFromXy(at._x    , at._y - 1);
            pixels[0][2] = getColorFromXy(at._x + 1, at._y - 1);
            
            pixels[1][0] = getColorFromXy(at._x - 1, at._y    );
            pixels[1][1] = getColorFromXy(at._x    , at._y    );
            pixels[1][2] = getColorFromXy(at._x + 1, at._y    );
            
            pixels[2][0] = getColorFromXy(at._x - 1, at._y + 1);
            pixels[2][1] = getColorFromXy(at._x    , at._y + 1);
            pixels[2][2] = getColorFromXy(at._x + 1, at._y + 1);
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

        int32 ColorImage::convertXyToIndex(const uint32 x, const uint32 y) const noexcept
        {
            return min(static_cast<int32>((_size._x * y) + x), static_cast<int32>(_colorArray.size() - 1));
        }

        const Color& ColorImage::getColorFromXy(const uint32 x, const uint32 y) const noexcept
        {
            return (x < static_cast<uint32>(_size._x) && y < static_cast<uint32>(_size._y)) ? _colorArray[(_size._x * y) + x] : Color::kTransparent;
        }
    }
}

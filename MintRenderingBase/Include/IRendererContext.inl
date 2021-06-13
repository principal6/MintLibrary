#pragma once


#undef max
#undef min


namespace mint
{
    namespace RenderingBase
    {
        inline const Color Color::kTransparent = Color(0, 0, 0, 0);
        inline const Color Color::kWhite = Color(255, 255, 255, 255);
        inline const Color Color::kBlack = Color(0, 0, 0, 255);

        
        MINT_INLINE void mint::RenderingBase::ColorImage::setSize(const mint::Int2& size) noexcept
        {
            _size = size;
            _colorArray.resize(_size._x * _size._y);
        }

        MINT_INLINE const mint::Int2& ColorImage::getSize() const noexcept
        {
            return _size;
        }

        MINT_INLINE void ColorImage::fill(const Color& color) noexcept
        {
            const uint32 colorCount = _colorArray.size();
            for (uint32 colorIndex = 0; colorIndex < colorCount; ++colorIndex)
            {
                Color& colorEntry = _colorArray[colorIndex];
                colorEntry = color;
            }
        }

        MINT_INLINE void ColorImage::fillRect(const mint::Int2& position, const mint::Int2& size, const Color& color) noexcept
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

        MINT_INLINE void ColorImage::fillCircle(const mint::Int2& center, const int32 radius, const Color& color) noexcept
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

        MINT_INLINE void ColorImage::setPixel(const mint::Int2& at, const Color& color) noexcept
        {
            const int32 index = convertXyToIndex(at._x, at._y);
            _colorArray[index] = color;
        }

        MINT_INLINE const Color& ColorImage::getPixel(const mint::Int2& at) const noexcept
        {
            const int32 index = convertXyToIndex(at._x, at._y);
            return _colorArray[index];
        }

        MINT_INLINE const Color ColorImage::getSubPixel(const mint::Float2& at) const noexcept
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

        MINT_INLINE void ColorImage::getAdjacentPixels(const mint::Int2& at, ColorImage::AdjacentPixels& outAdjacentPixels) const noexcept
        {
            outAdjacentPixels._top      = (at._y <= 0) ? Color::kTransparent : getColorFromXy(at._x, at._y - 1);
            outAdjacentPixels._bottom   = (_size._y - 1 <= at._y) ? Color::kTransparent : getColorFromXy(at._x, at._y + 1);
            outAdjacentPixels._left     = (at._x <= 0) ? Color::kTransparent : getColorFromXy(at._x - 1, at._y);
            outAdjacentPixels._right    = (_size._x - 1 <= at._x) ? Color::kTransparent : getColorFromXy(at._x + 1, at._y);
        }

        MINT_INLINE void ColorImage::getCoAdjacentPixels(const mint::Int2& at, ColorImage::CoAdjacentPixels& outCoAdjacentPixels) const noexcept
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

        MINT_INLINE const byte* ColorImage::buildPixelRgbaArray() noexcept
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

        MINT_INLINE const int32 ColorImage::convertXyToIndex(const uint32 x, const uint32 y) const noexcept
        {
            return mint::min(static_cast<int32>((_size._x * y) + x), static_cast<int32>(_colorArray.size() - 1));
        }

        MINT_INLINE const Color& ColorImage::getColorFromXy(const uint32 x, const uint32 y) const noexcept
        {
            return (x < static_cast<uint32>(_size._x) && y < static_cast<uint32>(_size._y)) ? _colorArray[(_size._x * y) + x] : Color::kTransparent;
        }


        inline IRendererContext::IRendererContext(mint::RenderingBase::GraphicDevice* const graphicDevice)
            : _graphicDevice{ graphicDevice }
            , _defaultColor{ Color::kWhite }
            , _viewportIndex{ 0.0f }
            , _useMultipleViewports{ false }
        {
            MINT_ASSERT("김장원", nullptr != _graphicDevice, "GraphicDevice 가 nullptr 이면 안 됩니다!");
        }

        MINT_INLINE void IRendererContext::setUseMultipleViewports() noexcept
        {
            _useMultipleViewports = true;
        }

        MINT_INLINE const bool IRendererContext::getUseMultipleViewports() const noexcept
        {
            return _useMultipleViewports;
        }


        MINT_INLINE const bool Color::isTransparent() const noexcept
        {
            return (_raw._w <= 0.0f);
        }

        MINT_INLINE constexpr float Color::toLuma() const noexcept
        {
            return _raw._x * 0.299f + _raw._y * 0.587f + _raw._z * 0.114f;
        }

        MINT_INLINE void IRendererContext::setPosition(const mint::Float4& position) noexcept
        {
            _position = position;
        }

        MINT_INLINE void IRendererContext::setPositionZ(const float s) noexcept
        {
            _position._z = s;
        }

        MINT_INLINE void IRendererContext::setColor(const mint::RenderingBase::Color& color) noexcept
        {
            _colorArray.clear();
            _defaultColor = color;
        }

        MINT_INLINE void IRendererContext::setColor(const mint::Vector<mint::RenderingBase::Color>& colorArray) noexcept
        {
            _colorArray = colorArray;

            if (_colorArray.empty() == false)
            {
                _defaultColor = _colorArray.back();
            }
        }

        MINT_INLINE void IRendererContext::setViewportIndex(const uint32 viewportIndex) noexcept
        {
            _viewportIndex = static_cast<float>(viewportIndex);
        }

        MINT_INLINE const mint::Float4& IRendererContext::getColorInternal(const uint32 index) const noexcept
        {
            const uint32 colorCount = static_cast<uint32>(_colorArray.size());
            return (colorCount <= index) ? _defaultColor : _colorArray[index];
        }
        
        MINT_INLINE const float IRendererContext::packBits2_30AsFloat(const uint32 _2bits, const uint32 _30bits) noexcept
        {
            const uint32 packedUint = (_2bits << 30) | (_30bits);
            return *reinterpret_cast<const float*>(&packedUint);
        }
    }
}

#pragma once


#undef max
#undef min


namespace mint
{
    namespace Rendering
    {
        MINT_INLINE void IRendererContext::setUseMultipleViewports() noexcept
        {
            _useMultipleViewports = true;
        }

        MINT_INLINE bool IRendererContext::isUsingMultipleViewports() const noexcept
        {
            return _useMultipleViewports;
        }

        MINT_INLINE void IRendererContext::setPosition(const Float4& position) noexcept
        {
            _position = position;
        }

        MINT_INLINE void IRendererContext::setPositionZ(const float s) noexcept
        {
            _position._z = s;
        }

        MINT_INLINE void IRendererContext::setColor(const Color& color) noexcept
        {
            _colorArray.clear();
            _defaultColor = color;
        }

        MINT_INLINE void IRendererContext::setColor(const Vector<Color>& colorArray) noexcept
        {
            _colorArray = colorArray;

            if (_colorArray.empty() == false)
            {
                _defaultColor = _colorArray.back();
            }
        }

        MINT_INLINE void IRendererContext::setClipRect(const Rect& clipRect) noexcept
        {
            _clipRect = clipRect;
        }

        MINT_INLINE const Float4& IRendererContext::getColorInternal(const uint32 index) const noexcept
        {
            const uint32 colorCount = static_cast<uint32>(_colorArray.size());
            return (colorCount <= index) ? _defaultColor : _colorArray[index];
        }
        
        MINT_INLINE float IRendererContext::packBits2_30AsFloat(const uint32 _2bits, const uint32 _30bits) noexcept
        {
            const uint32 packedUint = (_2bits << 30) | (_30bits);
            return *reinterpret_cast<const float*>(&packedUint);
        }

        MINT_INLINE float IRendererContext::packBits4_28AsFloat(const uint32 _4bits, const uint32 _28bits) noexcept
        {
            const uint32 packedUint = (_4bits << 28) | (_28bits);
            return *reinterpret_cast<const float*>(&packedUint);
        }
    }
}

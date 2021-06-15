#pragma once


#undef max
#undef min


namespace mint
{
    namespace RenderingBase
    {
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

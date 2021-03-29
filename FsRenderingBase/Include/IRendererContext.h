#pragma once


#ifndef FS_I_RENDERER_CONTEXT_H
#define FS_I_RENDERER_CONTEXT_H


#include <FsRenderingBase/Include/RenderingBaseCommon.h>

#include <FsMath/Include/Float2.h>
#include <FsMath/Include/Float3.h>
#include <FsMath/Include/Float4.h>
#include <FsMath/Include/Int2.h>

#include <FsRenderingBase/Include/IDxObject.h>


namespace fs
{
    namespace RenderingBase
    {
        class GraphicDevice;


        struct Color
        {
        public:
            static const Color      kTransparent;
            static const Color      kWhite;
            static const Color      kBlack;


        public:
            constexpr               Color() : Color(255, 255, 255) { __noop; }
            constexpr               Color(const float r, const float g, const float b, const float a) : _raw{ r, g, b, a } { __noop; }
            constexpr               Color(const int32 r, const int32 g, const int32 b, const int32 a) : Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f) { __noop; }
            constexpr               Color(const float r, const float g, const float b) : Color(r, g, b, 1.0f) { __noop; }
            constexpr               Color(const int32 r, const int32 g, const int32 b) : Color(r, g, b, 255) { __noop; }
            constexpr               Color(const fs::Float3& rgb) : Color(rgb._x, rgb._y, rgb._z, 1.0f) { __noop; }
            constexpr explicit      Color(const fs::Float4& float4) : Color(float4._x, float4._y, float4._z, float4._w) { __noop; }

        public:
                                    operator fs::Float4&() noexcept { return _raw; }
                                    operator const fs::Float4&() const noexcept { return _raw; }
                                    operator const float*() const noexcept { return _raw._f; }
            Color                   operator*(const Color& rhs) const noexcept { return Color(_raw._x * rhs._raw._x, _raw._y * rhs._raw._y, _raw._z * rhs._raw._z, _raw._w * rhs._raw._w); }
            Color                   operator*(const float s) const noexcept { return Color(_raw * s); }
            Color                   operator/(const float s) const { return Color(_raw / s); }
            Color                   operator+(const Color& rhs) const { return Color(_raw + rhs._raw); }
            Color                   operator-(const Color& rhs) const { return Color(_raw - rhs._raw); }
            const bool              operator==(const Color& rhs) const noexcept { return _raw == rhs._raw; }

        public:
            const float             r() const noexcept { return _raw._x; }
            const float             g() const noexcept { return _raw._y; }
            const float             b() const noexcept { return _raw._z; }
            const float             a() const noexcept { return _raw._w; }

            const byte              rAsByte() const noexcept { return static_cast<byte>(_raw._x * 255.99f); }
            const byte              gAsByte() const noexcept { return static_cast<byte>(_raw._y * 255.99f); }
            const byte              bAsByte() const noexcept { return static_cast<byte>(_raw._z * 255.99f); }
            const byte              aAsByte() const noexcept { return static_cast<byte>(_raw._w * 255.99f); }

            void                    r(const int32 value) noexcept { _raw._x = (value / 255.0f); }
            void                    g(const int32 value) noexcept { _raw._y = (value / 255.0f); }
            void                    b(const int32 value) noexcept { _raw._z = (value / 255.0f); }
            void                    a(const int32 value) noexcept { _raw._w = (value / 255.0f); }

            void                    r(const float value) noexcept { _raw._x = value; }
            void                    g(const float value) noexcept { _raw._y = value; }
            void                    b(const float value) noexcept { _raw._z = value; }
            void                    a(const float value) noexcept { _raw._w = value; }

        public:
            void                    rgb(const Color& rhs) noexcept { _raw._x = rhs._raw._x; _raw._y = rhs._raw._y; _raw._z = rhs._raw._z; }
            Color                   addedRgb(const float s) const noexcept { return Color(fs::Math::saturate(_raw._x + s), fs::Math::saturate(_raw._y + s), fs::Math::saturate(_raw._z + s), _raw._w); }
            Color                   addedRgb(const int32 s) const noexcept { return addedRgb(s / 255.0f); }
            Color                   scaledRgb(const float s) const noexcept { return Color(_raw._x * s, _raw._y * s, _raw._z * s, _raw._w); }
            Color                   scaledA(const float s) const noexcept { return Color(_raw._x, _raw._y, _raw._z, _raw._w * s); }
            void                    scaleR(const float s) noexcept { _raw._x *= s; }
            void                    scaleG(const float s) noexcept { _raw._y *= s; }
            void                    scaleB(const float s) noexcept { _raw._z *= s; }
            void                    scaleA(const float s) noexcept { _raw._w *= s; }
            const bool              isTransparent() const noexcept;

        private:
            fs::Float4              _raw;
        };


        class ColorImage
        {
        public:
            struct AdjacentPixels
            {
                Color   _top;
                Color   _bottom;
                Color   _left;
                Color   _right;
                Color   _center;
            };

        public:
                                    ColorImage() = default;
                                    ~ColorImage() = default;

        public:
            void                    setSize(const fs::Int2& size) noexcept;
            const fs::Int2&         getSize() const noexcept;

        public:
            void                    fill(const Color& color) noexcept;
            void                    fillRect(const fs::Int2& position, const fs::Int2& size, const Color& color) noexcept;
            void                    setPixel(const fs::Int2& at, const Color& color) noexcept;
            const Color&            getPixel(const fs::Int2& at) const noexcept;
            void                    getAdjacentPixels(const fs::Int2& at, ColorImage::AdjacentPixels& outAdjacentPixels) const noexcept;

        public:
            const byte*             buildPixelRgbaArray() noexcept;

        private:
            const int32             convertXyToIndex(const uint32 x, const uint32 y) const noexcept;
            const Color&            getColorFromXy(const uint32 x, const uint32 y) const noexcept;

        private:
            fs::Int2                _size;
            std::vector<Color>      _colorArray;
            std::vector<byte>       _byteArray;
        };


        class IRendererContext abstract
        {
        public:
                                                    IRendererContext() = default;
                                                    IRendererContext(fs::RenderingBase::GraphicDevice* const graphicDevice);
            virtual                                 ~IRendererContext() = default;

        public:
            virtual void                            initializeShaders() noexcept abstract;
            virtual void                            flushData() noexcept abstract;
            virtual const bool                      hasData() const noexcept abstract;
            virtual void                            render() noexcept abstract;
        
        public:
            void                                    setUseMultipleViewports() noexcept;
            const bool                              getUseMultipleViewports() const noexcept;

        public:
            void                                    setPosition(const fs::Float4& position) noexcept;
            void                                    setPositionZ(const float s) noexcept;
            void                                    setColor(const fs::RenderingBase::Color& color) noexcept;
            void                                    setColor(const std::vector<fs::RenderingBase::Color>& colorArray) noexcept;
            virtual void                            setViewportIndex(const uint32 viewportIndex) noexcept;

        protected:
            const fs::Float4&                       getColorInternal(const uint32 index) const noexcept;
        
        protected:
            fs::RenderingBase::GraphicDevice*       _graphicDevice;
        
        protected:
            fs::Float4                              _position;
            std::vector<RenderingBase::Color>       _colorArray;
            fs::RenderingBase::Color                _defaultColor;
            float                                   _viewportIndex;
        
        private:
            bool                                    _useMultipleViewports;
        };
    }
}


#include <FsRenderingBase/Include/IRendererContext.inl>


#endif // !FS_I_RENDERER_CONTEXT_H

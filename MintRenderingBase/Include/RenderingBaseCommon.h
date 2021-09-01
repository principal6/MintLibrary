#pragma once


#ifndef MINT_SIMPLE_RENDERING_COMMON_H
#define MINT_SIMPLE_RENDERING_COMMON_H


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

        static constexpr const char* const          kDefaultFont = "Assets/noto_sans_kr_medium";
        static constexpr int32                      kDefaultFontSize = 16;

        using IndexElementType                      = uint16;

        // Triangle face
        struct Face
        {
            static constexpr uint8                  kIndexCountPerFace = 3;

            mint::Rendering::IndexElementType     _vertexIndexArray[kIndexCountPerFace];
        };

        struct Color
        {
        public:
            static const Color      kTransparent;
            static const Color      kWhite;
            static const Color      kBlack;
            static const Color      kRed;
            static const Color      kGreen;
            static const Color      kBlue;
            static const Color      kCyan;
            static const Color      kMagenta;
            static const Color      kYellow;

        public:
            constexpr               Color() : Color(255, 255, 255) { __noop; }
            constexpr               Color(const float r, const float g, const float b, const float a) : _raw{ r, g, b, a } { __noop; }
            constexpr               Color(const int32 r, const int32 g, const int32 b, const int32 a) : Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f) { __noop; }
            constexpr               Color(const float r, const float g, const float b) : Color(r, g, b, 1.0f) { __noop; }
            constexpr               Color(const int32 r, const int32 g, const int32 b) : Color(r, g, b, 255) { __noop; }
            constexpr               Color(const mint::Float3& rgb) : Color(rgb._x, rgb._y, rgb._z, 1.0f) { __noop; }
            constexpr explicit      Color(const mint::Float4& float4) : Color(float4._x, float4._y, float4._z, float4._w) { __noop; }

        public:
                                    operator mint::Float4&() noexcept { return _raw; }
                                    operator const mint::Float4&() const noexcept { return _raw; }
                                    operator const float*() const noexcept { return _raw._f; }
            Color                   operator*(const Color& rhs) const noexcept { return Color(_raw._x * rhs._raw._x, _raw._y * rhs._raw._y, _raw._z * rhs._raw._z, _raw._w * rhs._raw._w); }
            Color                   operator*(const float s) const noexcept { return Color(_raw * s); }
            Color                   operator/(const float s) const { return Color(_raw / s); }
            Color                   operator+(const Color& rhs) const { return Color(_raw + rhs._raw); }
            Color                   operator-(const Color& rhs) const { return Color(_raw - rhs._raw); }
            const bool              operator==(const Color& rhs) const noexcept { return _raw == rhs._raw; }

        public:
            constexpr float         r() const noexcept { return _raw._x; }
            constexpr float         g() const noexcept { return _raw._y; }
            constexpr float         b() const noexcept { return _raw._z; }
            constexpr float         a() const noexcept { return _raw._w; }

            constexpr byte          rAsByte() const noexcept { return static_cast<byte>(_raw._x * 255.99f); }
            constexpr byte          gAsByte() const noexcept { return static_cast<byte>(_raw._y * 255.99f); }
            constexpr byte          bAsByte() const noexcept { return static_cast<byte>(_raw._z * 255.99f); }
            constexpr byte          aAsByte() const noexcept { return static_cast<byte>(_raw._w * 255.99f); }

            constexpr void          r(const int32 value) noexcept { _raw._x = (value / 255.0f); }
            constexpr void          g(const int32 value) noexcept { _raw._y = (value / 255.0f); }
            constexpr void          b(const int32 value) noexcept { _raw._z = (value / 255.0f); }
            constexpr void          a(const int32 value) noexcept { _raw._w = (value / 255.0f); }

            constexpr void          r(const float value) noexcept { _raw._x = value; }
            constexpr void          g(const float value) noexcept { _raw._y = value; }
            constexpr void          b(const float value) noexcept { _raw._z = value; }
            constexpr void          a(const float value) noexcept { _raw._w = value; }

        public:
            void                    rgb(const Color& rhs) noexcept { _raw._x = rhs._raw._x; _raw._y = rhs._raw._y; _raw._z = rhs._raw._z; }
            Color                   addedRgb(const float s) const noexcept { return Color(mint::Math::saturate(_raw._x + s), mint::Math::saturate(_raw._y + s), mint::Math::saturate(_raw._z + s), _raw._w); }
            Color                   addedRgb(const int32 s) const noexcept { return addedRgb(s / 255.0f); }
            Color                   scaledRgb(const float s) const noexcept { return Color(_raw._x * s, _raw._y * s, _raw._z * s, _raw._w); }
            Color                   scaledA(const float s) const noexcept { return Color(_raw._x, _raw._y, _raw._z, _raw._w * s); }
            void                    scaleR(const float s) noexcept { _raw._x *= s; }
            void                    scaleG(const float s) noexcept { _raw._y *= s; }
            void                    scaleB(const float s) noexcept { _raw._z *= s; }
            void                    scaleA(const float s) noexcept { _raw._w *= s; }
            const bool              isTransparent() const noexcept;

        public:
            constexpr float         toLuma() const noexcept; // Rec. 601

        private:
            mint::Float4            _raw;
        };


        class ColorImage
        {
        public:
            struct AdjacentPixels
            {
                Color   _left;
                Color   _top;
                Color   _right;
                Color   _bottom;
            };

            struct CoAdjacentPixels
            {
                Color   _topLeft;
                Color   _topRight;
                Color   _bottomRight;
                Color   _bottomLeft;
            };

        public:
                                    ColorImage() = default;
                                    ~ColorImage() = default;

        public:
            void                    setSize(const mint::Int2& size) noexcept;
            const mint::Int2&       getSize() const noexcept;

        public:
            void                    fill(const Color& color) noexcept;
            void                    fillRect(const mint::Int2& position, const mint::Int2& size, const Color& color) noexcept;
            void                    fillCircle(const mint::Int2& center, const int32 radius, const Color& color) noexcept;
            void                    setPixel(const mint::Int2& at, const Color& color) noexcept;
            const Color&            getPixel(const mint::Int2& at) const noexcept;
            const Color             getSubPixel(const mint::Float2& at) const noexcept;
            void                    getAdjacentPixels(const mint::Int2& at, ColorImage::AdjacentPixels& outAdjacentPixels) const noexcept;
            void                    getCoAdjacentPixels(const mint::Int2& at, ColorImage::CoAdjacentPixels& outCoAdjacentPixels) const noexcept;

        public:
            const byte*             buildPixelRgbaArray() noexcept;

        private:
            const int32             convertXyToIndex(const uint32 x, const uint32 y) const noexcept;
            const Color&            getColorFromXy(const uint32 x, const uint32 y) const noexcept;

        private:
            mint::Int2              _size;
            mint::Vector<Color>     _colorArray;
            mint::Vector<byte>      _byteArray;
        };
    }
}


#endif // !MINT_SIMPLE_RENDERING_COMMON_H

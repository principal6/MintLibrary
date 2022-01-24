#pragma once


#ifndef MINT_SIMPLE_RENDERING_COMMON_H
#define MINT_SIMPLE_RENDERING_COMMON_H


#include <d3d11.h>
#include <wrl.h>

#include <MintContainer/Include/Vector.h>

#include <MintMath/Include/Float3.h>
#include <MintMath/Include/Float4.h>
#include <MintMath/Include/Int2.h>


#undef max
#undef min


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
            static constexpr uint8  kVertexCountPerFace = 3;

            IndexElementType        _vertexIndexArray[kVertexCountPerFace];
        };

        class Color
        {
        public:
            static const Color          kTransparent;
            static const Color          kWhite;
            static const Color          kBlack;
            static const Color          kRed;
            static const Color          kGreen;
            static const Color          kBlue;
            static const Color          kCyan;
            static const Color          kMagenta;
            static const Color          kYellow;

        public:
            constexpr                   Color() : Color(255, 255, 255) { __noop; }
            constexpr                   Color(const float r, const float g, const float b, const float a) : _raw{ r, g, b, a } { __noop; }
            constexpr                   Color(const int32 r, const int32 g, const int32 b, const int32 a) : Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f) { __noop; }
            constexpr                   Color(const float r, const float g, const float b) : Color(r, g, b, 1.0f) { __noop; }
            constexpr                   Color(const int32 r, const int32 g, const int32 b) : Color(r, g, b, 255) { __noop; }
            constexpr                   Color(const Float3& rgb) : Color(rgb._x, rgb._y, rgb._z, 1.0f) { __noop; }
            constexpr explicit          Color(const Float4& float4) : Color(float4._x, float4._y, float4._z, float4._w) { __noop; }

        public:
                                        operator Float4&() noexcept { return _raw; }
                                        operator const Float4&() const noexcept { return _raw; }
                                        operator const float*() const noexcept { return _raw._c; }
            Color                       operator*(const Color& rhs) const noexcept { return Color(_raw._x * rhs._raw._x, _raw._y * rhs._raw._y, _raw._z * rhs._raw._z, _raw._w * rhs._raw._w); }
            Color                       operator*(const float s) const noexcept { return Color(_raw * s); }
            Color                       operator/(const float s) const { return Color(_raw / s); }
            Color                       operator+(const Color& rhs) const { return Color(_raw + rhs._raw); }
            Color                       operator-(const Color& rhs) const { return Color(_raw - rhs._raw); }
            const bool                  operator==(const Color& rhs) const noexcept { return _raw == rhs._raw; }

        public:
            MINT_INLINE constexpr float r() const noexcept { return _raw._x; }
            MINT_INLINE constexpr float g() const noexcept { return _raw._y; }
            MINT_INLINE constexpr float b() const noexcept { return _raw._z; }
            MINT_INLINE constexpr float a() const noexcept { return _raw._w; }

            MINT_INLINE constexpr byte  rAsByte() const noexcept { return static_cast<byte>(_raw._x * 255.99f); }
            MINT_INLINE constexpr byte  gAsByte() const noexcept { return static_cast<byte>(_raw._y * 255.99f); }
            MINT_INLINE constexpr byte  bAsByte() const noexcept { return static_cast<byte>(_raw._z * 255.99f); }
            MINT_INLINE constexpr byte  aAsByte() const noexcept { return static_cast<byte>(_raw._w * 255.99f); }

            MINT_INLINE constexpr void  r(const int32 value) noexcept { _raw._x = (value / 255.0f); }
            MINT_INLINE constexpr void  g(const int32 value) noexcept { _raw._y = (value / 255.0f); }
            MINT_INLINE constexpr void  a(const int32 value) noexcept { _raw._w = (value / 255.0f); }
            MINT_INLINE constexpr void  b(const int32 value) noexcept { _raw._z = (value / 255.0f); }

            MINT_INLINE constexpr void  r(const float value) noexcept { _raw._x = value; }
            MINT_INLINE constexpr void  g(const float value) noexcept { _raw._y = value; }
            MINT_INLINE constexpr void  b(const float value) noexcept { _raw._z = value; }
            MINT_INLINE constexpr void  a(const float value) noexcept { _raw._w = value; }

        public:
            MINT_INLINE void            set(const Color& rhs) noexcept { _raw._x = rhs._raw._x; _raw._y = rhs._raw._y; _raw._z = rhs._raw._z; }
            MINT_INLINE void            scaleR(const float s) noexcept { _raw._x *= s; }
            MINT_INLINE void            scaleG(const float s) noexcept { _raw._y *= s; }
            MINT_INLINE void            scaleB(const float s) noexcept { _raw._z *= s; }
            MINT_INLINE void            scaleA(const float s) noexcept { _raw._w *= s; }
            MINT_INLINE Color           cloneAddRGB(const float s) const noexcept { return Color(Math::saturate(_raw._x + s), Math::saturate(_raw._y + s), Math::saturate(_raw._z + s), _raw._w); }
            MINT_INLINE Color           cloneAddRGB(const int32 s) const noexcept { return cloneAddRGB(s / 255.0f); }
            MINT_INLINE Color           cloneScaleRGB(const float s) const noexcept { return Color(_raw._x * s, _raw._y * s, _raw._z * s, _raw._w); }
            MINT_INLINE Color           cloneScaleA(const float s) const noexcept { return Color(_raw._x, _raw._y, _raw._z, _raw._w * s); }
            MINT_INLINE const bool      isTransparent() const noexcept { return (_raw._w <= 0.0f); }

        public:
            // Rec. 601
            MINT_INLINE constexpr float toLuma() const noexcept { return _raw._x * 0.299f + _raw._y * 0.587f + _raw._z * 0.114f; }

        private:
            Float4                      _raw;
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

            template <typename T>
            struct Sample3x3
            {
                MINT_INLINE const T NW() const noexcept { return _m[0][0]; }
                MINT_INLINE const T N() const noexcept  { return _m[0][1]; }
                MINT_INLINE const T NE() const noexcept { return _m[0][2]; }
                
                MINT_INLINE const T W() const noexcept  { return _m[1][0]; }
                MINT_INLINE const T M() const noexcept  { return _m[1][1]; }
                MINT_INLINE const T E() const noexcept  { return _m[1][2]; }
                
                MINT_INLINE const T SW() const noexcept { return _m[2][0]; }
                MINT_INLINE const T S() const noexcept  { return _m[2][1]; }
                MINT_INLINE const T SE() const noexcept { return _m[2][2]; }

                MINT_INLINE const T maxAdjacent() const noexcept { return max(max(max(N(), S()), E()), W()); }
                MINT_INLINE const T minAdjacent() const noexcept { return min(min(min(N(), S()), E()), W()); }
                
                MINT_INLINE const T sumAdjacent() const noexcept { return N() + S() + E() + W(); }
                MINT_INLINE const T sumCoAdjacent() const noexcept { return NW() + NE() + SW() + SE(); }

                T                   _m[3][3];
            };

            struct Sample3x3Luma : public Sample3x3<float>
            {
                Sample3x3Luma(const Sample3x3<Color>& colorSample3x3)
                {
                    for (uint32 y = 0; y < 3; ++y)
                    {
                        for (uint32 x = 0; x < 3; ++x)
                        {
                            _m[y][x] = colorSample3x3._m[y][x].toLuma();
                        }
                    }
                }
            };

        public:
                                    ColorImage() = default;
                                    ~ColorImage() = default;

        public:
            void                    setSize(const Int2& size) noexcept;
            const Int2&             getSize() const noexcept;

        public:
            void                    fill(const Color& color) noexcept;
            void                    fillRect(const Int2& position, const Int2& size, const Color& color) noexcept;
            void                    fillCircle(const Int2& center, const int32 radius, const Color& color) noexcept;
            void                    setPixel(const Int2& at, const Color& color) noexcept;
            const Color&            getPixel(const Int2& at) const noexcept;
            const Color             getSubPixel(const Float2& at) const noexcept;
            void                    getAdjacentPixels(const Int2& at, ColorImage::AdjacentPixels& outAdjacentPixels) const noexcept;
            void                    getCoAdjacentPixels(const Int2& at, ColorImage::CoAdjacentPixels& outCoAdjacentPixels) const noexcept;
            void                    sample3x3(const Int2& at, ColorImage::Sample3x3<Color>& outSample3x3) const noexcept;

        public:
            const byte*             buildPixelRgbaArray() noexcept;

        private:
            const int32             convertXyToIndex(const uint32 x, const uint32 y) const noexcept;
            const Color&            getColorFromXy(const uint32 x, const uint32 y) const noexcept;

        private:
            Int2                    _size;
            Vector<Color>           _colorArray;
            Vector<byte>            _byteArray;
        };
    }
}


#endif // !MINT_SIMPLE_RENDERING_COMMON_H

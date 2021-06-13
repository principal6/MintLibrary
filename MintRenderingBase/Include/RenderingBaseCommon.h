#pragma once


#ifndef MINT_SIMPLE_RENDERING_COMMON_H
#define MINT_SIMPLE_RENDERING_COMMON_H


#include <d3d11.h>
#include <wrl.h>


namespace mint
{
    namespace RenderingBase
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
        static constexpr int32                      kDefaultFontSize = 18;

        using IndexElementType                      = uint16;

        // Triangle face
        struct Face
        {
            static constexpr uint8                  kIndexCountPerFace = 3;

            mint::RenderingBase::IndexElementType     _vertexIndexArray[kIndexCountPerFace];
        };
    }
}


#endif // !MINT_SIMPLE_RENDERING_COMMON_H

#pragma once


#ifndef MINT_CPP_HLSL_STRUCTURED_BUFFER_H
#define MINT_CPP_HLSL_STRUCTURED_BUFFER_H


#include <MintRenderingBase/Include/Language/CppHlslTypes.h>


namespace mint
{
    namespace RenderingBase
    {
        struct SB_Transform CPP_HLSL_REGISTER_INDEX(0)
        {
            float4x4    _transformMatrix;
        };

        struct SB_Material CPP_HLSL_REGISTER_INDEX(1)
        {
            float4      _diffuseColor;
        };
    }
}


#endif // !MINT_CPP_HLSL_STRUCTURED_BUFFER_H

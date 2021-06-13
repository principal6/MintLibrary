#pragma once


#ifndef MINT_CPP_HLSL_STRUCTURED_BUFFER_H
#define MINT_CPP_HLSL_STRUCTURED_BUFFER_H


#include <MintRenderingBase/Include/Language/CppHlslTypes.h>


namespace mint
{
    namespace RenderingBase
    {
        struct SB_Transform
        {
            float4x4    _transformMatrix;
        };
    }
}


#endif // !MINT_CPP_HLSL_STRUCTURED_BUFFER_H

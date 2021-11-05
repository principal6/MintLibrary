#pragma once


#ifndef MINT_CPP_HLSL_TYPES_H
#define MINT_CPP_HLSL_TYPES_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintMath/Include/Float4x4.h>


namespace mint
{
#define CPP_HLSL_SEMANTIC_NAME(SemanticName)
#define CPP_HLSL_REGISTER_INDEX(Index)
#define CPP_HLSL_INSTANCE_DATA(DataStepRate)


    using uint              = uint32;
    using float1            = float;
    using float2            = Float2;
    using float3            = Float3;
    using float4            = Float4;
    using float4x4          = Float4x4;
}


#endif // !MINT_CPP_HLSL_TYPES_H

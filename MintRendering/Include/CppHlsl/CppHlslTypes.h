#pragma once


#ifndef _MINT_RENDERING_BASE_CPP_HLSL_TYPES_H_
#define _MINT_RENDERING_BASE_CPP_HLSL_TYPES_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintMath/Include/Float4x4.h>


namespace mint
{
#define CPP_HLSL_SEMANTIC_NAME(SemanticName)
#define CPP_HLSL_REGISTER_INDEX(Index)
#define CPP_HLSL_INSTANCE_DATA(DataStepRate)


	using uint = uint32;
	using float1 = float;
	using float2 = Float2;
	using float3 = Float3;
	using float4 = Float4;
	using float4x4 = Float4x4;
}


#endif // !_MINT_RENDERING_BASE_CPP_HLSL_TYPES_H_

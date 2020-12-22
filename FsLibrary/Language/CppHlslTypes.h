#pragma once


#ifndef FS_CPP_HLSL_TYPES_H
#define FS_CPP_HLSL_TYPES_H


#include <CommonDefinitions.h>
#include <Math/Float4x4.h>


namespace fs
{
#define CPP_HLSL_SEMANTIC_NAME(SemanticName)
#define CPP_HLSL_REGISTER_INDEX(Index)


	using uint				= uint32;
	using float1			= float;
	using float2			= fs::Float2;
	using float3			= fs::Float3;
	using float4			= fs::Float4;
	using float4x4			= fs::Float4x4;
}


#endif // !FS_CPP_HLSL_TYPES_H

#pragma once


#ifndef FS_CPP_HLSL_STRUCTS_H
#define FS_CPP_HLSL_STRUCTS_H


#include <FsRenderingBase/Include/Language/CppHlslTypes.h>


namespace fs
{
	namespace RenderingBase
	{
		// Position
		// Tangent & Bitangent => Normal
		// TexCoord
		// Material ID
		// Instance ID
		struct VS_INPUT
		{
			float4				_positionU;       // position  + texCoord[0]
			float4				_tangentV;        // tangent   + texCoord[1]
			float4				_bitangentW;      // bitangent + texCoord[2]
			uint				_instanceId{ 0 };
		};

		struct VS_INPUT_SHAPE
		{
			float4				_position;
			float4				_color;
			float4				_texCoord;
			float4				_info;
		};


		struct VS_OUTPUT
		{
			float4				_screenPosition CPP_HLSL_SEMANTIC_NAME(SV_POSITION);
			float4				_worldPosition;
			float4				_worldNormal;
			float4				_worldTangent;
			float4				_worldBitangent;
			float4				_texCoord;
		};

		struct VS_OUTPUT_SHAPE
		{
			float4				_position CPP_HLSL_SEMANTIC_NAME(SV_POSITION);
			float4				_color;
			float4				_texCoord;
			float4				_info;
			uint				_viewportIndex CPP_HLSL_SEMANTIC_NAME(SV_ViewportArrayIndex);
		};
	}
}


#endif // !FS_CPP_HLSL_STRUCTS_H

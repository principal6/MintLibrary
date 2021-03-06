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
			uint				_materialId{ 0 };
			uint				_instanceId{ 0 };
		};

		struct VS_INPUT_COLOR
		{
		public:
								VS_INPUT_COLOR()
									: _flag{ 0 }
								{
									__noop;
								}
								VS_INPUT_COLOR(const float4& position, const float4& color)
									: _position{ position }
									, _color{ color }
									, _flag{ 0 }
								{
									__noop;
								}
								VS_INPUT_COLOR(const float4& position, const float2& texCoord)
									: _position{ position }
									, _texCoord{ texCoord }
									, _flag{ 1 }
								{
									__noop;
								}
								VS_INPUT_COLOR(const float4& position, const float4& color, const float2& texCoord)
									: _position{ position }
									, _color{ color }
									, _texCoord{ texCoord }
									, _flag{ 2 }
								{
									__noop;
								}

		public:
			float4				_position;
			float4				_color;
			float2				_texCoord;
			uint				_flag;
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
			uint				_materialId{ 0 };
		};

		struct VS_OUTPUT_COLOR
		{
			float4				_position CPP_HLSL_SEMANTIC_NAME(SV_POSITION);
			float4				_color;
			float2				_texCoord;
			uint				_flag{ 0 };
			uint				_viewportIndex CPP_HLSL_SEMANTIC_NAME(SV_ViewportArrayIndex);
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

#pragma once


#ifndef FS_CPP_HLSL_STRUCTS_H
#define FS_CPP_HLSL_STRUCTS_H


#include <FsLibrary/Language/CppHlslTypes.h>


namespace fs
{
	namespace CppHlsl
	{
		struct VS_INPUT
		{
		public:
								VS_INPUT()
									: _flag{ 0 }
								{
									__noop;
								}
								VS_INPUT(const float3& position, const float4& color)
									: _position{ position }
									, _color{ color }
									, _flag{ 0 }
								{
									__noop;
								}
								VS_INPUT(const float3& position, const float2& texCoord)
									: _position{ position }
									, _texCoord{ texCoord }
									, _flag{ 1 }
								{
									__noop;
								}
								VS_INPUT(const float3& position, const float4& color, const float2& texCoord)
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
			float4				_infoA;
			float4				_infoB;
			float4				_infoC;
			float4				_borderColor;
			float4				_color;
		};


		struct VS_OUTPUT
		{
			float4				_position CPP_HLSL_SEMANTIC_NAME(SV_POSITION);
			float4				_color;
			float2				_texCoord;
			uint				_flag{ 0 };
		};

		struct VS_OUTPUT_SHAPE
		{
			float4				_position CPP_HLSL_SEMANTIC_NAME(SV_POSITION);
			float4				_infoA;
			float4				_infoB;
			float4				_infoC;
			float4				_borderColor;
			float4				_color;
		};
	}
}


#endif // !FS_CPP_HLSL_STRUCTS_H

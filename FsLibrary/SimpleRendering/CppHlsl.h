#pragma once


#ifndef FS_HLSL_STRUCTS_H
#define FS_HLSL_STRUCTS_H


#include <SimpleRendering/CppHlslTypes.h>


namespace fs
{
	namespace CppHlsl
	{
		class alignas(4) VS_INPUT
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

		class alignas(4) VS_OUTPUT
		{
		public:
			float4				_position;
			float4				_color;
			float2				_texCoord;
			uint				_flag{ 0 };
		};

		class alignas(4) CB_Transforms
		{
		public:
			float4x4			_cbProjectionMatrix;
		};
	}
}


#endif // !FS_HLSL_STRUCTS_H

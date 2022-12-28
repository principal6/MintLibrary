#pragma once


#ifndef _MINT_RENDERING_BASE_CPP_HLSL_CONSTANT_BUFFERS_H_
#define _MINT_RENDERING_BASE_CPP_HLSL_CONSTANT_BUFFERS_H_


#include <MintRenderingBase/Include/CppHlsl/CppHlslTypes.h>


namespace mint
{
	namespace Rendering
	{
		struct CB_View CPP_HLSL_REGISTER_INDEX(0)
		{
			float4x4 _cb2DProjectionMatrix;
			float4x4 _cb3DProjectionMatrix;
			float4x4 _cbViewMatrix;
			float4x4 _cbViewProjectionMatrix;
		};

		struct CB_Transform CPP_HLSL_REGISTER_INDEX(1)
		{
			float4x4 _cbWorldMatrix;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_CPP_HLSL_CONSTANT_BUFFERS_H_

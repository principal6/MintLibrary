#pragma once


#ifndef _MINT_RENDERING_BASE_CPP_HLSL_STRUCTURED_BUFFER_H_
#define _MINT_RENDERING_BASE_CPP_HLSL_STRUCTURED_BUFFER_H_


#include <MintRendering/Include/CppHlsl/CppHlslTypes.h>


namespace mint
{
	namespace Rendering
	{
		// 128 비트의 배수로 struct 사이즈를 맞출 것!!

		struct SB_Transform CPP_HLSL_REGISTER_INDEX(0)
		{
			float4x4 _transformMatrix;
		};

		struct SB_Material CPP_HLSL_REGISTER_INDEX(1)
		{
			float4 _diffuseColor;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_CPP_HLSL_STRUCTURED_BUFFER_H_

#pragma once


#ifndef FS_CPP_HLSL_CBUFFERS_H
#define FS_CPP_HLSL_CBUFFERS_H


#include <Language/CppHlslTypes.h>


namespace fs
{
	namespace CppHlsl
	{
		struct CB_Transforms
		{
			float4x4			_cbProjectionMatrix;
		};
	}
}


#endif // !FS_CPP_HLSL_CBUFFERS_H

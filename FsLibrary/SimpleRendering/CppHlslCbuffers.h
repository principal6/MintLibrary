#pragma once


#ifndef FS_CPP_HLSL_CBUFFERS_H
#define FS_CPP_HLSL_CBUFFERS_H


#include <FsLibrary/Language/CppHlslTypes.h>


namespace fs
{
	namespace CppHlsl
	{
		struct CB_Transforms CPP_HLSL_REGISTER_INDEX(0)
		{
			float4x4			_cbProjectionMatrix;
		};
	}
}


#endif // !FS_CPP_HLSL_CBUFFERS_H

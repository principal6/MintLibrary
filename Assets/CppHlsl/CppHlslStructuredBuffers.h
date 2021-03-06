#pragma once


#ifndef FS_CPP_HLSL_STRUCTURED_BUFFER_H
#define FS_CPP_HLSL_STRUCTURED_BUFFER_H


#include <FsRenderingBase/Include/Language/CppHlslTypes.h>


namespace fs
{
	namespace RenderingBase
	{
		struct SB_Transform
		{
			float4x4	_transformMatrix;
		};
	}
}


#endif // !FS_CPP_HLSL_STRUCTURED_BUFFER_H

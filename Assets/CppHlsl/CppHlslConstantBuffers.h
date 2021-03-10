#pragma once


#ifndef FS_CPP_HLSL_CONSTANT_BUFFERS_H
#define FS_CPP_HLSL_CONSTANT_BUFFERS_H


#include <FsRenderingBase/Include/Language/CppHlslTypes.h>


namespace fs
{
	namespace RenderingBase
	{
		struct CB_View CPP_HLSL_REGISTER_INDEX(0)
		{
			float4x4			_cb2DProjectionMatrix;
			float4x4			_cbPerspectiveProjectionMatrix;
			float4x4			_cbViewMatrix;
			float4x4			_cbViewPerspectiveProjectionMatrix;
		};

		struct CB_Transform CPP_HLSL_REGISTER_INDEX(1)
		{
			float4x4			_cbWorldMatrix;
		};
	}
}


#endif // !FS_CPP_HLSL_CONSTANT_BUFFERS_H

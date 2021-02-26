#include <stdafx.h>
#include <FsRenderingBase/Include/IDxObject.h>


namespace fs
{
	namespace RenderingBase
	{
		const DxObjectId	DxObjectId::kInvalidObjectId;
		std::atomic<uint32>	DxObjectId::_lastRawId{ DxObjectId::kDxInvalidObjectRawId };
	}
}

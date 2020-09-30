#include <stdafx.h>
#include <SimpleRendering\IDxObject.h>


namespace fs
{
	const DxObjectId	DxObjectId::kInvalidObjectId;
	std::atomic<uint32>	DxObjectId::_lastRawId{ DxObjectId::kDxInvalidObjectRawId };
}

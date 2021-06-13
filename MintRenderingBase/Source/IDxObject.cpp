#include <stdafx.h>
#include <MintRenderingBase/Include/IDxObject.h>


namespace mint
{
    namespace RenderingBase
    {
        const DxObjectId    DxObjectId::kInvalidObjectId;
        std::atomic<uint32> DxObjectId::_lastRawId{ DxObjectId::kDxInvalidObjectRawId };
    }
}

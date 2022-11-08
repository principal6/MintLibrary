#include <MintRenderingBase/Include/IDxObject.h>


namespace mint
{
    namespace Rendering
    {
        const DxObjectID    DxObjectID::kInvalidObjectID;
        std::atomic<uint32> DxObjectID::_lastRawID{ DxObjectID::kDxInvalidObjectRawID };
    }
}

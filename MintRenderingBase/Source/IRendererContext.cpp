#include <stdafx.h>
#include <MintRenderingBase/Include/IRendererContext.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>


namespace mint
{
    namespace Rendering
    {
        void IRendererContext::flushTransformBuffer() noexcept
        {
            _sbTransformData.clear();
        }

        void IRendererContext::prepareTransformBuffer() noexcept
        {
            DxResourcePool& resourcePool = _graphicDevice.getResourcePool();
            DxResource& sbTransform = resourcePool.getResource(_graphicDevice.getCommonSBTransformID());
            sbTransform.updateBuffer(&_sbTransformData[0], _sbTransformData.size());
        }
    }
}

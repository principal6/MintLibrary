#include <stdafx.h>
#include <MintRenderingBase/Include/IRendererContext.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>


namespace mint
{
    namespace RenderingBase
    {
        void IRendererContext::flushTransformBuffer() noexcept
        {
            _sbTransformData.clear();
        }

        void IRendererContext::prepareTransformBuffer() noexcept
        {
            mint::RenderingBase::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
            mint::RenderingBase::DxResource& sbTransform = resourcePool.getResource(_graphicDevice->getCommonSbTransformId());
            sbTransform.updateBuffer(&_sbTransformData[0], _sbTransformData.size());
        }
    }
}

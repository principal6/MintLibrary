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

            const uint32 elementCount = static_cast<uint32>(_sbTransformData.size());
            if (_sbTransformBufferId.isValid() == false && 0 < elementCount)
            {
                _sbTransformBufferId = resourcePool.pushStructuredBuffer(&_sbTransformData[0], sizeof(_sbTransformData[0]), elementCount);
            }

            if (_sbTransformBufferId.isValid() == true)
            {
                mint::RenderingBase::DxResource& structuredBuffer = resourcePool.getResource(_sbTransformBufferId);
                structuredBuffer.updateBuffer(&_sbTransformData[0], elementCount);
            }
        }
    }
}

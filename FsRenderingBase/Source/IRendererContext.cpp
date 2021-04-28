#include <stdafx.h>
#include <FsRenderingBase/Include/IRendererContext.h>

#include <FsContainer/Include/Vector.hpp>

#include <FsRenderingBase/Include/GraphicDevice.h>


namespace fs
{
    namespace RenderingBase
    {
        void IRendererContext::flushTransformBuffer() noexcept
        {
            _sbTransformData.clear();
        }

        void IRendererContext::prepareTransformBuffer() noexcept
        {
            fs::RenderingBase::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();

            const uint32 elementCount = static_cast<uint32>(_sbTransformData.size());
            if (_sbTransformBufferId.isValid() == false && 0 < elementCount)
            {
                _sbTransformBufferId = resourcePool.pushStructuredBuffer(reinterpret_cast<byte*>(&_sbTransformData[0]), sizeof(_sbTransformData[0]), elementCount);
            }

            if (_sbTransformBufferId.isValid() == true)
            {
                fs::RenderingBase::DxResource& structuredBuffer = resourcePool.getResource(_sbTransformBufferId);
                structuredBuffer.updateBuffer(reinterpret_cast<byte*>(&_sbTransformData[0]), elementCount);
            }
        }
    }
}

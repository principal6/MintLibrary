#include <stdafx.h>
#include <MintRenderingBase/Include/IRendererContext.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>


namespace mint
{
    namespace Rendering
    {
        IRendererContext::IRendererContext(GraphicDevice& graphicDevice)
            : _graphicDevice{ graphicDevice }
            , _lowLevelRenderer{ nullptr }
            , _doesOwnLowLevelRenderer{ true }
            , _defaultColor{ Color::kWhite }
            , _useMultipleViewports{ false }
        {
            LowLevelRenderer<VS_INPUT_SHAPE>*& lowLevelRendererCasted = const_cast<LowLevelRenderer<VS_INPUT_SHAPE>*&>(_lowLevelRenderer);
            lowLevelRendererCasted = MINT_NEW(LowLevelRenderer<VS_INPUT_SHAPE>, graphicDevice);
        }

        IRendererContext::IRendererContext(GraphicDevice& graphicDevice, LowLevelRenderer<VS_INPUT_SHAPE>* const nonOwnedLowLevelRenderer)
            : _graphicDevice{ graphicDevice }
            , _lowLevelRenderer{ nonOwnedLowLevelRenderer }
            , _doesOwnLowLevelRenderer{ false }
            , _defaultColor{ Color::kWhite }
            , _useMultipleViewports{ false }
        {
            __noop;
        }

        inline IRendererContext::~IRendererContext()
        {
            if (_doesOwnLowLevelRenderer)
            {
                LowLevelRenderer<VS_INPUT_SHAPE>*& lowLevelRendererCasted = const_cast<LowLevelRenderer<VS_INPUT_SHAPE>*&>(_lowLevelRenderer);
                MINT_DELETE(lowLevelRendererCasted);
            }
        }

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

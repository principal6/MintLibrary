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
			, _ownsLowLevelRenderer{ true }
			, _defaultColor{ Color::kWhite }
			, _useMultipleViewports{ false }
		{
			LowLevelRenderer<VS_INPUT_SHAPE>*& lowLevelRendererCasted = const_cast<LowLevelRenderer<VS_INPUT_SHAPE>*&>(_lowLevelRenderer);
			lowLevelRendererCasted = MINT_NEW(LowLevelRenderer<VS_INPUT_SHAPE>, _graphicDevice);
		}

		IRendererContext::IRendererContext(GraphicDevice& graphicDevice, LowLevelRenderer<VS_INPUT_SHAPE>* const nonOwnedLowLevelRenderer)
			: _graphicDevice{ graphicDevice }
			, _lowLevelRenderer{ nonOwnedLowLevelRenderer }
			, _ownsLowLevelRenderer{ false }
			, _defaultColor{ Color::kWhite }
			, _useMultipleViewports{ false }
		{
			__noop;
		}

		IRendererContext::IRendererContext(const IRendererContext& rhs)
			: _graphicDevice{ rhs._graphicDevice }
			, _lowLevelRenderer{ nullptr }
			, _ownsLowLevelRenderer{ true }
			, _defaultColor{ Color::kWhite }
			, _useMultipleViewports{ false }
		{
			LowLevelRenderer<VS_INPUT_SHAPE>*& lowLevelRendererCasted = const_cast<LowLevelRenderer<VS_INPUT_SHAPE>*&>(_lowLevelRenderer);
			lowLevelRendererCasted = MINT_NEW(LowLevelRenderer<VS_INPUT_SHAPE>, _graphicDevice);
		}

		inline IRendererContext::~IRendererContext()
		{
			if (_ownsLowLevelRenderer)
			{
				LowLevelRenderer<VS_INPUT_SHAPE>*& lowLevelRendererCasted = const_cast<LowLevelRenderer<VS_INPUT_SHAPE>*&>(_lowLevelRenderer);
				MINT_DELETE(lowLevelRendererCasted);
			}
		}

		void IRendererContext::FlushTransformBuffer() noexcept
		{
			_sbTransformData.Clear();
		}

		void IRendererContext::PrepareTransformBuffer() noexcept
		{
			if (_sbTransformData.IsEmpty())
			{
				return;
			}

			GraphicResourcePool& resourcePool = _graphicDevice.GetResourcePool();
			GraphicResource& sbTransform = resourcePool.GetResource(_graphicDevice.GetCommonSBTransformID());
			sbTransform.UpdateBuffer(&_sbTransformData[0], _sbTransformData.Size());
		}
	}
}

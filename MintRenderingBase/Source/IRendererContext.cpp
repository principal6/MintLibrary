#include <MintRenderingBase/Include/IRendererContext.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicsDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>


namespace mint
{
	namespace Rendering
	{
		IRendererContext::IRendererContext(GraphicsDevice& graphicsDevice)
			: _graphicsDevice{ graphicsDevice }
			, _lowLevelRenderer{ nullptr }
			, _ownsLowLevelRenderer{ true }
			, _defaultColor{ Color::kWhite }
			, _useMultipleViewports{ false }
		{
			LowLevelRenderer<VS_INPUT_SHAPE>*& lowLevelRendererCasted = const_cast<LowLevelRenderer<VS_INPUT_SHAPE>*&>(_lowLevelRenderer);
			lowLevelRendererCasted = MINT_NEW(LowLevelRenderer<VS_INPUT_SHAPE>, _graphicsDevice);
		}

		IRendererContext::IRendererContext(GraphicsDevice& graphicsDevice, LowLevelRenderer<VS_INPUT_SHAPE>* const nonOwnedLowLevelRenderer)
			: _graphicsDevice{ graphicsDevice }
			, _lowLevelRenderer{ nonOwnedLowLevelRenderer }
			, _ownsLowLevelRenderer{ false }
			, _defaultColor{ Color::kWhite }
			, _useMultipleViewports{ false }
		{
			__noop;
		}

		IRendererContext::IRendererContext(const IRendererContext& rhs)
			: _graphicsDevice{ rhs._graphicsDevice }
			, _lowLevelRenderer{ nullptr }
			, _ownsLowLevelRenderer{ true }
			, _defaultColor{ Color::kWhite }
			, _useMultipleViewports{ false }
		{
			LowLevelRenderer<VS_INPUT_SHAPE>*& lowLevelRendererCasted = const_cast<LowLevelRenderer<VS_INPUT_SHAPE>*&>(_lowLevelRenderer);
			lowLevelRendererCasted = MINT_NEW(LowLevelRenderer<VS_INPUT_SHAPE>, _graphicsDevice);
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

			GraphicsResourcePool& resourcePool = _graphicsDevice.GetResourcePool();
			GraphicsResource& sbTransform = resourcePool.GetResource(_graphicsDevice.GetCommonSBTransformID());
			sbTransform.UpdateBuffer(&_sbTransformData[0], _sbTransformData.Size());
		}
	}
}

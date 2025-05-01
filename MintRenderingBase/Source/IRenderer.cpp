#include <MintRenderingBase/Include/IRenderer.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicsDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>


namespace mint
{
	namespace Rendering
	{
		IRenderer::IRenderer(GraphicsDevice& graphicsDevice)
			: _graphicsDevice{ graphicsDevice }
			, _lowLevelRenderer{ nullptr }
			, _ownsLowLevelRenderer{ true }
			, _color{ Color::kWhite }
			, _useMultipleViewports{ false }
		{
			LowLevelRenderer<VS_INPUT_SHAPE>*& lowLevelRendererCasted = const_cast<LowLevelRenderer<VS_INPUT_SHAPE>*&>(_lowLevelRenderer);
			lowLevelRendererCasted = MINT_NEW(LowLevelRenderer<VS_INPUT_SHAPE>, _graphicsDevice);
		}

		IRenderer::IRenderer(GraphicsDevice& graphicsDevice, LowLevelRenderer<VS_INPUT_SHAPE>* const nonOwnedLowLevelRenderer)
			: _graphicsDevice{ graphicsDevice }
			, _lowLevelRenderer{ nonOwnedLowLevelRenderer }
			, _ownsLowLevelRenderer{ false }
			, _color{ Color::kWhite }
			, _useMultipleViewports{ false }
		{
			__noop;
		}

		IRenderer::IRenderer(const IRenderer& rhs)
			: _graphicsDevice{ rhs._graphicsDevice }
			, _lowLevelRenderer{ nullptr }
			, _ownsLowLevelRenderer{ true }
			, _color{ Color::kWhite }
			, _useMultipleViewports{ false }
		{
			LowLevelRenderer<VS_INPUT_SHAPE>*& lowLevelRendererCasted = const_cast<LowLevelRenderer<VS_INPUT_SHAPE>*&>(_lowLevelRenderer);
			lowLevelRendererCasted = MINT_NEW(LowLevelRenderer<VS_INPUT_SHAPE>, _graphicsDevice);
		}

		inline IRenderer::~IRenderer()
		{
			if (_ownsLowLevelRenderer)
			{
				LowLevelRenderer<VS_INPUT_SHAPE>*& lowLevelRendererCasted = const_cast<LowLevelRenderer<VS_INPUT_SHAPE>*&>(_lowLevelRenderer);
				MINT_DELETE(lowLevelRendererCasted);
			}
		}

		void IRenderer::SetPosition(const Float4& position) noexcept
		{
			_position = position;

			if (_coordinateSpace == CoordinateSpace::Screen)
			{
				_position._y = static_cast<float>(_graphicsDevice.GetWindowSize()._y) - _position._y;
			}
		}

		void IRenderer::FlushTransformBuffer() noexcept
		{
			_sbTransformData.Clear();
		}

		void IRenderer::PrepareTransformBuffer() noexcept
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

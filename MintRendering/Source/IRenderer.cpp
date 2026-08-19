#include <MintRendering/Include/IRenderer.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRendering/Include/GraphicsDevice.h>
#include <MintRendering/Include/LowLevelRenderer.hpp>


namespace mint
{
	namespace Rendering
	{
		IRenderer::IRenderer(GraphicsDevice& graphicsDevice, LowLevelRenderer<VS_INPUT_SHAPE>& lowLevelRenderer, Vector<SB_Transform>& sbTransformData)
			: _graphicsDevice{ graphicsDevice }
			, _lowLevelRenderer{ lowLevelRenderer }
			, _sbTransformData{ sbTransformData }
			, _color{ Color::kWhite }
			, _useMultipleViewports{ false }
		{
			__noop;
		}

		IRenderer::~IRenderer()
		{
			__noop;
		}

		Float2 IRenderer::ToInternalPosition(const ScreenSpaceCoordinates& position) const
		{
			return position.ToInternal(Float2(_graphicsDevice.GetWindowSize()));
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

#include <MintRenderingBase/Include/IRenderer.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicsDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>


namespace mint
{
	namespace Rendering
	{
		IRenderer::IRenderer(GraphicsDevice& graphicsDevice, LowLevelRenderer<VS_INPUT_SHAPE>& lowLevelRenderer, Vector<SB_Transform>& sbTransformData)
			: _graphicsDevice{ graphicsDevice }
			, _lowLevelRenderer{ lowLevelRenderer }
			, _sbTransformData{ sbTransformData }
			, _coordinateSpace{ CoordinateSpace::World }
			, _color{ Color::kWhite }
			, _useMultipleViewports{ false }
		{
			__noop;
		}

		IRenderer::~IRenderer()
		{
			__noop;
		}

		Float3 IRenderer::ApplyCoordinateSpace(const Float3& position) const
		{
			Float3 outPosition = position;
			if (_coordinateSpace == CoordinateSpace::Screen)
			{
				outPosition._y = static_cast<float>(_graphicsDevice.GetWindowSize()._y) - outPosition._y;
			}
			return outPosition;
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

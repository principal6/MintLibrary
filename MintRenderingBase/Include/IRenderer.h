#pragma once


#ifndef _MINT_RENDERING_BASE_I_RENDERER_H_
#define _MINT_RENDERING_BASE_I_RENDERER_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintRenderingBase/Include/LowLevelRenderer.h>

#include <MintContainer/Include/Vector.h>

#include <MintMath/Include/Float2.h>
#include <MintMath/Include/Float3.h>
#include <MintMath/Include/Float4.h>
#include <MintMath/Include/Int2.h>
#include <MintMath/Include/Rect.h>

#include <MintRenderingBase/Include/GraphicsObject.h>

#include <Assets/Include/CppHlsl/CppHlslStructuredBuffers.h>


namespace mint
{
	namespace Rendering
	{
		class GraphicsDevice;
	}
}

namespace mint
{
	namespace Rendering
	{
		class IRenderer abstract
		{
		public:
			IRenderer(GraphicsDevice& graphicsDevice, LowLevelRenderer<VS_INPUT_SHAPE>& lowLevelRenderer, Vector<SB_Transform>& sbTransformData);
			IRenderer(const IRenderer& rhs) = delete;
			virtual ~IRenderer();

		public:
			void SetCoordinateSpace(const CoordinateSpace& coordinateSpace) noexcept;
			void SetColor(const ByteColor& color) noexcept;
			void SetColor(const Color& color) noexcept;
			void SetClipRect(const Rect& clipRect) noexcept;
			void SetUseMultipleViewports() noexcept;
		
		public:
			virtual bool IsEmpty() const noexcept abstract;
			const Rect& GetClipRect() const noexcept;
			bool IsUsingMultipleViewports() const noexcept;

		protected:
			Float3 ApplyCoordinateSpace(const Float3& position) const;
			void FlushTransformBuffer() noexcept;
			void PrepareTransformBuffer() noexcept;

		protected:
			GraphicsDevice& _graphicsDevice;
			LowLevelRenderer<VS_INPUT_SHAPE>& _lowLevelRenderer;
			Vector<SB_Transform>& _sbTransformData;

		protected:
			CoordinateSpace _coordinateSpace;
			Color _color;

		private:
			Rect _clipRect;
			bool _useMultipleViewports;
		};
	}
}


#include <MintRenderingBase/Include/IRenderer.inl>


#endif // !_MINT_RENDERING_BASE_I_RENDERER_H_

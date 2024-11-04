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
			IRenderer(GraphicsDevice& graphicsDevice);
			IRenderer(GraphicsDevice& graphicsDevice, LowLevelRenderer<VS_INPUT_SHAPE>* const nonOwnedLowLevelRenderer);
			IRenderer(const IRenderer& rhs);
			virtual ~IRenderer();

		public:
			virtual void InitializeShaders() noexcept abstract;
			virtual void Render() noexcept abstract;
			virtual void Flush() noexcept abstract;

		public:
			void SetPosition(const Float4& position) noexcept;
			void SetPositionZ(const float s) noexcept;
			void SetColor(const ByteColor& color) noexcept;
			void SetColor(const Color& color) noexcept;
			void SetClipRect(const Rect& clipRect) noexcept;
			void SetUseMultipleViewports() noexcept;
		
		public:
			virtual bool IsEmpty() const noexcept abstract;
			const Rect& GetClipRect() const noexcept;
			bool IsUsingMultipleViewports() const noexcept;

		protected:
			void FlushTransformBuffer() noexcept;
			void PrepareTransformBuffer() noexcept;

		protected:
			GraphicsDevice& _graphicsDevice;
			LowLevelRenderer<VS_INPUT_SHAPE>* const _lowLevelRenderer;

		private:
			bool _ownsLowLevelRenderer;

		protected:
			Float4 _position;
			Color _color;

		protected:
			Vector<SB_Transform> _sbTransformData;

		private:
			Rect _clipRect;
			bool _useMultipleViewports;
		};
	}
}


#include <MintRenderingBase/Include/IRenderer.inl>


#endif // !_MINT_RENDERING_BASE_I_RENDERER_H_

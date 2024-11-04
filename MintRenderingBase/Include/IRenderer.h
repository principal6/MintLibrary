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


		class IRenderer abstract
		{
		public:
			IRenderer(GraphicsDevice& graphicsDevice);
			IRenderer(GraphicsDevice& graphicsDevice, LowLevelRenderer<VS_INPUT_SHAPE>* const nonOwnedLowLevelRenderer);
			IRenderer(const IRenderer& rhs);
			virtual ~IRenderer();

		public:
			virtual void InitializeShaders() noexcept abstract;
			virtual bool IsEmpty() const noexcept abstract;
			virtual void Flush() noexcept abstract;
			virtual void Render() noexcept abstract;

		public:
			void SetUseMultipleViewports() noexcept;
			bool IsUsingMultipleViewports() const noexcept;

		public:
			void SetPosition(const Float4& position) noexcept;
			void SetPositionZ(const float s) noexcept;
			void SetColor(const ByteColor& color) noexcept;
			void SetColor(const Color& color) noexcept;
			void SetColor(const Vector<Color>& colorArray) noexcept;
			virtual void SetClipRect(const Rect& clipRect) noexcept;

		protected:
			void FlushTransformBuffer() noexcept;
			void PrepareTransformBuffer() noexcept;

		protected:
			const Float4& GetColorInternal(const uint32 index) const noexcept;
			static float PackBits2_30AsFloat(const uint32 _2bits, const uint32 _30bits) noexcept;
			static float PackBits4_28AsFloat(const uint32 _4bits, const uint32 _28bits) noexcept;

		protected:
			GraphicsDevice& _graphicsDevice;
			LowLevelRenderer<VS_INPUT_SHAPE>* const _lowLevelRenderer;

		private:
			bool _ownsLowLevelRenderer;

		protected:
			Float4 _position;
			Vector<Color> _colorArray;
			Color _defaultColor;
			Rect _clipRect;

		protected:
			Vector<SB_Transform> _sbTransformData;

		private:
			bool _useMultipleViewports;
		};
	}
}


#include <MintRenderingBase/Include/IRenderer.inl>


#endif // !_MINT_RENDERING_BASE_I_RENDERER_H_

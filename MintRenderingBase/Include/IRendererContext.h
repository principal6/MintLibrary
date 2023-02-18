#pragma once


#ifndef _MINT_RENDERING_BASE_I_RENDERER_CONTEXT_H_
#define _MINT_RENDERING_BASE_I_RENDERER_CONTEXT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintRenderingBase/Include/LowLevelRenderer.h>

#include <MintContainer/Include/Vector.h>

#include <MintMath/Include/Float2.h>
#include <MintMath/Include/Float3.h>
#include <MintMath/Include/Float4.h>
#include <MintMath/Include/Int2.h>
#include <MintMath/Include/Rect.h>

#include <MintRenderingBase/Include/GraphicObject.h>

#include <Assets/Include/CppHlsl/CppHlslStructuredBuffers.h>


namespace mint
{
	namespace Rendering
	{
		class GraphicDevice;


		class IRendererContext abstract
		{
		public:
			IRendererContext(GraphicDevice& graphicDevice);
			IRendererContext(GraphicDevice& graphicDevice, LowLevelRenderer<VS_INPUT_SHAPE>* const nonOwnedLowLevelRenderer);
			virtual ~IRendererContext();

		public:
			virtual void InitializeShaders() noexcept abstract;
			virtual void Flush() noexcept abstract;
			virtual void Render() noexcept abstract;

		protected:
			void FlushTransformBuffer() noexcept;
			void PrepareTransformBuffer() noexcept;

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
			const Float4& GetColorInternal(const uint32 index) const noexcept;
			static float PackBits2_30AsFloat(const uint32 _2bits, const uint32 _30bits) noexcept;
			static float PackBits4_28AsFloat(const uint32 _4bits, const uint32 _28bits) noexcept;

		public:
			LowLevelRenderer<VS_INPUT_SHAPE>& AccessLowLevelRenderer() noexcept;

		protected:
			GraphicDevice& _graphicDevice;
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


#include <MintRenderingBase/Include/IRendererContext.inl>


#endif // !_MINT_RENDERING_BASE_I_RENDERER_CONTEXT_H_

#pragma once


#ifndef _MINT_RENDERING_BASE_I_RENDERER_H_
#define _MINT_RENDERING_BASE_I_RENDERER_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRendering/Include/RenderingBaseCommon.h>
#include <MintRendering/Include/LowLevelRenderer.h>

#include <MintContainer/Include/Vector.h>

#include <MintMath/Include/Float2.h>
#include <MintMath/Include/Float3.h>
#include <MintMath/Include/Float4.h>
#include <MintMath/Include/Int2.h>
#include <MintMath/Include/Rect.h>
#include <MintMath/Include/ScreenSpaceCoordinates.h>

#include <MintRendering/Include/GraphicsObject.h>

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
			void SetColor(const ByteColor& color) noexcept;
			void SetColor(const Color& color) noexcept;
			// clipRect 는 D3D11 ScissorRect 로 그대로 넘어가므로 반드시 화면 좌상단이 원점인 픽셀 공간이어야 한다.
			// 내부 2D 좌표(화면 중앙 원점, +Y 위)에서 만들려면 ScreenSpaceCoordinates::FromInternal 로 변환할 것.
			void SetClipRect(const Rect& clipRect) noexcept;
			void SetUseMultipleViewports() noexcept;
		
		public:
			virtual bool IsEmpty() const noexcept abstract;
			const Rect& GetClipRect() const noexcept;
			bool IsUsingMultipleViewports() const noexcept;

		protected:
			// 화면 좌상단이 원점인 픽셀 좌표를 내부 2D 좌표(화면 중앙이 원점, +Y 가 위)로 변환한다.
			// 두 공간 사이의 변환은 오직 여기서만 일어난다.
			Float2 ToInternalPosition(const ScreenSpaceCoordinates& position) const;
			void FlushTransformBuffer() noexcept;
			void PrepareTransformBuffer() noexcept;

		protected:
			GraphicsDevice& _graphicsDevice;
			LowLevelRenderer<VS_INPUT_SHAPE>& _lowLevelRenderer;
			Vector<SB_Transform>& _sbTransformData;

		protected:
			Color _color;

		private:
			Rect _clipRect;
			bool _useMultipleViewports;
		};
	}
}


#include <MintRendering/Include/IRenderer.inl>


#endif // !_MINT_RENDERING_BASE_I_RENDERER_H_

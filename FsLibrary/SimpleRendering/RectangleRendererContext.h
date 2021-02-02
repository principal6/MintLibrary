#pragma once


#ifndef FS_RECTANGLE_RENDERER_CONTEXT_H
#define FS_RECTANGLE_RENDERER_CONTEXT_H


#include <CommonDefinitions.h>

#include <FsLibrary/SimpleRendering/IRendererContext.h>
#include <FsLibrary/SimpleRendering/TriangleRenderer.h>


namespace fs
{
	namespace SimpleRendering
	{
		class RectangleRendererContext final : public IRendererContext
		{
		public:
																	RectangleRendererContext(fs::SimpleRendering::GraphicDevice* const graphicDevice);
			virtual													~RectangleRendererContext() = default;

		public:
			virtual void											initializeShaders() noexcept override final;
			virtual void											flushData() noexcept override final;
			virtual const bool										hasData() const noexcept override final;
			virtual void											render() noexcept final;

		public:
			FS_INLINE void											setSize(const fs::Float2& size) { _size = size; }

		public:
			void													drawColored();
			void													drawTextured(const fs::Float2& texturePosition, const fs::Float2& textureSize);
			void													drawColoredTextured(const fs::Float2& texturePosition, const fs::Float2& textureSize);

		private:
			void													prepareIndexArray();

		private:
			fs::Float2												_size;
		
		private:
			SimpleRendering::TriangleRenderer<CppHlsl::VS_INPUT>	_triangleRenderer;
			DxObjectId												_vertexShaderId;
			DxObjectId												_pixelShaderId;
		};
	}
}


#endif // !FS_RECTANGLE_RENDERER_CONTEXT_H

﻿#pragma once


#ifndef FS_RECTANGLE_DRAW_H
#define FS_RECTANGLE_DRAW_H


#include <CommonDefinitions.h>

#include <SimpleRendering/IRenderer.h>
#include <SimpleRendering/TriangleBuffer.h>


namespace fs
{
	namespace SimpleRendering
	{
		class RectangleRenderer final : public IRenderer
		{
		public:
																	RectangleRenderer(fs::SimpleRendering::GraphicDevice* const graphicDevice);
			virtual													~RectangleRenderer() = default;

		public:
			virtual void											initializeShaders() noexcept override final;
			virtual void											flushData() noexcept override final;
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
			SimpleRendering::TriangleBuffer<CppHlsl::VS_INPUT>		_rectangleRendererBuffer;
			DxObjectId												_vertexShader;
			DxObjectId												_pixelShader;
		};
	}
}


#endif // !FS_RECTANGLE_DRAW_H

#pragma once


#ifndef FS_RECTANGLE_DRAW_H
#define FS_RECTANGLE_DRAW_H


#include <CommonDefinitions.h>
#include <SimpleRendering/IRenderer.h>


namespace fs
{
	class RectangleRenderer final : public IRenderer
	{
	public:
								RectangleRenderer(fs::GraphicDevice* const graphicDevice) : IRenderer(graphicDevice) {}
		virtual					~RectangleRenderer() = default;

	public:
		FS_INLINE void			setSize(const fs::Float2& size) { _size = size; }

	public:
		virtual void			drawColored() override final;
		virtual void			drawTextured(const fs::Float2& texturePosition, const fs::Float2& textureSize) override final;
		virtual void			drawColoredTextured(const fs::Float2& texturePosition, const fs::Float2& textureSize) override final;

	private:
		void					prepareIndexArray();

	private:
		fs::Float2				_size;
	};
}


#endif // !FS_RECTANGLE_DRAW_H

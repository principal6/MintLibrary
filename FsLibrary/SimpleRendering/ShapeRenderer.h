#pragma once


#ifndef FS_SHAPE_RENDERER_H
#define FS_SHAPE_RENDERER_H


#include <CommonDefinitions.h>

#include <SimpleRendering/IRenderer.h>
#include <SimpleRendering/TriangleBuffer.h>

#include <Math/Int2.h>


namespace fs
{
	namespace SimpleRendering
	{
		class ShapeRenderer final : public IRenderer
		{
		public:
																		ShapeRenderer(fs::SimpleRendering::GraphicDevice* const graphicDevice);
																		~ShapeRenderer() = default;

		public:
			virtual void												initializeShaders() noexcept override final;
			virtual void												flushData() noexcept override final;
			virtual void												render() noexcept final;

		public:
			void														drawRoundedRectangle(const fs::Int2& size, const float roundness);
			void														drawTaperedRectangle(const fs::Int2& size, const float tapering, const float bias);

		private:
			void														prepareVertexArray(fs::CppHlsl::VS_INPUT_SHAPE& data, const fs::Float2& position, const fs::Float2& halfSize);
			void														prepareIndexArray();

		private:
			SimpleRendering::TriangleBuffer<CppHlsl::VS_INPUT_SHAPE>	_shapeBuffer;
			DxObjectId													_vertexShader;
			DxObjectId													_pixelShader;
		};
	}
}


#endif // !FS_SHAPE_RENDERER_H

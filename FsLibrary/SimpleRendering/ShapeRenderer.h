#pragma once


#ifndef FS_SHAPE_RENDERER_H
#define FS_SHAPE_RENDERER_H


#include <CommonDefinitions.h>

#include <FsLibrary/SimpleRendering/IRenderer.h>
#include <FsLibrary/SimpleRendering/TriangleRenderer.h>

#include <FsMath/Include/Int2.h>

#include <Assets/CppHlsl/CppHlslStructuredBuffers.h>


namespace fs
{
	namespace SimpleRendering
	{
		class ShapeRenderer final : public IRenderer
		{
		public:
			static constexpr float												kRoundnessAbsoluteBase = 2.0f;

		private:
			static constexpr float												kInfoSolid = 1.0f;
			static constexpr float												kInfoCircular = 2.0f;

		public:
																				ShapeRenderer(fs::SimpleRendering::GraphicDevice* const graphicDevice);
																				~ShapeRenderer() = default;

		public:
			virtual void														initializeShaders() noexcept override final;
			virtual void														flushData() noexcept override final;
			virtual void														render() noexcept final;

		public:
			void																setBorderColor(const fs::Float4& borderColor) noexcept;

		public:
			// Independent from internal position set by setPosition() call
			// No rotation allowed
			void																drawQuadraticBezier(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& controlPoint, const bool validate = true);

		private:
			void																drawQuadraticBezierInternal(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& controlPoint, const bool validate = true);
			
		public:
			// Independent from internal position set by setPosition() call
			// No rotation allowed
			void																drawSolidTriangle(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& pointC);

		private:
			void																drawSolidTriangleInternal(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& pointC);

		public:
			void																drawCircularTriangle(const float radius, const float rotationAngle, const bool insideOut = false);
			void																drawQuarterCircle(const float radius, const float rotationAngle);
			void																drawHalfCircle(const float radius, const float rotationAngle, const bool insideOut = false);

			// arcAngle = [0, +pi]
			void																drawCircularArc(const float radius, const float arcAngle, const float rotationAngle);

			// arcAngle = [0, +pi]
			void																drawDoubleCircularArc(const float outerRadius, const float innerRadius, const float arcAngle, const float rotationAngle);

			void																drawRectangle(const fs::Float2& size, const float borderThickness, const float rotationAngle);
			void																drawTaperedRectangle(const fs::Float2& size, const float tapering, const float bias, const float borderThickness, const float rotationAngle);
			void																drawRoundedRectangle(const fs::Float2& size, const float roundness, const float borderThickness, const float rotationAngle);

			// Independent from internal position set by setPosition() call
			// No rotation allowed
			void																drawLine(const fs::Float2& p0, const fs::Float2& p1, const float thickness);

		private:
			void																flushShapeTransform();
			const float															getShapeTransformIndexAsFloat() const noexcept;
			void																pushShapeTransform(const float rotationAngle, const bool applyInternalPosition = true);
			void																prepareStructuredBuffer();

		public:
			// This function is slow...!!!
			void																drawColorPallete(const float radius);

		private:
			SimpleRendering::TriangleRenderer<CppHlsl::VS_INPUT_SHAPE_FAST>		_triangleRenderer;
			DxObjectId															_vertexShaderId;
			DxObjectId															_pixelShaderId;
			std::vector<fs::CppHlsl::SB_Transform>								_sbTransformData;
			DxObjectId															_sbTransformBufferId;
		
		private:
			fs::Float4															_borderColor;
		};
	}
}


#endif // !FS_SHAPE_RENDERER_H

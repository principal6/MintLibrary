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
			enum class ShapeType : uint8
			{
				None,
				Solid,
				Circular,
			};

		public:
			static constexpr float												kRoundnessAbsoluteBase	= 2.0f;

		private:
			static constexpr uint8												kInfoSolid				= 1;
			static constexpr uint8												kInfoCircular			= 2;

		public:
																				ShapeRenderer(fs::SimpleRendering::GraphicDevice* const graphicDevice);
																				~ShapeRenderer() = default;

		public:
			virtual void														initializeShaders() noexcept override final;
			virtual void														flushData() noexcept override final;
			virtual void														render() noexcept final;

		public:
			void																setBorderColor(const fs::SimpleRendering::Color& borderColor) noexcept;

		public:
			// Independent from internal position set by setPosition() call
			// No rotation allowed
			void																drawQuadraticBezier(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& controlPoint, const bool validate = true);

		private:
			void																drawQuadraticBezierInternal(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& controlPoint, const fs::SimpleRendering::Color& color, const bool validate = true);
			
		public:
			// Independent from internal position set by setPosition() call
			// No rotation allowed
			void																drawSolidTriangle(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& pointC);

		private:
			void																drawSolidTriangleInternal(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& pointC, const fs::SimpleRendering::Color& color);

		public:
			void																drawCircularTriangle(const float radius, const float rotationAngle, const bool insideOut = false);
			void																drawQuarterCircle(const float radius, const float rotationAngle);
		
		private:
			void																drawQuarterCircleInternal(const fs::Float2& offset, const float halfRadius, const fs::SimpleRendering::Color& color);

		public:
			void																drawHalfCircle(const float radius, const float rotationAngle, const bool insideOut = false);

			// arcAngle = [0, +pi]
			void																drawCircularArc(const float radius, const float arcAngle, const float rotationAngle);

			// arcAngle = [0, +pi]
			void																drawDoubleCircularArc(const float outerRadius, const float innerRadius, const float arcAngle, const float rotationAngle);

		public:
			void																drawRectangle(const fs::Float2& size, const float borderThickness, const float rotationAngle);

		private:
			void																drawRectangleInternal(const fs::Float2& offset, const fs::Float2& halfSize, const fs::SimpleRendering::Color& color);

		public:
			void																drawTaperedRectangle(const fs::Float2& size, const float tapering, const float bias, const float rotationAngle);
			void																drawRoundedRectangle(const fs::Float2& size, const float roundness, const float borderThickness, const float rotationAngle);

		private:
			void																drawRoundedRectangleInternal(const float radius, const fs::Float2& halfSize, const float roundness, const fs::SimpleRendering::Color& color);

		public:
			// Independent from internal position set by setPosition() call
			// No rotation allowed
			void																drawLine(const fs::Float2& p0, const fs::Float2& p1, const float thickness);

		private:
			void																flushShapeTransform();
			const float															getShapeInfoAsFloat(const ShapeType shapeType) const noexcept;
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
			fs::SimpleRendering::Color											_borderColor;
		};
	}
}


#endif // !FS_SHAPE_RENDERER_H

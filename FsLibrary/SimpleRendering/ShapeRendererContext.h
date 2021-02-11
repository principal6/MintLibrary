#pragma once


#ifndef FS_SHAPE_RENDERER_CONTEXT_H
#define FS_SHAPE_RENDERER_CONTEXT_H


#include <CommonDefinitions.h>

#include <FsLibrary/SimpleRendering/IRendererContext.h>
#include <FsLibrary/SimpleRendering/TriangleRenderer.h>

#include <FsMath/Include/Int2.h>

#include <Assets/CppHlsl/CppHlslStructuredBuffers.h>


namespace fs
{
	namespace SimpleRendering
	{
		class ShapeRendererContext : public IRendererContext
		{
			enum class ShapeType : uint8
			{
				QuadraticBezierTriangle,
				SolidTriangle,
				Circular,
				Circle,
			};

		public:
			static constexpr float												kRoundnessAbsoluteBase	= 2.0f;

		protected:
			static constexpr uint8												kInfoSolid				= 1;
			static constexpr uint8												kInfoCircular			= 2;

		public:
																				ShapeRendererContext(fs::SimpleRendering::GraphicDevice* const graphicDevice);
			virtual																~ShapeRendererContext();

		public:
			virtual void														initializeShaders() noexcept override;
			virtual void														flushData() noexcept override;
			virtual const bool													hasData() const noexcept override;
			virtual void														render() noexcept;

		public:
			void																setBorderColor(const fs::SimpleRendering::Color& borderColor) noexcept;

		public:
			// Independent from internal position set by setPosition() call
			// No rotation allowed
			void																drawQuadraticBezier(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& controlPoint, const bool validate = true);

		protected:
			void																drawQuadraticBezierInternal(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& controlPoint, const fs::SimpleRendering::Color& color, const bool validate = true);
			
		public:
			// Independent from internal position set by setPosition() call
			// No rotation allowed
			void																drawSolidTriangle(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& pointC);

		protected:
			void																drawSolidTriangleInternal(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& pointC, const fs::SimpleRendering::Color& color);

		public:
			void																drawCircularTriangle(const float radius, const float rotationAngle, const bool insideOut = false);
			void																drawQuarterCircle(const float radius, const float rotationAngle);
		
		protected:
			void																drawQuarterCircleInternal(const fs::Float2& offset, const float halfRadius, const fs::SimpleRendering::Color& color);

		public:
			// This function Interprets internal positon as the center of the entire circle (= center root of half circle)
			void																drawHalfCircle(const float radius, const float rotationAngle);

		public:
			void																drawCircle(const float radius, const bool insideOut = false);

			// arcAngle = [0, +pi]
			void																drawCircularArc(const float radius, const float arcAngle, const float rotationAngle);

			// arcAngle = [0, +pi]
			void																drawDoubleCircularArc(const float outerRadius, const float innerRadius, const float arcAngle, const float rotationAngle);

		public:
			void																drawRectangle(const fs::Float2& size, const float borderThickness, const float rotationAngle);

		protected:
			void																drawRectangleInternal(const fs::Float2& offset, const fs::Float2& halfSize, const fs::SimpleRendering::Color& color);

		public:
			void																drawTaperedRectangle(const fs::Float2& size, const float tapering, const float bias, const float rotationAngle);
			void																drawRoundedRectangle(const fs::Float2& size, const float roundness, const float borderThickness, const float rotationAngle);
			void																drawHalfRoundedRectangle(const fs::Float2& size, const float roundness, const float rotationAngle);

		protected:
			void																drawRoundedRectangleInternal(const float radius, const fs::Float2& halfSize, const float roundness, const fs::SimpleRendering::Color& color);
			void																drawHalfRoundedRectangleInternal(const float radius, const fs::Float2& halfSize, const float roundness, const fs::SimpleRendering::Color& color);

		public:
			// Independent from internal position set by setPosition() call
			// No rotation allowed
			void																drawLine(const fs::Float2& p0, const fs::Float2& p1, const float thickness);

		protected:
			void																flushShapeTransform();
			const float															getShapeInfoAsFloat(const ShapeType shapeType) const noexcept;
			void																pushShapeTransform(const float rotationAngle, const bool applyInternalPosition = true);
			void																prepareStructuredBuffer();

		public:
			// This function is slow...!!!
			void																drawColorPallete(const float radius);

		protected:
			SimpleRendering::TriangleRenderer<CppHlsl::VS_INPUT_SHAPE>*			_triangleRenderer;
			DxObjectId															_vertexShaderId;
			DxObjectId															_geometryShaderId;
			DxObjectId															_pixelShaderId;

		protected:
			std::vector<fs::CppHlsl::SB_Transform>								_sbTransformData;
			DxObjectId															_sbTransformBufferId;
		
		protected:
			fs::SimpleRendering::Color											_borderColor;
		};
	}
}


#endif // !FS_SHAPE_RENDERER_CONTEXT_H

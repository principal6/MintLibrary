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
			static constexpr float											kRoundnessAbsoluteBase = 2.0f;

		private:
			static constexpr float											kInfoSolid = 1.0f;
			static constexpr float											kInfoCircular = 2.0f;

		public:
																			ShapeRenderer(fs::SimpleRendering::GraphicDevice* const graphicDevice);
																			~ShapeRenderer() = default;

		public:
			virtual void													initializeShaders() noexcept override final;
			virtual void													flushData() noexcept override final;
			virtual void													render() noexcept final;

		public:
			void															setBorderColor(const fs::Float4& borderColor) noexcept;

		public:
			// Independent from internal position set by setPosition() call
			// No rotation allowed
			void															drawQuadraticBezier(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& controlPoint, const bool validate = true);

		private:
			void															drawQuadraticBezierInternal(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& controlPoint, const bool validate = true);
			
		public:
			// Independent from internal position set by setPosition() call
			// No rotation allowed
			void															drawSolidTriangle(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& pointC);

		private:
			void															drawSolidTriangleInternal(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& pointC);

		public:
			void															drawCircularTriangle(const float radius, const float rotationAngle, const bool insideOut = false);
			void															drawQuarterCircle(const float radius, const float rotationAngle);
			void															drawHalfCircle(const float radius, const float rotationAngle, const bool insideOut = false);

			// arcAngle = [0, +pi]
			void															drawCircularArcFast(const float radius, const float arcAngle, const float rotationAngle);

			// arcAngle = [0, +pi]
			void															drawDoubleCircularArcFast(const float outerRadius, const float innerRadius, const float arcAngle, const float rotationAngle);

			void															drawRectangleFast(const fs::Float2& size, const float borderThickness, const float rotationAngle);
			void															drawTaperedRectangleFast(const fs::Float2& size, const float tapering, const float bias, const float borderThickness, const float rotationAngle);
			void															drawRoundedRectangleFast(const fs::Float2& size, const float roundness, const float borderThickness, const float rotationAngle);

			// Independent from internal position set by setPosition() call
			// No rotation allowed
			void															drawLineFast(const fs::Float2& p0, const fs::Float2& p1, const float thickness);

		private:
			void															flushShapeTransform();
			const float														getShapeTransformIndexAsFloat() const noexcept;
			void															pushShapeTransform(const float rotationAngle, const bool applyInternalPosition = true);
			void															prepareStructuredBuffer();

		public:
			void															drawRectangle(const fs::Int2& size, const uint32 borderThickness, const float angle);
			void															drawRoundedRectangle(const fs::Int2& size, const float roundness, const uint32 borderThickness, const float angle);
			void															drawTaperedRectangle(const fs::Int2& size, const float tapering, const float bias, const uint32 borderThickness, const float angle);
			void															drawLine(const fs::Int2& p0, const fs::Int2& p1, const float thickness);
			
			// Arc Angle A & B = [-pi, pi]
			// A < B
			void															drawCircularArc(const float radius, const float arcAngleA = -fs::Math::kPi, const float arcAngleB = +fs::Math::kPi, const float innerRadius = 0.0f);

		public:
			// This function is slow...!!!
			void															drawColorPallete(const float radius);

		private:
			fs::Float2														normalizePosition(const fs::Float2& position, const fs::Float2& screenSize);
			void															prepareVertexArray(fs::CppHlsl::VS_INPUT_SHAPE& data, const fs::Float2& position, const fs::Float2& halfSize);
			void															prepareIndexArray();

		private:
			SimpleRendering::TriangleRenderer<CppHlsl::VS_INPUT_SHAPE>		_triangleRenderer;
			DxObjectId														_vertexShaderId;
			DxObjectId														_pixelShaderId;

		private:
			SimpleRendering::TriangleRenderer<CppHlsl::VS_INPUT_SHAPE_FAST>	_triangleRendererFast;
			DxObjectId														_vertexShaderFastId;
			DxObjectId														_pixelShaderFastId;
			std::vector<fs::CppHlsl::SB_Transform>							_sbTransformData;
			DxObjectId														_sbTransformBufferId;
		
		private:
			fs::Float4														_borderColor;
		};
	}
}


#endif // !FS_SHAPE_RENDERER_H

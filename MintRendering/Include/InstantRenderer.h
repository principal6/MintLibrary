#pragma once


#ifndef _MINT_RENDERING_INSTANT_RENDERER_H_
#define _MINT_RENDERING_INSTANT_RENDERER_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/LowLevelRenderer.h>

#include <Assets/Include/CppHlsl/CppHlslConstantBuffers.h>


namespace mint
{
	struct Transform;


	namespace Rendering
	{
		class GraphicDevice;
		struct SB_Material;
		class Color;


		class InstantRenderer final
		{
		public:
			InstantRenderer(GraphicDevice& graphicDevice);
			~InstantRenderer();

		public:
			void TestDraw(const Float3& worldOffset) noexcept;

		public:
			void DrawLine(const Float3& a, const Float3& b, const Color& color) noexcept;
			void DrawBox(const Transform& worldTransform, const Float3& extents, const Color& color) noexcept;
			void DrawCone(const Transform& worldTransform, const float radius, const float height, const uint8 detail, const Color& color) noexcept;
			void DrawCylinder(const Transform& worldTransform, const float radius, const float height, const uint8 subdivisionIteration, const Color& color) noexcept;
			void DrawSphere(const Float3& center, const float radius, const uint8 polarDetail, const uint8 azimuthalDetail, const Color& color) noexcept;
			void DrawGeoSphere(const Float3& center, const float radius, const uint8 subdivisionIteration, const Color& color) noexcept;
			void DrawCapsule(const Transform& worldTransform, const float sphereRadius, const float cylinderHeight, const uint8 subdivisionIteration, const Color& color) noexcept;

		private:
			void Initialize() noexcept;
			void PushMeshWithMaterial(MeshData& meshData, const Color& diffuseColor) noexcept;

		public:
			void Render() noexcept;

		private:
			GraphicDevice& _graphicDevice;

		private:
			LowLevelRenderer<VS_INPUT> _lowLevelRendererLine;
			LowLevelRenderer<VS_INPUT> _lowLevelRendererMesh;

		private:
			CB_Transform _cbTransformData;
			Vector<SB_Material> _sbMaterialDatas;
			GraphicObjectID _inputLayoutDefaultID;
			GraphicObjectID _vsDefaultID;
			GraphicObjectID _psDefaultID;
			GraphicObjectID _psColorID;
		};
	}
}


#endif // !_MINT_RENDERING_INSTANT_RENDERER_H_

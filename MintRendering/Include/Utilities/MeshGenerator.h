#pragma once


#ifndef _MINT_RENDERING_MESH_GENERATOR_H_
#define _MINT_RENDERING_MESH_GENERATOR_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>

#include <MintRendering/Include/RenderingBaseCommon.h>
#include <MintRendering/Include/MeshData.h>


namespace mint
{
	class Float2;
	class Float4;
	class Float4x4;


	namespace Rendering
	{
		class MeshGenerator abstract final
		{
		public:
			struct BoxParam
			{
				float _width = 1.0f;
				float _height = 1.0f;
				float _depth = 1.0f;
			};

			struct ConeParam
			{
				int16 _sideCount = 16;
				float _radius = 1.0f;
				float _height = 2.0f;
				bool _smooth = false;
			};

			struct CylinderParam
			{
				int16 _sideCount = 16;
				float _radius = 1.0f;
				float _height = 2.0f;
				bool _smooth = false;
			};

			struct RadiusParam
			{
				float _radius = 1.0f;
			};

			struct GeoSphereParam
			{
				float _radius = 1.0f;
				uint8 _subdivisionIteration = 1;
				bool _smooth = false;
			};

			struct SphereParam
			{
				float _radius = 1.0f;
				uint8 _polarDetail = 4;
				uint8 _azimuthalDetail = 8;
				bool _smooth = false;
			};

			struct CapsulePram
			{
				float _sphereRadius = 1.0f;
				float _cylinderHeight = 1.0f;
				uint8 _subdivisionIteration = 1;
				bool _smooth = false;
			};

		public:
			static void GenerateTriangle(const Float3(&vertices)[3], const Float2(&uvs)[3], MeshData& meshData) noexcept;
			static void GenerateQuad(const Float3(&vertices)[4], const Float2(&uvs)[4], MeshData& meshData) noexcept;
			static void GenerateBox(const BoxParam& boxParam, MeshData& meshData) noexcept;
			static void GenerateCone(const ConeParam& coneParam, MeshData& meshData) noexcept;
			static void GenerateCylinder(const CylinderParam& cylinderParam, MeshData& meshData) noexcept;
			static void GenerateSphere(const SphereParam& sphereParam, MeshData& meshData) noexcept;
			static void GenerateOctahedron(const RadiusParam& radiusParam, MeshData& meshData) noexcept;
			static void GenerateHalfOpenOctahedron(const RadiusParam& radiusParam, MeshData& meshData) noexcept;
			static void GenerateGeoSphere(const GeoSphereParam& geoSphereParam, MeshData& meshData) noexcept;
			static void GenerateHalfOpenGeoSphere(const GeoSphereParam& geoSphereParam, MeshData& meshData) noexcept;
			static void GenerateCapsule(const CapsulePram& capsulePram, MeshData& meshData) noexcept;

		private:
			static void _PushCirclularPoints(const float radius, const float y, const uint32 sideCount, MeshData& meshData) noexcept;
			static void _PushUpperUmbrellaTris(const int32 centerIndex, const int32 indexBase, const uint8 triangleCount, MeshData& meshData) noexcept;
			static void _PushLowerUmbrellaTris(const int32 centerIndex, const int32 indexBase, const uint8 triangleCount, MeshData& meshData) noexcept;
			static void _PushRingQuads(const int32 indexBase, const uint8 quadCount, MeshData& meshData) noexcept;

		public:
			static void TransformMeshData(MeshData& meshData, const Float4x4& transformationMatrix) noexcept;
			static void MergeMeshData(const MeshData& meshDataA, const MeshData& meshDataB, MeshData& outMeshData) noexcept;
			static void MergeMeshData(const MeshData& sourceMeshData, MeshData& inoutTargetMeshData) noexcept;

		private:
			static void PushPosition(const float(&xyz)[3], MeshData& meshData) noexcept;
			static void PushPosition(const Float3& xyz, MeshData& meshData) noexcept;
			static void PushPosition(const Float4& xyzw, MeshData& meshData) noexcept;
			static void PushTri(const int32(&positionIndices)[3], MeshData& meshData, const Float2(&uvs)[3]) noexcept;
			static void PushQuad(const int32(&positionIndices)[4], MeshData& meshData, const Float2(&uvs)[4]) noexcept;

		private:
			static void SubdivideTriByMidpoints(MeshData& oldMeshData) noexcept;
			static void ProjectVerticesToSphere(const RadiusParam& radiusParam, MeshData& meshData) noexcept;

		private:
			static void PushVertexWithPositionXXX(const uint32 positionIndex, MeshData& meshData) noexcept;
			static void SetVertexUV(MeshData& meshData, const uint32 vertexIndex, const float u, const float v) noexcept;
			static void SetVertexUV(VS_INPUT& vertex, const Float2& uv) noexcept;
			static Float2 GetVertexUV(const VS_INPUT& inoutVertex) noexcept;
			static VS_INPUT& GetFaceVertex0(const Face& face, MeshData& meshData) noexcept;
			static VS_INPUT& GetFaceVertex1(const Face& face, MeshData& meshData) noexcept;
			static VS_INPUT& GetFaceVertex2(const Face& face, MeshData& meshData) noexcept;
			static uint32 GetFaceVertexPositionIndex0(const Face& face, MeshData& meshData) noexcept;
			static uint32 GetFaceVertexPositionIndex1(const Face& face, MeshData& meshData) noexcept;
			static uint32 GetFaceVertexPositionIndex2(const Face& face, MeshData& meshData) noexcept;
			static Float4& GetFaceVertexPosition0(const Face& face, MeshData& meshData) noexcept;
			static Float4& GetFaceVertexPosition1(const Face& face, MeshData& meshData) noexcept;
			static Float4& GetFaceVertexPosition2(const Face& face, MeshData& meshData) noexcept;
			static void PushTriFaceXXX(const uint32 vertexOffset, MeshData& meshData) noexcept;
			static void PushQuadFaceXXX(const uint32 vertexOffset, MeshData& meshData) noexcept;
			static void RecomputeTangentBitangentFromNormal(const Float4& normal, VS_INPUT& vertex) noexcept;
			static Float4 ComputeNormalFromTangentBitangent(const VS_INPUT& vertex) noexcept;

		public:
			static void ComputeTangentBitangent(const Face& face, Vector<VS_INPUT>& inoutVertexArray) noexcept;
			static void SmoothNormals(MeshData& meshData) noexcept;
		};
	}
}


#endif // !_MINT_RENDERING_MESH_GENERATOR_H_

#include <MintRendering/Include/Utilities/MeshGenerator.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintMath/Include/Float2x2.h>

#include <Assets/Include/CppHlsl/CppHlslStreamData.h>


namespace mint
{
	namespace Rendering
	{
		MINT_INLINE void MeshGenerator::PushVertexWithPositionXXX(const uint32 positionIndex, MeshData& meshData) noexcept
		{
			VS_INPUT vertex;
			vertex._positionU = meshData._positionArray[positionIndex];
			meshData._vertexArray.PushBack(vertex);
			meshData._vertexToPositionTable.PushBack(positionIndex);
		}

		MINT_INLINE void MeshGenerator::SetVertexUV(MeshData& meshData, const uint32 vertexIndex, const float u, const float v) noexcept
		{
			meshData._vertexArray[vertexIndex]._positionU._w = u;
			meshData._vertexArray[vertexIndex]._tangentV._w = v;
		}

		MINT_INLINE void MeshGenerator::SetVertexUV(VS_INPUT& vertex, const Float2& uv) noexcept
		{
			vertex._positionU._w = uv._x;
			vertex._tangentV._w = uv._y;
		}

		MINT_INLINE Float2 MeshGenerator::GetVertexUV(const VS_INPUT& inoutVertex) noexcept
		{
			return Float2(inoutVertex._positionU._w, inoutVertex._tangentV._w);
		}

		MINT_INLINE VS_INPUT& MeshGenerator::GetFaceVertex0(const Face& face, MeshData& meshData) noexcept
		{
			return meshData._vertexArray[face._vertexIndexArray[0]];
		}

		MINT_INLINE VS_INPUT& MeshGenerator::GetFaceVertex1(const Face& face, MeshData& meshData) noexcept
		{
			return meshData._vertexArray[face._vertexIndexArray[1]];
		}

		MINT_INLINE VS_INPUT& MeshGenerator::GetFaceVertex2(const Face& face, MeshData& meshData) noexcept
		{
			return meshData._vertexArray[face._vertexIndexArray[2]];
		}

		MINT_INLINE uint32 MeshGenerator::GetFaceVertexPositionIndex0(const Face& face, MeshData& meshData) noexcept
		{
			return meshData._vertexToPositionTable[face._vertexIndexArray[0]];
		}

		MINT_INLINE uint32 MeshGenerator::GetFaceVertexPositionIndex1(const Face& face, MeshData& meshData) noexcept
		{
			return meshData._vertexToPositionTable[face._vertexIndexArray[1]];
		}

		MINT_INLINE uint32 MeshGenerator::GetFaceVertexPositionIndex2(const Face& face, MeshData& meshData) noexcept
		{
			return meshData._vertexToPositionTable[face._vertexIndexArray[2]];
		}

		MINT_INLINE Float4& MeshGenerator::GetFaceVertexPosition0(const Face& face, MeshData& meshData) noexcept
		{
			return meshData._positionArray[GetFaceVertexPositionIndex0(face, meshData)];
		}

		MINT_INLINE Float4& MeshGenerator::GetFaceVertexPosition1(const Face& face, MeshData& meshData) noexcept
		{
			return meshData._positionArray[GetFaceVertexPositionIndex1(face, meshData)];
		}

		MINT_INLINE Float4& MeshGenerator::GetFaceVertexPosition2(const Face& face, MeshData& meshData) noexcept
		{
			return meshData._positionArray[GetFaceVertexPositionIndex2(face, meshData)];
		}

		void MeshGenerator::PushTriFaceXXX(const uint32 vertexOffset, MeshData& meshData) noexcept
		{
			// Vertex winding 에 상관없이 항상 이 순서.
			Face face;
			face._vertexIndexArray[0] = vertexOffset + 0;
			face._vertexIndexArray[1] = vertexOffset + 1;
			face._vertexIndexArray[2] = vertexOffset + 2;
			ComputeTangentBitangent(face, meshData._vertexArray);
			meshData._faceArray.PushBack(face);
		}

		void MeshGenerator::PushQuadFaceXXX(const uint32 vertexOffset, MeshData& meshData) noexcept
		{
			// Vertex winding 에 상관없이 항상 이 순서.
			Face face;
			face._vertexIndexArray[0] = vertexOffset + 0;
			face._vertexIndexArray[1] = vertexOffset + 1;
			face._vertexIndexArray[2] = vertexOffset + 2;
			ComputeTangentBitangent(face, meshData._vertexArray);
			meshData._faceArray.PushBack(face);

			face._vertexIndexArray[0] = vertexOffset + 0;
			face._vertexIndexArray[1] = vertexOffset + 2;
			face._vertexIndexArray[2] = vertexOffset + 3;
			ComputeTangentBitangent(face, meshData._vertexArray);
			meshData._faceArray.PushBack(face);
		}

		void MeshGenerator::RecomputeTangentBitangentFromNormal(const Float4& normal, VS_INPUT& vertex) noexcept
		{
			// TODO: TexCoord._w computation
			const Float2 uv = GetVertexUV(vertex);
			vertex._tangentV._w = 0.0f;
			vertex._bitangentW = Float4::CrossAndNormalize(normal, vertex._tangentV);
			vertex._tangentV = Float4::CrossAndNormalize(vertex._bitangentW, normal);
			SetVertexUV(vertex, uv);
		}

		MINT_INLINE Float4 MeshGenerator::ComputeNormalFromTangentBitangent(const VS_INPUT& vertex) noexcept
		{
			return Float4::CrossAndNormalize(vertex._tangentV, vertex._bitangentW);
		}

		void MeshGenerator::ComputeTangentBitangent(const Face& face, Vector<VS_INPUT>& inoutVertexArray) noexcept
		{
			VS_INPUT& v0 = inoutVertexArray[face._vertexIndexArray[0]];
			VS_INPUT& v1 = inoutVertexArray[face._vertexIndexArray[1]];
			VS_INPUT& v2 = inoutVertexArray[face._vertexIndexArray[2]];

			const Float4 edgeA = v1._positionU.GetXYZ0() - v0._positionU.GetXYZ0();
			const Float4 edgeB = v2._positionU.GetXYZ0() - v0._positionU.GetXYZ0();

			const Float2 uv0 = GetVertexUV(v0);
			const Float2 uv1 = GetVertexUV(v1);
			const Float2 uv2 = GetVertexUV(v2);
			const Float2 uvEdgeA = uv1 - uv0;
			const Float2 uvEdgeB = uv2 - uv0;

			// T = tangent
			// B = bitangent
			//
			// edgeA = uvEdgeA.u0 * T + uvEdgeA.v0 * B
			// edgeB = uvEdgeB.u1 * T + uvEdgeB.v1 * B
			//
			// | edgeA | = | u0 v0 | * | T |
			// | edgeB |   | u1 v1 |   | B |
			//
			// | u0 v0 | ^(-1) * | edgeA | = | T |
			// | u1 v1 |         | edgeB |   | B |

			const Float2x2 uvMatrix{ uvEdgeA._x, uvEdgeA._y, uvEdgeB._x, uvEdgeB._y };
			Float4 tangent;
			Float4 bitangent;
			if (uvMatrix.isInvertible() == false)
			{
				MINT_ASSERT(false, "uvMatrix 가 Invertible 하지 않습니다!!!");

				tangent.SetXYZ(+1.0f, 0.0f, 0.0f);
				bitangent.SetXYZ(0.0f, 0.0f, -1.0f);
			}
			else
			{
				const Float2x2 uvMatrixInverse = uvMatrix.Inverse();

				tangent = edgeA * uvMatrixInverse._11 + edgeB * uvMatrixInverse._12;
				tangent.Normalize();

				bitangent = edgeA * uvMatrixInverse._21 + edgeB * uvMatrixInverse._22;
				bitangent.Normalize();
			}

			v0._tangentV.SetXYZ(tangent);
			v1._tangentV.SetXYZ(tangent);
			v2._tangentV.SetXYZ(tangent);

			v0._bitangentW.SetXYZ(bitangent);
			v1._bitangentW.SetXYZ(bitangent);
			v2._bitangentW.SetXYZ(bitangent);
		}

		void MeshGenerator::SmoothNormals(MeshData& meshData) noexcept
		{
			const uint32 vertexCount = meshData.GetVertexCount();
			const uint32 faceCount = meshData.GetFaceCount();
			const uint32 positionCount = meshData.GetPositionCount();
			Vector<Float4> normalArray(positionCount);
			for (uint32 faceIndex = 0; faceIndex < faceCount; ++faceIndex)
			{
				const uint16 v0Index = meshData._faceArray[faceIndex]._vertexIndexArray[0];
				const uint16 v1Index = meshData._faceArray[faceIndex]._vertexIndexArray[1];
				const uint16 v2Index = meshData._faceArray[faceIndex]._vertexIndexArray[2];
				const Float4 v0Normal = ComputeNormalFromTangentBitangent(meshData._vertexArray[v0Index]);
				const Float4 v1Normal = ComputeNormalFromTangentBitangent(meshData._vertexArray[v1Index]);
				const Float4 v2Normal = ComputeNormalFromTangentBitangent(meshData._vertexArray[v2Index]);

				normalArray[meshData._vertexToPositionTable[v0Index]] += v0Normal;
				normalArray[meshData._vertexToPositionTable[v0Index]]._w += 1.0f;
				normalArray[meshData._vertexToPositionTable[v1Index]] += v1Normal;
				normalArray[meshData._vertexToPositionTable[v1Index]]._w += 1.0f;
				normalArray[meshData._vertexToPositionTable[v2Index]] += v2Normal;
				normalArray[meshData._vertexToPositionTable[v2Index]]._w += 1.0f;
			}

			// Average normals
			for (uint32 positionIndex = 0; positionIndex < positionCount; ++positionIndex)
			{
				normalArray[positionIndex] /= normalArray[positionIndex]._w;
				normalArray[positionIndex]._w = 0.0f;
				normalArray[positionIndex].Normalize();
			}

			// Recompute tangent, bitangent
			for (uint32 vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
			{
				const Float4& normal = normalArray[meshData._vertexToPositionTable[vertexIndex]];

				const Float4 tangent = Float4::CrossAndNormalize(meshData._vertexArray[vertexIndex]._bitangentW, normal);
				const Float4 bitangent = Float4::CrossAndNormalize(normal, tangent);

				meshData._vertexArray[vertexIndex]._tangentV = tangent;
				meshData._vertexArray[vertexIndex]._bitangentW = bitangent;
			}
			normalArray.Clear();
		}

		void MeshGenerator::GenerateTriangle(const Float3(&vertices)[3], const Float2(&uvs)[3], MeshData& meshData) noexcept
		{
			meshData.Clear();

			meshData._positionArray.Reserve(3);

			PushPosition(vertices[0], meshData);
			PushPosition(vertices[1], meshData);
			PushPosition(vertices[2], meshData);

			PushTri({ 0, 1, 2 }, meshData, uvs);
		}

		void MeshGenerator::GenerateQuad(const Float3(&vertices)[4], const Float2(&uvs)[4], MeshData& meshData) noexcept
		{
			meshData.Clear();

			meshData._positionArray.Reserve(4);

			PushPosition(vertices[0], meshData);
			PushPosition(vertices[1], meshData);
			PushPosition(vertices[2], meshData);
			PushPosition(vertices[3], meshData);

			PushQuad({ 0, 1, 2, 3 }, meshData, uvs);
		}

		void MeshGenerator::GenerateBox(const BoxParam& boxParam, MeshData& meshData) noexcept
		{
			meshData.Clear();

			// Vertex positions
			meshData._positionArray.Reserve(8);
			const Float3 halfExtents{ boxParam._width * 0.5f, boxParam._height * 0.5f, boxParam._depth * 0.5f };
			PushPosition({ -halfExtents._x, +halfExtents._y, -halfExtents._z }, meshData);
			PushPosition({ -halfExtents._x, +halfExtents._y, +halfExtents._z }, meshData);
			PushPosition({ +halfExtents._x, +halfExtents._y, +halfExtents._z }, meshData);
			PushPosition({ +halfExtents._x, +halfExtents._y, -halfExtents._z }, meshData);

			PushPosition({ -halfExtents._x, -halfExtents._y, -halfExtents._z }, meshData);
			PushPosition({ -halfExtents._x, -halfExtents._y, +halfExtents._z }, meshData);
			PushPosition({ +halfExtents._x, -halfExtents._y, +halfExtents._z }, meshData);
			PushPosition({ +halfExtents._x, -halfExtents._y, -halfExtents._z }, meshData);

			const Float2 uvs[4]{ Float2(0.0f, 0.0f), Float2(1.0f, 0.0f), Float2(1.0f, 1.0f), Float2(0.0f, 1.0f) };
			PushQuad({ 0, 1, 2, 3 }, meshData, uvs); // Top
			PushQuad({ 7, 6, 5, 4 }, meshData, uvs); // Bottom
			_PushRingQuads(0, 4, meshData);
		}

		void MeshGenerator::GenerateCone(const ConeParam& coneParam, MeshData& meshData) noexcept
		{
			meshData.Clear();

			// Position
			{
				meshData._positionArray.Reserve(coneParam._sideCount + 2);

				PushPosition({ 0.0f, coneParam._height, 0.0f }, meshData);

				_PushCirclularPoints(coneParam._radius, 0.0f, coneParam._sideCount, meshData);

				PushPosition({ 0.0f, 0.0f, 0.0f }, meshData);
			}

			const int32 positionIndexTopCenter = 0;
			const int32 positionIndexBottomCenter = static_cast<int32>(meshData._positionArray.Size() - 1);

			// Cone sides
			_PushUpperUmbrellaTris(positionIndexTopCenter, 1, static_cast<uint8>(coneParam._sideCount), meshData);
			if (coneParam._smooth == true)
			{
				SmoothNormals(meshData);
			}

			// Cone bottom
			_PushLowerUmbrellaTris(positionIndexBottomCenter, 1, static_cast<uint8>(coneParam._sideCount), meshData);
		}

		void MeshGenerator::GenerateCylinder(const CylinderParam& cylinderParam, MeshData& meshData) noexcept
		{
			meshData.Clear();

			// Position
			{
				meshData._positionArray.Reserve(cylinderParam._sideCount * 2 + 2);

				PushPosition({ 0.0f, cylinderParam._height, 0.0f }, meshData);

				_PushCirclularPoints(cylinderParam._radius, cylinderParam._height, cylinderParam._sideCount, meshData);
				_PushCirclularPoints(cylinderParam._radius, 0.0f, cylinderParam._sideCount, meshData);

				PushPosition({ 0.0f, 0.0f, 0.0f }, meshData);
			}

			const int32 positionIndexTopCenter = 0;
			const int32 positionIndexBottomCenter = static_cast<int32>(meshData._positionArray.Size() - 1);

			// Clylinder sides
			_PushRingQuads(1, static_cast<uint8>(cylinderParam._sideCount), meshData);
			if (cylinderParam._smooth == true)
			{
				SmoothNormals(meshData);
			}

			// Clylinder top
			_PushUpperUmbrellaTris(positionIndexTopCenter, 1, static_cast<uint8>(cylinderParam._sideCount), meshData);

			// Clylinder bottom
			_PushLowerUmbrellaTris(positionIndexBottomCenter, 1 + cylinderParam._sideCount, static_cast<uint8>(cylinderParam._sideCount), meshData);
		}

		void MeshGenerator::GenerateSphere(const SphereParam& sphereParam, MeshData& meshData) noexcept
		{
			meshData.Clear();

			const float radius = sphereParam._radius;
			const uint8 polarDetail = sphereParam._polarDetail;
			const uint8 azimuthalDetail = sphereParam._azimuthalDetail;

			// polarAngle: 0 at +Y, Pi at -Y
			// azimuthalAngle: 0 at +X, rotates around +Y axis (counter-clockwise) all the way around back to +X again
			const float polarAngleStep = Math::kPi / polarDetail;

			// Position
			meshData._positionArray.Reserve(1 + (polarDetail - 1) * azimuthalDetail + 1);
			for (uint8 polarIter = 0; polarIter < polarDetail + 1; ++polarIter)
			{
				const float polarAngle = polarAngleStep * polarIter;
				_PushCirclularPoints(radius * ::sin(polarAngle), radius * ::cos(polarAngle), azimuthalDetail, meshData);
			}

			// Tris and quads
			for (uint8 polarIter = 0; polarIter < polarDetail; ++polarIter)
			{
				const int32 indexBase = 1 + Max(polarIter - 1, 0) * azimuthalDetail;
				if (polarIter == 0)
				{
					// Top center included
					_PushUpperUmbrellaTris(0, indexBase, azimuthalDetail, meshData);
				}
				else if (polarIter == polarDetail - 1)
				{
					// Bottom center included
					const int32 bottomCenterIndex = static_cast<int32>(meshData.GetPositionCount() - 1);
					_PushLowerUmbrellaTris(bottomCenterIndex, indexBase, azimuthalDetail, meshData);
				}
				else
				{
					// Middle quads
					_PushRingQuads(indexBase, azimuthalDetail, meshData);
				}
			}

			if (sphereParam._smooth == true)
			{
				SmoothNormals(meshData);
			}
		}

		void MeshGenerator::GenerateOctahedron(const RadiusParam& radiusParam, MeshData& meshData) noexcept
		{
			meshData.Clear();

			// Position
			{
				meshData._positionArray.Reserve(6);
				PushPosition({ 0.0f, +radiusParam._radius, 0.0f }, meshData);

				_PushCirclularPoints(radiusParam._radius, 0.0f, 4, meshData);

				PushPosition({ 0.0f, -radiusParam._radius, 0.0f }, meshData);
			}

			const int32 positionIndexTopCenter = 0;
			const int32 positionIndexBottomCenter = static_cast<int32>(meshData._positionArray.Size() - 1);
			_PushUpperUmbrellaTris(positionIndexTopCenter, 1, 4, meshData);
			_PushLowerUmbrellaTris(positionIndexBottomCenter, 1, 4, meshData);
		}

		void MeshGenerator::GenerateHalfOpenOctahedron(const RadiusParam& radiusParam, MeshData& meshData) noexcept
		{
			meshData.Clear();

			// Position
			{
				meshData._positionArray.Reserve(6);
				PushPosition({ 0.0f, +radiusParam._radius, 0.0f }, meshData);

				_PushCirclularPoints(radiusParam._radius, 0.0f, 4, meshData);
			}

			const int32 positionIndexTopCenter = 0;
			_PushUpperUmbrellaTris(positionIndexTopCenter, 1, 4, meshData);
		}

		void MeshGenerator::GenerateGeoSphere(const GeoSphereParam& geoSphereParam, MeshData& meshData) noexcept
		{
			RadiusParam radiusParam;
			radiusParam._radius = geoSphereParam._radius;

			GenerateOctahedron(radiusParam, meshData);

			SubdivideTriByMidpoints(meshData); // At least once
			for (int8 iteration = 0; iteration < geoSphereParam._subdivisionIteration; ++iteration)
			{
				SubdivideTriByMidpoints(meshData);
			}

			ProjectVerticesToSphere(radiusParam, meshData);

			if (geoSphereParam._smooth == true)
			{
				SmoothNormals(meshData);
			}
		}

		void MeshGenerator::GenerateHalfOpenGeoSphere(const GeoSphereParam& geoSphereParam, MeshData& meshData) noexcept
		{
			RadiusParam radiusParam;
			radiusParam._radius = geoSphereParam._radius;

			GenerateHalfOpenOctahedron(radiusParam, meshData);

			SubdivideTriByMidpoints(meshData); // At least once
			for (int8 iteration = 0; iteration < geoSphereParam._subdivisionIteration; ++iteration)
			{
				SubdivideTriByMidpoints(meshData);
			}

			ProjectVerticesToSphere(radiusParam, meshData);

			if (geoSphereParam._smooth == true)
			{
				SmoothNormals(meshData);
			}
		}

		void MeshGenerator::GenerateCapsule(const CapsulePram& capsulePram, MeshData& meshData) noexcept
		{
			meshData.Clear();

			const float halfHeight = capsulePram._cylinderHeight * 0.5f;
			const float radius = capsulePram._sphereRadius;

			const uint8 polarDetail = ((4 + capsulePram._subdivisionIteration) / 2) * 2; // e.g.) 4, 6, 8, 10, ...
			const uint8 halfPolarDetail = polarDetail / 2;
			const uint8 azimuthalDetail = polarDetail * 2;

			// polarAngle: 0 at +Y, Pi at -Y
		   // azimuthalAngle: 0 at +X, rotates around +Y axis (counter-clockwise) all the way around back to +X again
			const float polarAngleStep = Math::kPi / polarDetail;

			// Position
			{
				meshData._positionArray.Reserve(1 + (polarDetail - 1) * azimuthalDetail + 1);

				// Upper half-sphere
				for (uint8 polarIter = 0; polarIter < halfPolarDetail + 1; ++polarIter)
				{
					const float polarAngle = polarAngleStep * polarIter;
					_PushCirclularPoints(radius * ::sin(polarAngle), radius * ::cos(polarAngle) + halfHeight, azimuthalDetail, meshData);
				}

				// Lower half-sphere
				for (uint8 polarIter = 0; polarIter < halfPolarDetail + 1; ++polarIter)
				{
					const float polarAngle = polarAngleStep * (polarIter + halfPolarDetail);
					_PushCirclularPoints(radius * ::sin(polarAngle), radius * ::cos(polarAngle) - halfHeight, azimuthalDetail, meshData);
				}
			}

			// Tris and quads
			for (uint8 polarIter = 0; polarIter < polarDetail + 1; ++polarIter)
			{
				const int32 indexBase = 1 + Max(polarIter - 1, 0) * azimuthalDetail;
				if (polarIter == 0)
				{
					// Top center included
					_PushUpperUmbrellaTris(0, indexBase, azimuthalDetail, meshData);
				}
				else if (polarIter == polarDetail)
				{
					// Bottom center included
					const int32 bottomCenterIndex = static_cast<int32>(meshData.GetPositionCount() - 1);
					_PushLowerUmbrellaTris(bottomCenterIndex, indexBase, azimuthalDetail, meshData);
				}
				else
				{
					// Middle quads
					_PushRingQuads(indexBase, azimuthalDetail, meshData);
				}
			}

			if (capsulePram._smooth == true)
			{
				SmoothNormals(meshData);
			}
		}

		void MeshGenerator::_PushCirclularPoints(const float radius, const float y, const uint32 sideCount, MeshData& meshData) noexcept
		{
			if (radius < Math::kFloatEpsilon)
			{
				PushPosition({ 0.0f, y, 0.0f }, meshData);
				return;
			}

			const float angleStep = Math::kTwoPi / sideCount;
			for (uint8 sideIndex = 0; sideIndex < sideCount; ++sideIndex)
			{
				const float angle = angleStep * sideIndex;
				PushPosition({ ::cos(angle) * radius, y, -::sin(angle) * radius }, meshData);
			}
		}

		void MeshGenerator::_PushUpperUmbrellaTris(const int32 centerIndex, const int32 indexBase, const uint8 triangleCount, MeshData& meshData) noexcept
		{
			// 예시) triCount = 8
			//
			//    VERTEX INPUT   ||       TRI[0]               TRI[1]               TRI[2]            
			//                   ||                                                                   
			//         5         ||         .                    .                    .               
			//    6         4    ||    .         .          .         .          .         4          
			//                   ||                                                                   
			//  7      0      3  ||  .      0      .  =>  .      0      3  =>  .      0      3  => ...
			//                   ||                                                                   
			//    8         2    ||    .         2          .         2          .         .          
			//         1         ||         1                    .                    .               
			//                   ||                                                                   
			//
			const Float2 uvs[3]{ Float2(0.0f, 0.0f), Float2(1.0f, 1.0f), Float2(0.0f, 1.0f) };
			for (uint8 triangleIndex = 0; triangleIndex < triangleCount - 1; ++triangleIndex)
			{
				PushTri({ centerIndex, indexBase + triangleIndex, indexBase + triangleIndex + 1 }, meshData, uvs);
			}
			PushTri({ centerIndex, indexBase + triangleCount - 1,indexBase }, meshData, uvs);
		}

		void MeshGenerator::_PushLowerUmbrellaTris(const int32 centerIndex, const int32 indexBase, const uint8 triangleCount, MeshData& meshData) noexcept
		{
			const Float2 uvs[3]{ Float2(0.0f, 0.0f), Float2(1.0f, 0.0f), Float2(1.0f, 1.0f) };
			for (uint8 triangleIndex = 0; triangleIndex < triangleCount - 1; ++triangleIndex)
			{
				PushTri({ indexBase + triangleIndex, centerIndex, indexBase + triangleIndex + 1 }, meshData, uvs);
			}
			PushTri({ indexBase + triangleCount - 1, centerIndex, indexBase }, meshData, uvs);
		}

		void MeshGenerator::_PushRingQuads(const int32 indexBase, const uint8 quadCount, MeshData& meshData) noexcept
		{
			// 예시) quadCount = 4
			//
			// Vertex 는 아래처럼 윗줄 순서대로 먼저, 그다음 아랫줄 동일한 순서대로 들어와야 하며
			// 반드시 윗줄과 아랫줄의 Vertex 개수가 일치해야 한다!
			//
			// VERTEX INPUT  ||  QUAD[0]      QUAD[1]       QUAD[2]        QUAD[3]
			//               ||                                                   
			//   3 -- 2      ||                     2       3 -- 2        3       
			//  /    /       ||                    /|       |    |       /|       
			// 0 -- 1        ||  0 -- 1           1 |       |    |      0 |       
			//               ||  |    |  =>       | |   =>  |    |  =>  | |       
			//   7 -- 6      ||  |    |           | 6       7 -- 6      | 7       
			//  /    /       ||  |    |           |/                    |/        
			// 4 -- 5        ||  4 -- 5           5                     4         
			//               ||
			//
			const Float2 uvs[4]{ Float2(0.0f, 0.0f), Float2(1.0f, 0.0f), Float2(1.0f, 1.0f), Float2(0.0f, 1.0f) };
			for (uint8 quadIndex = 0; quadIndex < quadCount - 1; ++quadIndex)
			{
				PushQuad(
					{ indexBase + quadIndex,
					  indexBase + quadIndex + quadCount,
					  indexBase + quadIndex + quadCount + 1,
					  indexBase + quadIndex + 1,
					}, meshData, uvs);
			}

			PushQuad(
				{ indexBase + quadCount - 1,
				  indexBase + quadCount - 1 + quadCount,
				  indexBase + quadCount,
				  indexBase,
				}, meshData, uvs);
		}

		void MeshGenerator::SetMaterialID(MeshData& meshData, const uint32 materialID) noexcept
		{
			const uint32 vertexCount = meshData._vertexArray.Size();
			for (uint32 vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
			{
				meshData._vertexArray[vertexIndex]._materialID = materialID;
			}
		}

		void MeshGenerator::TransformMeshData(MeshData& meshData, const Float4x4& transformationMatrix) noexcept
		{
			const uint32 positionCount = meshData.GetPositionCount();
			for (uint32 positionIndex = 0; positionIndex < positionCount; ++positionIndex)
			{
				Float4& position = meshData._positionArray[positionIndex];
				position = transformationMatrix.Mul(position);
			}
			meshData.UpdateVertexFromPositions();
		}

		void MeshGenerator::MergeMeshData(const MeshData& meshDataA, const MeshData& meshDataB, MeshData& outMeshData) noexcept
		{
			MergeMeshData(meshDataA, outMeshData);
			MergeMeshData(meshDataB, outMeshData);
		}

		void MeshGenerator::MergeMeshData(const MeshData& sourceMeshData, MeshData& inoutTargetMeshData) noexcept
		{
			if (inoutTargetMeshData.IsEmpty() == true)
			{
				inoutTargetMeshData = sourceMeshData;
				return;
			}

			const uint32 oldPositionCount = inoutTargetMeshData.GetPositionCount();
			const uint32 deltaPositionCount = sourceMeshData.GetPositionCount();
			inoutTargetMeshData._positionArray.Reserve(inoutTargetMeshData._positionArray.Size() + deltaPositionCount);
			for (uint32 deltaPositionIndex = 0; deltaPositionIndex < deltaPositionCount; ++deltaPositionIndex)
			{
				inoutTargetMeshData._positionArray.PushBack(sourceMeshData._positionArray[deltaPositionIndex]);
			}

			const uint32 oldVertexCount = inoutTargetMeshData.GetVertexCount();
			const uint32 deltaVertexCount = sourceMeshData.GetVertexCount();
			inoutTargetMeshData._vertexArray.Reserve(inoutTargetMeshData._vertexArray.Size() + deltaVertexCount);
			inoutTargetMeshData._vertexToPositionTable.Reserve(inoutTargetMeshData._vertexToPositionTable.Size() + deltaVertexCount);
			for (uint32 deltaVertexIndex = 0; deltaVertexIndex < deltaVertexCount; ++deltaVertexIndex)
			{
				inoutTargetMeshData._vertexArray.PushBack(sourceMeshData._vertexArray[deltaVertexIndex]);
				inoutTargetMeshData._vertexToPositionTable.PushBack(oldPositionCount + sourceMeshData._vertexToPositionTable[deltaVertexIndex]);
			}

			const uint32 deltaFaceCount = sourceMeshData.GetFaceCount();
			inoutTargetMeshData._faceArray.Reserve(inoutTargetMeshData._faceArray.Size() + deltaFaceCount);
			Face face;
			for (uint32 deltaFaceIndex = 0; deltaFaceIndex < deltaFaceCount; ++deltaFaceIndex)
			{
				face._vertexIndexArray[0] = oldVertexCount + sourceMeshData._faceArray[deltaFaceIndex]._vertexIndexArray[0];
				face._vertexIndexArray[1] = oldVertexCount + sourceMeshData._faceArray[deltaFaceIndex]._vertexIndexArray[1];
				face._vertexIndexArray[2] = oldVertexCount + sourceMeshData._faceArray[deltaFaceIndex]._vertexIndexArray[2];

				inoutTargetMeshData._faceArray.PushBack(face);
			}
		}

		MINT_INLINE void MeshGenerator::PushPosition(const float(&xyz)[3], MeshData& meshData) noexcept
		{
			meshData._positionArray.PushBack(Float4(xyz[0], xyz[1], xyz[2], 1.0f));
		}

		MINT_INLINE void MeshGenerator::PushPosition(const Float3& xyz, MeshData& meshData) noexcept
		{
			meshData._positionArray.PushBack(Float4(xyz._x, xyz._y, xyz._z, 1.0f));
		}

		MINT_INLINE void MeshGenerator::PushPosition(const Float4& xyzw, MeshData& meshData) noexcept
		{
			meshData._positionArray.PushBack(xyzw);
		}

		void MeshGenerator::PushTri(const int32(&positionIndices)[3], MeshData& meshData, const Float2(&uvs)[3]) noexcept
		{
			const uint32 vertexCountOld = meshData.GetVertexCount();

			PushVertexWithPositionXXX(positionIndices[0], meshData);
			PushVertexWithPositionXXX(positionIndices[1], meshData);
			PushVertexWithPositionXXX(positionIndices[2], meshData);

			SetVertexUV(meshData, vertexCountOld + 0, uvs[0]._x, uvs[0]._y);
			SetVertexUV(meshData, vertexCountOld + 1, uvs[1]._x, uvs[1]._y);
			SetVertexUV(meshData, vertexCountOld + 2, uvs[2]._x, uvs[2]._y);

			PushTriFaceXXX(vertexCountOld, meshData);
		}

		void MeshGenerator::PushQuad(const int32(&positionIndices)[4], MeshData& meshData, const Float2(&uvs)[4]) noexcept
		{
			const uint32 vertexCountOld = meshData.GetVertexCount();

			PushVertexWithPositionXXX(positionIndices[0], meshData);
			PushVertexWithPositionXXX(positionIndices[1], meshData);
			PushVertexWithPositionXXX(positionIndices[2], meshData);
			PushVertexWithPositionXXX(positionIndices[3], meshData);

			SetVertexUV(meshData, vertexCountOld + 0, uvs[0]._x, uvs[0]._y);
			SetVertexUV(meshData, vertexCountOld + 1, uvs[1]._x, uvs[1]._y);
			SetVertexUV(meshData, vertexCountOld + 2, uvs[2]._x, uvs[2]._y);
			SetVertexUV(meshData, vertexCountOld + 3, uvs[3]._x, uvs[3]._y);

			PushQuadFaceXXX(vertexCountOld, meshData);
		}

		void MeshGenerator::SubdivideTriByMidpoints(MeshData& oldMeshData) noexcept
		{
			MeshData newMeshData;
			newMeshData._positionArray = oldMeshData._positionArray;
			newMeshData._positionArray.Reserve((newMeshData._positionArray.Size() - 2) * 4 + 2);
			newMeshData._vertexArray.Reserve(oldMeshData.GetVertexCount() * 4);
			newMeshData._vertexToPositionTable.Reserve(oldMeshData.GetVertexCount() * 4);
			newMeshData._faceArray.Reserve(oldMeshData.GetFaceCount() * 4);

			struct PositionEdge
			{
			public:
				PositionEdge(const int32 positionIndexA, const int32 positionIndexB)
					: _positionIndexA{ Min(positionIndexA, positionIndexB) }
					, _positionIndexB{ Max(positionIndexA, positionIndexB) }
				{
					__noop;
				}

			public:
				MINT_INLINE int32   key() const noexcept { return _positionIndexA; }
				MINT_INLINE int32   value() const noexcept { return _positionIndexB; }

			private:
				int32               _positionIndexA = -1;
				int32               _positionIndexB = -1;
			};

			class PositionEdgeGraph
			{
			public:
				MINT_INLINE void    setPositionCount(const uint32 positionCount) noexcept
				{
					_positionCount = positionCount;
					_edgeTable.Resize(positionCount * positionCount);
					const uint32 length = _edgeTable.Size();
					for (uint32 index = 0; index < length; ++index)
					{
						_edgeTable[index] = -1;
					}
				}
				MINT_INLINE void    setMidpoint(const PositionEdge& edge, const int32 midpointPositionIndex) noexcept
				{
					_edgeTable[getIndexFromEdge(edge)] = midpointPositionIndex;
				}
				MINT_INLINE bool    hasMidpoint(const PositionEdge& edge) const noexcept
				{
					return (_edgeTable[getIndexFromEdge(edge)] >= 0);
				}
				MINT_INLINE int32   getMidpointPositionIndex(const PositionEdge& edge) const noexcept
				{
					return _edgeTable[getIndexFromEdge(edge)];
				}

			private:
				MINT_INLINE uint32  getIndexFromEdge(const PositionEdge& edge) const noexcept
				{
					return edge.key() * _positionCount + edge.value();
				}

			private:
				int32           _positionCount = 0;
				Vector<int32>   _edgeTable;
			};

			PositionEdgeGraph positionEdgeGraph;
			positionEdgeGraph.setPositionCount(oldMeshData.GetPositionCount());
			const uint32 oldFaceCount = static_cast<uint32>(oldMeshData._faceArray.Size());
			for (uint32 oldFaceIndex = 0; oldFaceIndex < oldFaceCount; ++oldFaceIndex)
			{
				const Face& face = oldMeshData._faceArray[oldFaceIndex];
				const int32 faceVertexPositionIndices[3]
				{
					static_cast<int32>(GetFaceVertexPositionIndex0(face, oldMeshData)),
					static_cast<int32>(GetFaceVertexPositionIndex1(face, oldMeshData)),
					static_cast<int32>(GetFaceVertexPositionIndex2(face, oldMeshData)),
				};

				int32 midpointPositionIndex01 = 0;
				int32 midpointPositionIndex12 = 0;
				int32 midpointPositionIndex20 = 0;
				{
					const PositionEdge positionEdge01 = PositionEdge(faceVertexPositionIndices[0], faceVertexPositionIndices[1]);
					const PositionEdge positionEdge12 = PositionEdge(faceVertexPositionIndices[1], faceVertexPositionIndices[2]);
					const PositionEdge positionEdge20 = PositionEdge(faceVertexPositionIndices[2], faceVertexPositionIndices[0]);

					const Float4& faceVertexPosition0 = GetFaceVertexPosition0(face, oldMeshData);
					const Float4& faceVertexPosition1 = GetFaceVertexPosition1(face, oldMeshData);
					const Float4& faceVertexPosition2 = GetFaceVertexPosition2(face, oldMeshData);

					const int32 newPositionIndexBase = static_cast<int32>(newMeshData._positionArray.Size());
					int8 addedPointCount = 0;
					// Midpoint 0-1
					if (positionEdgeGraph.hasMidpoint(positionEdge01) == false)
					{
						Float4 midPointPosition_01 = (faceVertexPosition0 + faceVertexPosition1) * 0.5f;
						PushPosition(midPointPosition_01, newMeshData);

						midpointPositionIndex01 = newPositionIndexBase + addedPointCount;
						++addedPointCount;
						positionEdgeGraph.setMidpoint(positionEdge01, midpointPositionIndex01);
					}
					else
					{
						midpointPositionIndex01 = positionEdgeGraph.getMidpointPositionIndex(positionEdge01);
					}

					// Midpoint 1-2
					if (positionEdgeGraph.hasMidpoint(positionEdge12) == false)
					{
						Float4 midPointPosition_12 = (faceVertexPosition1 + faceVertexPosition2) * 0.5f;
						PushPosition(midPointPosition_12, newMeshData);

						midpointPositionIndex12 = newPositionIndexBase + addedPointCount;
						++addedPointCount;
						positionEdgeGraph.setMidpoint(positionEdge12, midpointPositionIndex12);
					}
					else
					{
						midpointPositionIndex12 = positionEdgeGraph.getMidpointPositionIndex(positionEdge12);
					}

					// Midpoint 2-0
					if (positionEdgeGraph.hasMidpoint(positionEdge20) == false)
					{
						Float4 midPointPosition_20 = (faceVertexPosition2 + faceVertexPosition0) * 0.5f;
						PushPosition(midPointPosition_20, newMeshData);

						midpointPositionIndex20 = newPositionIndexBase + addedPointCount;
						++addedPointCount;
						positionEdgeGraph.setMidpoint(positionEdge20, midpointPositionIndex20);
					}
					else
					{
						midpointPositionIndex20 = positionEdgeGraph.getMidpointPositionIndex(positionEdge20);
					}
				}

				// UV
				const Float2 faceVertexUvs[3]
				{
					GetVertexUV(GetFaceVertex0(face, oldMeshData)),
					GetVertexUV(GetFaceVertex1(face, oldMeshData)),
					GetVertexUV(GetFaceVertex2(face, oldMeshData))
				};
				const Float2 midPointUv01 = (faceVertexUvs[0] + faceVertexUvs[1]) * 0.5f;
				const Float2 midPointUv12 = (faceVertexUvs[1] + faceVertexUvs[2]) * 0.5f;
				const Float2 midPointUv20 = (faceVertexUvs[2] + faceVertexUvs[0]) * 0.5f;

				PushTri({ faceVertexPositionIndices[0], midpointPositionIndex01     , midpointPositionIndex20 }, newMeshData, { faceVertexUvs[0], midPointUv01, midPointUv20 });
				PushTri({ midpointPositionIndex01     , faceVertexPositionIndices[1], midpointPositionIndex12 }, newMeshData, { midPointUv01, faceVertexUvs[1], midPointUv12 });
				PushTri({ midpointPositionIndex12     , faceVertexPositionIndices[2], midpointPositionIndex20 }, newMeshData, { midPointUv12, faceVertexUvs[2], midPointUv20 });
				PushTri({ midpointPositionIndex01     , midpointPositionIndex12     , midpointPositionIndex20 }, newMeshData, { midPointUv01, midPointUv12, midPointUv20 });
			}

			newMeshData.ShrinkToFit();

			std::swap(oldMeshData, newMeshData);
		}

		void MeshGenerator::ProjectVerticesToSphere(const RadiusParam& radiusParam, MeshData& meshData) noexcept
		{
			Float4 vertexNormals[3];
			Float4 faceNormal;
			const uint32 faceCount = meshData.GetFaceCount();
			for (uint32 faceIndex = 0; faceIndex < faceCount; ++faceIndex)
			{
				const Face& face = meshData._faceArray[faceIndex];

				Float4& faceVertexPosition0 = GetFaceVertexPosition0(face, meshData);
				Float4& faceVertexPosition1 = GetFaceVertexPosition1(face, meshData);
				Float4& faceVertexPosition2 = GetFaceVertexPosition2(face, meshData);

				faceVertexPosition0._w = 0.0f;
				faceVertexPosition0.Normalize();
				vertexNormals[0] = faceVertexPosition0;
				faceVertexPosition0 *= radiusParam._radius;
				faceVertexPosition0._w = 1.0f;

				faceVertexPosition1._w = 0.0f;
				faceVertexPosition1.Normalize();
				vertexNormals[1] = faceVertexPosition1;
				faceVertexPosition1 *= radiusParam._radius;
				faceVertexPosition1._w = 1.0f;

				faceVertexPosition2._w = 0.0f;
				faceVertexPosition2.Normalize();
				vertexNormals[2] = faceVertexPosition2;
				faceVertexPosition2 *= radiusParam._radius;
				faceVertexPosition2._w = 1.0f;

				faceNormal = vertexNormals[0] + vertexNormals[1] + vertexNormals[2];
				faceNormal.Normalize();

				RecomputeTangentBitangentFromNormal(faceNormal, GetFaceVertex0(face, meshData));
				RecomputeTangentBitangentFromNormal(faceNormal, GetFaceVertex1(face, meshData));
				RecomputeTangentBitangentFromNormal(faceNormal, GetFaceVertex2(face, meshData));
			}

			meshData.UpdateVertexFromPositions();
		}
	}
}

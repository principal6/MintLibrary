#include <stdafx.h>
#include <FsRendering/Include/MeshComponent.h>

#include <FsRenderingBase/Include/DxResource.h>

#include <FsMath/Include/Float2x2.h>

#include <FsLibrary/Assets/CppHlsl/CppHlslStreamData.h>


namespace fs
{
	namespace Rendering
	{
		static void pushVertexWithPosition(const uint32 positionIndex, MeshComponent::MeshData& meshData)
		{
			fs::RenderingBase::VS_INPUT vertex;
			vertex._positionU = meshData._positionArray[positionIndex];
			meshData._vertexArray.emplace_back(vertex);
			meshData._vertexToPositionTable.emplace_back(positionIndex);
		}

		static void setVertexUv(MeshComponent::MeshData& meshData, const uint32 vertexIndex, const float u, const float v)
		{
			meshData._vertexArray[vertexIndex]._positionU._w = u;
			meshData._vertexArray[vertexIndex]._tangentV._w = v;
		}

		static fs::Float2 getVertexUv(const fs::RenderingBase::VS_INPUT& inoutVertex)
		{
			return fs::Float2(inoutVertex._positionU._w, inoutVertex._tangentV._w);
		}

		static void calculateTangentBitangent(const fs::RenderingBase::Face& face, std::vector<fs::RenderingBase::VS_INPUT>& inoutVertexArray)
		{
			fs::RenderingBase::VS_INPUT& v0 = inoutVertexArray[face._indexArray[0]];
			fs::RenderingBase::VS_INPUT& v1 = inoutVertexArray[face._indexArray[1]];
			fs::RenderingBase::VS_INPUT& v2 = inoutVertexArray[face._indexArray[2]];

			const fs::Float4 edgeA = v1._positionU.getXyz0() - v0._positionU.getXyz0();
			const fs::Float4 edgeB = v2._positionU.getXyz0() - v0._positionU.getXyz0();
			
			const fs::Float2 uv0 = getVertexUv(v0);
			const fs::Float2 uv1 = getVertexUv(v1);
			const fs::Float2 uv2 = getVertexUv(v2);
			const fs::Float2 uvEdgeA = uv1 - uv0;
			const fs::Float2 uvEdgeB = uv2 - uv0;

			// T = tangent
			// B = bitangent
			//
			// edgeA = uvEdgeA.u0 * T + uvEdgeA.v0 * B
			// edgeB = uvEdgeB.u1 * T + uvEdgeB.v1 * B
			//
			// | edgeA | = | u0 v0 | * | T |
			// | edgeB |   | u1 v1 |   | B |
			//
			// | u0 v0 | -1 * | edgeA | = | T |
			// | u1 v1 |      | edgeB |   | B |

			const fs::Float2x2 uvMatrix{ uvEdgeA._x, uvEdgeA._y, uvEdgeB._x, uvEdgeB._y };
			const fs::Float2x2 uvMatrixInverse = uvMatrix.inverse();
			
			fs::Float4 tangent = edgeA * uvMatrixInverse._11 + edgeB * uvMatrixInverse._12;
			tangent.normalize();
			
			fs::Float4 bitangent = edgeA * uvMatrixInverse._21 + edgeB * uvMatrixInverse._22;
			bitangent.normalize();

			v0._tangentV.setXyz(tangent);
			v1._tangentV.setXyz(tangent);
			v2._tangentV.setXyz(tangent);

			v0._bitangentW.setXyz(bitangent);
			v1._bitangentW.setXyz(bitangent);
			v2._bitangentW.setXyz(bitangent);
		}

		static void generateQuadFace(const uint32 vertexOffset, MeshComponent::MeshData& meshData)
		{
			fs::RenderingBase::Face face;
			face._indexArray[0] = vertexOffset + 0;
			face._indexArray[1] = vertexOffset + 1;
			face._indexArray[2] = vertexOffset + 2;
			calculateTangentBitangent(face, meshData._vertexArray);
			meshData._faceArray.emplace_back(face);

			face._indexArray[0] = vertexOffset + 2;
			face._indexArray[1] = vertexOffset + 1;
			face._indexArray[2] = vertexOffset + 3;
			calculateTangentBitangent(face, meshData._vertexArray);
			meshData._faceArray.emplace_back(face);
		}

		static void smoothNormals(MeshComponent::MeshData& meshData)
		{
			const uint32 vertexCount = meshData.getVertexCount();
			const uint32 faceCount = meshData.getFaceCount();
			const uint32 positionCount = meshData.getPositionCount();
			std::vector<fs::Float4> normalArray(positionCount);
			for (uint32 faceIndex = 0; faceIndex < faceCount; faceIndex++)
			{
				const uint16 v0Index = meshData._faceArray[faceIndex]._indexArray[0];
				const uint16 v1Index = meshData._faceArray[faceIndex]._indexArray[1];
				const uint16 v2Index = meshData._faceArray[faceIndex]._indexArray[2];
				const fs::Float4 v0Normal = fs::Float4::crossNormalize(meshData._vertexArray[v0Index]._tangentV, meshData._vertexArray[v0Index]._bitangentW);
				const fs::Float4 v1Normal = fs::Float4::crossNormalize(meshData._vertexArray[v1Index]._tangentV, meshData._vertexArray[v1Index]._bitangentW);
				const fs::Float4 v2Normal = fs::Float4::crossNormalize(meshData._vertexArray[v2Index]._tangentV, meshData._vertexArray[v2Index]._bitangentW);
				
				normalArray[meshData._vertexToPositionTable[v0Index]] += v0Normal;
				normalArray[meshData._vertexToPositionTable[v0Index]]._w += 1.0f;
				normalArray[meshData._vertexToPositionTable[v1Index]] += v1Normal;
				normalArray[meshData._vertexToPositionTable[v1Index]]._w += 1.0f;
				normalArray[meshData._vertexToPositionTable[v2Index]] += v2Normal;
				normalArray[meshData._vertexToPositionTable[v2Index]]._w += 1.0f;
			}

			// Average normals
			for (uint32 positionIndex = 0; positionIndex < positionCount; positionIndex++)
			{
				normalArray[positionIndex] /= normalArray[positionIndex]._w;
				normalArray[positionIndex]._w = 0.0f;
				normalArray[positionIndex].normalize();
			}

			// Recalculate tangent, bitangent
			for (uint32 vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
			{
				const fs::Float4& normal = normalArray[meshData._vertexToPositionTable[vertexIndex]];
				
				const fs::Float4 tangent = fs::Float4::crossNormalize(meshData._vertexArray[vertexIndex]._bitangentW, normal);
				const fs::Float4 bitangent = fs::Float4::crossNormalize(normal, tangent);

				meshData._vertexArray[vertexIndex]._tangentV = tangent;
				meshData._vertexArray[vertexIndex]._bitangentW = bitangent;
			}
			normalArray.clear();
		}


		fs::RenderingBase::VS_INPUT				MeshComponent::kNullVertex = fs::RenderingBase::VS_INPUT();
		fs::RenderingBase::IndexElementType		MeshComponent::kNullIndex = 0;
		MeshComponent::MeshComponent()
			: TransformComponent(ObjectComponentType::MeshComponent)
		{
			static constexpr float kHalfSize = 0.5f;

			_meshData.clear();

			// Vertex positions
			_meshData._positionArray.resize(8);
			_meshData._positionArray[0] = fs::Float4(-kHalfSize, +kHalfSize, -kHalfSize, 1.0f);
			_meshData._positionArray[1] = fs::Float4(+kHalfSize, +kHalfSize, -kHalfSize, 1.0f);
			_meshData._positionArray[2] = fs::Float4(+kHalfSize, -kHalfSize, -kHalfSize, 1.0f);
			_meshData._positionArray[3] = fs::Float4(-kHalfSize, -kHalfSize, -kHalfSize, 1.0f);
			_meshData._positionArray[4] = fs::Float4(-kHalfSize, +kHalfSize, +kHalfSize, 1.0f);
			_meshData._positionArray[5] = fs::Float4(+kHalfSize, +kHalfSize, +kHalfSize, 1.0f);
			_meshData._positionArray[6] = fs::Float4(+kHalfSize, -kHalfSize, +kHalfSize, 1.0f);
			_meshData._positionArray[7] = fs::Float4(-kHalfSize, -kHalfSize, +kHalfSize, 1.0f);

			// Top Face
			{
				const uint32 vertexCountOld = _meshData.getVertexCount();
				pushVertexWithPosition(4, _meshData);
				pushVertexWithPosition(5, _meshData);
				pushVertexWithPosition(0, _meshData);
				pushVertexWithPosition(1, _meshData);

				setVertexUv(_meshData, vertexCountOld + 0, 0.0f, 0.0f);
				setVertexUv(_meshData, vertexCountOld + 1, 1.0f, 0.0f);
				setVertexUv(_meshData, vertexCountOld + 2, 0.0f, 1.0f);
				setVertexUv(_meshData, vertexCountOld + 3, 1.0f, 1.0f);
				
				generateQuadFace(vertexCountOld, _meshData);
			}
			
			// Front Face
			{
				const uint32 vertexCountOld = _meshData.getVertexCount();
				pushVertexWithPosition(0, _meshData);
				pushVertexWithPosition(1, _meshData);
				pushVertexWithPosition(3, _meshData);
				pushVertexWithPosition(2, _meshData);

				setVertexUv(_meshData, vertexCountOld + 0, 0.0f, 0.0f);
				setVertexUv(_meshData, vertexCountOld + 1, 1.0f, 0.0f);
				setVertexUv(_meshData, vertexCountOld + 2, 0.0f, 1.0f);
				setVertexUv(_meshData, vertexCountOld + 3, 1.0f, 1.0f);

				generateQuadFace(vertexCountOld, _meshData);
			}

			// Right Face
			{
				const uint32 vertexCountOld = _meshData.getVertexCount();
				pushVertexWithPosition(1, _meshData);
				pushVertexWithPosition(5, _meshData);
				pushVertexWithPosition(2, _meshData);
				pushVertexWithPosition(6, _meshData);

				setVertexUv(_meshData, vertexCountOld + 0, 0.0f, 0.0f);
				setVertexUv(_meshData, vertexCountOld + 1, 1.0f, 0.0f);
				setVertexUv(_meshData, vertexCountOld + 2, 0.0f, 1.0f);
				setVertexUv(_meshData, vertexCountOld + 3, 1.0f, 1.0f);

				generateQuadFace(vertexCountOld, _meshData);
			}

			// Back Face
			{
				const uint32 vertexCountOld = _meshData.getVertexCount();
				pushVertexWithPosition(5, _meshData);
				pushVertexWithPosition(4, _meshData);
				pushVertexWithPosition(6, _meshData);
				pushVertexWithPosition(7, _meshData);

				setVertexUv(_meshData, vertexCountOld + 0, 0.0f, 0.0f);
				setVertexUv(_meshData, vertexCountOld + 1, 1.0f, 0.0f);
				setVertexUv(_meshData, vertexCountOld + 2, 0.0f, 1.0f);
				setVertexUv(_meshData, vertexCountOld + 3, 1.0f, 1.0f);

				generateQuadFace(vertexCountOld, _meshData);
			}

			// Left Face
			{
				const uint32 vertexCountOld = _meshData.getVertexCount();
				pushVertexWithPosition(4, _meshData);
				pushVertexWithPosition(0, _meshData);
				pushVertexWithPosition(7, _meshData);
				pushVertexWithPosition(3, _meshData);

				setVertexUv(_meshData, vertexCountOld + 0, 0.0f, 0.0f);
				setVertexUv(_meshData, vertexCountOld + 1, 1.0f, 0.0f);
				setVertexUv(_meshData, vertexCountOld + 2, 0.0f, 1.0f);
				setVertexUv(_meshData, vertexCountOld + 3, 1.0f, 1.0f);

				generateQuadFace(vertexCountOld, _meshData);
			}

			// Bottom Face
			{
				const uint32 vertexCountOld = _meshData.getVertexCount();
				pushVertexWithPosition(3, _meshData);
				pushVertexWithPosition(2, _meshData);
				pushVertexWithPosition(7, _meshData);
				pushVertexWithPosition(6, _meshData);

				setVertexUv(_meshData, vertexCountOld + 0, 0.0f, 0.0f);
				setVertexUv(_meshData, vertexCountOld + 1, 1.0f, 0.0f);
				setVertexUv(_meshData, vertexCountOld + 2, 0.0f, 1.0f);
				setVertexUv(_meshData, vertexCountOld + 3, 1.0f, 1.0f);

				generateQuadFace(vertexCountOld, _meshData);
			}
		}

		MeshComponent::~MeshComponent()
		{
			__noop;
		}

		const uint32 MeshComponent::getVertexCount() const noexcept
		{
			return static_cast<uint32>(_meshData._vertexArray.size());
		}

		const fs::RenderingBase::VS_INPUT* MeshComponent::getVertices() const noexcept
		{
			return (_meshData._vertexArray.empty()) ? &kNullVertex : &_meshData._vertexArray[0];
		}

		const uint32 MeshComponent::getIndexCount() const noexcept
		{
			return static_cast<uint32>(_meshData._faceArray.size() * fs::RenderingBase::Face::kIndexCountPerFace);
		}

		const fs::RenderingBase::IndexElementType* MeshComponent::getIndices() const noexcept
		{
			return (_meshData._faceArray.empty() == true) ? &kNullIndex : &_meshData._faceArray[0]._indexArray[0];
		}
	}
}

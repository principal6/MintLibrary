#include <stdafx.h>
#include <FsRendering/Include/MeshGenerator.h>

#include <FsMath/Include/Float2x2.h>

#include <Assets/CppHlsl/CppHlslStreamData.h>


namespace fs
{
    namespace Rendering
    {
        const fs::RenderingBase::VS_INPUT           MeshData::kNullVertex = fs::RenderingBase::VS_INPUT();
        const fs::RenderingBase::IndexElementType   MeshData::kNullIndex = 0;

        MeshData::MeshData()
        {
            __noop;
        }

        void MeshData::clear() noexcept
        {
            _positionArray.clear();
            _vertexToPositionTable.clear();
            _vertexArray.clear();
            _faceArray.clear();
        }

        const uint32 MeshData::getPositionCount() const noexcept
        {
            return static_cast<uint32>(_positionArray.size());
        }

        const uint32 MeshData::getVertexCount() const noexcept
        {
            return static_cast<uint32>(_vertexArray.size());
        }

        const uint32 MeshData::getFaceCount() const noexcept
        {
            return static_cast<uint32>(_faceArray.size());
        }


        void MeshGenerator::pushVertexWithPosition(const uint32 positionIndex, MeshData& meshData) noexcept
        {
            fs::RenderingBase::VS_INPUT vertex;
            vertex._positionU = meshData._positionArray[positionIndex];
            meshData._vertexArray.emplace_back(vertex);
            meshData._vertexToPositionTable.emplace_back(positionIndex);
        }

        void MeshGenerator::setVertexUv(MeshData& meshData, const uint32 vertexIndex, const float u, const float v) noexcept
        {
            meshData._vertexArray[vertexIndex]._positionU._w = u;
            meshData._vertexArray[vertexIndex]._tangentV._w = v;
        }

        fs::Float2 MeshGenerator::getVertexUv(const fs::RenderingBase::VS_INPUT& inoutVertex) noexcept
        {
            return fs::Float2(inoutVertex._positionU._w, inoutVertex._tangentV._w);
        }

        void MeshGenerator::generateQuadFace(const uint32 vertexOffset, MeshData& meshData) noexcept
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

        void MeshGenerator::calculateTangentBitangent(const fs::RenderingBase::Face& face, std::vector<fs::RenderingBase::VS_INPUT>& inoutVertexArray) noexcept
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
            // | u0 v0 | ^(-1) * | edgeA | = | T |
            // | u1 v1 |         | edgeB |   | B |

            const fs::Float2x2 uvMatrix{ uvEdgeA._x, uvEdgeA._y, uvEdgeB._x, uvEdgeB._y };
            fs::Float4 tangent;
            fs::Float4 bitangent;
            if (uvMatrix.isInvertible() == false)
            {
                FS_ASSERT("김장원", false, "uvMatrix 가 Invertible 하지 않습니다!!!");

                tangent.setXyz(+1.0f, 0.0f, 0.0f);
                bitangent.setXyz(0.0f, 0.0f, -1.0f);
            }
            else
            {
                const fs::Float2x2 uvMatrixInverse = uvMatrix.inverse();

                tangent = edgeA * uvMatrixInverse._11 + edgeB * uvMatrixInverse._12;
                tangent.normalize();

                bitangent = edgeA * uvMatrixInverse._21 + edgeB * uvMatrixInverse._22;
                bitangent.normalize();
            }

            v0._tangentV.setXyz(tangent);
            v1._tangentV.setXyz(tangent);
            v2._tangentV.setXyz(tangent);

            v0._bitangentW.setXyz(bitangent);
            v1._bitangentW.setXyz(bitangent);
            v2._bitangentW.setXyz(bitangent);
        }

        void MeshGenerator::smoothNormals(MeshData& meshData) noexcept
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
        
        void MeshGenerator::generateCube(MeshData& meshData) noexcept
        {
            static constexpr float kHalfSize = 0.5f;

            meshData.clear();

            // Vertex positions
            meshData._positionArray.resize(8);
            meshData._positionArray[0] = fs::Float4(-kHalfSize, +kHalfSize, -kHalfSize, 1.0f);
            meshData._positionArray[1] = fs::Float4(+kHalfSize, +kHalfSize, -kHalfSize, 1.0f);
            meshData._positionArray[2] = fs::Float4(+kHalfSize, -kHalfSize, -kHalfSize, 1.0f);
            meshData._positionArray[3] = fs::Float4(-kHalfSize, -kHalfSize, -kHalfSize, 1.0f);
            meshData._positionArray[4] = fs::Float4(-kHalfSize, +kHalfSize, +kHalfSize, 1.0f);
            meshData._positionArray[5] = fs::Float4(+kHalfSize, +kHalfSize, +kHalfSize, 1.0f);
            meshData._positionArray[6] = fs::Float4(+kHalfSize, -kHalfSize, +kHalfSize, 1.0f);
            meshData._positionArray[7] = fs::Float4(-kHalfSize, -kHalfSize, +kHalfSize, 1.0f);

            prepareCubeQuadFace({ 4, 5, 0, 1 }, meshData); // Top
            prepareCubeQuadFace({ 0, 1, 3, 2 }, meshData); // Front
            prepareCubeQuadFace({ 1, 5, 2, 6 }, meshData); // Right
            prepareCubeQuadFace({ 5, 4, 6, 7 }, meshData); // Back
            prepareCubeQuadFace({ 4, 0, 7, 3 }, meshData); // Left
            prepareCubeQuadFace({ 3, 2, 7, 6 }, meshData); // Bottom
        }
        
        void MeshGenerator::prepareCubeQuadFace(const int32(&positionIndices)[4], MeshData& meshData) noexcept
        {
            const uint32 vertexCountOld = meshData.getVertexCount();

            pushVertexWithPosition(positionIndices[0], meshData);
            pushVertexWithPosition(positionIndices[1], meshData);
            pushVertexWithPosition(positionIndices[2], meshData);
            pushVertexWithPosition(positionIndices[3], meshData);

            setVertexUv(meshData, vertexCountOld + 0, 0.0f, 0.0f);
            setVertexUv(meshData, vertexCountOld + 1, 1.0f, 0.0f);
            setVertexUv(meshData, vertexCountOld + 2, 0.0f, 1.0f);
            setVertexUv(meshData, vertexCountOld + 3, 1.0f, 1.0f);

            generateQuadFace(vertexCountOld, meshData);
        }
    }
}

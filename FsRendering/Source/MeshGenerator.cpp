#include <stdafx.h>
#include <FsRendering/Include/MeshGenerator.h>

#include <FsContainer/Include/Vector.hpp>

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

        void MeshData::shrinkToFit() noexcept
        {
            _positionArray.shrink_to_fit();
            _vertexToPositionTable.shrink_to_fit();
            _vertexArray.shrink_to_fit();
            _faceArray.shrink_to_fit();
        }

        void MeshData::updateVertexFromPositions() noexcept
        {
            const uint32 vertexCount = getVertexCount();
            for (uint32 vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
            {
                const uint32 positionIndex = _vertexToPositionTable[vertexIndex];
                _vertexArray[vertexIndex]._positionU.setXyz(_positionArray[positionIndex]);
            }
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


        FS_INLINE void MeshGenerator::pushVertexWithPositionXXX(const uint32 positionIndex, MeshData& meshData) noexcept
        {
            fs::RenderingBase::VS_INPUT vertex;
            vertex._positionU = meshData._positionArray[positionIndex];
            meshData._vertexArray.push_back(vertex);
            meshData._vertexToPositionTable.push_back(positionIndex);
        }

        FS_INLINE void MeshGenerator::setVertexUv(MeshData& meshData, const uint32 vertexIndex, const float u, const float v) noexcept
        {
            meshData._vertexArray[vertexIndex]._positionU._w = u;
            meshData._vertexArray[vertexIndex]._tangentV._w = v;
        }

        FS_INLINE void MeshGenerator::setVertexUv(fs::RenderingBase::VS_INPUT& vertex, const fs::Float2& uv) noexcept
        {
            vertex._positionU._w = uv._x;
            vertex._tangentV._w = uv._y;
        }

        FS_INLINE fs::Float2 MeshGenerator::getVertexUv(const fs::RenderingBase::VS_INPUT& inoutVertex) noexcept
        {
            return fs::Float2(inoutVertex._positionU._w, inoutVertex._tangentV._w);
        }

        FS_INLINE fs::RenderingBase::VS_INPUT& MeshGenerator::getFaceVertex0(const fs::RenderingBase::Face& face, MeshData& meshData) noexcept
        {
            return meshData._vertexArray[face._vertexIndexArray[0]];
        }

        FS_INLINE fs::RenderingBase::VS_INPUT& MeshGenerator::getFaceVertex1(const fs::RenderingBase::Face& face, MeshData& meshData) noexcept
        {
            return meshData._vertexArray[face._vertexIndexArray[1]];
        }

        FS_INLINE fs::RenderingBase::VS_INPUT& MeshGenerator::getFaceVertex2(const fs::RenderingBase::Face& face, MeshData& meshData) noexcept
        {
            return meshData._vertexArray[face._vertexIndexArray[2]];
        }

        FS_INLINE const uint32 MeshGenerator::getFaceVertexPositionIndex0(const fs::RenderingBase::Face& face, MeshData& meshData) noexcept
        {
            return meshData._vertexToPositionTable[face._vertexIndexArray[0]];
        }

        FS_INLINE const uint32 MeshGenerator::getFaceVertexPositionIndex1(const fs::RenderingBase::Face& face, MeshData& meshData) noexcept
        {
            return meshData._vertexToPositionTable[face._vertexIndexArray[1]];
        }

        FS_INLINE const uint32 MeshGenerator::getFaceVertexPositionIndex2(const fs::RenderingBase::Face& face, MeshData& meshData) noexcept
        {
            return meshData._vertexToPositionTable[face._vertexIndexArray[2]];
        }

        FS_INLINE fs::Float4& MeshGenerator::getFaceVertexPosition0(const fs::RenderingBase::Face& face, MeshData& meshData) noexcept
        {
            return meshData._positionArray[getFaceVertexPositionIndex0(face, meshData)];
        }

        FS_INLINE fs::Float4& MeshGenerator::getFaceVertexPosition1(const fs::RenderingBase::Face& face, MeshData& meshData) noexcept
        {
            return meshData._positionArray[getFaceVertexPositionIndex1(face, meshData)];
        }

        FS_INLINE fs::Float4& MeshGenerator::getFaceVertexPosition2(const fs::RenderingBase::Face& face, MeshData& meshData) noexcept
        {
            return meshData._positionArray[getFaceVertexPositionIndex2(face, meshData)];
        }

        void MeshGenerator::pushTriFaceXXX(const uint32 vertexOffset, MeshData& meshData) noexcept
        {
            fs::RenderingBase::Face face;
            face._vertexIndexArray[0] = vertexOffset + 0;
            face._vertexIndexArray[1] = vertexOffset + 1;
            face._vertexIndexArray[2] = vertexOffset + 2;
            calculateTangentBitangent(face, meshData._vertexArray);
            meshData._faceArray.push_back(face);
        }

        void MeshGenerator::pushQuadFaceXXX(const uint32 vertexOffset, MeshData& meshData) noexcept
        {
            fs::RenderingBase::Face face;
            face._vertexIndexArray[0] = vertexOffset + 0;
            face._vertexIndexArray[1] = vertexOffset + 1;
            face._vertexIndexArray[2] = vertexOffset + 2;
            calculateTangentBitangent(face, meshData._vertexArray);
            meshData._faceArray.push_back(face);

            face._vertexIndexArray[0] = vertexOffset + 2;
            face._vertexIndexArray[1] = vertexOffset + 1;
            face._vertexIndexArray[2] = vertexOffset + 3;
            calculateTangentBitangent(face, meshData._vertexArray);
            meshData._faceArray.push_back(face);
        }

        void MeshGenerator::recalculateTangentBitangentFromNormal(const fs::Float4& normal, fs::RenderingBase::VS_INPUT& vertex) noexcept
        {
            // TODO: TexCoord._w calculation
            const fs::Float2 uv = getVertexUv(vertex);
            vertex._tangentV._w = 0.0f;
            vertex._bitangentW = fs::Float4::crossNormalize(normal, vertex._tangentV);
            vertex._tangentV = fs::Float4::crossNormalize(vertex._bitangentW, normal);
            setVertexUv(vertex, uv);
        }

        FS_INLINE fs::Float4 MeshGenerator::computeNormalFromTangentBitangent(const fs::RenderingBase::VS_INPUT& vertex) noexcept
        {
            return fs::Float4::crossNormalize(vertex._tangentV, vertex._bitangentW);
        }

        void MeshGenerator::calculateTangentBitangent(const fs::RenderingBase::Face& face, fs::Vector<fs::RenderingBase::VS_INPUT>& inoutVertexArray) noexcept
        {
            fs::RenderingBase::VS_INPUT& v0 = inoutVertexArray[face._vertexIndexArray[0]];
            fs::RenderingBase::VS_INPUT& v1 = inoutVertexArray[face._vertexIndexArray[1]];
            fs::RenderingBase::VS_INPUT& v2 = inoutVertexArray[face._vertexIndexArray[2]];

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
            fs::Vector<fs::Float4> normalArray(positionCount);
            for (uint32 faceIndex = 0; faceIndex < faceCount; faceIndex++)
            {
                const uint16 v0Index = meshData._faceArray[faceIndex]._vertexIndexArray[0];
                const uint16 v1Index = meshData._faceArray[faceIndex]._vertexIndexArray[1];
                const uint16 v2Index = meshData._faceArray[faceIndex]._vertexIndexArray[2];
                const fs::Float4 v0Normal = computeNormalFromTangentBitangent(meshData._vertexArray[v0Index]);
                const fs::Float4 v1Normal = computeNormalFromTangentBitangent(meshData._vertexArray[v1Index]);
                const fs::Float4 v2Normal = computeNormalFromTangentBitangent(meshData._vertexArray[v2Index]);

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
            meshData._positionArray.reserve(8);
            pushPosition({ -kHalfSize, +kHalfSize, -kHalfSize }, meshData);
            pushPosition({ +kHalfSize, +kHalfSize, -kHalfSize }, meshData);
            pushPosition({ +kHalfSize, -kHalfSize, -kHalfSize }, meshData);
            pushPosition({ -kHalfSize, -kHalfSize, -kHalfSize }, meshData);
            pushPosition({ -kHalfSize, +kHalfSize, +kHalfSize }, meshData);
            pushPosition({ +kHalfSize, +kHalfSize, +kHalfSize }, meshData);
            pushPosition({ +kHalfSize, -kHalfSize, +kHalfSize }, meshData);
            pushPosition({ -kHalfSize, -kHalfSize, +kHalfSize }, meshData);

            const fs::Float2 uvs[4]{ fs::Float2(0.0f, 0.0f), fs::Float2(1.0f, 0.0f), fs::Float2(1.0f, 1.0f), fs::Float2(0.0f, 1.0f) };
            pushQuad({ 4, 5, 1, 0 }, meshData, uvs); // Top
            pushQuad({ 0, 1, 2, 3 }, meshData, uvs); // Front
            pushQuad({ 1, 5, 6, 2 }, meshData, uvs); // Right
            pushQuad({ 5, 4, 7, 6 }, meshData, uvs); // Back
            pushQuad({ 4, 0, 3, 7 }, meshData, uvs); // Left
            pushQuad({ 3, 2, 6, 7 }, meshData, uvs); // Bottom
        }

        void MeshGenerator::generateCone(const ConeParam& coneParam, MeshData& meshData) noexcept
        {
            meshData.clear();

            // Position
            {
                meshData._positionArray.reserve(coneParam._sideCount + 2);

                pushPosition({ 0.0f, coneParam._height, 0.0f }, meshData);

                const float angleStep = fs::Math::kTwoPi / static_cast<float>(coneParam._sideCount);
                for (int16 sideIndex = 0; sideIndex < coneParam._sideCount; ++sideIndex)
                {
                    const float x = ::cos(angleStep * sideIndex) * coneParam._radius;
                    const float z = ::sin(angleStep * sideIndex) * coneParam._radius;
                    pushPosition({ x, 0.0f, z }, meshData);
                }

                pushPosition({ 0.0f, 0.0f, 0.0f }, meshData);
            }
            const int32 positionIndexTopCenter      = 0;
            const int32 positionIndexBottomCenter   = static_cast<int32>(meshData._positionArray.size() - 1);

            // Cone sides
            {
                const fs::Float2 uvs[3]{ fs::Float2(0.5f, 0.0f), fs::Float2(1.0f, 1.0f), fs::Float2(0.0f, 1.0f) };
                for (int16 sideIndex = 0; sideIndex < coneParam._sideCount - 1; ++sideIndex)
                {
                    pushTri({ positionIndexTopCenter, sideIndex + 2, sideIndex + 1 }, meshData, uvs);
                }
                pushTri({ positionIndexTopCenter, 1, coneParam._sideCount }, meshData, uvs);

                if (coneParam._smooth == true)
                {
                    smoothNormals(meshData);
                }
            }

            // Cone bottom
            {
                const fs::Float2 uvs[3]{ fs::Float2(0.5f, 1.0f), fs::Float2(0.0f, 0.0f), fs::Float2(1.0f, 0.0f) };
                for (int16 sideIndex = 0; sideIndex < coneParam._sideCount - 1; ++sideIndex)
                {
                    pushTri({ positionIndexBottomCenter, sideIndex + 1, sideIndex + 2 }, meshData, uvs);
                }
                pushTri({ positionIndexBottomCenter, coneParam._sideCount, 1 }, meshData, uvs);
            }
        }

        void MeshGenerator::generateCylinder(const CylinderParam& cylinderParam, MeshData& meshData) noexcept
        {
            meshData.clear();

            // Position
            {
                meshData._positionArray.reserve(cylinderParam._sideCount * 2 + 2);

                pushPosition({ 0.0f, cylinderParam._height, 0.0f }, meshData);

                const float angleStep = fs::Math::kTwoPi / static_cast<float>(cylinderParam._sideCount);
                for (int16 sideIndex = 0; sideIndex < cylinderParam._sideCount; ++sideIndex)
                {
                    const float x = ::cos(angleStep * sideIndex) * cylinderParam._radius;
                    const float z = ::sin(angleStep * sideIndex) * cylinderParam._radius;
                    pushPosition({ x, cylinderParam._height, z }, meshData);
                    pushPosition({ x, 0.0f, z }, meshData);
                }

                pushPosition({ 0.0f, 0.0f, 0.0f }, meshData);
            }
            const int32 positionIndexTopCenter      = 0;
            const int32 positionIndexBottomCenter   = static_cast<int32>(meshData._positionArray.size() - 1);

            // Cylinder sides
            {
                const int16 positionIndexBase = 1;
                const fs::Float2 uvs[4]{ fs::Float2(0.0f, 0.0f), fs::Float2(1.0f, 0.0f), fs::Float2(1.0f, 1.0f), fs::Float2(0.0f, 1.0f) };
                for (int16 sideIndex = 0; sideIndex < cylinderParam._sideCount - 1; ++sideIndex)
                {
                    const int32 positionIndex = positionIndexBase + sideIndex * 2;
                    pushQuad({ positionIndex + 0, positionIndex + 2, positionIndex + 3, positionIndex + 1 }, meshData, uvs);
                }
                const int32 positionIndex = positionIndexBase + (cylinderParam._sideCount - 1) * 2;
                pushQuad({ positionIndex + 0, 1, 2, positionIndex + 1 }, meshData, uvs);

                if (cylinderParam._smooth == true)
                {
                    smoothNormals(meshData);
                }
            }

            // Clylinder top
            {
                const int16 positionIndexBase = 1;
                const fs::Float2 uvs[3]{ fs::Float2(0.5f, 0.0f), fs::Float2(1.0f, 1.0f), fs::Float2(0.0f, 1.0f) };
                for (int16 sideIndex = 0; sideIndex < cylinderParam._sideCount - 1; ++sideIndex)
                {
                    const int32 positionIndex = positionIndexBase + sideIndex * 2;
                    pushTri({ positionIndexTopCenter, positionIndex + 2, positionIndex }, meshData, uvs);
                }
                const int32 positionIndex = positionIndexBase + (cylinderParam._sideCount - 1) * 2;
                pushTri({ positionIndexTopCenter, positionIndexBase, positionIndex }, meshData, uvs);
            }

            // Clylinder bottom
            {
                const int16 positionIndexBase = 1;
                const fs::Float2 uvs[3]{ fs::Float2(0.5f, 1.0f), fs::Float2(0.0f, 0.0f), fs::Float2(1.0f, 0.0f) };
                for (int16 sideIndex = 0; sideIndex < cylinderParam._sideCount - 1; ++sideIndex)
                {
                    const int32 positionIndex = positionIndexBase + sideIndex * 2;
                    pushTri({ positionIndexBottomCenter, positionIndex + 1, positionIndex + 3 }, meshData, uvs);
                }
                const int32 positionIndex = positionIndexBase + (cylinderParam._sideCount - 1) * 2;
                pushTri({ positionIndexBottomCenter, positionIndex + 1, positionIndexBase + 1 }, meshData, uvs);
            }
        }

        void MeshGenerator::generateOctahedron(const RadiusParam& radiusParam, MeshData& meshData) noexcept
        {
            meshData.clear();

            // Position
            {
                meshData._positionArray.reserve(6);
                pushPosition({ 0.0f, +radiusParam._radius, 0.0f }, meshData);
                pushPosition({ ::cos(fs::Math::kTwoPi * 0.0f  + radiusParam._radius), 0.0f, ::sin(fs::Math::kTwoPi * 0.0f  + radiusParam._radius) }, meshData);
                pushPosition({ ::cos(fs::Math::kTwoPi * 0.25f + radiusParam._radius), 0.0f, ::sin(fs::Math::kTwoPi * 0.25f + radiusParam._radius) }, meshData);
                pushPosition({ ::cos(fs::Math::kTwoPi * 0.5f  + radiusParam._radius), 0.0f, ::sin(fs::Math::kTwoPi * 0.5f  + radiusParam._radius) }, meshData);
                pushPosition({ ::cos(fs::Math::kTwoPi * 0.75f + radiusParam._radius), 0.0f, ::sin(fs::Math::kTwoPi * 0.75f + radiusParam._radius) }, meshData);
                pushPosition({ 0.0f, -radiusParam._radius, 0.0f }, meshData);
            }
            const int32 positionIndexTopCenter = 0;
            const int32 positionIndexBottomCenter = static_cast<int32>(meshData._positionArray.size() - 1);

            // Upper
            {
                const int16 positionIndexBase = 1;
                const fs::Float2 uvs[3]{ fs::Float2(0.5f, 0.0f), fs::Float2(1.0f, 1.0f), fs::Float2(0.0f, 1.0f) };
                for (int16 sideIndex = 0; sideIndex < 3; ++sideIndex)
                {
                    const int16 positionIndex = positionIndexBase + sideIndex;
                    pushTri({ positionIndexTopCenter, positionIndex + 1, positionIndex }, meshData, uvs);
                }
                const int16 positionIndex = positionIndexBase + 3;
                pushTri({ positionIndexTopCenter, 1, positionIndex }, meshData, uvs);
            }

            // Lower
            {
                const int16 positionIndexBase = 1;
                const fs::Float2 uvs[3]{ fs::Float2(0.5f, 1.0f), fs::Float2(0.0f, 0.0f), fs::Float2(1.0f, 0.0f) };
                for (int16 sideIndex = 0; sideIndex < 3; ++sideIndex)
                {
                    const int16 positionIndex = positionIndexBase + sideIndex;
                    pushTri({ positionIndexBottomCenter, positionIndex, positionIndex + 1}, meshData, uvs);
                }
                const int16 positionIndex = positionIndexBase + 3;
                pushTri({ positionIndexBottomCenter, positionIndex, 1 }, meshData, uvs);
            }
        }

        void MeshGenerator::generateGeoSphere(const GeoSpherePram& geoSpherePram, MeshData& meshData) noexcept
        {
            RadiusParam radiusParam;
            radiusParam._radius = geoSpherePram._radius;

            generateOctahedron(radiusParam, meshData);

            subdivideTriByMidpoints(meshData); // At least once
            for (int8 iteration = 0; iteration < geoSpherePram._subdivisionIteration; ++iteration)
            {
                subdivideTriByMidpoints(meshData);
            }

            projectVerticesToSphere(radiusParam, meshData);

            if (geoSpherePram._smooth == true)
            {
                smoothNormals(meshData);
            }
        }

        void MeshGenerator::transformMeshData(MeshData& meshData, const fs::Float4x4& transformationMatrix) noexcept
        {
            const uint32 positionCount = meshData.getPositionCount();
            for (uint32 positionIndex = 0; positionIndex < positionCount; ++positionIndex)
            {
                fs::Float4& position = meshData._positionArray[positionIndex];
                position = transformationMatrix.mul(position);
            }
            meshData.updateVertexFromPositions();
        }

        void MeshGenerator::mergeMeshData(const MeshData& meshDataA, const MeshData& meshDataB, MeshData& outMeshData) noexcept
        {
            outMeshData = meshDataA;

            mergeMeshData(meshDataB, outMeshData);
        }

        void MeshGenerator::mergeMeshData(const MeshData& sourceMeshData, MeshData& inoutTargetMeshData) noexcept
        {
            const uint32 oldPositionCount = inoutTargetMeshData.getPositionCount();
            const uint32 deltaPositionCount = sourceMeshData.getPositionCount();
            inoutTargetMeshData._positionArray.reserve(inoutTargetMeshData._positionArray.size() + deltaPositionCount);
            for (uint32 deltaPositionIndex = 0; deltaPositionIndex < deltaPositionCount; ++deltaPositionIndex)
            {
                inoutTargetMeshData._positionArray.push_back(sourceMeshData._positionArray[deltaPositionIndex]);
            }

            const uint32 oldVertexCount = inoutTargetMeshData.getVertexCount();
            const uint32 deltaVertexCount = sourceMeshData.getVertexCount();
            inoutTargetMeshData._vertexArray.reserve(inoutTargetMeshData._vertexArray.size() + deltaVertexCount);
            inoutTargetMeshData._vertexToPositionTable.reserve(inoutTargetMeshData._vertexToPositionTable.size() + deltaVertexCount);
            for (uint32 deltaVertexIndex = 0; deltaVertexIndex < deltaVertexCount; ++deltaVertexIndex)
            {
                inoutTargetMeshData._vertexArray.push_back(sourceMeshData._vertexArray[deltaVertexIndex]);
                inoutTargetMeshData._vertexToPositionTable.push_back(oldPositionCount + inoutTargetMeshData._vertexToPositionTable[deltaVertexIndex]);
            }

            const uint32 deltaFaceCount = sourceMeshData.getFaceCount();
            inoutTargetMeshData._faceArray.reserve(inoutTargetMeshData._faceArray.size() + deltaFaceCount);
            fs::RenderingBase::Face face;
            for (uint32 deltaFaceIndex = 0; deltaFaceIndex < deltaFaceCount; ++deltaFaceIndex)
            {
                face._vertexIndexArray[0] = oldVertexCount + sourceMeshData._faceArray[deltaFaceIndex]._vertexIndexArray[0];
                face._vertexIndexArray[1] = oldVertexCount + sourceMeshData._faceArray[deltaFaceIndex]._vertexIndexArray[1];
                face._vertexIndexArray[2] = oldVertexCount + sourceMeshData._faceArray[deltaFaceIndex]._vertexIndexArray[2];

                inoutTargetMeshData._faceArray.push_back(face);
            }
        }
        
        FS_INLINE void MeshGenerator::pushPosition(const float(&xyz)[3], MeshData& meshData) noexcept
        {
            meshData._positionArray.push_back(fs::Float4(xyz[0], xyz[1], xyz[2], 1.0f));
        }

        FS_INLINE void MeshGenerator::pushPosition(const fs::Float4& xyzw, MeshData& meshData) noexcept
        {
            meshData._positionArray.push_back(xyzw);
        }

        void MeshGenerator::pushTri(const int32(&positionIndices)[3], MeshData& meshData, const fs::Float2(&uvs)[3]) noexcept
        {
            const uint32 vertexCountOld = meshData.getVertexCount();

            pushVertexWithPositionXXX(positionIndices[0], meshData);
            pushVertexWithPositionXXX(positionIndices[1], meshData);
            pushVertexWithPositionXXX(positionIndices[2], meshData);

            setVertexUv(meshData, vertexCountOld + 0, uvs[0]._x, uvs[0]._y);
            setVertexUv(meshData, vertexCountOld + 1, uvs[1]._x, uvs[1]._y);
            setVertexUv(meshData, vertexCountOld + 2, uvs[2]._x, uvs[2]._y);

            pushTriFaceXXX(vertexCountOld, meshData);
        }

        void MeshGenerator::pushQuad(const int32(&positionIndicesInClockwise)[4], MeshData& meshData, const fs::Float2(&uvsInClockwise)[4]) noexcept
        {
            const uint32 vertexCountOld = meshData.getVertexCount();

            pushVertexWithPositionXXX(positionIndicesInClockwise[0], meshData);
            pushVertexWithPositionXXX(positionIndicesInClockwise[1], meshData);
            pushVertexWithPositionXXX(positionIndicesInClockwise[3], meshData);
            pushVertexWithPositionXXX(positionIndicesInClockwise[2], meshData);

            setVertexUv(meshData, vertexCountOld + 0, uvsInClockwise[0]._x, uvsInClockwise[0]._y);
            setVertexUv(meshData, vertexCountOld + 1, uvsInClockwise[1]._x, uvsInClockwise[1]._y);
            setVertexUv(meshData, vertexCountOld + 2, uvsInClockwise[3]._x, uvsInClockwise[3]._y);
            setVertexUv(meshData, vertexCountOld + 3, uvsInClockwise[2]._x, uvsInClockwise[2]._y);

            pushQuadFaceXXX(vertexCountOld, meshData);
        }
        
        void MeshGenerator::subdivideTriByMidpoints(MeshData& oldMeshData) noexcept
        {
            MeshData newMeshData;
            newMeshData._positionArray = oldMeshData._positionArray;
            newMeshData._positionArray.reserve((newMeshData._positionArray.size() - 2) * 4 + 2);
            newMeshData._vertexArray.reserve(oldMeshData.getVertexCount() * 4);
            newMeshData._vertexToPositionTable.reserve(oldMeshData.getVertexCount() * 4);
            newMeshData._faceArray.reserve(oldMeshData.getFaceCount() * 4);

            struct PositionEdge
            {
            public:
                PositionEdge(const int32 positionIndexA, const int32 positionIndexB) 
                    : _positionIndexA{ fs::min(positionIndexA, positionIndexB) }
                    , _positionIndexB{ fs::max(positionIndexA, positionIndexB) } 
                {
                    __noop;
                }

            public:
                FS_INLINE const int32   key() const noexcept { return _positionIndexA; }
                FS_INLINE const int32   value() const noexcept { return _positionIndexB; }

            private:
                int32                   _positionIndexA = -1;
                int32                   _positionIndexB = -1;
            };

            class PositionEdgeGraph
            {
            public:
                FS_INLINE void          setPositionCount(const uint32 positionCount) noexcept
                {
                    _edgeDataMap.resize(positionCount);
                }
                FS_INLINE void          setMidpoint(const PositionEdge& edge, const int32 midpointPositionIndex) noexcept
                {
                    _edgeDataMap[edge.key()][edge.value()] = midpointPositionIndex;
                }
                FS_INLINE const bool    hasMidpoint(const PositionEdge& edge) const noexcept
                {
                    if (_edgeDataMap[edge.key()].end() != _edgeDataMap[edge.key()].find(edge.value()))
                    {
                        return true;
                    }
                    return false;
                }
                FS_INLINE const int32   getMidpointPositionIndex(const PositionEdge& edge) const noexcept
                {
                    return _edgeDataMap[edge.key()].at(edge.value());
                }

            private:
                fs::Vector<std::unordered_map<int32, int32>>    _edgeDataMap;
            };

            PositionEdgeGraph positionEdgeGraph;
            positionEdgeGraph.setPositionCount(oldMeshData.getPositionCount());
            const uint32 oldFaceCount = static_cast<uint32>(oldMeshData._faceArray.size());
            for (uint32 oldFaceIndex = 0; oldFaceIndex < oldFaceCount; ++oldFaceIndex)
            {
                const fs::RenderingBase::Face& face = oldMeshData._faceArray[oldFaceIndex]; 
                const int32 faceVertexPositionIndices[3]
                {
                    static_cast<int32>(getFaceVertexPositionIndex0(face, oldMeshData)),
                    static_cast<int32>(getFaceVertexPositionIndex1(face, oldMeshData)),
                    static_cast<int32>(getFaceVertexPositionIndex2(face, oldMeshData)),
                };

                int32 midpointPositionIndex01 = 0;
                int32 midpointPositionIndex12 = 0;
                int32 midpointPositionIndex20 = 0;
                {
                    const PositionEdge positionEdge01 = PositionEdge(faceVertexPositionIndices[0], faceVertexPositionIndices[1]);
                    const PositionEdge positionEdge12 = PositionEdge(faceVertexPositionIndices[1], faceVertexPositionIndices[2]);
                    const PositionEdge positionEdge20 = PositionEdge(faceVertexPositionIndices[2], faceVertexPositionIndices[0]);

                    const fs::Float4& faceVertexPosition0 = getFaceVertexPosition0(face, oldMeshData);
                    const fs::Float4& faceVertexPosition1 = getFaceVertexPosition1(face, oldMeshData);
                    const fs::Float4& faceVertexPosition2 = getFaceVertexPosition2(face, oldMeshData);

                    const int32 newPositionIndexBase = static_cast<int32>(newMeshData._positionArray.size());
                    int8 addedPointCount = 0;
                    // Midpoint 0-1
                    if (positionEdgeGraph.hasMidpoint(positionEdge01) == false)
                    {
                        fs::Float4 midPointPosition_01 = (faceVertexPosition0 + faceVertexPosition1) * 0.5f;
                        pushPosition(midPointPosition_01, newMeshData);

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
                        fs::Float4 midPointPosition_12 = (faceVertexPosition1 + faceVertexPosition2) * 0.5f;
                        pushPosition(midPointPosition_12, newMeshData);

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
                        fs::Float4 midPointPosition_20 = (faceVertexPosition2 + faceVertexPosition0) * 0.5f;
                        pushPosition(midPointPosition_20, newMeshData);

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
                const fs::Float2 faceVertexUvs[3]
                {
                    getVertexUv(getFaceVertex0(face, oldMeshData)),
                    getVertexUv(getFaceVertex1(face, oldMeshData)),
                    getVertexUv(getFaceVertex2(face, oldMeshData))
                };
                const fs::Float2 midPointUv01 = (faceVertexUvs[0] + faceVertexUvs[1]) * 0.5f;
                const fs::Float2 midPointUv12 = (faceVertexUvs[1] + faceVertexUvs[2]) * 0.5f;
                const fs::Float2 midPointUv20 = (faceVertexUvs[2] + faceVertexUvs[0]) * 0.5f;

                pushTri({ faceVertexPositionIndices[0], midpointPositionIndex01     , midpointPositionIndex20 }, newMeshData, { faceVertexUvs[0], midPointUv01, midPointUv20 });
                pushTri({ midpointPositionIndex01     , faceVertexPositionIndices[1], midpointPositionIndex12 }, newMeshData, { midPointUv01, faceVertexUvs[1], midPointUv12 });
                pushTri({ midpointPositionIndex12     , faceVertexPositionIndices[2], midpointPositionIndex20 }, newMeshData, { midPointUv12, faceVertexUvs[2], midPointUv20 });
                pushTri({ midpointPositionIndex01     , midpointPositionIndex12     , midpointPositionIndex20 }, newMeshData, { midPointUv01, midPointUv12, midPointUv20 });
            }
            
            newMeshData.shrinkToFit();

            std::swap(oldMeshData, newMeshData);
        }

        void MeshGenerator::projectVerticesToSphere(const RadiusParam& radiusParam, MeshData& meshData) noexcept
        {
            fs::Float4 vertexNormals[3];
            fs::Float4 faceNormal;
            const uint32 faceCount = meshData.getFaceCount();
            for (uint32 faceIndex = 0; faceIndex < faceCount; ++faceIndex)
            {
                const fs::RenderingBase::Face& face = meshData._faceArray[faceIndex];
                
                fs::Float4& faceVertexPosition0 = getFaceVertexPosition0(face, meshData);
                fs::Float4& faceVertexPosition1 = getFaceVertexPosition1(face, meshData);
                fs::Float4& faceVertexPosition2 = getFaceVertexPosition2(face, meshData);

                faceVertexPosition0._w = 0.0f;
                faceVertexPosition0.normalize();
                vertexNormals[0] = faceVertexPosition0;
                faceVertexPosition0 *= radiusParam._radius;
                faceVertexPosition0._w = 1.0f;

                faceVertexPosition1._w = 0.0f;
                faceVertexPosition1.normalize();
                vertexNormals[1] = faceVertexPosition1;
                faceVertexPosition1 *= radiusParam._radius;
                faceVertexPosition1._w = 1.0f;

                faceVertexPosition2._w = 0.0f;
                faceVertexPosition2.normalize();
                vertexNormals[2] = faceVertexPosition2;
                faceVertexPosition2 *= radiusParam._radius;
                faceVertexPosition2._w = 1.0f;

                faceNormal = vertexNormals[0] + vertexNormals[1] + vertexNormals[2];
                faceNormal.normalize();

                recalculateTangentBitangentFromNormal(faceNormal, getFaceVertex0(face, meshData));
                recalculateTangentBitangentFromNormal(faceNormal, getFaceVertex1(face, meshData));
                recalculateTangentBitangentFromNormal(faceNormal, getFaceVertex2(face, meshData));
            }

            meshData.updateVertexFromPositions();
        }
    }
}

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

        void MeshData::shrinkToFit() noexcept
        {
            _positionArray.shrink_to_fit();
            _vertexToPositionTable.shrink_to_fit();
            _vertexArray.shrink_to_fit();
            _faceArray.shrink_to_fit();
        }

        void MeshData::updateVertexPositions() noexcept
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


        void MeshGenerator::pushVertexWithPositionXXX(const uint32 positionIndex, MeshData& meshData) noexcept
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

        void MeshGenerator::setVertexUv(fs::RenderingBase::VS_INPUT& vertex, const fs::Float2& uv) noexcept
        {
            vertex._positionU._w = uv._x;
            vertex._tangentV._w = uv._y;
        }

        fs::Float2 MeshGenerator::getVertexUv(const fs::RenderingBase::VS_INPUT& inoutVertex) noexcept
        {
            return fs::Float2(inoutVertex._positionU._w, inoutVertex._tangentV._w);
        }

        fs::RenderingBase::VS_INPUT& MeshGenerator::getFaceVertex0(const fs::RenderingBase::Face& face, MeshData& meshData) noexcept
        {
            return meshData._vertexArray[face._vertexIndexArray[0]];
        }

        fs::RenderingBase::VS_INPUT& MeshGenerator::getFaceVertex1(const fs::RenderingBase::Face& face, MeshData& meshData) noexcept
        {
            return meshData._vertexArray[face._vertexIndexArray[1]];
        }

        fs::RenderingBase::VS_INPUT& MeshGenerator::getFaceVertex2(const fs::RenderingBase::Face& face, MeshData& meshData) noexcept
        {
            return meshData._vertexArray[face._vertexIndexArray[2]];
        }

        fs::Float4& MeshGenerator::getFaceVertexPosition0(const fs::RenderingBase::Face& face, MeshData& meshData) noexcept
        {
            return meshData._positionArray[meshData._vertexToPositionTable[face._vertexIndexArray[0]]];
        }

        fs::Float4& MeshGenerator::getFaceVertexPosition1(const fs::RenderingBase::Face& face, MeshData& meshData) noexcept
        {
            return meshData._positionArray[meshData._vertexToPositionTable[face._vertexIndexArray[1]]];
        }

        fs::Float4& MeshGenerator::getFaceVertexPosition2(const fs::RenderingBase::Face& face, MeshData& meshData) noexcept
        {
            return meshData._positionArray[meshData._vertexToPositionTable[face._vertexIndexArray[2]]];
        }

        void MeshGenerator::pushTriFaceXXX(const uint32 vertexOffset, MeshData& meshData) noexcept
        {
            fs::RenderingBase::Face face;
            face._vertexIndexArray[0] = vertexOffset + 0;
            face._vertexIndexArray[1] = vertexOffset + 1;
            face._vertexIndexArray[2] = vertexOffset + 2;
            calculateTangentBitangent(face, meshData._vertexArray);
            meshData._faceArray.emplace_back(face);
        }

        void MeshGenerator::pushQuadFaceXXX(const uint32 vertexOffset, MeshData& meshData) noexcept
        {
            fs::RenderingBase::Face face;
            face._vertexIndexArray[0] = vertexOffset + 0;
            face._vertexIndexArray[1] = vertexOffset + 1;
            face._vertexIndexArray[2] = vertexOffset + 2;
            calculateTangentBitangent(face, meshData._vertexArray);
            meshData._faceArray.emplace_back(face);

            face._vertexIndexArray[0] = vertexOffset + 2;
            face._vertexIndexArray[1] = vertexOffset + 1;
            face._vertexIndexArray[2] = vertexOffset + 3;
            calculateTangentBitangent(face, meshData._vertexArray);
            meshData._faceArray.emplace_back(face);
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

        void MeshGenerator::calculateTangentBitangent(const fs::RenderingBase::Face& face, std::vector<fs::RenderingBase::VS_INPUT>& inoutVertexArray) noexcept
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
            std::vector<fs::Float4> normalArray(positionCount);
            for (uint32 faceIndex = 0; faceIndex < faceCount; faceIndex++)
            {
                const uint16 v0Index = meshData._faceArray[faceIndex]._vertexIndexArray[0];
                const uint16 v1Index = meshData._faceArray[faceIndex]._vertexIndexArray[1];
                const uint16 v2Index = meshData._faceArray[faceIndex]._vertexIndexArray[2];
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
        
        inline void MeshGenerator::pushPosition(const float(&xyz)[3], MeshData& meshData) noexcept
        {
            meshData._positionArray.push_back(fs::Float4(xyz[0], xyz[1], xyz[2], 1.0f));
        }

        inline void MeshGenerator::pushPosition(const fs::Float4& xyzw, MeshData& meshData) noexcept
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
        
        void MeshGenerator::subdivideTriByMidpoints(MeshData& meshData) noexcept
        {
            MeshData newMeshData = meshData;
            newMeshData._positionArray.reserve(newMeshData.getPositionCount() * 2);
            newMeshData._vertexArray.reserve(newMeshData.getFaceCount() * 4);
            newMeshData._vertexToPositionTable.reserve(newMeshData.getFaceCount() * 4);
            newMeshData._faceArray.reserve(newMeshData.getFaceCount() * 4);

            const uint32 oldFaceCount = static_cast<uint32>(newMeshData._faceArray.size());
            for (uint32 oldFaceIndex = 0; oldFaceIndex < oldFaceCount; ++oldFaceIndex)
            {
                const fs::RenderingBase::Face& oldFace = meshData._faceArray[oldFaceIndex];
                const fs::RenderingBase::IndexElementType oldFaceVertexIndices[3]
                {
                    oldFace._vertexIndexArray[0],
                    oldFace._vertexIndexArray[1],
                    oldFace._vertexIndexArray[2]
                };

                const fs::Float2 oldFaceVertexUvs[3]
                {
                    getVertexUv(getFaceVertex0(oldFace, meshData)),
                    getVertexUv(getFaceVertex1(oldFace, meshData)),
                    getVertexUv(getFaceVertex2(oldFace, meshData))
                };
                
                const int32 oldFaceVertexPositionIndices[3]
                {
                    static_cast<int32>(meshData._vertexToPositionTable[oldFaceVertexIndices[0]]),
                    static_cast<int32>(meshData._vertexToPositionTable[oldFaceVertexIndices[1]]),
                    static_cast<int32>(meshData._vertexToPositionTable[oldFaceVertexIndices[2]])
                };

                const fs::Float4& faceVertexPosition0 = getFaceVertexPosition0(oldFace, meshData);
                const fs::Float4& faceVertexPosition1 = getFaceVertexPosition1(oldFace, meshData);
                const fs::Float4& faceVertexPosition2 = getFaceVertexPosition2(oldFace, meshData);

                // Midpoint 0-1
                fs::Float4 midPointPosition_01 = (faceVertexPosition0 + faceVertexPosition1) * 0.5f;
                fs::Float2 midPointUv_01 = (oldFaceVertexUvs[0] + oldFaceVertexUvs[1]) * 0.5f;

                // Midpoint 1-2
                fs::Float4 midPointPosition_12 = (faceVertexPosition1 + faceVertexPosition2) * 0.5f;
                fs::Float2 midPointUv_12 = (oldFaceVertexUvs[1] + oldFaceVertexUvs[2]) * 0.5f;
                
                // Midpoint 2-0
                fs::Float4 midPointPosition_20 = (faceVertexPosition2 + faceVertexPosition0) * 0.5f;
                fs::Float2 midPointUv_20 = (oldFaceVertexUvs[2] + oldFaceVertexUvs[0]) * 0.5f;

                const int32 newPositionIndexBase = static_cast<int32>(newMeshData._positionArray.size());
                pushPosition(midPointPosition_01, newMeshData);
                pushPosition(midPointPosition_12, newMeshData);
                pushPosition(midPointPosition_20, newMeshData);

                pushTri({ oldFaceVertexPositionIndices[0], newPositionIndexBase + 0, newPositionIndexBase + 2 }, newMeshData, { oldFaceVertexUvs[0], midPointUv_01, midPointUv_20 });
                pushTri({ newPositionIndexBase + 0, oldFaceVertexPositionIndices[1], newPositionIndexBase + 1 }, newMeshData, { midPointUv_01, oldFaceVertexUvs[1], midPointUv_12 });
                pushTri({ newPositionIndexBase + 1, oldFaceVertexPositionIndices[2], newPositionIndexBase + 2 }, newMeshData, { midPointUv_12, oldFaceVertexUvs[2], midPointUv_20 });
                pushTri({ newPositionIndexBase + 0, newPositionIndexBase + 1, newPositionIndexBase + 2 }, newMeshData, { midPointUv_01, midPointUv_12, midPointUv_20 });
            }
            
            newMeshData.shrinkToFit();

            std::swap(meshData, newMeshData);
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

            meshData.updateVertexPositions();
        }
    }
}

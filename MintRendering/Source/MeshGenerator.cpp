#include <stdafx.h>
#include <MintRendering/Include/MeshGenerator.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintMath/Include/Float2x2.h>

#include <Assets/CppHlsl/CppHlslStreamData.h>


namespace mint
{
    namespace Rendering
    {
        MINT_INLINE void MeshGenerator::pushVertexWithPositionXXX(const uint32 positionIndex, mint::RenderingBase::MeshData& meshData) noexcept
        {
            mint::RenderingBase::VS_INPUT vertex;
            vertex._positionU = meshData._positionArray[positionIndex];
            meshData._vertexArray.push_back(vertex);
            meshData._vertexToPositionTable.push_back(positionIndex);
        }

        MINT_INLINE void MeshGenerator::setVertexUv(mint::RenderingBase::MeshData& meshData, const uint32 vertexIndex, const float u, const float v) noexcept
        {
            meshData._vertexArray[vertexIndex]._positionU._w = u;
            meshData._vertexArray[vertexIndex]._tangentV._w = v;
        }

        MINT_INLINE void MeshGenerator::setVertexUv(mint::RenderingBase::VS_INPUT& vertex, const mint::Float2& uv) noexcept
        {
            vertex._positionU._w = uv._x;
            vertex._tangentV._w = uv._y;
        }

        MINT_INLINE mint::Float2 MeshGenerator::getVertexUv(const mint::RenderingBase::VS_INPUT& inoutVertex) noexcept
        {
            return mint::Float2(inoutVertex._positionU._w, inoutVertex._tangentV._w);
        }

        MINT_INLINE mint::RenderingBase::VS_INPUT& MeshGenerator::getFaceVertex0(const mint::RenderingBase::Face& face, mint::RenderingBase::MeshData& meshData) noexcept
        {
            return meshData._vertexArray[face._vertexIndexArray[0]];
        }

        MINT_INLINE mint::RenderingBase::VS_INPUT& MeshGenerator::getFaceVertex1(const mint::RenderingBase::Face& face, mint::RenderingBase::MeshData& meshData) noexcept
        {
            return meshData._vertexArray[face._vertexIndexArray[1]];
        }

        MINT_INLINE mint::RenderingBase::VS_INPUT& MeshGenerator::getFaceVertex2(const mint::RenderingBase::Face& face, mint::RenderingBase::MeshData& meshData) noexcept
        {
            return meshData._vertexArray[face._vertexIndexArray[2]];
        }

        MINT_INLINE const uint32 MeshGenerator::getFaceVertexPositionIndex0(const mint::RenderingBase::Face& face, mint::RenderingBase::MeshData& meshData) noexcept
        {
            return meshData._vertexToPositionTable[face._vertexIndexArray[0]];
        }

        MINT_INLINE const uint32 MeshGenerator::getFaceVertexPositionIndex1(const mint::RenderingBase::Face& face, mint::RenderingBase::MeshData& meshData) noexcept
        {
            return meshData._vertexToPositionTable[face._vertexIndexArray[1]];
        }

        MINT_INLINE const uint32 MeshGenerator::getFaceVertexPositionIndex2(const mint::RenderingBase::Face& face, mint::RenderingBase::MeshData& meshData) noexcept
        {
            return meshData._vertexToPositionTable[face._vertexIndexArray[2]];
        }

        MINT_INLINE mint::Float4& MeshGenerator::getFaceVertexPosition0(const mint::RenderingBase::Face& face, mint::RenderingBase::MeshData& meshData) noexcept
        {
            return meshData._positionArray[getFaceVertexPositionIndex0(face, meshData)];
        }

        MINT_INLINE mint::Float4& MeshGenerator::getFaceVertexPosition1(const mint::RenderingBase::Face& face, mint::RenderingBase::MeshData& meshData) noexcept
        {
            return meshData._positionArray[getFaceVertexPositionIndex1(face, meshData)];
        }

        MINT_INLINE mint::Float4& MeshGenerator::getFaceVertexPosition2(const mint::RenderingBase::Face& face, mint::RenderingBase::MeshData& meshData) noexcept
        {
            return meshData._positionArray[getFaceVertexPositionIndex2(face, meshData)];
        }

        void MeshGenerator::pushTriFaceXXX(const uint32 vertexOffset, mint::RenderingBase::MeshData& meshData) noexcept
        {
            mint::RenderingBase::Face face;
            face._vertexIndexArray[0] = vertexOffset + 0;
            face._vertexIndexArray[1] = vertexOffset + 1;
            face._vertexIndexArray[2] = vertexOffset + 2;
            calculateTangentBitangent(face, meshData._vertexArray);
            meshData._faceArray.push_back(face);
        }

        void MeshGenerator::pushQuadFaceXXX(const uint32 vertexOffset, mint::RenderingBase::MeshData& meshData) noexcept
        {
            mint::RenderingBase::Face face;
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

        void MeshGenerator::recalculateTangentBitangentFromNormal(const mint::Float4& normal, mint::RenderingBase::VS_INPUT& vertex) noexcept
        {
            // TODO: TexCoord._w calculation
            const mint::Float2 uv = getVertexUv(vertex);
            vertex._tangentV._w = 0.0f;
            vertex._bitangentW = mint::Float4::crossNormalize(normal, vertex._tangentV);
            vertex._tangentV = mint::Float4::crossNormalize(vertex._bitangentW, normal);
            setVertexUv(vertex, uv);
        }

        MINT_INLINE mint::Float4 MeshGenerator::computeNormalFromTangentBitangent(const mint::RenderingBase::VS_INPUT& vertex) noexcept
        {
            return mint::Float4::crossNormalize(vertex._tangentV, vertex._bitangentW);
        }

        void MeshGenerator::calculateTangentBitangent(const mint::RenderingBase::Face& face, mint::Vector<mint::RenderingBase::VS_INPUT>& inoutVertexArray) noexcept
        {
            mint::RenderingBase::VS_INPUT& v0 = inoutVertexArray[face._vertexIndexArray[0]];
            mint::RenderingBase::VS_INPUT& v1 = inoutVertexArray[face._vertexIndexArray[1]];
            mint::RenderingBase::VS_INPUT& v2 = inoutVertexArray[face._vertexIndexArray[2]];

            const mint::Float4 edgeA = v1._positionU.getXyz0() - v0._positionU.getXyz0();
            const mint::Float4 edgeB = v2._positionU.getXyz0() - v0._positionU.getXyz0();

            const mint::Float2 uv0 = getVertexUv(v0);
            const mint::Float2 uv1 = getVertexUv(v1);
            const mint::Float2 uv2 = getVertexUv(v2);
            const mint::Float2 uvEdgeA = uv1 - uv0;
            const mint::Float2 uvEdgeB = uv2 - uv0;

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

            const mint::Float2x2 uvMatrix{ uvEdgeA._x, uvEdgeA._y, uvEdgeB._x, uvEdgeB._y };
            mint::Float4 tangent;
            mint::Float4 bitangent;
            if (uvMatrix.isInvertible() == false)
            {
                MINT_ASSERT("김장원", false, "uvMatrix 가 Invertible 하지 않습니다!!!");

                tangent.setXyz(+1.0f, 0.0f, 0.0f);
                bitangent.setXyz(0.0f, 0.0f, -1.0f);
            }
            else
            {
                const mint::Float2x2 uvMatrixInverse = uvMatrix.inverse();

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

        void MeshGenerator::smoothNormals(mint::RenderingBase::MeshData& meshData) noexcept
        {
            const uint32 vertexCount = meshData.getVertexCount();
            const uint32 faceCount = meshData.getFaceCount();
            const uint32 positionCount = meshData.getPositionCount();
            mint::Vector<mint::Float4> normalArray(positionCount);
            for (uint32 faceIndex = 0; faceIndex < faceCount; faceIndex++)
            {
                const uint16 v0Index = meshData._faceArray[faceIndex]._vertexIndexArray[0];
                const uint16 v1Index = meshData._faceArray[faceIndex]._vertexIndexArray[1];
                const uint16 v2Index = meshData._faceArray[faceIndex]._vertexIndexArray[2];
                const mint::Float4 v0Normal = computeNormalFromTangentBitangent(meshData._vertexArray[v0Index]);
                const mint::Float4 v1Normal = computeNormalFromTangentBitangent(meshData._vertexArray[v1Index]);
                const mint::Float4 v2Normal = computeNormalFromTangentBitangent(meshData._vertexArray[v2Index]);

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
                const mint::Float4& normal = normalArray[meshData._vertexToPositionTable[vertexIndex]];

                const mint::Float4 tangent = mint::Float4::crossNormalize(meshData._vertexArray[vertexIndex]._bitangentW, normal);
                const mint::Float4 bitangent = mint::Float4::crossNormalize(normal, tangent);

                meshData._vertexArray[vertexIndex]._tangentV = tangent;
                meshData._vertexArray[vertexIndex]._bitangentW = bitangent;
            }
            normalArray.clear();
        }
        
        void MeshGenerator::generateCube(mint::RenderingBase::MeshData& meshData) noexcept
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

            const mint::Float2 uvs[4]{ mint::Float2(0.0f, 0.0f), mint::Float2(1.0f, 0.0f), mint::Float2(1.0f, 1.0f), mint::Float2(0.0f, 1.0f) };
            pushQuad({ 4, 5, 1, 0 }, meshData, uvs); // Top
            pushQuad({ 0, 1, 2, 3 }, meshData, uvs); // Front
            pushQuad({ 1, 5, 6, 2 }, meshData, uvs); // Right
            pushQuad({ 5, 4, 7, 6 }, meshData, uvs); // Back
            pushQuad({ 4, 0, 3, 7 }, meshData, uvs); // Left
            pushQuad({ 3, 2, 6, 7 }, meshData, uvs); // Bottom
        }

        void MeshGenerator::generateCone(const ConeParam& coneParam, mint::RenderingBase::MeshData& meshData) noexcept
        {
            meshData.clear();

            // Position
            {
                meshData._positionArray.reserve(coneParam._sideCount + 2);

                pushPosition({ 0.0f, coneParam._height, 0.0f }, meshData);

                const float angleStep = mint::Math::kTwoPi / static_cast<float>(coneParam._sideCount);
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
                const mint::Float2 uvs[3]{ mint::Float2(0.5f, 0.0f), mint::Float2(1.0f, 1.0f), mint::Float2(0.0f, 1.0f) };
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
                const mint::Float2 uvs[3]{ mint::Float2(0.5f, 1.0f), mint::Float2(0.0f, 0.0f), mint::Float2(1.0f, 0.0f) };
                for (int16 sideIndex = 0; sideIndex < coneParam._sideCount - 1; ++sideIndex)
                {
                    pushTri({ positionIndexBottomCenter, sideIndex + 1, sideIndex + 2 }, meshData, uvs);
                }
                pushTri({ positionIndexBottomCenter, coneParam._sideCount, 1 }, meshData, uvs);
            }
        }

        void MeshGenerator::generateCylinder(const CylinderParam& cylinderParam, mint::RenderingBase::MeshData& meshData) noexcept
        {
            meshData.clear();

            // Position
            {
                meshData._positionArray.reserve(cylinderParam._sideCount * 2 + 2);

                pushPosition({ 0.0f, cylinderParam._height, 0.0f }, meshData);

                const float angleStep = mint::Math::kTwoPi / static_cast<float>(cylinderParam._sideCount);
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
                const mint::Float2 uvs[4]{ mint::Float2(0.0f, 0.0f), mint::Float2(1.0f, 0.0f), mint::Float2(1.0f, 1.0f), mint::Float2(0.0f, 1.0f) };
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
                const mint::Float2 uvs[3]{ mint::Float2(0.5f, 0.0f), mint::Float2(1.0f, 1.0f), mint::Float2(0.0f, 1.0f) };
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
                const mint::Float2 uvs[3]{ mint::Float2(0.5f, 1.0f), mint::Float2(0.0f, 0.0f), mint::Float2(1.0f, 0.0f) };
                for (int16 sideIndex = 0; sideIndex < cylinderParam._sideCount - 1; ++sideIndex)
                {
                    const int32 positionIndex = positionIndexBase + sideIndex * 2;
                    pushTri({ positionIndexBottomCenter, positionIndex + 1, positionIndex + 3 }, meshData, uvs);
                }
                const int32 positionIndex = positionIndexBase + (cylinderParam._sideCount - 1) * 2;
                pushTri({ positionIndexBottomCenter, positionIndex + 1, positionIndexBase + 1 }, meshData, uvs);
            }
        }

        void MeshGenerator::generateOctahedron(const RadiusParam& radiusParam, mint::RenderingBase::MeshData& meshData) noexcept
        {
            meshData.clear();

            // Position
            {
                meshData._positionArray.reserve(6);
                pushPosition({ 0.0f, +radiusParam._radius, 0.0f }, meshData);
                pushPosition({ ::cos(mint::Math::kTwoPi * 0.0f ) * radiusParam._radius, 0.0f, ::sin(mint::Math::kTwoPi * 0.0f ) * radiusParam._radius }, meshData);
                pushPosition({ ::cos(mint::Math::kTwoPi * 0.25f) * radiusParam._radius, 0.0f, ::sin(mint::Math::kTwoPi * 0.25f) * radiusParam._radius }, meshData);
                pushPosition({ ::cos(mint::Math::kTwoPi * 0.5f ) * radiusParam._radius, 0.0f, ::sin(mint::Math::kTwoPi * 0.5f ) * radiusParam._radius }, meshData);
                pushPosition({ ::cos(mint::Math::kTwoPi * 0.75f) * radiusParam._radius, 0.0f, ::sin(mint::Math::kTwoPi * 0.75f) * radiusParam._radius }, meshData);
                pushPosition({ 0.0f, -radiusParam._radius, 0.0f }, meshData);
            }
            const int32 positionIndexTopCenter = 0;
            const int32 positionIndexBottomCenter = static_cast<int32>(meshData._positionArray.size() - 1);

            // Upper
            {
                const int16 positionIndexBase = 1;
                const mint::Float2 uvs[3]{ mint::Float2(0.5f, 0.0f), mint::Float2(1.0f, 1.0f), mint::Float2(0.0f, 1.0f) };
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
                const mint::Float2 uvs[3]{ mint::Float2(0.5f, 1.0f), mint::Float2(0.0f, 0.0f), mint::Float2(1.0f, 0.0f) };
                for (int16 sideIndex = 0; sideIndex < 3; ++sideIndex)
                {
                    const int16 positionIndex = positionIndexBase + sideIndex;
                    pushTri({ positionIndexBottomCenter, positionIndex, positionIndex + 1}, meshData, uvs);
                }
                const int16 positionIndex = positionIndexBase + 3;
                pushTri({ positionIndexBottomCenter, positionIndex, 1 }, meshData, uvs);
            }
        }

        void MeshGenerator::generateGeoSphere(const GeoSpherePram& geoSpherePram, mint::RenderingBase::MeshData& meshData) noexcept
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

        void MeshGenerator::setMaterialId(mint::RenderingBase::MeshData& meshData, const uint32 materialId) noexcept
        {
            const uint32 vertexCount = meshData._vertexArray.size();
            for (uint32 vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
            {
                meshData._vertexArray[vertexIndex]._materialId = materialId;
            }
        }

        void MeshGenerator::transformMeshData(mint::RenderingBase::MeshData& meshData, const mint::Float4x4& transformationMatrix) noexcept
        {
            const uint32 positionCount = meshData.getPositionCount();
            for (uint32 positionIndex = 0; positionIndex < positionCount; ++positionIndex)
            {
                mint::Float4& position = meshData._positionArray[positionIndex];
                position = transformationMatrix.mul(position);
            }
            meshData.updateVertexFromPositions();
        }

        void MeshGenerator::mergeMeshData(const mint::RenderingBase::MeshData& meshDataA, const mint::RenderingBase::MeshData& meshDataB, mint::RenderingBase::MeshData& outMeshData) noexcept
        {
            outMeshData = meshDataA;

            mergeMeshData(meshDataB, outMeshData);
        }

        void MeshGenerator::mergeMeshData(const mint::RenderingBase::MeshData& sourceMeshData, mint::RenderingBase::MeshData& inoutTargetMeshData) noexcept
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
            mint::RenderingBase::Face face;
            for (uint32 deltaFaceIndex = 0; deltaFaceIndex < deltaFaceCount; ++deltaFaceIndex)
            {
                face._vertexIndexArray[0] = oldVertexCount + sourceMeshData._faceArray[deltaFaceIndex]._vertexIndexArray[0];
                face._vertexIndexArray[1] = oldVertexCount + sourceMeshData._faceArray[deltaFaceIndex]._vertexIndexArray[1];
                face._vertexIndexArray[2] = oldVertexCount + sourceMeshData._faceArray[deltaFaceIndex]._vertexIndexArray[2];

                inoutTargetMeshData._faceArray.push_back(face);
            }
        }
        
        MINT_INLINE void MeshGenerator::pushPosition(const float(&xyz)[3], mint::RenderingBase::MeshData& meshData) noexcept
        {
            meshData._positionArray.push_back(mint::Float4(xyz[0], xyz[1], xyz[2], 1.0f));
        }

        MINT_INLINE void MeshGenerator::pushPosition(const mint::Float4& xyzw, mint::RenderingBase::MeshData& meshData) noexcept
        {
            meshData._positionArray.push_back(xyzw);
        }

        void MeshGenerator::pushTri(const int32(&positionIndices)[3], mint::RenderingBase::MeshData& meshData, const mint::Float2(&uvs)[3]) noexcept
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

        void MeshGenerator::pushQuad(const int32(&positionIndicesInClockwise)[4], mint::RenderingBase::MeshData& meshData, const mint::Float2(&uvsInClockwise)[4]) noexcept
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
        
        void MeshGenerator::subdivideTriByMidpoints(mint::RenderingBase::MeshData& oldMeshData) noexcept
        {
            mint::RenderingBase::MeshData newMeshData;
            newMeshData._positionArray = oldMeshData._positionArray;
            newMeshData._positionArray.reserve((newMeshData._positionArray.size() - 2) * 4 + 2);
            newMeshData._vertexArray.reserve(oldMeshData.getVertexCount() * 4);
            newMeshData._vertexToPositionTable.reserve(oldMeshData.getVertexCount() * 4);
            newMeshData._faceArray.reserve(oldMeshData.getFaceCount() * 4);

            struct PositionEdge
            {
            public:
                PositionEdge(const int32 positionIndexA, const int32 positionIndexB) 
                    : _positionIndexA{ mint::min(positionIndexA, positionIndexB) }
                    , _positionIndexB{ mint::max(positionIndexA, positionIndexB) } 
                {
                    __noop;
                }

            public:
                MINT_INLINE const int32   key() const noexcept { return _positionIndexA; }
                MINT_INLINE const int32   value() const noexcept { return _positionIndexB; }

            private:
                int32                   _positionIndexA = -1;
                int32                   _positionIndexB = -1;
            };

            class PositionEdgeGraph
            {
            public:
                MINT_INLINE void          setPositionCount(const uint32 positionCount) noexcept
                {
                    _positionCount = positionCount;
                    _edgeTable.resize(positionCount * positionCount);
                    const uint32 length = _edgeTable.size();
                    for (uint32 index = 0; index < length; ++index)
                    {
                        _edgeTable[index] = -1;
                    }
                }
                MINT_INLINE void          setMidpoint(const PositionEdge& edge, const int32 midpointPositionIndex) noexcept
                {
                    _edgeTable[getIndexFromEdge(edge)] = midpointPositionIndex;
                }
                MINT_INLINE const bool    hasMidpoint(const PositionEdge& edge) const noexcept
                {
                    return (0 <= _edgeTable[getIndexFromEdge(edge)]);
                }
                MINT_INLINE const int32   getMidpointPositionIndex(const PositionEdge& edge) const noexcept
                {
                    return _edgeTable[getIndexFromEdge(edge)];
                }

            private:
                MINT_INLINE const uint32  getIndexFromEdge(const PositionEdge& edge) const noexcept
                {
                    return edge.key() * _positionCount + edge.value();
                }

            private:
                int32               _positionCount = 0;
                mint::Vector<int32> _edgeTable;
            };

            PositionEdgeGraph positionEdgeGraph;
            positionEdgeGraph.setPositionCount(oldMeshData.getPositionCount());
            const uint32 oldFaceCount = static_cast<uint32>(oldMeshData._faceArray.size());
            for (uint32 oldFaceIndex = 0; oldFaceIndex < oldFaceCount; ++oldFaceIndex)
            {
                const mint::RenderingBase::Face& face = oldMeshData._faceArray[oldFaceIndex]; 
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

                    const mint::Float4& faceVertexPosition0 = getFaceVertexPosition0(face, oldMeshData);
                    const mint::Float4& faceVertexPosition1 = getFaceVertexPosition1(face, oldMeshData);
                    const mint::Float4& faceVertexPosition2 = getFaceVertexPosition2(face, oldMeshData);

                    const int32 newPositionIndexBase = static_cast<int32>(newMeshData._positionArray.size());
                    int8 addedPointCount = 0;
                    // Midpoint 0-1
                    if (positionEdgeGraph.hasMidpoint(positionEdge01) == false)
                    {
                        mint::Float4 midPointPosition_01 = (faceVertexPosition0 + faceVertexPosition1) * 0.5f;
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
                        mint::Float4 midPointPosition_12 = (faceVertexPosition1 + faceVertexPosition2) * 0.5f;
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
                        mint::Float4 midPointPosition_20 = (faceVertexPosition2 + faceVertexPosition0) * 0.5f;
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
                const mint::Float2 faceVertexUvs[3]
                {
                    getVertexUv(getFaceVertex0(face, oldMeshData)),
                    getVertexUv(getFaceVertex1(face, oldMeshData)),
                    getVertexUv(getFaceVertex2(face, oldMeshData))
                };
                const mint::Float2 midPointUv01 = (faceVertexUvs[0] + faceVertexUvs[1]) * 0.5f;
                const mint::Float2 midPointUv12 = (faceVertexUvs[1] + faceVertexUvs[2]) * 0.5f;
                const mint::Float2 midPointUv20 = (faceVertexUvs[2] + faceVertexUvs[0]) * 0.5f;

                pushTri({ faceVertexPositionIndices[0], midpointPositionIndex01     , midpointPositionIndex20 }, newMeshData, { faceVertexUvs[0], midPointUv01, midPointUv20 });
                pushTri({ midpointPositionIndex01     , faceVertexPositionIndices[1], midpointPositionIndex12 }, newMeshData, { midPointUv01, faceVertexUvs[1], midPointUv12 });
                pushTri({ midpointPositionIndex12     , faceVertexPositionIndices[2], midpointPositionIndex20 }, newMeshData, { midPointUv12, faceVertexUvs[2], midPointUv20 });
                pushTri({ midpointPositionIndex01     , midpointPositionIndex12     , midpointPositionIndex20 }, newMeshData, { midPointUv01, midPointUv12, midPointUv20 });
            }
            
            newMeshData.shrinkToFit();

            std::swap(oldMeshData, newMeshData);
        }

        void MeshGenerator::projectVerticesToSphere(const RadiusParam& radiusParam, mint::RenderingBase::MeshData& meshData) noexcept
        {
            mint::Float4 vertexNormals[3];
            mint::Float4 faceNormal;
            const uint32 faceCount = meshData.getFaceCount();
            for (uint32 faceIndex = 0; faceIndex < faceCount; ++faceIndex)
            {
                const mint::RenderingBase::Face& face = meshData._faceArray[faceIndex];
                
                mint::Float4& faceVertexPosition0 = getFaceVertexPosition0(face, meshData);
                mint::Float4& faceVertexPosition1 = getFaceVertexPosition1(face, meshData);
                mint::Float4& faceVertexPosition2 = getFaceVertexPosition2(face, meshData);

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

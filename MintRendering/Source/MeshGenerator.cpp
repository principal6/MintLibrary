#include <stdafx.h>
#include <MintRendering/Include/MeshGenerator.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintMath/Include/Float2x2.h>

#include <Assets/CppHlsl/CppHlslStreamData.h>


namespace mint
{
    namespace Rendering
    {
        MINT_INLINE void MeshGenerator::pushVertexWithPositionXXX(const uint32 positionIndex, MeshData& meshData) noexcept
        {
            VS_INPUT vertex;
            vertex._positionU = meshData._positionArray[positionIndex];
            meshData._vertexArray.push_back(vertex);
            meshData._vertexToPositionTable.push_back(positionIndex);
        }

        MINT_INLINE void MeshGenerator::setVertexUv(MeshData& meshData, const uint32 vertexIndex, const float u, const float v) noexcept
        {
            meshData._vertexArray[vertexIndex]._positionU._w = u;
            meshData._vertexArray[vertexIndex]._tangentV._w = v;
        }

        MINT_INLINE void MeshGenerator::setVertexUv(VS_INPUT& vertex, const Float2& uv) noexcept
        {
            vertex._positionU._w = uv._x;
            vertex._tangentV._w = uv._y;
        }

        MINT_INLINE Float2 MeshGenerator::getVertexUv(const VS_INPUT& inoutVertex) noexcept
        {
            return Float2(inoutVertex._positionU._w, inoutVertex._tangentV._w);
        }

        MINT_INLINE VS_INPUT& MeshGenerator::getFaceVertex0(const Face& face, MeshData& meshData) noexcept
        {
            return meshData._vertexArray[face._vertexIndexArray[0]];
        }

        MINT_INLINE VS_INPUT& MeshGenerator::getFaceVertex1(const Face& face, MeshData& meshData) noexcept
        {
            return meshData._vertexArray[face._vertexIndexArray[1]];
        }

        MINT_INLINE VS_INPUT& MeshGenerator::getFaceVertex2(const Face& face, MeshData& meshData) noexcept
        {
            return meshData._vertexArray[face._vertexIndexArray[2]];
        }

        MINT_INLINE const uint32 MeshGenerator::getFaceVertexPositionIndex0(const Face& face, MeshData& meshData) noexcept
        {
            return meshData._vertexToPositionTable[face._vertexIndexArray[0]];
        }

        MINT_INLINE const uint32 MeshGenerator::getFaceVertexPositionIndex1(const Face& face, MeshData& meshData) noexcept
        {
            return meshData._vertexToPositionTable[face._vertexIndexArray[1]];
        }

        MINT_INLINE const uint32 MeshGenerator::getFaceVertexPositionIndex2(const Face& face, MeshData& meshData) noexcept
        {
            return meshData._vertexToPositionTable[face._vertexIndexArray[2]];
        }

        MINT_INLINE Float4& MeshGenerator::getFaceVertexPosition0(const Face& face, MeshData& meshData) noexcept
        {
            return meshData._positionArray[getFaceVertexPositionIndex0(face, meshData)];
        }

        MINT_INLINE Float4& MeshGenerator::getFaceVertexPosition1(const Face& face, MeshData& meshData) noexcept
        {
            return meshData._positionArray[getFaceVertexPositionIndex1(face, meshData)];
        }

        MINT_INLINE Float4& MeshGenerator::getFaceVertexPosition2(const Face& face, MeshData& meshData) noexcept
        {
            return meshData._positionArray[getFaceVertexPositionIndex2(face, meshData)];
        }

        void MeshGenerator::pushTriFaceXXX(const uint32 vertexOffset, MeshData& meshData) noexcept
        {
            // Vertex winding 에 상관없이 항상 이 순서.
            Face face;
            face._vertexIndexArray[0] = vertexOffset + 0;
            face._vertexIndexArray[1] = vertexOffset + 1;
            face._vertexIndexArray[2] = vertexOffset + 2;
            calculateTangentBitangent(face, meshData._vertexArray);
            meshData._faceArray.push_back(face);
        }

        void MeshGenerator::pushQuadFaceXXX(const uint32 vertexOffset, MeshData& meshData) noexcept
        {
            // Vertex winding 에 상관없이 항상 이 순서.
            Face face;
            face._vertexIndexArray[0] = vertexOffset + 0;
            face._vertexIndexArray[1] = vertexOffset + 1;
            face._vertexIndexArray[2] = vertexOffset + 2;
            calculateTangentBitangent(face, meshData._vertexArray);
            meshData._faceArray.push_back(face);

            face._vertexIndexArray[0] = vertexOffset + 0;
            face._vertexIndexArray[1] = vertexOffset + 2;
            face._vertexIndexArray[2] = vertexOffset + 3;
            calculateTangentBitangent(face, meshData._vertexArray);
            meshData._faceArray.push_back(face);
        }

        void MeshGenerator::recalculateTangentBitangentFromNormal(const Float4& normal, VS_INPUT& vertex) noexcept
        {
            // TODO: TexCoord._w calculation
            const Float2 uv = getVertexUv(vertex);
            vertex._tangentV._w = 0.0f;
            vertex._bitangentW = Float4::crossNormalize(normal, vertex._tangentV);
            vertex._tangentV = Float4::crossNormalize(vertex._bitangentW, normal);
            setVertexUv(vertex, uv);
        }

        MINT_INLINE Float4 MeshGenerator::computeNormalFromTangentBitangent(const VS_INPUT& vertex) noexcept
        {
            return Float4::crossNormalize(vertex._tangentV, vertex._bitangentW);
        }

        void MeshGenerator::calculateTangentBitangent(const Face& face, Vector<VS_INPUT>& inoutVertexArray) noexcept
        {
            VS_INPUT& v0 = inoutVertexArray[face._vertexIndexArray[0]];
            VS_INPUT& v1 = inoutVertexArray[face._vertexIndexArray[1]];
            VS_INPUT& v2 = inoutVertexArray[face._vertexIndexArray[2]];

            const Float4 edgeA = v1._positionU.getXyz0() - v0._positionU.getXyz0();
            const Float4 edgeB = v2._positionU.getXyz0() - v0._positionU.getXyz0();

            const Float2 uv0 = getVertexUv(v0);
            const Float2 uv1 = getVertexUv(v1);
            const Float2 uv2 = getVertexUv(v2);
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
                MINT_ASSERT("김장원", false, "uvMatrix 가 Invertible 하지 않습니다!!!");

                tangent.setXyz(+1.0f, 0.0f, 0.0f);
                bitangent.setXyz(0.0f, 0.0f, -1.0f);
            }
            else
            {
                const Float2x2 uvMatrixInverse = uvMatrix.inverse();

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
            Vector<Float4> normalArray(positionCount);
            for (uint32 faceIndex = 0; faceIndex < faceCount; ++faceIndex)
            {
                const uint16 v0Index = meshData._faceArray[faceIndex]._vertexIndexArray[0];
                const uint16 v1Index = meshData._faceArray[faceIndex]._vertexIndexArray[1];
                const uint16 v2Index = meshData._faceArray[faceIndex]._vertexIndexArray[2];
                const Float4 v0Normal = computeNormalFromTangentBitangent(meshData._vertexArray[v0Index]);
                const Float4 v1Normal = computeNormalFromTangentBitangent(meshData._vertexArray[v1Index]);
                const Float4 v2Normal = computeNormalFromTangentBitangent(meshData._vertexArray[v2Index]);

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
                normalArray[positionIndex].normalize();
            }

            // Recalculate tangent, bitangent
            for (uint32 vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
            {
                const Float4& normal = normalArray[meshData._vertexToPositionTable[vertexIndex]];

                const Float4 tangent = Float4::crossNormalize(meshData._vertexArray[vertexIndex]._bitangentW, normal);
                const Float4 bitangent = Float4::crossNormalize(normal, tangent);

                meshData._vertexArray[vertexIndex]._tangentV = tangent;
                meshData._vertexArray[vertexIndex]._bitangentW = bitangent;
            }
            normalArray.clear();
        }
        
        void MeshGenerator::generateBox(const BoxParam& boxParam, MeshData& meshData) noexcept
        {
            meshData.clear();

            // Vertex positions
            meshData._positionArray.reserve(8);
            const Float3 halfExtents{ boxParam._width * 0.5f, boxParam._height * 0.5f, boxParam._depth * 0.5f };
            pushPosition({ -halfExtents._x, +halfExtents._y, -halfExtents._z }, meshData);
            pushPosition({ +halfExtents._x, +halfExtents._y, -halfExtents._z }, meshData);
            pushPosition({ +halfExtents._x, +halfExtents._y, +halfExtents._z }, meshData);
            pushPosition({ -halfExtents._x, +halfExtents._y, +halfExtents._z }, meshData);
            pushPosition({ -halfExtents._x, -halfExtents._y, -halfExtents._z }, meshData);
            pushPosition({ +halfExtents._x, -halfExtents._y, -halfExtents._z }, meshData);
            pushPosition({ +halfExtents._x, -halfExtents._y, +halfExtents._z }, meshData);
            pushPosition({ -halfExtents._x, -halfExtents._y, +halfExtents._z }, meshData);

            const Float2 uvs[4]{ Float2(0.0f, 0.0f), Float2(1.0f, 0.0f), Float2(1.0f, 1.0f), Float2(0.0f, 1.0f) };
            pushQuad({ 3, 2, 1, 0 }, meshData, uvs); // Top
            pushQuad({ 4, 5, 6, 7 }, meshData, uvs); // Bottom
            _pushRingQuads(0, 4, meshData);
        }

        void MeshGenerator::generateCone(const ConeParam& coneParam, MeshData& meshData) noexcept
        {
            meshData.clear();

            // Position
            {
                meshData._positionArray.reserve(coneParam._sideCount + 2);

                pushPosition({ 0.0f, coneParam._height, 0.0f }, meshData);

                _pushCirclularPoints(coneParam._radius, 0.0f, coneParam._sideCount, meshData);

                pushPosition({ 0.0f, 0.0f, 0.0f }, meshData);
            }

            const int32 positionIndexTopCenter      = 0;
            const int32 positionIndexBottomCenter   = static_cast<int32>(meshData._positionArray.size() - 1);

            // Cone sides
            _pushUpperUmbrellaTris(positionIndexTopCenter, 1, static_cast<uint8>(coneParam._sideCount), meshData);
            if (coneParam._smooth == true)
            {
                smoothNormals(meshData);
            }

            // Cone bottom
            _pushLowerUmbrellaTris(positionIndexBottomCenter, 1, static_cast<uint8>(coneParam._sideCount), meshData);
        }

        void MeshGenerator::generateCylinder(const CylinderParam& cylinderParam, MeshData& meshData) noexcept
        {
            meshData.clear();

            // Position
            {
                meshData._positionArray.reserve(cylinderParam._sideCount * 2 + 2);

                pushPosition({ 0.0f, cylinderParam._height, 0.0f }, meshData);

                _pushCirclularPoints(cylinderParam._radius, cylinderParam._height, cylinderParam._sideCount, meshData);
                _pushCirclularPoints(cylinderParam._radius, 0.0f, cylinderParam._sideCount, meshData);

                pushPosition({ 0.0f, 0.0f, 0.0f }, meshData);
            }

            const int32 positionIndexTopCenter      = 0;
            const int32 positionIndexBottomCenter   = static_cast<int32>(meshData._positionArray.size() - 1);

            // Clylinder sides
            _pushRingQuads(1, static_cast<uint8>(cylinderParam._sideCount), meshData);
            if (cylinderParam._smooth == true)
            {
                smoothNormals(meshData);
            }

            // Clylinder top
            _pushUpperUmbrellaTris(positionIndexTopCenter, 1, static_cast<uint8>(cylinderParam._sideCount), meshData);
            
            // Clylinder bottom
            _pushLowerUmbrellaTris(positionIndexBottomCenter, 1 + cylinderParam._sideCount, static_cast<uint8>(cylinderParam._sideCount), meshData);
        }

        void MeshGenerator::generateSphere(const SphereParam& sphereParam, MeshData& meshData) noexcept
        {
            meshData.clear();

            const float radius = sphereParam._radius;
            const uint8 polarDetail = sphereParam._polarDetail;
            const uint8 azimuthalDetail = sphereParam._azimuthalDetail;

            // polarAngle: 0 at +Y, Pi at -Y
            // azimuthalAngle: 0 at +X, rotates around +Y axis (counter-clockwise) all the way around back to +X again
            const float polarAngleStep = Math::kPi / polarDetail;

            // Position
            meshData._positionArray.reserve(1 + (polarDetail - 1) * azimuthalDetail + 1);
            for (uint8 polarIter = 0; polarIter < polarDetail + 1; ++polarIter)
            {
                const float polarAngle = polarAngleStep * polarIter;
                _pushCirclularPoints(radius * ::sin(polarAngle), radius * ::cos(polarAngle), azimuthalDetail, meshData);
            }

            // Tris and quads
            for (uint8 polarIter = 0; polarIter < polarDetail; ++polarIter)
            {
                const int32 indexBase = 1 + max(polarIter - 1, 0) * azimuthalDetail;
                if (polarIter == 0)
                {
                    // Top center included
                    _pushUpperUmbrellaTris(0, indexBase, azimuthalDetail, meshData);
                }
                else if (polarIter == polarDetail - 1)
                {
                    // Bottom center included
                    const int32 bottomCenterIndex = static_cast<int32>(meshData.getPositionCount() - 1);
                    _pushLowerUmbrellaTris(bottomCenterIndex, indexBase, azimuthalDetail, meshData);
                }
                else
                {
                    // Middle quads
                    _pushRingQuads(indexBase, azimuthalDetail, meshData);
                }
            }

            if (sphereParam._smooth == true)
            {
                smoothNormals(meshData);
            }
        }

        void MeshGenerator::generateOctahedron(const RadiusParam& radiusParam, MeshData& meshData) noexcept
        {
            meshData.clear();

            // Position
            {
                meshData._positionArray.reserve(6);
                pushPosition({ 0.0f, +radiusParam._radius, 0.0f }, meshData);

                _pushCirclularPoints(radiusParam._radius, 0.0f, 4, meshData);
                
                pushPosition({ 0.0f, -radiusParam._radius, 0.0f }, meshData);
            }

            const int32 positionIndexTopCenter = 0;
            const int32 positionIndexBottomCenter = static_cast<int32>(meshData._positionArray.size() - 1);
            _pushUpperUmbrellaTris(positionIndexTopCenter, 1, 4, meshData);
            _pushLowerUmbrellaTris(positionIndexBottomCenter, 1, 4, meshData);
        }

        void MeshGenerator::generateHalfOpenOctahedron(const RadiusParam& radiusParam, MeshData& meshData) noexcept
        {
            meshData.clear();

            // Position
            {
                meshData._positionArray.reserve(6);
                pushPosition({ 0.0f, +radiusParam._radius, 0.0f }, meshData);
                
                _pushCirclularPoints(radiusParam._radius, 0.0f, 4, meshData);
            }

            const int32 positionIndexTopCenter = 0;
            _pushUpperUmbrellaTris(positionIndexTopCenter, 1, 4, meshData);
        }

        void MeshGenerator::generateGeoSphere(const GeoSphereParam& geoSphereParam, MeshData& meshData) noexcept
        {
            RadiusParam radiusParam;
            radiusParam._radius = geoSphereParam._radius;

            generateOctahedron(radiusParam, meshData);

            subdivideTriByMidpoints(meshData); // At least once
            for (int8 iteration = 0; iteration < geoSphereParam._subdivisionIteration; ++iteration)
            {
                subdivideTriByMidpoints(meshData);
            }

            projectVerticesToSphere(radiusParam, meshData);

            if (geoSphereParam._smooth == true)
            {
                smoothNormals(meshData);
            }
        }

        void MeshGenerator::generateHalfOpenGeoSphere(const GeoSphereParam& geoSphereParam, MeshData& meshData) noexcept
        {
            RadiusParam radiusParam;
            radiusParam._radius = geoSphereParam._radius;

            generateHalfOpenOctahedron(radiusParam, meshData);

            subdivideTriByMidpoints(meshData); // At least once
            for (int8 iteration = 0; iteration < geoSphereParam._subdivisionIteration; ++iteration)
            {
                subdivideTriByMidpoints(meshData);
            }

            projectVerticesToSphere(radiusParam, meshData);

            if (geoSphereParam._smooth == true)
            {
                smoothNormals(meshData);
            }
        }

        void MeshGenerator::generateCapsule(const CapsulePram& capsulePram, MeshData& meshData) noexcept
        {
            meshData.clear();
            
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
                meshData._positionArray.reserve(1 + (polarDetail - 1) * azimuthalDetail + 1);

                // Upper half-sphere
                for (uint8 polarIter = 0; polarIter < halfPolarDetail + 1; ++polarIter)
                {
                    const float polarAngle = polarAngleStep * polarIter;
                    _pushCirclularPoints(radius * ::sin(polarAngle), radius * ::cos(polarAngle) + halfHeight, azimuthalDetail, meshData);
                }

                // Lower half-sphere
                for (uint8 polarIter = 0; polarIter < halfPolarDetail + 1; ++polarIter)
                {
                    const float polarAngle = polarAngleStep * (polarIter + halfPolarDetail);
                    _pushCirclularPoints(radius * ::sin(polarAngle), radius * ::cos(polarAngle) - halfHeight, azimuthalDetail, meshData);
                }
            }

            // Tris and quads
            for (uint8 polarIter = 0; polarIter < polarDetail + 1; ++polarIter)
            {
                const int32 indexBase = 1 + max(polarIter - 1, 0) * azimuthalDetail;
                if (polarIter == 0)
                {
                    // Top center included
                    _pushUpperUmbrellaTris(0, indexBase, azimuthalDetail, meshData);
                }
                else if (polarIter == polarDetail)
                {
                    // Bottom center included
                    const int32 bottomCenterIndex = static_cast<int32>(meshData.getPositionCount() - 1);
                    _pushLowerUmbrellaTris(bottomCenterIndex, indexBase, azimuthalDetail, meshData);
                }
                else
                {
                    // Middle quads
                    _pushRingQuads(indexBase, azimuthalDetail, meshData);
                }
            }

            if (capsulePram._smooth == true)
            {
                smoothNormals(meshData);
            }
        }

        void MeshGenerator::_pushCirclularPoints(const float radius, const float y, const uint32 sideCount, MeshData& meshData) noexcept
        {
            if (radius < Math::kFloatEpsilon)
            {
                pushPosition({ 0.0f, y, 0.0f }, meshData);
                return;
            }

            const float angleStep = Math::kTwoPi / sideCount;
            for (uint8 sideIndex = 0; sideIndex < sideCount; ++sideIndex)
            {
                const float angle = angleStep * sideIndex;
                pushPosition({ ::cos(angle) * radius, y, ::sin(angle) * radius }, meshData);
            }
        }

        void MeshGenerator::_pushUpperUmbrellaTris(const int32 centerIndex, const int32 indexBase, const uint8 triangleCount, MeshData& meshData) noexcept
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
                pushTri({ centerIndex, indexBase + triangleIndex + 1, indexBase + triangleIndex }, meshData, uvs);
            }
            pushTri({ centerIndex, indexBase, indexBase + triangleCount - 1 }, meshData, uvs);
        }

        void MeshGenerator::_pushLowerUmbrellaTris(const int32 centerIndex, const int32 indexBase, const uint8 triangleCount, MeshData& meshData) noexcept
        {
            const Float2 uvs[3]{ Float2(0.0f, 0.0f), Float2(1.0f, 0.0f), Float2(1.0f, 1.0f) };
            for (uint8 triangleIndex = 0; triangleIndex < triangleCount - 1; ++triangleIndex)
            {
                pushTri({ indexBase + triangleIndex, indexBase + triangleIndex + 1, centerIndex }, meshData, uvs);
            }
            pushTri({ indexBase + triangleCount - 1, indexBase, centerIndex }, meshData, uvs);
        }

        void MeshGenerator::_pushRingQuads(const int32 indexBase, const uint8 quadCount, MeshData& meshData) noexcept
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
                pushQuad(
                    { indexBase + quadIndex,
                      indexBase + quadIndex + 1,
                      indexBase + quadIndex + quadCount + 1,
                      indexBase + quadIndex + quadCount,
                    }, meshData, uvs);
            }

            pushQuad(
                { indexBase + quadCount - 1,
                  indexBase,
                  indexBase + quadCount,
                  indexBase + quadCount - 1 + quadCount,
                }, meshData, uvs);
        }

        void MeshGenerator::setMaterialId(MeshData& meshData, const uint32 materialId) noexcept
        {
            const uint32 vertexCount = meshData._vertexArray.size();
            for (uint32 vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
            {
                meshData._vertexArray[vertexIndex]._materialId = materialId;
            }
        }

        void MeshGenerator::transformMeshData(MeshData& meshData, const Float4x4& transformationMatrix) noexcept
        {
            const uint32 positionCount = meshData.getPositionCount();
            for (uint32 positionIndex = 0; positionIndex < positionCount; ++positionIndex)
            {
                Float4& position = meshData._positionArray[positionIndex];
                position = transformationMatrix.mul(position);
            }
            meshData.updateVertexFromPositions();
        }

        void MeshGenerator::mergeMeshData(const MeshData& meshDataA, const MeshData& meshDataB, MeshData& outMeshData) noexcept
        {
            mergeMeshData(meshDataA, outMeshData);
            mergeMeshData(meshDataB, outMeshData);
        }

        void MeshGenerator::mergeMeshData(const MeshData& sourceMeshData, MeshData& inoutTargetMeshData) noexcept
        {
            if (inoutTargetMeshData.isEmpty() == true)
            {
                inoutTargetMeshData = sourceMeshData;
                return;
            }

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
                inoutTargetMeshData._vertexToPositionTable.push_back(oldPositionCount + sourceMeshData._vertexToPositionTable[deltaVertexIndex]);
            }

            const uint32 deltaFaceCount = sourceMeshData.getFaceCount();
            inoutTargetMeshData._faceArray.reserve(inoutTargetMeshData._faceArray.size() + deltaFaceCount);
            Face face;
            for (uint32 deltaFaceIndex = 0; deltaFaceIndex < deltaFaceCount; ++deltaFaceIndex)
            {
                face._vertexIndexArray[0] = oldVertexCount + sourceMeshData._faceArray[deltaFaceIndex]._vertexIndexArray[0];
                face._vertexIndexArray[1] = oldVertexCount + sourceMeshData._faceArray[deltaFaceIndex]._vertexIndexArray[1];
                face._vertexIndexArray[2] = oldVertexCount + sourceMeshData._faceArray[deltaFaceIndex]._vertexIndexArray[2];

                inoutTargetMeshData._faceArray.push_back(face);
            }
        }
        
        MINT_INLINE void MeshGenerator::pushPosition(const float(&xyz)[3], MeshData& meshData) noexcept
        {
            meshData._positionArray.push_back(Float4(xyz[0], xyz[1], xyz[2], 1.0f));
        }

        MINT_INLINE void MeshGenerator::pushPosition(const Float4& xyzw, MeshData& meshData) noexcept
        {
            meshData._positionArray.push_back(xyzw);
        }

        void MeshGenerator::pushTri(const int32(&positionIndices)[3], MeshData& meshData, const Float2(&uvs)[3]) noexcept
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

        void MeshGenerator::pushQuad(const int32(&positionIndices)[4], MeshData& meshData, const Float2(&uvs)[4]) noexcept
        {
            const uint32 vertexCountOld = meshData.getVertexCount();

            pushVertexWithPositionXXX(positionIndices[0], meshData);
            pushVertexWithPositionXXX(positionIndices[1], meshData);
            pushVertexWithPositionXXX(positionIndices[2], meshData);
            pushVertexWithPositionXXX(positionIndices[3], meshData);

            setVertexUv(meshData, vertexCountOld + 0, uvs[0]._x, uvs[0]._y);
            setVertexUv(meshData, vertexCountOld + 1, uvs[1]._x, uvs[1]._y);
            setVertexUv(meshData, vertexCountOld + 2, uvs[2]._x, uvs[2]._y);
            setVertexUv(meshData, vertexCountOld + 3, uvs[3]._x, uvs[3]._y);

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
                    : _positionIndexA{ min(positionIndexA, positionIndexB) }
                    , _positionIndexB{ max(positionIndexA, positionIndexB) } 
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
                int32           _positionCount = 0;
                Vector<int32>   _edgeTable;
            };

            PositionEdgeGraph positionEdgeGraph;
            positionEdgeGraph.setPositionCount(oldMeshData.getPositionCount());
            const uint32 oldFaceCount = static_cast<uint32>(oldMeshData._faceArray.size());
            for (uint32 oldFaceIndex = 0; oldFaceIndex < oldFaceCount; ++oldFaceIndex)
            {
                const Face& face = oldMeshData._faceArray[oldFaceIndex]; 
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

                    const Float4& faceVertexPosition0 = getFaceVertexPosition0(face, oldMeshData);
                    const Float4& faceVertexPosition1 = getFaceVertexPosition1(face, oldMeshData);
                    const Float4& faceVertexPosition2 = getFaceVertexPosition2(face, oldMeshData);

                    const int32 newPositionIndexBase = static_cast<int32>(newMeshData._positionArray.size());
                    int8 addedPointCount = 0;
                    // Midpoint 0-1
                    if (positionEdgeGraph.hasMidpoint(positionEdge01) == false)
                    {
                        Float4 midPointPosition_01 = (faceVertexPosition0 + faceVertexPosition1) * 0.5f;
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
                        Float4 midPointPosition_12 = (faceVertexPosition1 + faceVertexPosition2) * 0.5f;
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
                        Float4 midPointPosition_20 = (faceVertexPosition2 + faceVertexPosition0) * 0.5f;
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
                const Float2 faceVertexUvs[3]
                {
                    getVertexUv(getFaceVertex0(face, oldMeshData)),
                    getVertexUv(getFaceVertex1(face, oldMeshData)),
                    getVertexUv(getFaceVertex2(face, oldMeshData))
                };
                const Float2 midPointUv01 = (faceVertexUvs[0] + faceVertexUvs[1]) * 0.5f;
                const Float2 midPointUv12 = (faceVertexUvs[1] + faceVertexUvs[2]) * 0.5f;
                const Float2 midPointUv20 = (faceVertexUvs[2] + faceVertexUvs[0]) * 0.5f;

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
            Float4 vertexNormals[3];
            Float4 faceNormal;
            const uint32 faceCount = meshData.getFaceCount();
            for (uint32 faceIndex = 0; faceIndex < faceCount; ++faceIndex)
            {
                const Face& face = meshData._faceArray[faceIndex];
                
                Float4& faceVertexPosition0 = getFaceVertexPosition0(face, meshData);
                Float4& faceVertexPosition1 = getFaceVertexPosition1(face, meshData);
                Float4& faceVertexPosition2 = getFaceVertexPosition2(face, meshData);

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

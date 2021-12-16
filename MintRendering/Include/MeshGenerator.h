#pragma once


#ifndef MINT_MESH_GENERATOR_H
#define MINT_MESH_GENERATOR_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintRenderingBase/Include/MeshData.h>


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
                float     _width = 1.0f;
                float     _height = 1.0f;
                float     _depth = 1.0f;
            };
            
            struct ConeParam
            {
                int16     _sideCount = 16;
                float     _radius = 1.0f;
                float     _height = 2.0f;
                bool      _smooth = false;
            };

            struct CylinderParam
            {
                int16     _sideCount = 16;
                float     _radius = 1.0f;
                float     _height = 2.0f;
                bool      _smooth = false;
            };

            struct RadiusParam
            {
                float     _radius = 1.0f;
            };

            struct GeoSphereParam
            {
                float     _radius = 1.0f;
                uint8     _subdivisionIteration = 1;
                bool      _smooth = false;
            };

            struct SphereParam
            {
                float     _radius = 1.0f;
                uint8     _polarDetail = 4;
                uint8     _azimuthalDetail = 8;
                bool      _smooth = false;
            };

            struct CapsulePram
            {
                float     _sphereRadius = 1.0f;
                float     _cylinderHeight = 1.0f;
                uint8     _subdivisionIteration = 1;
                bool      _smooth = false;
            };

        public:
            static void             generateBox(const BoxParam& boxParam, MeshData& meshData) noexcept;
            static void             generateCone(const ConeParam& coneParam, MeshData& meshData) noexcept;
            static void             generateCylinder(const CylinderParam& cylinderParam, MeshData& meshData) noexcept;
            static void             generateSphere(const SphereParam& sphereParam, MeshData& meshData) noexcept;
            static void             generateOctahedron(const RadiusParam& radiusParam, MeshData& meshData) noexcept;
            static void             generateHalfOpenOctahedron(const RadiusParam& radiusParam, MeshData& meshData) noexcept;
            static void             generateGeoSphere(const GeoSphereParam& geoSphereParam, MeshData& meshData) noexcept;
            static void             generateHalfOpenGeoSphere(const GeoSphereParam& geoSphereParam, MeshData& meshData) noexcept;
            static void             generateCapsule(const CapsulePram& capsulePram, MeshData& meshData) noexcept;

        private:
            static void             _pushCirclularPoints(const float radius, const float y, const uint32 sideCount, MeshData& meshData) noexcept;
            static void             _pushUpperUmbrellaTris(const int32 centerIndex, const int32 indexBase, const uint8 triangleCount, MeshData& meshData) noexcept;
            static void             _pushLowerUmbrellaTris(const int32 centerIndex, const int32 indexBase, const uint8 triangleCount, MeshData& meshData) noexcept;
            static void             _pushRingQuads(const int32 indexBase, const uint8 quadCount, MeshData& meshData) noexcept;

        public:
            static void             setMaterialId(MeshData& meshData, const uint32 materialId) noexcept;
            static void             transformMeshData(MeshData& meshData, const Float4x4& transformationMatrix) noexcept;
            static void             mergeMeshData(const MeshData& meshDataA, const MeshData& meshDataB, MeshData& outMeshData) noexcept;
            static void             mergeMeshData(const MeshData& sourceMeshData, MeshData& inoutTargetMeshData) noexcept;

        private:
            static void             pushPosition(const float(&xyz)[3], MeshData& meshData) noexcept;
            static void             pushPosition(const Float4& xyzw, MeshData& meshData) noexcept;
            static void             pushTri(const int32(&positionIndices)[3], MeshData& meshData, const Float2(&uvs)[3]) noexcept;
            static void             pushQuad(const int32(&positionIndices)[4], MeshData& meshData, const Float2(&uvs)[4]) noexcept;

        private:
            static void             subdivideTriByMidpoints(MeshData& oldMeshData) noexcept;
            static void             projectVerticesToSphere(const RadiusParam& radiusParam, MeshData& meshData) noexcept;

        private:
            static void             pushVertexWithPositionXXX(const uint32 positionIndex, MeshData& meshData) noexcept;
            static void             setVertexUv(MeshData& meshData, const uint32 vertexIndex, const float u, const float v) noexcept;
            static void             setVertexUv(VS_INPUT& vertex, const Float2& uv) noexcept;
            static Float2           getVertexUv(const VS_INPUT& inoutVertex) noexcept;
            static VS_INPUT&        getFaceVertex0(const Face& face, MeshData& meshData) noexcept;
            static VS_INPUT&        getFaceVertex1(const Face& face, MeshData& meshData) noexcept;
            static VS_INPUT&        getFaceVertex2(const Face& face, MeshData& meshData) noexcept;
            static const uint32     getFaceVertexPositionIndex0(const Face& face, MeshData& meshData) noexcept;
            static const uint32     getFaceVertexPositionIndex1(const Face& face, MeshData& meshData) noexcept;
            static const uint32     getFaceVertexPositionIndex2(const Face& face, MeshData& meshData) noexcept;
            static Float4&          getFaceVertexPosition0(const Face& face, MeshData& meshData) noexcept;
            static Float4&          getFaceVertexPosition1(const Face& face, MeshData& meshData) noexcept;
            static Float4&          getFaceVertexPosition2(const Face& face, MeshData& meshData) noexcept;
            static void             pushTriFaceXXX(const uint32 vertexOffset, MeshData& meshData) noexcept;
            static void             pushQuadFaceXXX(const uint32 vertexOffset, MeshData& meshData) noexcept;
            static void             recalculateTangentBitangentFromNormal(const Float4& normal, VS_INPUT& vertex) noexcept;
            static Float4           computeNormalFromTangentBitangent(const VS_INPUT& vertex) noexcept;

        public:
            static void             calculateTangentBitangent(const Face& face, Vector<VS_INPUT>& inoutVertexArray) noexcept;
            static void             smoothNormals(MeshData& meshData) noexcept;
        };
    }
}


#endif // !MINT_MESH_GENERATOR_H

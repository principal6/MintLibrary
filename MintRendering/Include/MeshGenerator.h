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

            struct GeoSpherePram
            {
                float     _radius = 1.0f;
                uint8     _subdivisionIteration = 1;
                bool      _smooth = false;
            };

        public:
            static void                             generateCube(mint::Rendering::MeshData& meshData) noexcept;
            static void                             generateCone(const ConeParam& coneParam, mint::Rendering::MeshData& meshData) noexcept;
            static void                             generateCylinder(const CylinderParam& cylinderParam, mint::Rendering::MeshData& meshData) noexcept;
            static void                             generateOctahedron(const RadiusParam& radiusParam, mint::Rendering::MeshData& meshData) noexcept;
            static void                             generateGeoSphere(const GeoSpherePram& geoSpherePram, mint::Rendering::MeshData& meshData) noexcept;

        public:
            static void                             setMaterialId(mint::Rendering::MeshData& meshData, const uint32 materialId) noexcept;
            static void                             transformMeshData(mint::Rendering::MeshData& meshData, const mint::Float4x4& transformationMatrix) noexcept;
            static void                             mergeMeshData(const mint::Rendering::MeshData& meshDataA, const mint::Rendering::MeshData& meshDataB, mint::Rendering::MeshData& outMeshData) noexcept;
            static void                             mergeMeshData(const mint::Rendering::MeshData& sourceMeshData, mint::Rendering::MeshData& inoutTargetMeshData) noexcept;

        private:
            static void                             pushPosition(const float(&xyz)[3], mint::Rendering::MeshData& meshData) noexcept;
            static void                             pushPosition(const mint::Float4& xyzw, mint::Rendering::MeshData& meshData) noexcept;
            static void                             pushTri(const int32(&positionIndices)[3], mint::Rendering::MeshData& meshData, const mint::Float2(&uvs)[3]) noexcept;
            static void                             pushQuad(const int32(&positionIndicesInClockwise)[4], mint::Rendering::MeshData& meshData, const mint::Float2(&uvsInClockwise)[4]) noexcept;
        
        private:
            static void                             subdivideTriByMidpoints(mint::Rendering::MeshData& oldMeshData) noexcept;
            static void                             projectVerticesToSphere(const RadiusParam& radiusParam, mint::Rendering::MeshData& meshData) noexcept;

        private:
            static void                             pushVertexWithPositionXXX(const uint32 positionIndex, mint::Rendering::MeshData& meshData) noexcept;
            static void                             setVertexUv(mint::Rendering::MeshData& meshData, const uint32 vertexIndex, const float u, const float v) noexcept;
            static void                             setVertexUv(mint::Rendering::VS_INPUT& vertex, const mint::Float2& uv) noexcept;
            static mint::Float2                     getVertexUv(const mint::Rendering::VS_INPUT& inoutVertex) noexcept;
            static mint::Rendering::VS_INPUT&   getFaceVertex0(const mint::Rendering::Face& face, mint::Rendering::MeshData& meshData) noexcept;
            static mint::Rendering::VS_INPUT&   getFaceVertex1(const mint::Rendering::Face& face, mint::Rendering::MeshData& meshData) noexcept;
            static mint::Rendering::VS_INPUT&   getFaceVertex2(const mint::Rendering::Face& face, mint::Rendering::MeshData& meshData) noexcept;
            static const uint32                     getFaceVertexPositionIndex0(const mint::Rendering::Face& face, mint::Rendering::MeshData& meshData) noexcept;
            static const uint32                     getFaceVertexPositionIndex1(const mint::Rendering::Face& face, mint::Rendering::MeshData& meshData) noexcept;
            static const uint32                     getFaceVertexPositionIndex2(const mint::Rendering::Face& face, mint::Rendering::MeshData& meshData) noexcept;
            static mint::Float4&                    getFaceVertexPosition0(const mint::Rendering::Face& face, mint::Rendering::MeshData& meshData) noexcept;
            static mint::Float4&                    getFaceVertexPosition1(const mint::Rendering::Face& face, mint::Rendering::MeshData& meshData) noexcept;
            static mint::Float4&                    getFaceVertexPosition2(const mint::Rendering::Face& face, mint::Rendering::MeshData& meshData) noexcept;
            static void                             pushTriFaceXXX(const uint32 vertexOffset, mint::Rendering::MeshData& meshData) noexcept;
            static void                             pushQuadFaceXXX(const uint32 vertexOffset, mint::Rendering::MeshData& meshData) noexcept;
            static void                             recalculateTangentBitangentFromNormal(const mint::Float4& normal, mint::Rendering::VS_INPUT& vertex) noexcept;
            static mint::Float4                     computeNormalFromTangentBitangent(const mint::Rendering::VS_INPUT& vertex) noexcept;

        public:
            static void                             calculateTangentBitangent(const mint::Rendering::Face& face, mint::Vector<mint::Rendering::VS_INPUT>& inoutVertexArray) noexcept;
            static void                             smoothNormals(mint::Rendering::MeshData& meshData) noexcept;
        };
    }
}


#endif // !MINT_MESH_GENERATOR_H

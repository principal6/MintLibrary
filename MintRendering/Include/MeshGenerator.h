#pragma once


#ifndef MINT_MESH_GENERATOR_H
#define MINT_MESH_GENERATOR_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>


namespace mint
{
    class Float2;
    class Float4;
    class Float4x4;


    namespace RenderingBase
    {
        struct VS_INPUT;
        struct Face;
    }


    namespace Rendering
    {
        struct MeshData
        {
        public:
                            MeshData();

        public:
            void            clear() noexcept;
            void            shrinkToFit() noexcept;
            void            updateVertexFromPositions() noexcept;
            const uint32    getPositionCount() const noexcept;
            const uint32    getVertexCount() const noexcept;
            const uint32    getFaceCount() const noexcept;

        public:
            static const mint::RenderingBase::VS_INPUT            kNullVertex;
            static const mint::RenderingBase::IndexElementType    kNullIndex;

        public:
            mint::Vector<mint::Float4>                              _positionArray;
            mint::Vector<uint32>                                  _vertexToPositionTable;
            mint::Vector<mint::RenderingBase::VS_INPUT>             _vertexArray;
            mint::Vector<mint::RenderingBase::Face>                 _faceArray;
        };


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
            static void                             generateCube(MeshData& meshData) noexcept;
            static void                             generateCone(const ConeParam& coneParam, MeshData& meshData) noexcept;
            static void                             generateCylinder(const CylinderParam& cylinderParam, MeshData& meshData) noexcept;
            static void                             generateOctahedron(const RadiusParam& radiusParam, MeshData& meshData) noexcept;
            static void                             generateGeoSphere(const GeoSpherePram& geoSpherePram, MeshData& meshData) noexcept;

        public:
            static void                             transformMeshData(MeshData& meshData, const mint::Float4x4& transformationMatrix) noexcept;
            static void                             mergeMeshData(const MeshData& meshDataA, const MeshData& meshDataB, MeshData& outMeshData) noexcept;
            static void                             mergeMeshData(const MeshData& sourceMeshData, MeshData& inoutTargetMeshData) noexcept;

        private:
            static void                             pushPosition(const float(&xyz)[3], MeshData& meshData) noexcept;
            static void                             pushPosition(const mint::Float4& xyzw, MeshData& meshData) noexcept;
            static void                             pushTri(const int32(&positionIndices)[3], MeshData& meshData, const mint::Float2(&uvs)[3]) noexcept;
            static void                             pushQuad(const int32(&positionIndicesInClockwise)[4], MeshData& meshData, const mint::Float2(&uvsInClockwise)[4]) noexcept;
        
        private:
            static void                             subdivideTriByMidpoints(MeshData& oldMeshData) noexcept;
            static void                             projectVerticesToSphere(const RadiusParam& radiusParam, MeshData& meshData) noexcept;

        private:
            static void                             pushVertexWithPositionXXX(const uint32 positionIndex, MeshData& meshData) noexcept;
            static void                             setVertexUv(MeshData& meshData, const uint32 vertexIndex, const float u, const float v) noexcept;
            static void                             setVertexUv(mint::RenderingBase::VS_INPUT& vertex, const mint::Float2& uv) noexcept;
            static mint::Float2                       getVertexUv(const mint::RenderingBase::VS_INPUT& inoutVertex) noexcept;
            static mint::RenderingBase::VS_INPUT&     getFaceVertex0(const mint::RenderingBase::Face& face, MeshData& meshData) noexcept;
            static mint::RenderingBase::VS_INPUT&     getFaceVertex1(const mint::RenderingBase::Face& face, MeshData& meshData) noexcept;
            static mint::RenderingBase::VS_INPUT&     getFaceVertex2(const mint::RenderingBase::Face& face, MeshData& meshData) noexcept;
            static const uint32                     getFaceVertexPositionIndex0(const mint::RenderingBase::Face& face, MeshData& meshData) noexcept;
            static const uint32                     getFaceVertexPositionIndex1(const mint::RenderingBase::Face& face, MeshData& meshData) noexcept;
            static const uint32                     getFaceVertexPositionIndex2(const mint::RenderingBase::Face& face, MeshData& meshData) noexcept;
            static mint::Float4&                      getFaceVertexPosition0(const mint::RenderingBase::Face& face, MeshData& meshData) noexcept;
            static mint::Float4&                      getFaceVertexPosition1(const mint::RenderingBase::Face& face, MeshData& meshData) noexcept;
            static mint::Float4&                      getFaceVertexPosition2(const mint::RenderingBase::Face& face, MeshData& meshData) noexcept;
            static void                             pushTriFaceXXX(const uint32 vertexOffset, MeshData& meshData) noexcept;
            static void                             pushQuadFaceXXX(const uint32 vertexOffset, MeshData& meshData) noexcept;
            static void                             recalculateTangentBitangentFromNormal(const mint::Float4& normal, mint::RenderingBase::VS_INPUT& vertex) noexcept;
            static mint::Float4                       computeNormalFromTangentBitangent(const mint::RenderingBase::VS_INPUT& vertex) noexcept;

        public:
            static void                             calculateTangentBitangent(const mint::RenderingBase::Face& face, mint::Vector<mint::RenderingBase::VS_INPUT>& inoutVertexArray) noexcept;
            static void                             smoothNormals(MeshData& meshData) noexcept;
        };
    }
}


#endif // !MINT_MESH_GENERATOR_H

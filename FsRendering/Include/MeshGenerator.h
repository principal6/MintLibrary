#pragma once


#ifndef FS_MESH_GENERATOR_H
#define FS_MESH_GENERATOR_H


#include <CommonDefinitions.h>

#include <FsRenderingBase/Include/RenderingBaseCommon.h>


namespace fs
{
    class Float2;
    class Float4;

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
            const uint32    getPositionCount() const noexcept;
            const uint32    getVertexCount() const noexcept;
            const uint32    getFaceCount() const noexcept;

        public:
            static const fs::RenderingBase::VS_INPUT            kNullVertex;
            static const fs::RenderingBase::IndexElementType    kNullIndex;

        public:
            std::vector<fs::Float4>                             _positionArray;
            std::vector<uint32>                                 _vertexToPositionTable;
            std::vector<fs::RenderingBase::VS_INPUT>            _vertexArray;
            std::vector<fs::RenderingBase::Face>                _faceArray;
        };


        class MeshGenerator abstract final
        {
        public:
            struct ConeParam
            {
                const int16     _sideCount = 16;
                const float     _radius = 1.0f;
                const float     _height = 2.0f;
            };

            struct CylinderParam
            {
                const int16     _sideCount = 16;
                const float     _radius = 1.0f;
                const float     _height = 2.0f;
            };

            struct OctahedronParam
            {
                const float     _radius = 1.0f;
            };

        public:
            static void         generateCube(MeshData& meshData) noexcept;
            static void         generateCone(const ConeParam& coneParam, MeshData& meshData) noexcept;
            static void         generateCylinder(const CylinderParam& cylinderParam, MeshData& meshData) noexcept;
            static void         generateOctahedron(const OctahedronParam& octahedronParam, MeshData& meshData) noexcept;

        private:
            static void         pushTri(const int32(&positionIndices)[3], MeshData& meshData, const fs::Float2(&uvs)[3]) noexcept;
            static void         pushQuad(const int32(&positionIndicesInClockwise)[4], MeshData& meshData, const fs::Float2(&uvsInClockwise)[4]) noexcept;

        private:
            static void         pushVertexWithPositionXXX(const uint32 positionIndex, MeshData& meshData) noexcept;
            static void         setVertexUv(MeshData& meshData, const uint32 vertexIndex, const float u, const float v) noexcept;
            static fs::Float2   getVertexUv(const fs::RenderingBase::VS_INPUT& inoutVertex) noexcept;
            static void         pushTriFaceXXX(const uint32 vertexOffset, MeshData& meshData) noexcept;
            static void         pushQuadFaceXXX(const uint32 vertexOffset, MeshData& meshData) noexcept;

        public:
            static void         calculateTangentBitangent(const fs::RenderingBase::Face& face, std::vector<fs::RenderingBase::VS_INPUT>& inoutVertexArray) noexcept;
            static void         smoothNormals(MeshData& meshData) noexcept;
        };
    }
}


#endif // !FS_MESH_GENERATOR_H

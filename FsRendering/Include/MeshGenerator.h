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
            static void         generateCube(MeshData& meshData) noexcept;

        private:
            static void         prepareCubeQuadFace(const int32(&positionIndices)[4], MeshData& meshData) noexcept;

        private:
            static void         pushVertexWithPosition(const uint32 positionIndex, MeshData& meshData) noexcept;
            static void         setVertexUv(MeshData& meshData, const uint32 vertexIndex, const float u, const float v) noexcept;
            static fs::Float2   getVertexUv(const fs::RenderingBase::VS_INPUT& inoutVertex) noexcept;
            static void         generateQuadFace(const uint32 vertexOffset, MeshData& meshData) noexcept;

        public:
            static void         calculateTangentBitangent(const fs::RenderingBase::Face& face, std::vector<fs::RenderingBase::VS_INPUT>& inoutVertexArray) noexcept;
            static void         smoothNormals(MeshData& meshData) noexcept;
        };
    }
}


#endif // !FS_MESH_GENERATOR_H

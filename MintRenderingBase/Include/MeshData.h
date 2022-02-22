#pragma once


#ifndef MINT_RENDERING_BASE_MESH_DATA_H
#define MINT_RENDERING_BASE_MESH_DATA_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>

#include <Assets/Include/CppHlsl/CppHlslStreamData.h>


namespace mint
{
    class Float4;


    namespace Rendering
    {
        struct VS_INPUT;
        struct Face;


        struct MeshData
        {
        public:
                                                MeshData();

        public:
            const bool                          isEmpty() const noexcept;
            void                                clear() noexcept;
            void                                shrinkToFit() noexcept;
            void                                updateVertexFromPositions() noexcept;

        public:
            const uint32                        getPositionCount() const noexcept;
            const uint32                        getVertexCount() const noexcept;
            const uint32                        getFaceCount() const noexcept;
            const uint32                        getIndexCount() const noexcept;
            const VS_INPUT*                     getVertices() const noexcept;
            const IndexElementType*             getIndices() const noexcept;

        public:
            static const VS_INPUT               kNullVertex;
            static const IndexElementType       kNullIndex;

        public:
            Vector<Float4>                      _positionArray;
            Vector<uint32>                      _vertexToPositionTable;
            Vector<VS_INPUT>                    _vertexArray;
            Vector<Face>                        _faceArray;
        };
    }
}


#endif // !MINT_RENDERING_BASE_MESH_DATA_H

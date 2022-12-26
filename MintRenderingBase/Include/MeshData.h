﻿#pragma once


#ifndef _MINT_RENDERING_BASE_MESH_DATA_H_
#define _MINT_RENDERING_BASE_MESH_DATA_H_


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
            bool                                isEmpty() const noexcept;
            void                                clear() noexcept;
            void                                shrinkToFit() noexcept;
            void                                updateVertexFromPositions() noexcept;

        public:
            uint32                              getPositionCount() const noexcept;
            uint32                              getVertexCount() const noexcept;
            uint32                              getFaceCount() const noexcept;
            uint32                              getIndexCount() const noexcept;
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


#endif // !_MINT_RENDERING_BASE_MESH_DATA_H_

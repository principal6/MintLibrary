#pragma once


#ifndef MINT_RENDERING_BASE_MESH_DATA_H
#define MINT_RENDERING_BASE_MESH_DATA_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>

#include <Assets/CppHlsl/CppHlslStreamData.h>


namespace mint
{
    class Float4;


    namespace RenderingBase
    {
        struct VS_INPUT;
        struct Face;


        struct MeshData
        {
        public:
                                MeshData();

        public:
            void                clear() noexcept;
            void                shrinkToFit() noexcept;
            void                updateVertexFromPositions() noexcept;
            const uint32        getPositionCount() const noexcept;
            const uint32        getVertexCount() const noexcept;
            const uint32        getFaceCount() const noexcept;
            const uint32        getIndexCount() const noexcept;
            const mint::RenderingBase::VS_INPUT*            getVertices() const noexcept;
            const mint::RenderingBase::IndexElementType*    getIndices() const noexcept;

        public:
            static const mint::RenderingBase::VS_INPUT          kNullVertex;
            static const mint::RenderingBase::IndexElementType  kNullIndex;

        public:
            mint::Vector<mint::Float4>                          _positionArray;
            mint::Vector<uint32>                                _vertexToPositionTable;
            mint::Vector<mint::RenderingBase::VS_INPUT>         _vertexArray;
            mint::Vector<mint::RenderingBase::Face>             _faceArray;
        };
    }
}


#endif // !MINT_RENDERING_BASE_MESH_DATA_H

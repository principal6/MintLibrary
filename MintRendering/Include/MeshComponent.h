#pragma once


#ifndef MINT_MESH_COMPONENT_H
#define MINT_MESH_COMPONENT_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintRenderingBase/Include/MeshData.h>

#include <MintRendering/Include/TransformComponent.h>
#include <MintRendering/Include/MeshGenerator.h>


namespace mint
{    
    namespace Rendering
    {
        class MeshComponent final : public TransformComponent
        {
        public:
                                                            MeshComponent();
            virtual                                         ~MeshComponent();

        public:
            const mint::RenderingBase::MeshData&            getMeshData() const noexcept;
            const uint32                                    getVertexCount() const noexcept;
            const mint::RenderingBase::VS_INPUT*            getVertices() const noexcept;
            const uint32                                    getIndexCount() const noexcept;
            const mint::RenderingBase::IndexElementType*    getIndices() const noexcept;

        public:
            void                                            shouldDrawNormals(const bool shouldDrawNormals) noexcept;
            const bool                                      shouldDrawNormals() const noexcept;
            void                                            shouldDrawEdges(const bool shouldDrawEdges) noexcept;
            const bool                                      shouldDrawEdges() const noexcept;

        private:
            mint::RenderingBase::MeshData                   _meshData;
            bool                                            _shouldDrawNormals;
            bool                                            _shouldDrawEdges;
        };
    }
}


#endif // !MINT_MESH_COMPONENT_H

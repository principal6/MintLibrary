#pragma once


#ifndef FS_MESH_COMPONENT_H
#define FS_MESH_COMPONENT_H


#include <CommonDefinitions.h>

#include <FsRenderingBase/Include/RenderingBaseCommon.h>

#include <FsRendering/Include/TransformComponent.h>
#include <FsRendering/Include/MeshGenerator.h>

namespace fs
{    
    namespace Rendering
    {
        class MeshComponent final : public TransformComponent
        {
        public:
                                                            MeshComponent();
            virtual                                         ~MeshComponent();

        public:
            const uint32                                    getVertexCount() const noexcept;
            const fs::RenderingBase::VS_INPUT*              getVertices() const noexcept;
            const uint32                                    getIndexCount() const noexcept;
            const fs::RenderingBase::IndexElementType*      getIndices() const noexcept;

        private:
            MeshData                                        _meshData;
        };
    }
}


#endif // !FS_MESH_COMPONENT_H

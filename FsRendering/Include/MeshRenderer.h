#pragma once


#ifndef FS_MESH_RENDERER_H
#define FS_MESH_RENDERER_H


#include <CommonDefinitions.h>

#include <FsRenderingBase/Include/TriangleRenderer.h>

#include <Assets/CppHlsl/CppHlslConstantBuffers.h>

#include <FsRendering/Include/ObjectPool.h>


namespace fs
{
    namespace RenderingBase
    {
        class GraphicDevice;
    }


    namespace Rendering
    {
        class MeshComponent;


        class MeshRenderer final
        {
        public:
                                                        MeshRenderer(fs::RenderingBase::GraphicDevice* const graphicDevice);
                                                        ~MeshRenderer();
        
        public:
            void                                        initialize() noexcept;
            void                                        render(const fs::Rendering::ObjectPool& objectPool) noexcept;

        private:
            fs::RenderingBase::GraphicDevice* const     _graphicDevice;

        private:
            fs::RenderingBase::TriangleRenderer<fs::RenderingBase::VS_INPUT>    _triangleRenderer;
            fs::RenderingBase::CB_Transform                                     _cbTransformData;
            fs::RenderingBase::DxObjectId                                       _cbTransformId;

        private:
            fs::RenderingBase::DxObjectId               _vertexShaderId;
            fs::RenderingBase::DxObjectId               _pixelShaderId;
        };
    }
}


#endif // !FS_MESH_RENDERER_H

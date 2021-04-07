#pragma once


#ifndef FS_LOW_LEVEL_RENDERER_H
#define FS_LOW_LEVEL_RENDERER_H


#include <CommonDefinitions.h>

#include <Assets/CppHlsl/CppHlslStreamData.h>

#include <FsRenderingBase/Include/DxResource.h>


namespace fs
{
    namespace RenderingBase
    {
        class GraphicDevice;


        enum class RenderingPrimitive
        {
            LineList,
            TriangleList,
        };


        template <typename T>
        class LowLevelRenderer
        {
        public:
                                                        LowLevelRenderer(fs::RenderingBase::GraphicDevice* const graphicDevice);
                                                        ~LowLevelRenderer() = default;

        public:
            std::vector<T>&                             vertexArray() noexcept;
            std::vector<IndexElementType>&              indexArray() noexcept;

        public:
            void                                        flush() noexcept;
            const bool                                  isRenderable() const noexcept;
            void                                        render(const RenderingPrimitive renderingPrimitive) noexcept;

        private:
            void                                        prepareBuffers() noexcept;

        private:
            fs::RenderingBase::GraphicDevice* const     _graphicDevice;
            const uint32                                _vertexStride;
        
        private:
            std::vector<T>                              _vertexArray;
            DxObjectId                                  _vertexBufferId;

        private:
            std::vector<IndexElementType>               _indexArray;
            DxObjectId                                  _indexBufferId;
        };
    }
}


#endif // !FS_LOW_LEVEL_RENDERER_H

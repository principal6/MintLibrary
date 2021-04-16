#pragma once


#ifndef FS_LOW_LEVEL_RENDERER_H
#define FS_LOW_LEVEL_RENDERER_H


#include <FsCommon/Include/CommonDefinitions.h>

#include <FsContainer/Include/Vector.h>

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
            fs::Vector<T>&                              vertexArray() noexcept;
            fs::Vector<IndexElementType>&               indexArray() noexcept;

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
            fs::Vector<T>                               _vertexArray;
            DxObjectId                                  _vertexBufferId;

        private:
            fs::Vector<IndexElementType>                _indexArray;
            DxObjectId                                  _indexBufferId;
        };
    }
}


#endif // !FS_LOW_LEVEL_RENDERER_H

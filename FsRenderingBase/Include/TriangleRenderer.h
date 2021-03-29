#pragma once


#ifndef FS_TRIANGLE_BUFFER_H
#define FS_TRIANGLE_BUFFER_H


#include <CommonDefinitions.h>

#include <Assets/CppHlsl/CppHlslStreamData.h>

#include <FsRenderingBase/Include/DxResource.h>


namespace fs
{
    namespace RenderingBase
    {
        class GraphicDevice;


        using Microsoft::WRL::ComPtr;


        template <typename T>
        class TriangleRenderer
        {
        public:
                                                        TriangleRenderer(fs::RenderingBase::GraphicDevice* const graphicDevice);
                                                        ~TriangleRenderer() = default;

        public:
            std::vector<T>&                             vertexArray() noexcept;
            std::vector<IndexElementType>&              indexArray() noexcept;

        public:
            void                                        flush();
            const bool                                  isRenderable() const noexcept;
            void                                        render();

        private:
            void                                        prepareBuffer();

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


#endif // !FS_TRIANGLE_BUFFER_H

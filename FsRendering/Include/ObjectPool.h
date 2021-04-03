#pragma once


#ifndef FS_RENDERER_CONTEXT_H
#define FS_RENDERER_CONTEXT_H


#include <FsRendering/Include/Object.h>


namespace fs
{
    namespace RenderingBase
    {
        class GraphicDevice;
    }


    namespace Rendering
    {
        class TransformComponent;
        class MeshComponent;
        class CameraObject;


        class ObjectPool final
        {
        public:
                                                                                ObjectPool();
                                                                                ~ObjectPool();

        public:
            fs::Rendering::Object*                                              createObject();
            fs::Rendering::CameraObject*                                        createCameraObject();

        private:
            void                                                                destroyObjects();

        private:
            fs::Rendering::Object*                                              createObjectInternalXXX(fs::Rendering::Object* const object);
        
        public:
            fs::Rendering::TransformComponent*                                  createTransformComponent();
            fs::Rendering::MeshComponent*                                       createMeshComponent();
        
        public:
            void                                                                destroyObjectComponents(Object& object);

        public:
            void                                                                registerMeshComponent(fs::Rendering::MeshComponent* const meshComponent);
            void                                                                deregisterMeshComponent(fs::Rendering::MeshComponent* const meshComponent);

        public:
            void                                                                updateScreenSize(const fs::Float2& screenSize);

        public:
            const std::vector<fs::Rendering::MeshComponent*>&                   getMeshComponents() const noexcept;

        public:
            const uint32                                                        getObjectCount() const noexcept;

        private:
            std::vector<fs::Rendering::Object*>                                 _objectArray;

        private:
            std::vector<fs::Rendering::MeshComponent*>                          _meshComponentArray;
        };
    }
}


#endif // !FS_RENDERER_CONTEXT_H

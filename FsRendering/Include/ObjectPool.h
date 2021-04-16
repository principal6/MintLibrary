#pragma once


#ifndef FS_RENDERER_CONTEXT_H
#define FS_RENDERER_CONTEXT_H


#include <FsRendering/Include/Object.h>

#include <FsContainer/Include/Vector.h>


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
        class DeltaTimer;


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
            void                                                                computeDeltaTime() const noexcept;
            void                                                                updateScreenSize(const fs::Float2& screenSize);

        public:
            const fs::Vector<fs::Rendering::MeshComponent*>&                    getMeshComponents() const noexcept;

        public:
            const uint32                                                        getObjectCount() const noexcept;
            const DeltaTimer*                                                   getDeltaTimerXXX() const noexcept;

        private:
            const DeltaTimer* const                                             _deltaTimer;

        private:
            fs::Vector<fs::Rendering::Object*>                                  _objectArray;

        private:
            fs::Vector<fs::Rendering::MeshComponent*>                           _meshComponentArray;
        };
    }
}


#endif // !FS_RENDERER_CONTEXT_H

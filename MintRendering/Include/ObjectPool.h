#pragma once


#ifndef MINT_RENDERER_CONTEXT_H
#define MINT_RENDERER_CONTEXT_H


#include <MintRendering/Include/Object.h>

#include <MintContainer/Include/Vector.h>


namespace mint
{
    namespace Rendering
    {
        class GraphicDevice;
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
            Object*                             createObject();
            CameraObject*                       createCameraObject();

        private:
            void                                destroyObjects();

        private:
            Object*                             createObjectInternalXXX(Object* const object);

        public:
            TransformComponent*                 createTransformComponent();
            MeshComponent*                      createMeshComponent();

        public:
            void                                destroyObjectComponents(Object& object);

        public:
            void                                registerMeshComponent(MeshComponent* const meshComponent);
            void                                deregisterMeshComponent(MeshComponent* const meshComponent);

        public:
            void                                computeDeltaTime() const noexcept;
            void                                updateScreenSize(const Float2& screenSize);

        public:
            const Vector<MeshComponent*>&       getMeshComponents() const noexcept;

        public:
            const uint32                        getObjectCount() const noexcept;
            const DeltaTimer*                   getDeltaTimerXXX() const noexcept;

        private:
            const DeltaTimer* const             _deltaTimer;

        private:
            Vector<Object*>                     _objectArray;

        private:
            Vector<MeshComponent*>              _meshComponentArray;
        };
    }
}


#endif // !MINT_RENDERER_CONTEXT_H

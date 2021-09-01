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
            mint::Rendering::Object*                                              createObject();
            mint::Rendering::CameraObject*                                        createCameraObject();

        private:
            void                                                                destroyObjects();

        private:
            mint::Rendering::Object*                                              createObjectInternalXXX(mint::Rendering::Object* const object);
        
        public:
            mint::Rendering::TransformComponent*                                  createTransformComponent();
            mint::Rendering::MeshComponent*                                       createMeshComponent();
        
        public:
            void                                                                destroyObjectComponents(Object& object);

        public:
            void                                                                registerMeshComponent(mint::Rendering::MeshComponent* const meshComponent);
            void                                                                deregisterMeshComponent(mint::Rendering::MeshComponent* const meshComponent);

        public:
            void                                                                computeDeltaTime() const noexcept;
            void                                                                updateScreenSize(const mint::Float2& screenSize);

        public:
            const mint::Vector<mint::Rendering::MeshComponent*>&                    getMeshComponents() const noexcept;

        public:
            const uint32                                                        getObjectCount() const noexcept;
            const DeltaTimer*                                                   getDeltaTimerXXX() const noexcept;

        private:
            const DeltaTimer* const                                             _deltaTimer;

        private:
            mint::Vector<mint::Rendering::Object*>                                  _objectArray;

        private:
            mint::Vector<mint::Rendering::MeshComponent*>                           _meshComponentArray;
        };
    }
}


#endif // !MINT_RENDERER_CONTEXT_H

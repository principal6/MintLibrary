#pragma once


#ifndef FS_OBJECT_H
#define FS_OBJECT_H


#include <FsCommon/Include/CommonDefinitions.h>

#include <FsMath/Include/Float4x4.h>
#include <FsRendering/Include/IObjectComponent.h>


namespace fs
{
    namespace Rendering
    {
        class IObjectComponent;
        class TransformComponent;
        class ObjectPool;
        enum class ObjectComponentType;
        struct Srt;

        enum class ObjectType
        {
            INVALID,
            Object,
            CameraObject
        };


        class Object
        {
            friend ObjectPool;

        private:
                                            Object(const ObjectPool* const objectPool);

        protected:
                                            Object(const ObjectPool* const objectPool, const ObjectType objectType);
            virtual                         ~Object();
        
        public:
            void                            attachComponent(IObjectComponent* const objectComponent);
            void                            detachComponent(IObjectComponent* const objectComponent);
        
        public:
            const ObjectType                getType() const noexcept;
            const bool                      isTypeOf(const ObjectType objectType) const noexcept;
            const uint32                    getComponentCount() const noexcept;
            IObjectComponent*                getComponent(const ObjectComponentType type) const noexcept;
            fs::Rendering::Srt&             getObjectTransformSrt() noexcept;
            const fs::Rendering::Srt&       getObjectTransformSrt() const noexcept;
            fs::Float4x4                    getObjectTransformMatrix() const noexcept;

        protected:
            TransformComponent*             getObjectTransformComponent() const noexcept;
            const float                     getDeltaTimeS() const noexcept;

        protected:
            const ObjectPool* const         _objectPool;
            const ObjectType                _objectType;

        protected:
            std::vector<IObjectComponent*>   _componentArray;
        };

    }
}


#include <FsRendering/Include/Object.inl>


#endif // !FS_OBJECT_H

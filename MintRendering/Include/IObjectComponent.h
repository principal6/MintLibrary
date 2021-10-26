#pragma once


#ifndef MINT_OBJECT_COMPONENT_H
#define MINT_OBJECT_COMPONENT_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/IId.h>

#include <MintMath/Include/Float4x4.h>


namespace mint
{
    namespace Rendering
    {
        class Object;
        class IObjectComponent;


        enum class ObjectComponentType
        {
            Invalid,
            TransformComponent,
            MeshComponent,
        };


        class ObjectComponentId final : public IId
        {
            friend IObjectComponent;

        private:
            virtual void            assignRawId(const uint32 rawId) noexcept override final;

        public:
            const uint32            getRawId() const noexcept;
        };


        struct Srt
        {
        public:
            Float4x4                toMatrix() const noexcept { return Float4x4::srtMatrix(_scale, _rotation, _translation); }

        public:
            Float3                  _scale = Float3::kUnitScale;
            Quaternion              _rotation;
            Float3                  _translation;
        };


        class IObjectComponent abstract
        {
            friend Object;

        private:
            static std::atomic<uint32>              _nextRawId;

        public:
                                                    IObjectComponent(const ObjectComponentType type);
            virtual                                 ~IObjectComponent() { __noop; }

        public:
            MINT_INLINE const ObjectComponentType   getType() const noexcept { return _type; }
            MINT_INLINE const bool                  isTypeOf(const ObjectComponentType type) const noexcept { return _type == type; }
            MINT_INLINE const ObjectComponentId&    getId() const noexcept { return _id; }
            MINT_INLINE Object*                     getOwnerObject() const noexcept { return _ownerObject; }
        
        protected:
            Object*                                 _ownerObject;
            ObjectComponentType                     _type;
            ObjectComponentId                       _id;
        };
    }
}


#endif // !MINT_OBJECT_COMPONENT_H

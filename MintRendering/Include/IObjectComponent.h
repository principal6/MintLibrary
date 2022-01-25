#pragma once


#ifndef MINT_OBJECT_COMPONENT_H
#define MINT_OBJECT_COMPONENT_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/IID.h>

#include <MintMath/Include/Float4x4.h>
#include <MintMath/Include/Quaternion.h>


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


        class ObjectComponentID final : public IID
        {
            friend IObjectComponent;

        private:
            virtual void            assignRawID(const uint32 rawID) noexcept override final;

        public:
            const uint32            getRawID() const noexcept;
        };


        class IObjectComponent abstract
        {
            friend Object;

        private:
            static std::atomic<uint32>              _nextRawID;

        public:
                                                    IObjectComponent(const ObjectComponentType type);
            virtual                                 ~IObjectComponent() { __noop; }

        public:
            MINT_INLINE const ObjectComponentType   getType() const noexcept { return _type; }
            MINT_INLINE const bool                  isTypeOf(const ObjectComponentType type) const noexcept { return _type == type; }
            MINT_INLINE const ObjectComponentID&    getID() const noexcept { return _id; }
            MINT_INLINE Object*                     getOwnerObject() const noexcept { return _ownerObject; }
        
        protected:
            Object*                                 _ownerObject;
            ObjectComponentType                     _type;
            ObjectComponentID                       _id;
        };
    }
}


#endif // !MINT_OBJECT_COMPONENT_H

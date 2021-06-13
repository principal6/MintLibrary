#pragma once


#ifndef MINT_TRANSFORM_COMPONENT_H
#define MINT_TRANSFORM_COMPONENT_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRendering/Include/IObjectComponent.h>

#include <MintMath/Include/Float4x4.h>
#include <MintMath/Include/Quaternion.h>


namespace mint
{
    namespace Rendering
    {
        class TransformComponent : public IObjectComponent
        {
        public:
                                    TransformComponent() : IObjectComponent(ObjectComponentType::TransformComponent) { __noop; }
                                    TransformComponent(const ObjectComponentType derivedType) : IObjectComponent(derivedType) { __noop; }
            virtual                 ~TransformComponent() { __noop; }

        public:
            Srt                     _srt;
        };
    }
}


#endif // !MINT_TRANSFORM_COMPONENT_H

#pragma once


#ifndef FS_TRANSFORM_COMPONENT_H
#define FS_TRANSFORM_COMPONENT_H


#include <CommonDefinitions.h>

#include <FsRendering/Include/IObjectComponent.h>

#include <FsMath/Include/Float4x4.h>
#include <FsMath/Include/Quaternion.h>


namespace fs
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


#endif // !FS_TRANSFORM_COMPONENT_H

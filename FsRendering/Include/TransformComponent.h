#pragma once


#ifndef FS_TRANSFORM_COMPONENT_H
#define FS_TRANSFORM_COMPONENT_H


#include <CommonDefinitions.h>

#include <FsRendering/Include/ObjectComponent.h>

#include <FsMath/Include/Float4x4.h>
#include <FsMath/Include/Quaternion.h>


namespace fs
{
    namespace Rendering
    {
        class TransformComponent : public ObjectComponent
        {
        public:
                                    TransformComponent() : ObjectComponent(ObjectComponentType::TransformComponent) { __noop; }
                                    TransformComponent(const ObjectComponentType derivedType) : ObjectComponent(derivedType) { __noop; }
            virtual                 ~TransformComponent() { __noop; }

        public:
            Srt                     _srt;
        };
    }
}


#endif // !FS_TRANSFORM_COMPONENT_H

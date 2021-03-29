#include <stdafx.h>
#include <FsRendering/Include/ObjectComponent.h>


namespace fs
{
    namespace Rendering
    {
        void ObjectComponentId::assignRawId(const uint32 rawId) noexcept
        {
            _rawId = rawId;
        }

        const uint32 ObjectComponentId::getRawId() const noexcept
        {
            return _rawId;
        }


        std::atomic<uint32> ObjectComponent::_nextRawId = 0;
        ObjectComponent::ObjectComponent(const ObjectComponentType type)
            : _ownerObject{ nullptr }
            , _type{ type }
        {
            _id.assignRawId(ObjectComponent::_nextRawId);

            ++ObjectComponent::_nextRawId;
        }
    }
}

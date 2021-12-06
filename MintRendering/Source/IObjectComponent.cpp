#include <stdafx.h>
#include <MintRendering/Include/IObjectComponent.h>


namespace mint
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


        SRT::SRT()
            : SRT(Float3::kZero)
        {
            __noop;
        }

        SRT::SRT(const Float3& translation)
            : _scale{ Float3::kUnitScale }
            , _translation{ translation }
        {
            __noop;
        }


        std::atomic<uint32> IObjectComponent::_nextRawId = 0;
        IObjectComponent::IObjectComponent(const ObjectComponentType type)
            : _ownerObject{ nullptr }
            , _type{ type }
        {
            _id.assignRawId(IObjectComponent::_nextRawId);

            ++IObjectComponent::_nextRawId;
        }
    }
}

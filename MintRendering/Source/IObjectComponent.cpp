#include <stdafx.h>
#include <MintRendering/Include/IObjectComponent.h>


namespace mint
{
    namespace Rendering
    {
        void ObjectComponentID::assignRawID(const uint32 rawID) noexcept
        {
            _rawID = rawID;
        }

        const uint32 ObjectComponentID::getRawID() const noexcept
        {
            return _rawID;
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


        std::atomic<uint32> IObjectComponent::_nextRawID = 0;
        IObjectComponent::IObjectComponent(const ObjectComponentType type)
            : _ownerObject{ nullptr }
            , _type{ type }
        {
            _id.assignRawID(IObjectComponent::_nextRawID);

            ++IObjectComponent::_nextRawID;
        }
    }
}

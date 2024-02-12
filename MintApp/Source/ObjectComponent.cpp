#include <MintApp/Include/ObjectComponent.h>


namespace mint
{
	std::atomic<uint32> ObjectComponent::_nextRawID = 0;
	ObjectComponent::ObjectComponent(const ObjectComponentType type)
		: _ownerObject{ nullptr }
		, _type{ type }
	{
		_id.Assign(ObjectComponent::_nextRawID);

		++ObjectComponent::_nextRawID;
	}
}

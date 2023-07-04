#include <MintRendering/Include/IObjectComponent.h>


namespace mint
{
	namespace Rendering
	{
		std::atomic<uint32> IObjectComponent::_nextRawID = 0;
		IObjectComponent::IObjectComponent(const ObjectComponentType type)
			: _ownerObject{ nullptr }
			, _type{ type }
		{
			_id.Assign(IObjectComponent::_nextRawID);

			++IObjectComponent::_nextRawID;
		}
	}
}

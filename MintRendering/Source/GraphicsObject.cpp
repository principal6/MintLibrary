#include <MintRendering/Include/GraphicsObject.h>
#include <MintContainer/Include/OwnPtr.h>
#include <MintContainer/Include/RefCounted.hpp>


namespace mint
{
	namespace Rendering
	{
		const GraphicsObjectID GraphicsObjectID::kInvalidGraphicsObjectID;
		std::atomic<uint32> GraphicsObjectID::_lastRawID{ GraphicsObjectID::kInvalidGraphicsObjectRawID };
	}
}

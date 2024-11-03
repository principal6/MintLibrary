#include <MintRenderingBase/Include/GraphicsObject.h>


namespace mint
{
	namespace Rendering
	{
		const GraphicsObjectID GraphicsObjectID::kInvalidGraphicsObjectID;
		std::atomic<uint32> GraphicsObjectID::_lastRawID{ GraphicsObjectID::kInvalidGraphicsObjectRawID };
	}
}

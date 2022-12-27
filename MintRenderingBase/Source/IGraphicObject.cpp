#include <MintRenderingBase/Include/IGraphicObject.h>


namespace mint
{
	namespace Rendering
	{
		const GraphicObjectID GraphicObjectID::kInvalidObjectID;
		std::atomic<uint32> GraphicObjectID::_lastRawID{ GraphicObjectID::kInvalidGraphicObjectRawID };
	}
}

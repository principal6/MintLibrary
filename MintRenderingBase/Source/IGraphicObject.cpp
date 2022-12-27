#include <MintRenderingBase/Include/IGraphicObject.h>


namespace mint
{
	namespace Rendering
	{
		const GraphicObjectID GraphicObjectID::kInvalidGraphicObjectID;
		std::atomic<uint32> GraphicObjectID::_lastRawID{ GraphicObjectID::kInvalidGraphicObjectRawID };
	}
}

#include <MintRenderingBase/Include/GraphicsObject.h>
#include <MintContainer/Include/OwnPtr.h>


namespace mint
{
	namespace Rendering
	{
		const GraphicsObjectID GraphicsObjectID::kInvalidGraphicsObjectID;
		std::atomic<uint32> GraphicsObjectID::_lastRawID{ GraphicsObjectID::kInvalidGraphicsObjectRawID };

		const GraphicsObjectID& GraphicsObject::Evaluator::operator()(const GraphicsObject& rhs) const noexcept
		{
			return rhs._objectID;
		}

		const GraphicsObjectID& GraphicsObject::Evaluator::operator()(const OwnPtr<GraphicsObject>& rhs) const noexcept
		{
			return rhs->_objectID;
		}

		bool GraphicsObject::AscendingComparator::operator()(const GraphicsObject& lhs, const GraphicsObject& rhs) const noexcept
		{
			return lhs._objectID < rhs._objectID;
		}

		bool GraphicsObject::AscendingComparator::operator()(const OwnPtr<GraphicsObject>& lhs, const OwnPtr<GraphicsObject>& rhs) const noexcept
		{
			return lhs->_objectID < rhs->_objectID;
		}
	}
}

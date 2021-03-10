#pragma once


namespace fs
{
	namespace Rendering
	{
		FS_INLINE const ObjectType Object::getType() const noexcept
		{
			return _objectType;
		}

		FS_INLINE const bool Object::isTypeOf(const ObjectType objectType) const noexcept 
		{
			return _objectType == objectType; 
		}
	}
}
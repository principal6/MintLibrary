#pragma once


namespace mint
{
	namespace Game
	{
		MINT_INLINE ObjectType Object::GetType() const noexcept
		{
			return _objectType;
		}

		MINT_INLINE bool Object::IsTypeOf(const ObjectType objectType) const noexcept
		{
			return _objectType == objectType;
		}
	}
}

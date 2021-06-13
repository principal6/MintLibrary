#pragma once


namespace mint
{
    namespace Rendering
    {
        MINT_INLINE const ObjectType Object::getType() const noexcept
        {
            return _objectType;
        }

        MINT_INLINE const bool Object::isTypeOf(const ObjectType objectType) const noexcept 
        {
            return _objectType == objectType; 
        }
    }
}

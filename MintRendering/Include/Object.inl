#pragma once


namespace mint
{
    namespace Rendering
    {
        MINT_INLINE ObjectType Object::getType() const noexcept
        {
            return _objectType;
        }

        MINT_INLINE bool Object::isTypeOf(const ObjectType objectType) const noexcept 
        {
            return _objectType == objectType; 
        }
    }
}

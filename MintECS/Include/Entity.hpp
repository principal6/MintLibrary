#pragma once


#ifndef _MINT_ECS_ENTITY_HPP_
#define _MINT_ECS_ENTITY_HPP_


#include <MintECS/Include/Entity.h>


namespace mint
{
	template<typename UintType, uint8 IndexBits>
	inline uint64 Hasher<ECS::EntityBase<UintType, IndexBits>>::operator()(const ECS::EntityBase<UintType, IndexBits>& value) const noexcept
	{
		return ComputeHash(value.GetRawValue());
	}
}


#endif // !_MINT_ECS_ENTITY_HPP_
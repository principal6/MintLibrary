#pragma once


#ifndef _MINT_ECS_ENTITY_H_
#define _MINT_ECS_ENTITY_H_


#include <MintContainer/Include/SerialAndIndex.h>
#include <MintContainer/Include/Hash.h>


namespace mint
{
	namespace ECS
	{
		template<typename UintType, uint8 IndexBits>
		using EntityBase = SerialAndIndex<UintType, IndexBits>;
	}

	template<typename UintType, uint8 IndexBits>
	struct Hasher<ECS::EntityBase<UintType, IndexBits>> final
	{
		uint64 operator()(const ECS::EntityBase<UintType, IndexBits>& value) const noexcept;
	};
}


#endif // !_MINT_ECS_ENTITY_H_

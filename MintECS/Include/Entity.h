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
		class EntityBase abstract : public SerialAndIndex<UintType, IndexBits> {};
	}

	template<typename UintType, uint8 IndexBits>
	struct Hasher<ECS::EntityBase<UintType, IndexBits>> final
	{
		uint64 operator()(const ECS::EntityBase<UintType, IndexBits>& value) const noexcept;
	};

	template<typename T> requires std::derived_from<T, ECS::EntityBase<typename T::UintType, T::kIndexBits>>
	struct Hasher<T> final
	{
		uint64 operator()(const T& value) const noexcept;
	};
}


#endif // !_MINT_ECS_ENTITY_H_

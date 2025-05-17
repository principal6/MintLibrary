#pragma once


#ifndef _MINT_ECS_ENTITY_POOL_HPP_
#define _MINT_ECS_ENTITY_POOL_HPP_


#include <MintECS/Include/EntityPool.h>
#include <MintContainer/Include/Vector.hpp>
#include <MintECS/Include/EntityComponentPool.hpp>


namespace mint
{
	namespace ECS
	{
		template<typename EntityType>
		inline EntityPool<EntityType>::EntityPool()
		{
			_nextEmptyEntityIndex = 0;
		}

		template<typename EntityType>
		inline EntityPool<EntityType>::~EntityPool()
		{
			for (const EntityType& s : _entities)
			{
				if (s.IsValid() == false)
				{
					continue;
				}

				DestroyEntity(s);
			}
		}

		template<typename EntityType>
		MINT_INLINE EntityType EntityPool<EntityType>::CreateEntity()
		{
			if (_nextEmptyEntityIndex >= _entities.Size())
			{
				if (_entities.IsEmpty() == true)
				{
					_entities.Resize(16);
				}
				else
				{
					_entities.Resize(_entities.Capacity() * 2);
				}
			}

			EntityType& entity = _entities[_nextEmptyEntityIndex];
			entity.SetSerialAndIndex(entity.GetSerial() + 1, _nextEmptyEntityIndex);
			++_aliveEntityCount;

			const uint32 nextEmptyEntityIndexCache = _nextEmptyEntityIndex;
			const uint32 poolSize = _entities.Size();
			for (uint32 i = _nextEmptyEntityIndex + 1; i < poolSize; ++i)
			{
				if (_entities[i].IsValid() == false)
				{
					_nextEmptyEntityIndex = i;
					break;
				}
			}
			if (nextEmptyEntityIndexCache == _nextEmptyEntityIndex)
			{
				_nextEmptyEntityIndex = _entities.Size();
			}
			OnEntityCreated(entity);
			return entity;
		}

		template<typename EntityType>
		inline void EntityPool<EntityType>::DestroyEntity(EntityType entity)
		{
			MINT_ASSERT(_aliveEntityCount > 0, "로직 상 반드시 보장되어야 합니다!");
			--_aliveEntityCount;

			const uint32 index = static_cast<uint32>(entity.GetIndex());
			MINT_ASSERT(index < _entities.Size(), "로직 상 반드시 보장되어야 합니다!");
			_entities[index].SetInvalidIndex();

			_nextEmptyEntityIndex = Min(index, _nextEmptyEntityIndex);

			const Vector<IEntityComponentPool<EntityType>*>& componentPools = EntityComponentPoolRegistry<EntityType>::GetInstance().GetComponentPools();
			for (IEntityComponentPool<EntityType>* const componentPool : componentPools)
			{
				componentPool->RemoveComponentFrom(entity);
			}
		}

		template<typename EntityType>
		template<typename ComponentType>
		inline void EntityPool<EntityType>::AttachComponent(const EntityType& entity, const ComponentType& component)
		{
			return EntityComponentPool<EntityType, ComponentType>::GetInstance().AddComponentTo(entity, component);
		}

		template<typename EntityType>
		template<typename ComponentType>
		inline void EntityPool<EntityType>::AttachComponent(const EntityType& entity, ComponentType&& component)
		{
			return EntityComponentPool<EntityType, ComponentType>::GetInstance().AddComponentTo(entity, std::move(component));
		}

		template<typename EntityType>
		template<typename ComponentType>
		inline ComponentType* EntityPool<EntityType>::GetComponent(const EntityType& entity)
		{
			return EntityComponentPool<EntityType, ComponentType>::GetInstance().GetComponent(entity);
		}

		template<typename EntityType>
		template<typename ComponentType>
		inline ComponentType& EntityPool<EntityType>::GetComponentMust(const EntityType& entity)
		{
			ComponentType* const component = GetComponent<ComponentType>(entity);
			MINT_ASSERT(component != nullptr, "해당 Component 가 Entity 에 존재하지 않습니다!");
			return *component;
		}
	}
}


#endif // !_MINT_ECS_ENTITY_POOL_HPP_

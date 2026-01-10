#pragma once


#ifndef _MINT_ECS_ENTITY_POOL_HPP_
#define _MINT_ECS_ENTITY_POOL_HPP_


#include <MintECS/Include/EntityRegistry.h>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/ContiguousHashMap.hpp>


namespace mint
{
	namespace ECS
	{
#pragma region EntityComponentPool
		template<typename EntityType, typename ComponentType>
		inline EntityComponentPool<EntityType, ComponentType>::EntityComponentPool()
		{
		}

		template<typename EntityType, typename ComponentType>
		inline EntityComponentPool<EntityType, ComponentType>::~EntityComponentPool()
		{
			__noop;
		}

		template<typename EntityType, typename ComponentType>
		inline void EntityComponentPool<EntityType, ComponentType>::AddComponentTo(const EntityType& entity, const ComponentType& component)
		{
			MINT_ASSERT(HasComponent(entity) == false, "This entity already has the component.");
			_componentMap.Insert(entity, component);
		}

		template<typename EntityType, typename ComponentType>
		inline void EntityComponentPool<EntityType, ComponentType>::AddComponentTo(const EntityType& entity, ComponentType&& component)
		{
			MINT_ASSERT(HasComponent(entity) == false, "This entity already has the component.");
			_componentMap.Insert(entity, std::move(component));
		}

		template<typename EntityType, typename ComponentType>
		inline void EntityComponentPool<EntityType, ComponentType>::RemoveComponentFrom(const EntityType& entity)
		{
			MINT_ASSERT(HasComponent(entity) == true, "This entity does not have the component.");
			_componentMap.Erase(entity);
		}

		template<typename EntityType, typename ComponentType>
		inline bool EntityComponentPool<EntityType, ComponentType>::HasComponent(const EntityType& entity) const
		{
			return _componentMap.Contains(entity);
		}

		template<typename EntityType, typename ComponentType>
		inline void EntityComponentPool<EntityType, ComponentType>::CopyComponent(const EntityType& sourceEntity, const EntityType& targetEntity)
		{
			MINT_ASSERT(HasComponent(sourceEntity) == true, "SourceEntity does not have the component.");
			const ComponentType* const sourceComponent = GetComponent(sourceEntity);
			AddComponentTo(targetEntity, ComponentType(*sourceComponent));
		}

		template<typename EntityType, typename ComponentType>
		inline ComponentType* EntityComponentPool<EntityType, ComponentType>::GetComponent(const EntityType& entity)
		{
			return _componentMap.Find(entity);
		}

		template<typename EntityType, typename ComponentType>
		inline const ComponentType* EntityComponentPool<EntityType, ComponentType>::GetComponent(const EntityType& entity) const
		{
			return _componentMap.Find(entity);
		}

		template<typename EntityType, typename ComponentType>
		inline ContiguousHashMap<EntityType, ComponentType>& EntityComponentPool<EntityType, ComponentType>::GetComponentMap()
		{
			return _componentMap;
		}

		template<typename EntityType, typename ComponentType>
		inline const ContiguousHashMap<EntityType, ComponentType>& EntityComponentPool<EntityType, ComponentType>::GetComponentMap() const
		{
			return _componentMap;
		}
#pragma endregion

#pragma region EntityRegistry
		template<typename EntityType>
		inline EntityRegistry<EntityType>::EntityRegistry()
		{
			_nextEmptyEntityIndex = 0;
		}

		template<typename EntityType>
		inline EntityRegistry<EntityType>::~EntityRegistry()
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
		MINT_INLINE EntityType EntityRegistry<EntityType>::CreateEntity()
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
		inline void EntityRegistry<EntityType>::DestroyEntity(EntityType entity)
		{
			MINT_ASSERT(_aliveEntityCount > 0, "로직 상 반드시 보장되어야 합니다!");
			--_aliveEntityCount;

			const uint32 index = static_cast<uint32>(entity.GetIndex());
			MINT_ASSERT(index < _entities.Size(), "로직 상 반드시 보장되어야 합니다!");
			_entities[index].SetInvalidIndex();

			_nextEmptyEntityIndex = Min(index, _nextEmptyEntityIndex);

			for (OwnPtr<IEntityComponentPool<EntityType>>& componentPool : _componentPools)
			{
				if (componentPool->HasComponent(entity) == false)
				{
					continue;
				}

				componentPool->RemoveComponentFrom(entity);
			}
		}

		template<typename EntityType>
		template<typename ComponentType>
		inline void EntityRegistry<EntityType>::AttachComponent(const EntityType& entity, const ComponentType& component)
		{
			return GetComponentPool<ComponentType>().AddComponentTo(entity, component);
		}

		template<typename EntityType>
		template<typename ComponentType>
		inline void EntityRegistry<EntityType>::AttachComponent(const EntityType& entity, ComponentType&& component)
		{
			return GetComponentPool<ComponentType>().AddComponentTo(entity, std::move(component));
		}

		template<typename EntityType>
		template<typename ComponentType>
		inline ComponentType* EntityRegistry<EntityType>::GetComponent(const EntityType& entity)
		{
			return GetComponentPool<ComponentType>().GetComponent(entity);
		}

		template<typename EntityType>
		template<typename ComponentType>
		inline const ComponentType* EntityRegistry<EntityType>::GetComponent(const EntityType& entity) const
		{
			return GetComponentPool<ComponentType>().GetComponent(entity);
		}

		template<typename EntityType>
		template<typename ComponentType>
		inline ComponentType& EntityRegistry<EntityType>::GetComponentMust(const EntityType& entity)
		{
			ComponentType* const component = GetComponent<ComponentType>(entity);
			MINT_ASSERT(component != nullptr, "해당 Component 가 Entity 에 존재하지 않습니다!");
			return *component;
		}

		template<typename EntityType>
		template<typename ComponentType>
		inline const ComponentType& EntityRegistry<EntityType>::GetComponentMust(const EntityType& entity) const
		{
			const ComponentType* const component = GetComponent<ComponentType>(entity);
			MINT_ASSERT(component != nullptr, "해당 Component 가 Entity 에 존재하지 않습니다!");
			return *component;
		}

		template<typename EntityType>
		template<typename ComponentType>
		inline EntityComponentPool<EntityType, ComponentType>& EntityRegistry<EntityType>::GetComponentPool()
		{
			const EntityRegistry& constEntityRegistry = static_cast<const EntityRegistry&>(*this);
			return const_cast<EntityComponentPool<EntityType, ComponentType>&>(constEntityRegistry.GetComponentPool<ComponentType>());
		}

		template<typename EntityType>
		template<typename ComponentType>
		inline const EntityComponentPool<EntityType, ComponentType>& EntityRegistry<EntityType>::GetComponentPool() const
		{
			OwnPtr<IEntityComponentPool<EntityType>>* const found = _componentPools.Find(typeid(ComponentType).hash_code());
			if (found != nullptr)
			{
				return static_cast<const EntityComponentPool<EntityType, ComponentType>&>(**found);
			}

			OwnPtr<IEntityComponentPool<EntityType>> newComponentPool = MINT_NEW((EntityComponentPool<EntityType, ComponentType>));
			const EntityComponentPool<EntityType, ComponentType>& result = static_cast<const EntityComponentPool<EntityType, ComponentType>&>(*newComponentPool);
			_componentPools.Insert(typeid(ComponentType).hash_code(), std::move(newComponentPool));
			return result;
		}

		template<typename EntityType>
		inline const Vector<OwnPtr<IEntityComponentPool<EntityType>>>& EntityRegistry<EntityType>::GetComponentPools() const
		{
			return _componentPools.GetValues();
		}
#pragma endregion
	}
}


#endif // !_MINT_ECS_ENTITY_POOL_HPP_

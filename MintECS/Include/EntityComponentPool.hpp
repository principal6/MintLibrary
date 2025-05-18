#pragma once


#ifndef _MINT_ECS_ENTITY_COMPONENT_POOL_HPP_
#define _MINT_ECS_ENTITY_COMPONENT_POOL_HPP_


#include <MintECS/Include/EntityComponentPool.h>
#include <MintContainer/Include/Vector.hpp>
#include <mintContainer/Include/ContiguousHashMap.hpp>


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
		inline EntityComponentPool<EntityType, ComponentType>& EntityComponentPool<EntityType, ComponentType>::GetInstance()
		{
			static EntityComponentPool<EntityType, ComponentType> sInstance;
			static bool isRegistered = false;
			if (isRegistered == false)
			{
				EntityComponentPoolRegistry<EntityType>::GetInstance().RegisterComponentPool(sInstance);
				isRegistered = true;
			}
			return sInstance;
		}

		template<typename EntityType, typename ComponentType>
		inline void EntityComponentPool<EntityType, ComponentType>::AddComponentTo(const EntityType& entity, const ComponentType& component)
		{
			_componentMap.Insert(entity, component);
		}

		template<typename EntityType, typename ComponentType>
		inline void EntityComponentPool<EntityType, ComponentType>::AddComponentTo(const EntityType& entity, ComponentType&& component)
		{
			_componentMap.Insert(entity, std::move(component));
		}

		template<typename EntityType, typename ComponentType>
		inline void EntityComponentPool<EntityType, ComponentType>::RemoveComponentFrom(const EntityType& entity)
		{
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
			const ComponentType* const sourceComponent = GetComponent(sourceEntity);
			AddComponentTo(targetEntity, ComponentType(*sourceComponent));
		}

		template<typename EntityType, typename ComponentType>
		inline ComponentType* EntityComponentPool<EntityType, ComponentType>::GetComponent(const EntityType& entity)
		{
			return _componentMap.Find(entity);
		}

		template<typename EntityType, typename ComponentType>
		inline ContiguousHashMap<EntityType, ComponentType>& EntityComponentPool<EntityType, ComponentType>::GetComponentMap()
		{
			return _componentMap;
		}
#pragma endregion


#pragma region EntityComponentPoolRegistry
		template<typename EntityType>
		inline EntityComponentPoolRegistry<EntityType>::EntityComponentPoolRegistry()
		{
			__noop;
		}

		template<typename EntityType>
		inline EntityComponentPoolRegistry<EntityType>::~EntityComponentPoolRegistry()
		{
			__noop;
		}

		template<typename EntityType>
		inline EntityComponentPoolRegistry<EntityType>& EntityComponentPoolRegistry<EntityType>::GetInstance()
		{
			static EntityComponentPoolRegistry<EntityType> sInstance;
			return sInstance;
		}

		template<typename EntityType>
		inline void EntityComponentPoolRegistry<EntityType>::RegisterComponentPool(IEntityComponentPool<EntityType>& componentPool)
		{
			for (const auto& iter : _componentPools)
			{
				if (iter == &componentPool)
				{
					MINT_ASSERT(false, "This component pool is already registered!!!");
					return;
				}
			}

			_componentPools.PushBack(&componentPool);
		}

		template<typename EntityType>
		inline const Vector<IEntityComponentPool<EntityType>*>& EntityComponentPoolRegistry<EntityType>::GetComponentPools() const
		{
			return _componentPools;
		}
#pragma endregion
	}
}


#endif // !_MINT_ECS_ENTITY_COMPONENT_POOL_HPP_

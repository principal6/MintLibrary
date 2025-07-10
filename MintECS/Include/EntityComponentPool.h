#pragma once


#ifndef _MINT_ECS_ENTITY_COMPONENT_POOL_H_
#define _MINT_ECS_ENTITY_COMPONENT_POOL_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/ContiguousHashMap.h>

#include <typeindex>

namespace mint
{
	namespace ECS
	{
		template<typename EntityType>
		class EntityRegistry;

		// type-erasure for EntityComponentPool
		template<typename EntityType>
		class IEntityComponentPool abstract
		{
		public:
			IEntityComponentPool() { __noop; }
			virtual ~IEntityComponentPool() { __noop; }

		public:
			virtual void RemoveComponentFrom(const EntityType& entity) = 0;
			virtual bool HasComponent(const EntityType& entity) const = 0;
			virtual void CopyComponent(const EntityType& sourceEntity, const EntityType& targetEntity) = 0;
			virtual std::type_index GetTypeIndex() const = 0;
		};


		template<typename EntityType, typename ComponentType>
		class EntityComponentPool final : public IEntityComponentPool<EntityType>
		{
			friend EntityRegistry;

		public:
			virtual ~EntityComponentPool();

		public:
			void AddComponentTo(const EntityType& entity, const ComponentType& component);
			void AddComponentTo(const EntityType& entity, ComponentType&& component);
			virtual void RemoveComponentFrom(const EntityType& entity) override final;
			virtual bool HasComponent(const EntityType& entity) const override final;
			virtual void CopyComponent(const EntityType& sourceEntity, const EntityType& targetEntity) override final;
			ComponentType* GetComponent(const EntityType& entity);
			const ComponentType* GetComponent(const EntityType& entity) const;
			ContiguousHashMap<EntityType, ComponentType>& GetComponentMap();
			const ContiguousHashMap<EntityType, ComponentType>& GetComponentMap() const;
			std::type_index GetTypeIndex() const override { return typeid(ComponentType); }

		protected:
			EntityComponentPool();

		protected:
			ContiguousHashMap<EntityType, ComponentType> _componentMap;
		};
	}
}


#endif // !_MINT_ECS_ENTITY_COMPONENT_POOL_H_

#pragma once


#ifndef _MINT_ECS_ENTITY_COMPONENT_POOL_H_
#define _MINT_ECS_ENTITY_COMPONENT_POOL_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/ContiguousHashMap.h>


namespace mint
{
	namespace ECS
	{
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
		};


		template<typename EntityType, typename ComponentType>
		class EntityComponentPool final : public IEntityComponentPool<EntityType>
		{
		public:
			virtual ~EntityComponentPool();

		public:
			static EntityComponentPool& GetInstance();

		public:
			void AddComponentTo(const EntityType& entity, const ComponentType& component);
			void AddComponentTo(const EntityType& entity, ComponentType&& component);
			virtual void RemoveComponentFrom(const EntityType& entity) override final;
			virtual bool HasComponent(const EntityType& entity) const override final;
			virtual void CopyComponent(const EntityType& sourceEntity, const EntityType& targetEntity) override final;
			ComponentType* GetComponent(const EntityType& entity);
			ContiguousHashMap<EntityType, ComponentType>& GetComponentMap();

		protected:
			EntityComponentPool();

		protected:
			ContiguousHashMap<EntityType, ComponentType> _componentMap;
		};


		template<typename EntityType>
		class EntityComponentPoolRegistry final
		{
		public:
			virtual ~EntityComponentPoolRegistry();

		public:
			static EntityComponentPoolRegistry& GetInstance();

		public:
			void RegisterComponentPool(IEntityComponentPool<EntityType>& componentPool);
			const Vector<IEntityComponentPool<EntityType>*>& GetComponentPools() const;

		protected:
			EntityComponentPoolRegistry();
			EntityComponentPoolRegistry(const EntityComponentPoolRegistry& rhs) = delete;
			EntityComponentPoolRegistry(EntityComponentPoolRegistry&& rhs) noexcept = delete;

		protected:
			Vector<IEntityComponentPool<EntityType>*> _componentPools;
		};
	}
}


#endif // !_MINT_ECS_ENTITY_COMPONENT_POOL_H_

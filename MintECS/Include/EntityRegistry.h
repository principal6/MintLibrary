#pragma once


#ifndef _MINT_ECS_ENTITY_POOL_H_
#define _MINT_ECS_ENTITY_POOL_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/OwnPtr.h>
#include <MintContainer/Include/ContiguousHashMap.h>
#include <MintECS/Include/Entity.h>
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

		template<typename EntityType>
		class EntityRegistry
		{
		public:
			EntityRegistry();
			virtual ~EntityRegistry();

		public:
			EntityType CreateEntity();
			void DestroyEntity(EntityType entity);
			template<typename ComponentType>
			void AttachComponent(const EntityType& entity, const ComponentType& component);
			template<typename ComponentType>
			void AttachComponent(const EntityType& entity, ComponentType&& component);
			template<typename ComponentType>
			ComponentType* GetComponent(const EntityType& entity);
			template<typename ComponentType>
			const ComponentType* GetComponent(const EntityType& entity) const;
			template<typename ComponentType>
			ComponentType& GetComponentMust(const EntityType& entity);
			template<typename ComponentType>
			const ComponentType& GetComponentMust(const EntityType& entity) const;

		public:
			template<typename ComponentType>
			EntityComponentPool<EntityType, ComponentType>& GetComponentPool();
			template<typename ComponentType>
			const EntityComponentPool<EntityType, ComponentType>& GetComponentPool() const;
			const Vector<OwnPtr<IEntityComponentPool<EntityType>>>& GetComponentPools() const;

		protected:
			virtual void OnEntityCreated(EntityType entity) = 0;

		protected:
			Vector<EntityType> _entities;
			uint32 _nextEmptyEntityIndex{ 0 };
			uint32 _aliveEntityCount{ 0 };
		
		protected:
			mutable ContiguousHashMap<size_t, OwnPtr<IEntityComponentPool<EntityType>>> _componentPools;
		};
	}
}


#endif // !#define _MINT_ECS_ENTITY_POOL_H_

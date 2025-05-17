#pragma once


#ifndef _MINT_ECS_ENTITY_POOL_H_
#define _MINT_ECS_ENTITY_POOL_H_


#include <MintContainer/Include/Vector.h>
#include <MintECS/Include/Entity.h>


namespace mint
{
	namespace ECS
	{
		template<typename EntityType>
		class EntityPool
		{
		public:
			EntityPool();
			virtual ~EntityPool();

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
			ComponentType& GetComponentMust(const EntityType& entity);

		protected:
			virtual void OnEntityCreated(EntityType entity) = 0;

		protected:
			Vector<EntityType> _entities;
			uint32 _nextEmptyEntityIndex{ 0 };
			uint32 _aliveEntityCount{ 0 };
		};
	}
}


#endif // !#define _MINT_ECS_ENTITY_POOL_H_

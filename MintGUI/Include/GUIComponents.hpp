#pragma once


#ifndef _MINT_GUI_GUI_COMPONENTS_HPP_H_
#define _MINT_GUI_GUI_COMPONENTS_HPP_H_


#include <MintGUI/Include/GUIComponents.h>
#include <MintReflection/Include/Reflection.hpp>


namespace mint
{
	namespace GUI
	{
#pragma region GUIComponentPool
		template<typename ComponentType>
		GUIComponentPool<ComponentType>::GUIComponentPool()
		{
		}

		template<typename ComponentType>
		GUIComponentPool<ComponentType>::~GUIComponentPool()
		{
		}

		template<typename ComponentType>
		GUIComponentPool<ComponentType>& GUIComponentPool<ComponentType>::GetInstance()
		{
			static GUIComponentPool<ComponentType> sInstance;
			return sInstance;
		}

		template<typename ComponentType>
		void GUIComponentPool<ComponentType>::AddComponentTo(const GUIEntity& entity, ComponentType&& component)
		{
			_entities.PushBack(entity);
			_components.PushBack(std::move(component));
		}

		template<typename ComponentType>
		bool GUIComponentPool<ComponentType>::HasComponent(const GUIEntity& entity) const
		{
			for (const GUIEntity& iter : _entities)
			{
				if (iter == entity)
				{
					return true;
				}
			}
			return false;
		}

		template<typename ComponentType>
		void GUIComponentPool<ComponentType>::CopyComponent(const GUIEntity& sourceEntity, const GUIEntity& targetEntity)
		{
			const ComponentType* const sourceComponent = GetComponent(sourceEntity);
			AddComponentTo(targetEntity, ComponentType(*sourceComponent));
		}

		template<typename ComponentType>
		ComponentType* GUIComponentPool<ComponentType>::GetComponent(const GUIEntity& entity)
		{
			const uint32 entityCount = _entities.Size();
			for (uint32 i = 0; i < entityCount; ++i)
			{
				if (_entities[i] == entity)
				{
					return &_components.At(i);
				}
			}
			return nullptr;
		}
#pragma endregion
	}
}


#endif // !_MINT_GUI_GUI_COMPONENTS_HPP_H_

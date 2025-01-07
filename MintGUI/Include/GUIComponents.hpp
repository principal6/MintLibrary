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
			QuickSort(_entities, ComparatorAscending<GUIEntity>());
			_components.PushBack(std::move(component));
		}

		template<typename ComponentType>
		bool GUIComponentPool<ComponentType>::HasComponent(const GUIEntity& entity) const
		{
			return _entities.Find(entity);
		}

		template<typename ComponentType>
		ComponentType* GUIComponentPool<ComponentType>::GetComponent(const GUIEntity& entity)
		{
			const int32 index = mint::BinarySearch(_entities, entity);
			return (index >= 0 ? &_components.At(index) : nullptr);
		}
#pragma endregion
	}
}


#endif // !_MINT_GUI_GUI_COMPONENTS_HPP_H_

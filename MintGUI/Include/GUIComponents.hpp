#pragma once


#ifndef _MINT_GUI_GUI_COMPONENTS_HPP_H_
#define _MINT_GUI_GUI_COMPONENTS_HPP_H_


#include <MintGUI/Include/GUIComponents.h>
#include <MintContainer/Include/ContiguousHashMap.hpp>
#include <MintReflection/Include/Reflection.hpp>


namespace mint
{
	inline uint64 Hasher<GUI::GUIEntity>::operator()(const GUI::GUIEntity& value) const noexcept
	{
		return ComputeHash(value.Value());
	}

	inline uint64 Hasher<GUI::GUIEntityTemplate>::operator()(const GUI::GUIEntityTemplate& value) const noexcept
	{
		return ComputeHash(value.Value());
	}

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
			__noop;
		}

		template<typename ComponentType>
		GUIComponentPool<ComponentType>& GUIComponentPool<ComponentType>::GetInstance()
		{
			static GUIComponentPool<ComponentType> sInstance;
			static bool isRegistered = false;
			if (isRegistered == false)
			{
				GUIComponentPoolRegistry::GetInstance().RegisterComponentPool(sInstance);
				isRegistered = true;
			}
			return sInstance;
		}

		template<typename ComponentType>
		void GUIComponentPool<ComponentType>::AddComponentTo(const GUIEntity& entity, ComponentType&& component)
		{
			_entityComponents.Insert(entity, std::move(component));
		}

		template<typename ComponentType>
		void GUIComponentPool<ComponentType>::AddComponentToTemplate(const GUIEntityTemplate& entityTemplate, ComponentType&& component)
		{
			_entityTemplateComponents.Insert(entityTemplate, std::move(component));
		}

		template<typename ComponentType>
		bool GUIComponentPool<ComponentType>::HasComponent(const GUIEntity& entity) const
		{
			return _entityComponents.Contains(entity);
		}

		template<typename ComponentType>
		void GUIComponentPool<ComponentType>::CopyComponent(const GUIEntity& sourceEntity, const GUIEntity& targetEntity)
		{
			const ComponentType* const sourceComponent = GetComponent(sourceEntity);
			AddComponentTo(targetEntity, ComponentType(*sourceComponent));
		}

		template<typename ComponentType>
		void GUIComponentPool<ComponentType>::CopyComponentToTemplate(const GUIEntity& sourceEntity, const GUIEntityTemplate& targetEntityTemplate)
		{
			const ComponentType* const sourceComponent = GetComponent(sourceEntity);
			AddComponentToTemplate(targetEntityTemplate, ComponentType(*sourceComponent));
		}

		template<typename ComponentType>
		void GUIComponentPool<ComponentType>::CopyComponentFromTemplate(const GUIEntityTemplate& sourceEntityTemplate, const GUIEntity& targetEntity)
		{
			const ComponentType* const sourceComponent = GetTemplateComponent(sourceEntityTemplate);
			AddComponentTo(targetEntity, ComponentType(*sourceComponent));
		}

		template<typename ComponentType>
		ComponentType* GUIComponentPool<ComponentType>::GetComponent(const GUIEntity& entity)
		{
			return _entityComponents.Find(entity);
		}

		template<typename ComponentType>
		ComponentType* GUIComponentPool<ComponentType>::GetTemplateComponent(const GUIEntityTemplate& entityTemplate)
		{
			return _entityTemplateComponents.Find(entityTemplate);
		}
#pragma endregion
	}
}


#endif // !_MINT_GUI_GUI_COMPONENTS_HPP_H_

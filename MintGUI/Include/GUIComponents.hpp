#pragma once


#ifndef _MINT_GUI_GUI_COMPONENTS_HPP_H_
#define _MINT_GUI_GUI_COMPONENTS_HPP_H_


#include <MintGUI/Include/GUIComponents.h>
#include <MintContainer/Include/HashMap.hpp>
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
			_components.PushBack(std::move(component));
			_entityToComponentMap.Insert(entity, _components.Size() - 1);
		}
		
		template<typename ComponentType>
		void GUIComponentPool<ComponentType>::AddComponentToTemplate(const GUIEntityTemplate& targetEntityTemplate, ComponentType&& component)
		{
			_templateComponents.PushBack(std::move(component));
			_entityTemplateToTemplateComponentMap.Insert(targetEntityTemplate, _templateComponents.Size() - 1);
		}

		template<typename ComponentType>
		bool GUIComponentPool<ComponentType>::HasComponent(const GUIEntity& entity) const
		{
			return _entityToComponentMap.Find(entity).IsValid();
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
			for (auto iter = _entityToComponentMap.begin(); iter != _entityToComponentMap.end(); ++iter)
			{
				if (iter.GetKey() == entity)
				{
					return &_components[iter.GetValue()];
				}
			}
			return nullptr;
		}

		template<typename ComponentType>
		ComponentType* GUIComponentPool<ComponentType>::GetTemplateComponent(const GUIEntityTemplate& entityTemplate)
		{
			for (auto iter = _entityTemplateToTemplateComponentMap.begin(); iter != _entityTemplateToTemplateComponentMap.end(); ++iter)
			{
				if (iter.GetKey() == entityTemplate)
				{
					return &_templateComponents[iter.GetValue()];
				}
			}
			return nullptr;
		}
#pragma endregion
	}
}


#endif // !_MINT_GUI_GUI_COMPONENTS_HPP_H_

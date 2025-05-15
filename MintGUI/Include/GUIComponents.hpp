#pragma once


#ifndef _MINT_GUI_GUI_COMPONENTS_HPP_H_
#define _MINT_GUI_GUI_COMPONENTS_HPP_H_


#include <MintGUI/Include/GUIComponents.h>
#include <MintContainer/Include/ContiguousHashMap.hpp>
#include <MintReflection/Include/Reflection.hpp>


namespace mint
{
	inline uint64 Hasher<GUI::GUIControl>::operator()(const GUI::GUIControl& value) const noexcept
	{
		return ComputeHash(value.Value());
	}

	inline uint64 Hasher<GUI::GUIControlTemplate>::operator()(const GUI::GUIControlTemplate& value) const noexcept
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
		void GUIComponentPool<ComponentType>::AddComponentTo(const GUIControl& control, ComponentType&& component)
		{
			_controlComponents.Insert(control, std::move(component));
		}

		template<typename ComponentType>
		void GUIComponentPool<ComponentType>::AddComponentToTemplate(const GUIControlTemplate& controlTemplate, ComponentType&& component)
		{
			_controlTemplateComponents.Insert(controlTemplate, std::move(component));
		}

		template<typename ComponentType>
		bool GUIComponentPool<ComponentType>::HasComponent(const GUIControl& control) const
		{
			return _controlComponents.Contains(control);
		}

		template<typename ComponentType>
		void GUIComponentPool<ComponentType>::CopyComponent(const GUIControl& sourceControl, const GUIControl& targetControl)
		{
			const ComponentType* const sourceComponent = GetComponent(sourceControl);
			AddComponentTo(targetControl, ComponentType(*sourceComponent));
		}

		template<typename ComponentType>
		void GUIComponentPool<ComponentType>::CopyComponentToTemplate(const GUIControl& sourceControl, const GUIControlTemplate& targetControlTemplate)
		{
			const ComponentType* const sourceComponent = GetComponent(sourceControl);
			AddComponentToTemplate(targetControlTemplate, ComponentType(*sourceComponent));
		}

		template<typename ComponentType>
		void GUIComponentPool<ComponentType>::CopyComponentFromTemplate(const GUIControlTemplate& sourceControlTemplate, const GUIControl& targetControl)
		{
			const ComponentType* const sourceComponent = GetTemplateComponent(sourceControlTemplate);
			AddComponentTo(targetControl, ComponentType(*sourceComponent));
		}

		template<typename ComponentType>
		ComponentType* GUIComponentPool<ComponentType>::GetComponent(const GUIControl& control)
		{
			return _controlComponents.Find(control);
		}

		template<typename ComponentType>
		ComponentType* GUIComponentPool<ComponentType>::GetTemplateComponent(const GUIControlTemplate& controlTemplate)
		{
			return _controlTemplateComponents.Find(controlTemplate);
		}
#pragma endregion
	}
}


#endif // !_MINT_GUI_GUI_COMPONENTS_HPP_H_

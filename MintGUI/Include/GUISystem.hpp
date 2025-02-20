#pragma once


#ifndef _MINT_GUI_GUI_SYSTEM_HPP_
#define _MINT_GUI_GUI_SYSTEM_HPP_


#include <MintGUI/Include/GUISystem.h>


namespace mint
{
	namespace GUI
	{
		template<typename ComponentType>
		void GUISystem::AttachComponent(const GUIEntity& entity, ComponentType&& component)
		{
			return GUIComponentPool<ComponentType>::GetInstance().AddComponentTo(entity, std::move(component));
		}
		
		template<typename ComponentType>
		void GUISystem::AttachComponent(const GUIEntityTemplate& entityTemplate, ComponentType&& component)
		{
			return GUIComponentPool<ComponentType>::GetInstance().AddComponentToTemplate(entityTemplate, std::move(component));
		}

		template<typename ComponentType>
		ComponentType* GUISystem::GetComponent(const GUIEntity& entity)
		{
			return GUIComponentPool<ComponentType>::GetInstance().GetComponent(entity);
		}
	}
}


#endif // !_MINT_GUI_GUI_SYSTEM_HPP_

#pragma once


#ifndef _MINT_GUI_GUI_SYSTEM_HPP_
#define _MINT_GUI_GUI_SYSTEM_HPP_


#include <MintGUI/Include/GUISystem.h>


namespace mint
{
	namespace GUI
	{
		template<typename ComponentType>
		void GUISystem::AttachComponent(const GUIControl& control, ComponentType&& component)
		{
			return GUIComponentPool<ComponentType>::GetInstance().AddComponentTo(control, std::move(component));
		}
		
		template<typename ComponentType>
		void GUISystem::AttachComponent(const GUIControlTemplate& controlTemplate, ComponentType&& component)
		{
			return GUIComponentPool<ComponentType>::GetInstance().AddComponentToTemplate(controlTemplate, std::move(component));
		}

		template<typename ComponentType>
		ComponentType* GUISystem::GetComponent(const GUIControl& control)
		{
			return GUIComponentPool<ComponentType>::GetInstance().GetComponent(control);
		}
	}
}


#endif // !_MINT_GUI_GUI_SYSTEM_HPP_

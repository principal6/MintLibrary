#pragma once


#ifndef _MINT_GUI_GUI_OBJECT_HPP_H_
#define _MINT_GUI_GUI_OBJECT_HPP_H_


#include <MintGUI/Include/GUIObject.h>
#include <MintReflection/Include/Reflection.hpp>


namespace mint
{
	namespace GUI
	{
		template<typename T>
		SharedPtr<T> GUIObject::GetComponent() const
		{
			for (const SharedPtr<GUIComponent>& component : _components)
			{
				if (component->IsTypeOf<T>() == true)
				{
					return component;
				}
			}
			return SharedPtr<T>();
		}
	}
}


#endif // !_MINT_GUI_GUI_OBJECT_HPP_H_

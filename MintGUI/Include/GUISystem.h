#pragma once


#ifndef _MINT_GUI_GUI_SYSTEM_H_
#define _MINT_GUI_GUI_SYSTEM_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/StringReference.h>
#include <MintContainer/Include/OwnPtr.h>
#include <MintContainer/Include/SharedPtr.h>
#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintGUI/Include/GUIComponents.h>


namespace mint
{
	namespace Rendering
	{
		class GraphicsDevice;
	}
}

namespace mint
{
	namespace GUI
	{
		class GUISystem
		{
		public:
			GUISystem();
			~GUISystem();

		public:
			void RegisterComponentPool(IGUIComponentPool& componentPool);

			GUIEntity CreateEntity();
			GUIEntity CreateEntity(const GUIEntityTemplate& entityTemplate);
			GUIEntity CloneEntity(const GUIEntity& sourceEntity);
			
			GUIEntityTemplate CreateTemplate();
			GUIEntityTemplate CreateTemplate(const GUIEntity& sourceEntity);

			template<typename ComponentType>
			void AttachComponent(const GUIEntity& entity, ComponentType&& component);
			template<typename ComponentType>
			void AttachComponent(const GUIEntityTemplate& entityTemplate, ComponentType&& component);
			template<typename ComponentType>
			ComponentType* GetComponent(const GUIEntity& entity);

			void Update();
			void Render(Rendering::GraphicsDevice& graphicsDevice);

		private:
			void InputSystem(const Vector<GUIEntity>& entities);
			void RenderSystem(const Vector<GUIEntity>& entities, Rendering::GraphicsDevice& graphicsDevice);

		private:
			uint32 _nextEntityID;
			Vector<GUIEntity> _entities;

		private:
			uint16 _nextEntityTemplateID;
			Vector<GUIEntityTemplate> _entityTemplates;

		private:
			Vector<IGUIComponentPool*> _componentPools;
		};
	}
}


#endif // !_MINT_GUI_GUI_SYSTEM_H_

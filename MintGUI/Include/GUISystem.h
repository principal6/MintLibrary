﻿#pragma once


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

	namespace GUI
	{
		class GUISystem;
	}
}

namespace mint
{
	namespace GUI
	{
		class GUIEntity : public ID32 { friend GUISystem; };

		class GUISystem
		{
		public:
			GUISystem();
			~GUISystem();

		public:
			GUIEntity CreateEntity();

			template<typename ComponentType>
			void AttachComponent(const GUIEntity& entity, ComponentType&& component);
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
		};
	}
}


#endif // !_MINT_GUI_GUI_SYSTEM_H_

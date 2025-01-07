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

	namespace GUI
	{
		class GUIEntityPool;
	}
}

namespace mint
{
	namespace GUI
	{
		class GUIEntity : public ID32 { friend GUIEntityPool; };

		class GUIEntityPool
		{
		public:
			GUIEntityPool();
			~GUIEntityPool();

		public:
			GUIEntity CreateEntity();
			const Vector<GUIEntity>& GetEntities() const;

		private:
			uint32 _nextEntityID;
			Vector<GUIEntity> _entities;
		};

		class GUISystem
		{
		public:
			template<typename ComponentType>
			static void AttachComponent(const GUIEntity& entity, ComponentType&& component);

			template<typename ComponentType>
			static ComponentType* GetComponent(const GUIEntity& entity);

			static void InputSystem(const Vector<GUIEntity>& entities);
			static void RenderSystem(const Vector<GUIEntity>& entities, Rendering::GraphicsDevice& graphicsDevice);
		};
	}
}


#endif // !_MINT_GUI_GUI_SYSTEM_H_

#pragma once


#ifndef _MINT_GUI_GUI_SYSTEM_H_
#define _MINT_GUI_GUI_SYSTEM_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/StringReference.h>
#include <MintContainer/Include/OwnPtr.h>
#include <MintContainer/Include/SharedPtr.h>
#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintRenderingBase/Include/GraphicsObject.h>
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
			GUISystem(Rendering::GraphicsDevice& graphicsDevice);
			~GUISystem();

		public:
			GUIControl CreateControl();
			GUIControl CreateControl(const GUIControlTemplate& controlTemplate);
			GUIControl CloneControl(const GUIControl& sourceControl);
			
			GUIControlTemplate CreateTemplate();
			GUIControlTemplate CreateTemplate(const GUIControl& sourceControl);

			template<typename ComponentType>
			void AttachComponent(const GUIControl& control, ComponentType&& component);
			template<typename ComponentType>
			void AttachComponent(const GUIControlTemplate& controlTemplate, ComponentType&& component);
			template<typename ComponentType>
			ComponentType* GetComponent(const GUIControl& control);

			void Update();
			void Render();

		private:
			void Initialize();
			void Terminate();
			void InputSystem(const Vector<GUIControl>& controls);
			void RenderSystem(const Vector<GUIControl>& controls, Rendering::GraphicsDevice& graphicsDevice);

		private:
			Rendering::GraphicsDevice& _graphicsDevice;
			Rendering::GraphicsObjectID _defaultMaterialID;

		private:
			uint32 _nextControlID;
			Vector<GUIControl> _controls;

		private:
			uint16 _nextControlTemplateID;
			Vector<GUIControlTemplate> _controlTemplates;
		};
	}
}


#endif // !_MINT_GUI_GUI_SYSTEM_H_

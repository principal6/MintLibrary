#pragma once


#ifndef _MINT_GUI_GUI_SYSTEM_H_
#define _MINT_GUI_GUI_SYSTEM_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/StringReference.h>
#include <MintContainer/Include/OwnPtr.h>
#include <MintContainer/Include/SharedPtr.h>
#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintGUI/Include/GUIControl.h>


namespace mint
{
	namespace Rendering
	{
		class GraphicDevice;
		class ShapeRendererContext;
	}

	namespace GUI
	{
		class GUISystem
		{
		public:
			GUISystem(Rendering::GraphicDevice& graphicDevice);
			~GUISystem();

		public:
			GUIControlTemplateID RegisterTemplate(const StringU8& name, GUIControlTemplate&& controlTemplate);
			GUIControlTemplate& AccessTemplate(const GUIControlTemplateID& controlTemplateID);

		public:
			GUIControlID AddControl(const GUIControlTemplateID& controlTemplateID);
			void RemoveControl(const GUIControlID& controlID);
			GUIControl& AccessControl(const GUIControlID& controlID);

		public:
			void Update();
			void Render();

		private:
			Rendering::GraphicDevice& _graphicDevice;
			OwnPtr<Rendering::ShapeRendererContext> _shapeRendererContext;

		private:
			bool _isUpdated;
			struct GUIControlUpdateContext
			{
				Float2 _mouseLeftButtonPressedPosition;
			} _controlUpdateContext;

			class GUIControlManager
			{
			public:
				GUIControlManager();

			public:
				GUIControlTemplateID RegisterTemplate(const StringU8& controlTemplateName, GUIControlTemplate&& controlTemplate);
				GUIControlTemplate& AccessTemplate(const GUIControlTemplateID& controlTemplateID);
				GUIControlID AddControl(const GUIControlTemplateID& controlTemplateID);
				void RemoveControl(const GUIControlID& controlID);

			public:
				void UpdateControls(const GUIControlUpdateContext& controlUpdateContext);

			public:
				GUIControl& AccessControl(const GUIControlID& controlID);
				Vector<SharedPtr<GUIControl>>& AccessControlInstances();
				const GUIControlID& GetHoveredControlID() const { return _hoveredControlID; }
				const GUIControlID& GetPressedControlID() const { return _pressedControlID; }

			private:
				void UpdateControl(const GUIControlUpdateContext& controlUpdateContext, GUIControl& control);

			private:
				Vector<SharedPtr<GUIControlTemplate>> _controlTemplates;

			private:
				uint64 _nextControlRawID;
				Vector<SharedPtr<GUIControl>> _controlInstances;
			
			private:
				GUIControlID _hoveredControlID;
				GUIControlID _pressedControlID;

			private:
			} _controlManager;
		};
	}
}


#endif // !_MINT_GUI_GUI_SYSTEM_H_

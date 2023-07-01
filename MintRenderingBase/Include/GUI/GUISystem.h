#pragma once


#ifndef _MINT_RENDERING_BASE_GUI_SYSTEM_H_
#define _MINT_RENDERING_BASE_GUI_SYSTEM_H_


#include <MintContainer/Include/HashMap.h>
#include <MintContainer/Include/StringReference.h>
#include <MintContainer/Include/OwnPtr.h>
#include <MintContainer/Include/SharedPtr.h>
#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintRenderingBase/Include/ShapeGenerator.h>
#include <MintRenderingBase/Include/GUI/GUIControl.h>


namespace mint
{
	namespace Physics
	{
		class ConvexShape2D;
	}


	namespace Rendering
	{
		class GraphicDevice;

		class GUISystem
		{
		public:
			GUISystem(GraphicDevice& graphicDevice);
			~GUISystem();

		public:
			void DefineControl(const StringU8& name, GUIControlTemplate&& controlTemplate);

		public:
			GUIControlID AddControl(const StringU8& controlName);
			void RemoveControl(const GUIControlID& controlID);
			GUIControl& AccessControl(const GUIControlID& controlID);

		public:
			void Update();
			void Render();

		private:
			GraphicDevice& _graphicDevice;

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
				void DefineControl(const StringU8& name, GUIControlTemplate&& controlTemplate);
				GUIControlID AddControl(const StringU8& controlName);
				void RemoveControl(const GUIControlID& controlID);

			public:
				void UpdateControls(const GUIControlUpdateContext& controlUpdateContext);

			public:
				GUIControl& AccessControl(const GUIControlID& controlID);
				HashMap<GUIControlID, SharedPtr<GUIControl>>& AccessControlInstanceMap();
				const GUIControlID& GetHoveredControlID() const { return _hoveredControlID; }
				const GUIControlID& GetPressedControlID() const { return _pressedControlID; }

			private:
				void UpdateControl(const GUIControlUpdateContext& controlUpdateContext, GUIControl& control);

			private:
				HashMap<StringU8, SharedPtr<GUIControlTemplate>> _controlTemplateMap;

			private:
				uint64 _nextControlRawID;
				HashMap<GUIControlID, SharedPtr<GUIControl>> _controlInstanceMap;
			
			private:
				GUIControlID _hoveredControlID;
				GUIControlID _pressedControlID;

			private:
			} _controlManager;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_GUI_SYSTEM_H_

#pragma once


#ifndef _MINT_GUI_GUI_SYSTEM_H_
#define _MINT_GUI_GUI_SYSTEM_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/StringReference.h>
#include <MintContainer/Include/OwnPtr.h>
#include <MintContainer/Include/SharedPtr.h>
#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintGUI/Include/GUIObject.h>


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
			GUIObjectTemplateID RegisterTemplate(const StringU8& name, GUIObjectTemplate&& objectTemplate);
			GUIObjectTemplate& AccessTemplate(const GUIObjectTemplateID& objectTemplateID);

		public:
			GUIObjectID AddObject(const GUIObjectTemplateID& objectTemplateID);
			void RemoveObject(const GUIObjectID& objectID);
			GUIObject& AccessObject(const GUIObjectID& objectID);

		public:
			void Update();
			void Render();

		private:
			Rendering::GraphicsDevice& _graphicsDevice;

		private:
			bool _isUpdated;
			struct GUIObjectUpdateContext
			{
				Float2 _mouseLeftButtonPressedPosition;
			} _objectUpdateContext;

			class GUIObjectManager
			{
			public:
				GUIObjectManager();

			public:
				GUIObjectTemplateID RegisterTemplate(const StringU8& objectTemplateName, GUIObjectTemplate&& objectTemplate);
				GUIObjectTemplate& AccessTemplate(const GUIObjectTemplateID& objectTemplateID);
				GUIObjectID AddObject(const GUIObjectTemplateID& objectTemplateID);
				void RemoveObject(const GUIObjectID& objectID);

			public:
				void UpdateObjects(const GUIObjectUpdateContext& objectUpdateContext);

			public:
				GUIObject& AccessObject(const GUIObjectID& objectID);
				Vector<SharedPtr<GUIObject>>& AccessObjectInstances();
				const GUIObjectID& GetHoveredObjectID() const { return _hoveredObjectID; }
				const GUIObjectID& GetPressedObjectID() const { return _pressedObjectID; }

			private:
				void UpdateObject(const GUIObjectUpdateContext& objectUpdateContext, GUIObject& guiObject);

			private:
				Vector<SharedPtr<GUIObjectTemplate>> _objectTemplates;

			private:
				uint64 _nextObjectRawID;
				Vector<SharedPtr<GUIObject>> _objectInstances;
			
			private:
				GUIObjectID _hoveredObjectID;
				GUIObjectID _pressedObjectID;

			private:
			} _objectManager;
		};
	}
}


#endif // !_MINT_GUI_GUI_SYSTEM_H_

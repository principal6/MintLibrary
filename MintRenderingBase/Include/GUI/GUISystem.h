﻿#pragma once


#ifndef _MINT_RENDERING_BASE_GUI_SYSTEM_H_
#define _MINT_RENDERING_BASE_GUI_SYSTEM_H_


#include <MintContainer/Include/HashMap.h>
#include <MintContainer/Include/StringReference.h>
#include <MintContainer/Include/OwnPtr.h>
#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintRenderingBase/Include/ShapeGenerator.h>


namespace mint
{
	namespace Rendering
	{
		class GraphicDevice;
		class GUISystem;


		struct GUIControlDesc
		{
			GUIControlDesc(const StringU8& name) :_name{ name } { __noop; }
			StringU8 _name;
			Shape _normalShape;
			Shape _hoveredShape;
			Shape _pressedShape;
		};

		class GUIControlID
		{
			friend GUISystem;
		public:
			GUIControlID() : GUIControlID(kUint64Max) { __noop; }
			MINT_INLINE bool operator==(const GUIControlID& rhs) const { return _raw == rhs._raw; }
			MINT_INLINE uint64 ComputeHash() const { return mint::ComputeHash(_raw); }
		private:
			GUIControlID(const uint64 raw) :_raw{ raw } { __noop; }
			uint64 _raw;
		};

		enum class GUIControlState
		{
			Normal,
			Hovered,
			Pressed,
		};

		class GUIControl
		{
			friend GUISystem;
		public:
			void SetPosition(const Float2& position) { _position = position; }
			void SetText(const StringW& text) { _text = text; }
			bool CheckControlState(const GUIControlState controlState) const { return _controlState == controlState; }
		private:
			GUIControlID _controlID;
			GUIControlState _controlState = GUIControlState::Normal;
			uint32 _controlDefinitionIndex = 0;
			Float2 _position;
			// TODO: StringU8 로 변경할 것.
			StringW _text;
		};

		class GUISystem
		{
		public:
			GUISystem(GraphicDevice& graphicDevice);
			~GUISystem();

		public:
			void DefineControl(const GUIControlDesc& controlDesc);

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
			struct GUIControlUpdateContext
			{
				Float2 _mouseLeftButtonPressedPosition;
			};
			// ControlDefinitionData
			struct GUIControlDefinition;
			class GUIControlManager
			{
			public:
				GUIControlManager();
			public:
				void DefineControl(const GUIControlDesc& controlDesc);
				GUIControlID AddControl(const StringU8& controlName);
				void RemoveControl(const GUIControlID& controlID);
			public:
				void UpdateControls(const GUIControlUpdateContext& controlUpdateContext);
			public:
				GUIControl& AccessControl(const GUIControlID& controlID);
				HashMap<GUIControlID, GUIControl>& AccessControlInstanceMap();
				const GUIControlDefinition& GetControlDefinition(const uint32 controlDefinitionIndex);
			private:
				void UpdateControl(const GUIControlUpdateContext& controlUpdateContext, GUIControl& control);
			private:
				Vector<GUIControlDefinition> _controlDefinitions;
				HashMap<StringU8, uint32> _controlDefinitionMap;
			private:
				uint64 _nextControlRawID;
				HashMap<GUIControlID, GUIControl> _controlInstanceMap;
			private:
			} _controlManager;

		private:
			GUIControlUpdateContext _controlUpdateContext;
			bool _isUpdated;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_GUI_SYSTEM_H_

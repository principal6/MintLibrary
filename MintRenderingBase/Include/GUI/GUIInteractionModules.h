#pragma once


#ifndef _MINT_RENDERING_BASE_GUI_INTERACTION_MODULES_H_
#define _MINT_RENDERING_BASE_GUI_INTERACTION_MODULES_H_


#include <MintRenderingBase/Include/GUI/ControlData.h>


namespace mint
{
	namespace Rendering
	{
		namespace GUI
		{
#pragma region InteractionModule
			template<typename Input>
			class InteractionModule
			{
			public:
				virtual bool Begin(const Input& input) abstract;
				virtual void End() abstract;

			public:
				virtual bool IsInteracting() const abstract;
			};
#pragma endregion


#pragma region InteractionMousePressModule
			struct InteractionMousePressModuleInput
			{
				ControlID _controlID;
				Float2 _controlPosition;
				Float2 _mousePressedPosition;
			};

			template<typename Input>
			class InteractionMousePressModule : public InteractionModule<Input>
			{
			public:
				virtual bool Begin(const Input& input) override;
				virtual void End() override;

			public:
				virtual bool IsInteracting() const override;
				virtual bool IsInteractingWith(const ControlID& controlID) const;

			public:
				Float2 ComputeRelativeMousePressedPosition() const;
				const Float2& GetMousePressedPosition() const { return _input._mousePressedPosition; }
				const Float2& GetInitialControlPosition() const { return _input._controlPosition; }
				const ControlID& GetControlID() const { return _input._controlID; }

			protected:
				Input _input;
			};
#pragma endregion


			using DraggingModule = InteractionMousePressModule<InteractionMousePressModuleInput>;
			using FocusingModule = InteractionMousePressModule<InteractionMousePressModuleInput>;


			struct ResizingModuleInput : public InteractionMousePressModuleInput
			{
				Float2 _controlSize;
				ControlData::ResizingFlags _resizingInteraction;
			};
			class ResizingModule final : public InteractionMousePressModule<ResizingModuleInput>
			{
			public:
				static void MakeOuterAndInenrRects(const ControlData& controlData, const Rect& outerResizingDistances, const Rect& innerResizingDistances, Rect& outerRect, Rect& innerRect);
				static ControlData::ResizingFlags MakeResizingFlags(const Float2& mousePosition, const ControlData& controlData, const Rect& outerRect, const Rect& innerRect);

			public:
				virtual bool Begin(const ResizingModuleInput& resizingModuleInput) override;

			public:
				MINT_INLINE const Float2& GetInitialControlSize() const { return _input._controlSize; }
				MINT_INLINE const ControlData::ResizingFlags& GetResizingFlags() const { return _input._resizingInteraction; }
			};
		}
	}
}


#endif // !_MINT_RENDERING_BASE_GUI_INTERACTION_MODULES_H_

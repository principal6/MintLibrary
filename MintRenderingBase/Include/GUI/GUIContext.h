#pragma once


#ifndef _MINT_RENDERING_BASE_GUI_CONTEXT_H_
#define _MINT_RENDERING_BASE_GUI_CONTEXT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/BitVector.h>
#include <MintContainer/Include/String.h>
#include <MintContainer/Include/HashMap.h>

#include <MintMath/Include/Float2.h>
#include <MintMath/Include/Float4.h>
#include <MintMath/Include/Rect.h>

#include <MintRenderingBase/Include/GUI/ControlData.h>
#include <MintRenderingBase/Include/GUI/GUIInteractionModules.h>
#include <MintRenderingBase/Include/GUI/GUITheme.h>

#include <MintPlatform/Include/IWindow.h>


namespace mint
{
	namespace Rendering
	{
		class GraphicDevice;


		namespace GUI
		{
#pragma region Controls
			struct LabelDesc
			{
				const wchar_t* _text = nullptr;
				TextRenderDirectionHorz _directionHorz = TextRenderDirectionHorz::Centered;
				TextRenderDirectionVert _directionVert = TextRenderDirectionVert::Centered;

			public:
				void SetBackgroundColor(const Color& color) { _useThemeColor = false; _color = color; }
				void SetTextColor(const Color& color) { _useThemeTextColor = false; _textColor = color; }
				const Color& GetBackgroundColor(const Theme& theme) const { return (_useThemeColor ? theme._defaultLabelBackgroundColor : _color); }
				const Color& GetTextColor(const Theme& theme) const { return (_useThemeTextColor ? theme._textColor : _textColor); }

			private:
				bool _useThemeColor = true;
				Color _color = Color::kTransparent;
				bool _useThemeTextColor = true;
				Color _textColor;
			};

			struct ButtonDesc
			{
				const wchar_t* _text = nullptr;
				bool _isRoundButton = false;

				bool _customizeColor = false;
				HoverPressColorSet _customizedColorSet;
			};

			struct WindowDesc
			{
				const wchar_t* _title = nullptr;
				Float2 _initialPosition;
				Float2 _initialSize;
			};
#pragma endregion


			class GUIContext final
			{
				// 생성자가 private 이라서 friend 선언
				friend GraphicDevice;

			private:
				GUIContext(GraphicDevice& graphicDevice);

			public:
				~GUIContext();

			public:
				void Initialize();

			public:
				void ProcessEvent() noexcept;
				void UpdateScreenSize(const Float2& newScreenSize);
				void Render() noexcept;

				// Next control's ControlRenderingDesc
			public:
				void NextControlSameLine();
				void NextControlPosition(const Float2& position);
				void NextControlSize(const Float2& contentSize);
				void NextControlMargin(const Rect& margin);
				void NextControlPadding(const Rect& padding);

				// Control creation #0 make-()
			public:
				void MakeLabel(const FileLine& fileLine, const LabelDesc& labelDesc);
				bool MakeButton(const FileLine& fileLine, const ButtonDesc& buttonDesc);

				// Control creation #1 begin-() && end-()
			public:
				bool BeginWindow(const FileLine& fileLine, const WindowDesc& windowDesc);
				void EndWindow();

				// Control rendering
			private:
				void MakeLabel_Render(const LabelDesc& labelDesc, const ControlData& controlData);
				void MakeButton_Render(const ButtonDesc& buttonDesc, const ControlData& controlData);
				void BeginWindow_Render(const ControlData& controlData);

			private:
				ControlData& AccessControlData(const ControlID& controlID) const;
				ControlData& AccessControlData(const ControlID& controlID, const ControlType controlType);
				ControlID GetStackParentControlID();
				ControlID FindAncestorWindowControl(const ControlID& controlID) const;

			private:
				void UpdateControlData(ControlData& controlData);
				void UpdateControlData_ProcessResizing(const ControlData& controlData);
				void UpdateControlData_ProcessDragging(const ControlData& controlData);
				void UpdateControlData_RenderingData(ControlData& controlData);
				void UpdateControlData_Interaction(ControlData& controlData);
				void UpdateControlData_Interaction_focusing(ControlData& controlData);
				void UpdateControlData_Interaction_resizing(ControlData& controlData);
				void UpdateControlData_Interaction_dragging(ControlData& controlData);
				void UpdateControlData_ResetNextControlDesc();

			private:
				void SelectResizingCursorType(const ControlData::ResizingFlags& resizingFlags);

				// Internal rendering functions
			private:
				void DrawText_(const ControlID& controlID, const Color& color, const FontRenderingOption& fontRenderingOption);
				void DrawText_(const Float2& position, const Float2& size, const wchar_t* const text, const Color& color, const FontRenderingOption& fontRenderingOption);

				// Internal rendering helpers
			private:
				Float4 ComputeShapePosition(const ControlID& controlID) const;
				Float4 ComputeShapePosition(const Float2& position, const Float2& size) const;
				float ComputeRoundness(const ControlID& controlID) const;

			private:
				void DebugRender_Control(const ControlData& controlData);

			public:
				struct DebugSwitch
				{
					union
					{
						uint8 _raw = 0;
						struct
						{
							bool _renderZoneOverlay : 1;
							bool _renderMousePoints : 1;
							bool _renderResizingArea : 1;
						};
					};
				};
				DebugSwitch _debugSwitch;
				Theme _theme;

			private:
				GraphicDevice& _graphicDevice;
				ShapeRendererContext _rendererContext;
				float _fontSize;
				Platform::CursorType _currentCursor;

			private:
				struct NextControlDesc
				{
					bool _sameLine = false;
					Float2 _position;
					Float2 _size;
					Rect _margin;
					Rect _padding;
				};
				NextControlDesc _nextControlDesc;

			private:
				// Size, Position 등은 Control 마다 기록되어야 하는 State 이다.
				HashMap<ControlID, ControlData> _controlDataMap;
				Vector<ControlID> _controlStack;
				ControlID _rootControlID;
				Vector<ControlID> _controlIDsOfCurrentFrame;
				bool _isInBeginWindow = false;

			private:
				// Interaction
				Float2 _mousePressedPosition;
				DraggingModule _draggingModule;
				ResizingModule _resizingModule;
				FocusingModule _focusingModule;
			};
		}
	}
}


#endif // !_MINT_RENDERING_BASE_GUI_CONTEXT_H_

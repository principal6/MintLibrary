#pragma once


#ifndef FS_GUI_CONTEXT_H
#define FS_GUI_CONTEXT_H


#include <CommonDefinitions.h>

#include <FsLibrary/SimpleRendering/ShapeRenderer.h>
#include <FsLibrary/SimpleRendering/FontRenderer.h>
#include <FsLibrary/Container/IId.h>

#include <FsMath/Include/Float2.h>


namespace fs
{
	namespace SimpleRendering
	{
		class GraphicDevice;
	}

	namespace Window
	{
		class IWindow;
	}


	namespace Gui
	{
		class GuiContext;


		enum class ControlType : uint16
		{
			ROOT,

			Button,
			TitleBar,
			Window,

			COUNT
		};

		enum class ControlState
		{
			Visible,

			VisibleOpen,
			VisibleClosed,

			Invisible
		};


		class GuiContext final
		{
			static constexpr float						kDefaultIntervalX = 5.0f;
			static constexpr float						kDefaultIntervalY = 5.0f;
			static constexpr float						kDefaultRoundnessInPixel = 10.0f;
			static constexpr float						kDefaultControlWidth = 150.0f;
			static constexpr float						kDefaultFocusedAlpha = 0.9375f;
			static constexpr float						kDefaultOutOfFocusAlpha = 0.5f;

			struct ControlData
			{
			public:
									ControlData();
									ControlData(const fs::Float4& innerPadding);

			public:
				uint64				_hashKey;
				uint64				_parentHashKey;

				fs::Float4			_innerPadding; // L-R-T-B
				fs::Float2			_displaySize;
				fs::Float2			_interactionSize;
				fs::Float2			_position; // In screen space, at left-top corner
				fs::Float2			_childAt;
				fs::Float2			_offset;
				bool				_isFocusable;
				ControlType			_controlTypeForDebug;
				ControlState		_controlState;
			};

			struct ControlStackData
			{
			public:
									ControlStackData() = default;
									ControlStackData(const ControlType controlType, const uint64 hashKey);

			public:
				ControlType			_controlType;
				uint64				_hashKey;
			};

			enum class NamedColor
			{
				NormalState,
				HoverState,
				PressedState,
				WindowFocused,
				WindowOutOfFocus,
				TitleBarFocused,
				TitleBarOutOfFocus,
				LightFont,
				DarkFont,
				
				COUNT
			};

		public:
														GuiContext(fs::SimpleRendering::GraphicDevice* const graphicDevice);
														~GuiContext();

		public:
			void										initialize(const char* const font);

		public:
			void										handleEvents(fs::Window::IWindow* const window);

		private:
			const bool									isInControl(const fs::Float2& mousePosition, const ControlData& controlData) const noexcept;


#pragma region Next-states
		public:
			void										nextSameLine();
			void										nextControlSize(const fs::Float2& size, const bool force = false);
			void										nextNoAutoPositioned();

		private:
			void										resetNextStates();
#pragma endregion


#pragma region Controls
			// 
			// Button Label ToolTip
			// RatioButton CheckBox
			// ListView TreeView
			// ComboBox SpinBox
			// ScrollBar Slider
			// TextEdit
			// Splitter
			// Window

		public:
			// [Button]
			// Return 'true' if clicked
			const bool									beginButton(const wchar_t* const text);
			void										endButton();

			// [Window | Control with ID]
			// title is used as unique id for windows
			const bool									beginWindow(const wchar_t* const title, const fs::Float2& size, const fs::Float2& position);
			void										endWindow();

		private:
			void										beginTitleBar(const wchar_t* const title, const float width);
			void										endTitleBar();

		private:
			const ControlData&							getStackTopControlData() noexcept;
			ControlData&								getControlData(const uint64 hashKey) noexcept;
			const ControlData&							getControlData(const uint64 hashKey) const noexcept;
			fs::Float3									getControlCenterPosition(const ControlData& controlData) const noexcept;
			const uint64								generateControlHashKey(const wchar_t* const text, const ControlType controlType) const noexcept;
			ControlData&								getControlData(const wchar_t* const text, const fs::Float2& defaultSize, const ControlType controlType, const fs::Float2& desiredPosition = fs::Float2::kZero, const fs::Float4& innerPadding = fs::Float4::kZero) noexcept;
			void										calculateControlChildAt(ControlData& controlData) noexcept;
#pragma endregion


#pragma region Before drawing controls
		private:
			const bool									processClickControl(const ControlData& controlData, fs::SimpleRendering::Color& outBackgroundColor) const noexcept;
			const bool									processFocusControl(const ControlData& controlData, const fs::SimpleRendering::Color& focusedColor, const fs::SimpleRendering::Color& nonFocusedColor, fs::SimpleRendering::Color& outBackgroundColor) const noexcept;
			
			const bool									isDraggingControl(const ControlData& controlData) const noexcept;

			const bool									isMeOrAncestorFocusedXXX(const ControlData& controlData) const noexcept;
			const bool									isAncestorFocused(const ControlData& controlData) const noexcept;
			const bool									isAncestorFocusedRecursiveXXX(const uint64 hashKey) const noexcept;

			const fs::SimpleRendering::Color&			getNamedColor(const NamedColor namedColor) const noexcept;
			fs::SimpleRendering::Color&					getNamedColor(const NamedColor namedColor) noexcept;
#pragma endregion


		public:
			void										render();

		private:
			void										resetControlStatesPerFrame();

		private:
			fs::SimpleRendering::GraphicDevice* const	_graphicDevice;

			fs::SimpleRendering::ShapeRenderer			_shapeRendererBackground;
			fs::SimpleRendering::FontRenderer			_fontRendererBackground;

			fs::SimpleRendering::ShapeRenderer			_shapeRendererForeground;
			fs::SimpleRendering::FontRenderer			_fontRendererForeground;

		private:
			const ControlData							kNullControlData;
			ControlData									_rootControlData;
		
		private:
			std::vector<ControlStackData>				_controlStackPerFrame;

		private:
			mutable uint64								_focusedControlHashKey;
			mutable uint64								_draggedControlHashKey;
			mutable fs::Float2							_draggedControlInitialPosition;
			mutable bool								_dragStarted;
			std::unordered_map<uint64, ControlData>		_controlIdMap;


#pragma region Next-states
		private:
			bool										_nextSameLine;
			fs::Float2									_nextControlSize;
			bool										_nextSizingForced;
			bool										_nextNoAutoPositioned;
#pragma endregion


#pragma region Mouse states
		private:
			fs::Float2									_mousePosition;
			fs::Float2									_mouseDownPosition;
			fs::Float2									_mouseUpPosition;
			bool										_mouseButtonDown;
			bool										_mouseDownUp;
#pragma endregion

		private:
			fs::SimpleRendering::Color					_namedColors[static_cast<uint32>(NamedColor::COUNT)];
		};
	}
}


#include <FsLibrary/Gui/GuiContext.inl>


#endif // !FS_GUI_CONTEXT_H

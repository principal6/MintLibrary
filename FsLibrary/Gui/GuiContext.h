#pragma once


#ifndef FS_GUI_CONTEXT_H
#define FS_GUI_CONTEXT_H


#include <CommonDefinitions.h>

#include <FsLibrary/SimpleRendering/ShapeRenderer.h>
#include <FsLibrary/SimpleRendering/FontRenderer.h>

#include <FsLibrary/Container/IId.h>

#include <FsLibrary/Platform/IWindow.h>

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
			TitleBar, // PRIVATE
			RoundButton, // PRIVATE
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

		enum class ResizeMethod
		{
			ResizeOnly,
			RepositionHorz,
			RepositionVert,
			RepositionBoth,
		};


		class GuiContext final
		{
			static constexpr float						kDefaultIntervalX = 5.0f;
			static constexpr float						kDefaultIntervalY = 5.0f;
			static constexpr float						kDefaultRoundnessInPixel = 8.0f;
			static constexpr float						kDefaultControlWidth = 150.0f;
			static constexpr float						kDefaultFocusedAlpha = 0.875f;
			static constexpr float						kDefaultOutOfFocusAlpha = 0.5f;
			static constexpr float						kDefaultRoundButtonRadius = 7.0f;
			static constexpr float						kControlDisplayMinWidth = 10.0f;
			static constexpr float						kControlDisplayMinHeight = 10.0f;

			class ControlData
			{
			public:
										ControlData();
										ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType);
										ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType, const fs::Float2& size);
			
			public:
				const uint64			getHashKey() const noexcept;
				const uint64			getParentHashKey() const noexcept;
				const fs::Float4&		getInnerPadding() const noexcept;
				const fs::Float2&		getDisplaySize() const noexcept;
				const fs::Float2&		getChildAt() const noexcept;
				const fs::Float2&		getOffset() const noexcept;
				const ControlType		getControlType() const noexcept;
				const bool				isControlState(const ControlState controlState) const noexcept;

			public:
				void					setControlState(const ControlState controlState) noexcept;
			
			public:
				void					setOffsetY_XXX(const float offsetY) noexcept;

			public:
				fs::Float2				_interactionSize;
				fs::Float2				_position; // In screen space, at left-top corner
				bool					_isFocusable;
				bool					_isResizable;
				bool					_isDraggable;
				uint64					_dragTargetHashKey;

			private:
				uint64					_hashKey;
				uint64					_parentHashKey;
				fs::Float4				_innerPadding; // L-R-T-B
				fs::Float2				_displaySize;
				fs::Float2				_childAt;
				fs::Float2				_offset;
				ControlType				_controlType;
				ControlState			_controlState;
			};

			struct ControlStackData
			{
			public:
									ControlStackData() = default;
									ControlStackData(const ControlData& controlData);

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
			const bool									isInControlInternal(const fs::Float2& screenPosition, const fs::Float2& controlPosition, const fs::Float2& interactionSize) const noexcept;
			const bool									isInControlInteractionArea(const fs::Float2& screenPosition, const ControlData& controlData) const noexcept;
			const bool									isInControlBorderArea(const fs::Float2& screenPosition, const ControlData& controlData, fs::Window::CursorType& outCursorType, ResizeMethod& outResizeMethod) const noexcept;


#pragma region Next-states
		public:
			void										nextSameLine();
			void										nextControlSize(const fs::Float2& size, const bool force = false);
			void										nextNoAutoPositioned();
			// Only works if NoAutoPositioned!
			void										nextControlPosition(const fs::Float2& position);

		private:
			void										resetNextStates();
#pragma endregion


#pragma region Controls
			// 
			// Button Window
			// Label ToolTip
			// ScrollBar Slider
			// Window docking system!!!
			// RadioButton CheckBox
			// ListView TreeView
			// ComboBox SpinBox
			// TextEdit
			// Splitter

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
			// Returns size of titlebar
			fs::Float2									beginTitleBar(const wchar_t* const windowTitle, const fs::Float2& parentWindowDisplaySize);
			void										endTitleBar();

			const bool									beginRoundButton(const wchar_t* const windowTitle, const fs::SimpleRendering::Color& color);
			void										endRoundButton();

		private:
			const ControlData&							getStackTopControlData() noexcept;
			ControlData&								getControlData(const uint64 hashKey) noexcept;
			const ControlData&							getControlData(const uint64 hashKey) const noexcept;
			fs::Float3									getControlCenterPosition(const ControlData& controlData) const noexcept;
			const uint64								generateControlHashKey(const wchar_t* const text, const ControlType controlType) const noexcept;
			ControlData&								getControlData(const wchar_t* const text, const fs::Float2& initialDisplaySize, const ControlType controlType, const fs::Float2& desiredPosition = fs::Float2::kZero, 
				const fs::Float4& innerPadding = fs::Float4::kZero, const fs::Float2& deltaInteractionSize = fs::Float2::kZero, const bool resetDisplaySize = false) noexcept;
			void										calculateControlChildAt(ControlData& controlData) noexcept;
#pragma endregion


#pragma region Before drawing controls
		private:
			const bool									processClickControl(ControlData& controlData, const fs::SimpleRendering::Color& normalColor, const fs::SimpleRendering::Color& hoverColor, const fs::SimpleRendering::Color& pressedColor, fs::SimpleRendering::Color& outBackgroundColor) noexcept;
			const bool									processFocusControl(ControlData& controlData, const fs::SimpleRendering::Color& focusedColor, const fs::SimpleRendering::Color& nonFocusedColor, fs::SimpleRendering::Color& outBackgroundColor) noexcept;
			void										processControlCommonInternal(ControlData& controlData) noexcept;
			const bool									shouldApplyChange(const ControlData& controlData) const noexcept;
			
			const bool									isDraggingControl(const ControlData& controlData) const noexcept;
			const bool									isResizingControl(const ControlData& controlData) const noexcept;

			const bool									isMeOrAncestorFocusedXXX(const ControlData& controlData) const noexcept;
			const bool									isAncestorFocused(const ControlData& controlData) const noexcept;
			const bool									isAncestorFocusedRecursiveXXX(const uint64 hashKey) const noexcept;

			const fs::SimpleRendering::Color&			getNamedColor(const NamedColor namedColor) const noexcept;
			fs::SimpleRendering::Color&					getNamedColor(const NamedColor namedColor) noexcept;
#pragma endregion


		public:
			void										render();

		private:
			void										resetStatesPerFrame();

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
		

#pragma region Mouse Capture States
		private:
			mutable bool								_isDragBegun;
			mutable uint64								_draggedControlHashKey;
			mutable fs::Float2							_draggedControlInitialPosition;
		
		private:
			mutable bool								_isResizeBegun;
			mutable uint64								_resizedControlHashKey;
			mutable fs::Float2							_resizedControlInitialPosition;
			mutable fs::Float2							_resizedControlInitialDisplaySize;
			mutable ResizeMethod						_resizeMethod;
#pragma endregion
		
		private:
			std::unordered_map<uint64, ControlData>		_controlIdMap;


#pragma region Next-states
		private:
			bool										_nextSameLine;
			fs::Float2									_nextControlSize;
			bool										_nextSizingForced;
			bool										_nextNoAutoPositioned;
			fs::Float2									_nextControlPosition;
#pragma endregion


#pragma region Mouse states
		private:
			fs::Float2									_mousePosition;
			fs::Float2									_mouseDownPosition;
			fs::Float2									_mouseUpPosition;
			bool										_mouseButtonDown;
			bool										_mouseDownUp;
			mutable fs::Window::CursorType				_cursorType; // per frame
#pragma endregion

		private:
			fs::SimpleRendering::Color					_namedColors[static_cast<uint32>(NamedColor::COUNT)];
		};
	}
}


#include <FsLibrary/Gui/GuiContext.inl>


#endif // !FS_GUI_CONTEXT_H

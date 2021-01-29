#pragma once


#ifndef FS_GUI_CONTEXT_H
#define FS_GUI_CONTEXT_H


#include <CommonDefinitions.h>

#include <FsLibrary/Gui/GuiCommon.h>

#include <FsLibrary/SimpleRendering/ShapeRenderer.h>
#include <FsLibrary/SimpleRendering/FontRenderer.h>

#include <FsLibrary/Container/IId.h>

#include <FsLibrary/Platform/IWindow.h>

#include <FsMath/Include/Float2.h>
#include <FsMath/Include/Float4.h>


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


		// If no value is set, default values will be used properly
		struct LabelParam
		{
			fs::SimpleRendering::Color	_backgroundColor	= fs::SimpleRendering::Color::kTransparent;
			fs::SimpleRendering::Color	_fontColor			= fs::SimpleRendering::Color::kTransparent;
			fs::Float2					_size				= fs::Float2::kZero;
			TextAlignmentHorz			_alignmentHorz		= TextAlignmentHorz::Middle;
			TextAlignmentVert			_alignmentVert		= TextAlignmentVert::Center;
		};

		struct WindowParam
		{
			fs::Float2			_size				= fs::Float2(180, 100);
			fs::Float2			_position			= fs::Float2(100, 100);
			ScrollBarType		_scrollBarType		= ScrollBarType::None;
		};

		union ControlValue
		{
			int32	_i{ 0 };
			float	_f;
		};

		class GuiContext final
		{
			static constexpr float						kDefaultIntervalX = 5.0f;
			static constexpr float						kDefaultIntervalY = 5.0f;
			static constexpr float						kDefaultRoundnessInPixel = 8.0f;
			static constexpr float						kDefaultFocusedAlpha = 0.875f;
			static constexpr float						kDefaultOutOfFocusAlpha = 0.5f;
			static constexpr float						kDefaultRoundButtonRadius = 7.0f;
			static constexpr float						kControlDisplayMinWidth = 10.0f;
			static constexpr float						kControlDisplayMinHeight = 10.0f;
			static constexpr float						kFontScaleA = 1.0f;
			static constexpr float						kFontScaleB = 0.875f;
			static constexpr float						kFontScaleC = 0.8125f;
			static constexpr float						kScrollBarVertWidth = 8.0f;
			static constexpr Rect						kTitleBarInnerPadding = Rect(12.0f, 6.0f, 6.0f, 6.0f);
			static constexpr fs::Float2					kTitleBarBaseSize = fs::Float2(0.0f, fs::SimpleRendering::kDefaultFontSize + kTitleBarInnerPadding.top() + kTitleBarInnerPadding.bottom());
			static constexpr float						kHalfBorderThickness = 5.0f;

			class ControlData
			{
			public:
										ControlData();
										ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType);
										ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType, const fs::Float2& size);
			
			public:
				const uint64			getHashKey() const noexcept;
				const uint64			getParentHashKey() const noexcept;
				const Rect&				getInnerPadding() const noexcept;
				const fs::Float2&		getDisplaySize() const noexcept;
				const fs::Float2&		getDisplaySizeMin() const noexcept;
				const fs::Float2&		getClientSize() const noexcept;
				const fs::Float2&		getChildAt() const noexcept;
				const fs::Float2&		getNextChildOffset() const noexcept;
				const ControlType		getControlType() const noexcept;
				const bool				isControlState(const ControlState controlState) const noexcept;
				const uint32			getViewportIndex() const noexcept;
				const float				getViewportIndexAsFloat() const noexcept;

			public:
				void					setControlState(const ControlState controlState) noexcept;
			
			public:
				void					setParentHashKeyXXX(const uint64 parentHashKey) noexcept;
				void					setOffsetY_XXX(const float offsetY) noexcept;
				void					setViewportIndexXXX(const uint32 viewportIndex) noexcept;

			public:
				fs::Float2				_interactionSize;
				fs::Float2				_position; // In screen space, at left-top corner
				fs::Float2				_displayOffset; // Used for scrolling
				bool					_isFocusable;
				bool					_isResizable;
				bool					_isDraggable;
				Rect					_draggingConstraints; // MUST set all four values if want to limit dragging area
				uint64					_delegateHashKey; // Used for drag, resize and focus
				ControlValue			_value;

			private:
				uint64					_hashKey;
				uint64					_parentHashKey;
				Rect					_innerPadding; // For child controls
				fs::Float2				_displaySize;
				fs::Float2				_displaySizeMin;
				fs::Float2				_clientSize; // Could be smaller or larger than _displaySize
				fs::Float2				_childAt; // In screen space, Next child control will be positioned according to this
				fs::Float2				_nextChildOffset; // Every new child sets this offset to calculate next _childAt
				ControlType				_controlType;
				ControlState			_controlState;
				uint32					_viewportIndex;
			};
			
			struct ControlDataParam
			{
				Rect				_innerPadding;
				fs::Float2			_initialDisplaySize;
				fs::Float2			_desiredPositionInParent	= fs::Float2::kZero;
				fs::Float2			_deltaInteractionSize		= fs::Float2::kZero;
				fs::Float2			_displaySizeMin				= fs::Float2(kControlDisplayMinWidth, kControlDisplayMinHeight);
				uint64				_parentHashKeyOverride		= 0;
				bool				_alwaysResetDisplaySize		= false;
				bool				_alwaysResetParent			= false;
				bool				_alwaysResetPosition		= true;
				const wchar_t*		_hashGenerationKeyOverride	= nullptr;
				bool				_ignoreForClientSize		= false;
				bool				_useParentViewport			= false;
			};
			
			struct ControlStackData
			{
			public:
									ControlStackData() = default;
									ControlStackData(const ControlData& controlData);

			public:
				ControlType			_controlType;
				uint64				_hashKey;
				ScrollBarType		_scrollBarTypeForWindow;
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

				TooltipBackground,

				ScrollBarTrack,
				ScrollBarThumb,

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
			void										nextTooltip(const wchar_t* const tooltipText);

		private:
			void										resetNextStates();
#pragma endregion


#pragma region Controls
			// 
			// Button Window
			// Tooltip Label
			// ScrollBar Slider
			// Window docking system!!!
			// RadioButton CheckBox
			// ListView TreeView
			// ComboBox
			// TextEdit SpinBox
			// Splitter

		public:
			// [Window | Control with ID]
			// title is used as unique id for windows
			const bool									beginWindow(const wchar_t* const title, const WindowParam& windowParam);
			void										endWindow();

			// [Button]
			// Return 'true' if clicked
			const bool									beginButton(const wchar_t* const text);
			void										endButton();

			// [Label]
			void										pushLabel(const wchar_t* const text, const LabelParam& labelParam = LabelParam());

		private:
			// Returns size of titlebar
			fs::Float2									beginTitleBar(const wchar_t* const windowTitle, const fs::Float2& titleBarSize, const Rect& innerPadding);
			void										endTitleBar();

			const bool									beginRoundButton(const wchar_t* const windowTitle, const fs::SimpleRendering::Color& color);
			void										endRoundButton();

			// [Tooltip]
			// Unique control
			void										pushTooltipWindow(const wchar_t* const tooltipText, const fs::Float2& position);

			// [ScrollBar - Vertical]
			// Return 'true' if value was changed
			const bool									pushScrollBarVert();

			// [ScrollBar - Horizontal]
			// Return 'true' if value was changed
			const bool									pushScrollBarHorz();

		private:
			const ControlData&							getControlDataStackTop() noexcept;
			ControlData&								getControlData(const uint64 hashKey) noexcept;
			const ControlData&							getControlData(const uint64 hashKey) const noexcept;
			fs::Float3									getControlCenterPosition(const ControlData& controlData) const noexcept;
			const wchar_t*								generateControlKeyString(const ControlData& parentControlData, const wchar_t* const text, const ControlType controlType) const noexcept;
			const uint64								generateControlHashKeyXXX(const wchar_t* const text, const ControlType controlType) const noexcept;
			ControlData&								getControlData(const wchar_t* const text, const ControlType controlType, const ControlDataParam& controlDataParam) noexcept;
			void										calculateControlChildAt(ControlData& controlData) noexcept;
			const ControlData&							getParentWindowControlData(const ControlData& controlData) const noexcept;
			const ControlData&							getParentWindowControlDataInternal(const uint64 hashKey) const noexcept;
#pragma endregion


#pragma region Before drawing controls
		private:
			const bool									processClickControl(ControlData& controlData, const fs::SimpleRendering::Color& normalColor, const fs::SimpleRendering::Color& hoverColor, const fs::SimpleRendering::Color& pressedColor, fs::SimpleRendering::Color& outBackgroundColor) noexcept;
			const bool									processFocusControl(ControlData& controlData, const fs::SimpleRendering::Color& focusedColor, const fs::SimpleRendering::Color& nonFocusedColor, fs::SimpleRendering::Color& outBackgroundColor) noexcept;
			void										processShowOnlyControl(ControlData& controlData, fs::SimpleRendering::Color& outBackgroundColor) noexcept;
			const bool									processScrollableControl(ControlData& controlData, const fs::SimpleRendering::Color& normalColor, const fs::SimpleRendering::Color& dragColor, fs::SimpleRendering::Color& outBackgroundColor) noexcept;
			
			void										processControlInteractionInternal(ControlData& controlData) noexcept;
			void										processControlCommonInternal(ControlData& controlData) noexcept;
			const bool									isInteractingInternal(const ControlData& controlData) const noexcept;
			
			// These functions must be called after process- functions
			const bool									isDraggingControl(const ControlData& controlData) const noexcept;
			const bool									isResizingControl(const ControlData& controlData) const noexcept;
			const bool									isControlHovered(const ControlData& controlData) const noexcept;
			const bool									isControlPressed(const ControlData& controlData) const noexcept;
			const bool									isControlClicked(const ControlData& controlData) const noexcept;

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

			std::vector<D3D11_VIEWPORT>					_viewportArrayBackgroundPerFrame;
			std::vector<D3D11_RECT>						_scissorRectangleArrayBackgroundPerFrame;

			std::vector<D3D11_VIEWPORT>					_viewportArrayForegroundPerFrame;
			std::vector<D3D11_RECT>						_scissorRectangleArrayForegroundPerFrame;

		private:
			const ControlData							kNullControlData;
			ControlData									_rootControlData;
		
		private:
			std::vector<ControlStackData>				_controlStackPerFrame;

		private:
			mutable uint64								_focusedControlHashKey;
			mutable uint64								_hoveredControlHashKey;
			mutable uint64								_pressedControlHashKey;
			mutable fs::Float2							_pressedControlInitialPosition;
			mutable uint64								_clickedControlHashKeyPerFrame;
			uint64										_hoverStartTimeMs;
			bool										_hoverStarted;
		

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
			const wchar_t*								_nextTooltipText;
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
			fs::Float2									_tooltipPosition;
			uint64										_tooltipParentWindowHashKey;
			const wchar_t*								_tooltipTextFinal;

		private:
			fs::SimpleRendering::Color					_namedColors[static_cast<uint32>(NamedColor::COUNT)];
		};
	}
}


#include <FsLibrary/Gui/GuiContext.inl>


#endif // !FS_GUI_CONTEXT_H

#pragma once


#ifndef FS_GUI_CONTEXT_H
#define FS_GUI_CONTEXT_H


#include <CommonDefinitions.h>

#include <FsRenderingBase/Include/GuiCommon.h>

#include <FsRenderingBase/Include/ShapeFontRendererContext.h>

#include <FsContainer/Include/IId.h>

#include <FsPlatform/Include/IWindow.h>

#include <FsMath/Include/Float2.h>
#include <FsMath/Include/Float4.h>
#include <FsMath/Include/Rect.h>


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

		enum class ViewportUsage
		{
			Parent,
			ParentDock,
			Child,
		};

		struct WindowParam
		{
			fs::Float2			_size					= fs::Float2(180, 100);
			fs::Float2			_position				= fs::Float2(100, 100);
			ScrollBarType		_scrollBarType			= ScrollBarType::None;
			DockingMethod		_initialDockingMethod	= DockingMethod::COUNT;
			fs::Float2			_initialDockingSize		= fs::Float2(160);
		};

		// If no value is set, default values will be used properly
		struct LabelParam
		{
			fs::SimpleRendering::Color	_backgroundColor	= fs::SimpleRendering::Color::kTransparent;
			fs::SimpleRendering::Color	_fontColor			= fs::SimpleRendering::Color::kTransparent;
			fs::Float2					_size				= fs::Float2::kZero;
			TextAlignmentHorz			_alignmentHorz		= TextAlignmentHorz::Middle;
			TextAlignmentVert			_alignmentVert		= TextAlignmentVert::Center;
		};

		struct SliderParam
		{
			//uint32		_stepCount		= 0; // If stepcount is 0, the value is treated as real number
			//float			_min			= 0.0f;
			//float			_max			= 1.0f;
			//float			_stride			= 0.1f; // Only applies when (_stepCount == 0)
			//bool			_isVertical		= false; // Horizontal if false
			fs::Float2		_size			= fs::Float2(128.0f, 0.0f);
		};

		union ControlValue
		{
			int32	_i{ 0 };
			float	_f;
		};

		struct ResizingMask
		{
		public:
									ResizingMask();
									ResizingMask(const ResizingMask& rhs)		= default;
									ResizingMask(ResizingMask&& rhs) noexcept	= default;
									~ResizingMask()								= default;

		public:
			ResizingMask&			operator=(const ResizingMask& rhs)			= default;
			ResizingMask&			operator=(ResizingMask&& rhs) noexcept		= default;

		public:
			static ResizingMask		fromDockingMethod(const DockingMethod dockingMethod) noexcept;

		public:
			void					setAllTrue() noexcept;
			void					setAllFalse() noexcept;
			const bool				isResizable() const noexcept;
			const bool				topLeft() const noexcept;
			const bool				topRight() const noexcept;
			const bool				bottomLeft() const noexcept;
			const bool				bottomRight() const noexcept;
			const bool				overlaps(const ResizingMask& rhs) const noexcept;

		public:
			union
			{
				uint8	_rawMask;
				struct
				{
					bool	_top : 1;
					bool	_left : 1;
					bool	_right : 1;
					bool	_bottom : 1;
				};
			};
		};


		class TaskWhenMouseUp
		{
		public:
			void				clear() noexcept;
			const bool			isSet() const noexcept;
			void				setUpdateDockDatum(const uint64 controlHashKey) noexcept;
			const uint64		getUpdateDockDatum() const noexcept;

		private:
			uint64				_controlHashKeyForUpdateDockDatum = 0;
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
			static constexpr float						kScrollBarThickness = 8.0f;
			static constexpr fs::Rect					kTitleBarInnerPadding = fs::Rect(12.0f, 6.0f, 6.0f, 6.0f);
			static constexpr fs::Float2					kTitleBarBaseSize = fs::Float2(0.0f, fs::SimpleRendering::kDefaultFontSize + kTitleBarInnerPadding.top() + kTitleBarInnerPadding.bottom());
			static constexpr float						kHalfBorderThickness = 5.0f;
			static constexpr float						kSliderTrackThicknes = 6.0f;
			static constexpr float						kSliderThumbRadius = 8.0f;
			static constexpr float						kDockingInteractionShort = 30.0f;
			static constexpr float						kDockingInteractionLong = 40.0f;
			static constexpr float						kDockingInteractionDisplayBorderThickness = 2.0f;
			static constexpr float						kDockingInteractionOffset = 5.0f;


			class DockDatum
			{
			public:
											DockDatum();
											~DockDatum() = default;

			public:
				const bool					hasDockedControls() const noexcept;

			public:
				const bool					isRawDockSizeSet() const noexcept;
				void						setRawDockSize(const fs::Float2& rawDockSize) noexcept;
				const fs::Float2&			getRawDockSizeXXX() const noexcept;
				void						swapDockedControlsXXX(const int32 indexA, const int32 indexB) noexcept;
				const int32					getDockedControlIndex(const uint64 dockedControlHashKey) const noexcept;
				const uint64				getDockedControlHashKey(const int32 dockedControlIndex) const noexcept;
				const float					getDockedControlTitleBarOffset(const int32 dockedControlIndex) const noexcept;
				const int32					getDockedControlIndexByMousePosition(const float relativeMousePositionX) const noexcept;

			public:
				std::vector<uint64>			_dockedControlHashArray;
				int32						_dockedControlIndexShown;
				std::vector<float>			_dockedControlTitleBarOffsetArray; // TitleBar 렌더링 위치 계산에 사용
				std::vector<float>			_dockedControlTitleBarWidthArray; // TitleBar 순서 변경 시 마우스 Interaction 에 사용!

			private:
				fs::Float2					_rawDockSize;
			};


			struct DockingStateContext
			{
				fs::Float2							_displaySize;
				ResizingMask						_resizingMask;
			};


			class ControlData
			{
			public:
															ControlData();
															ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType);
															ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType, const fs::Float2& size);
			
			public:
				void										clearPerFrameData() noexcept;

			public:
				const uint64								getHashKey() const noexcept;
				const uint64								getParentHashKey() const noexcept;
				const fs::Rect&								getInnerPadding() const noexcept;
				fs::Float2									getClientSize() const noexcept;
				const fs::Float2&							getDisplaySizeMin() const noexcept;
				const fs::Float2&							getInteractionSize() const noexcept;
				const fs::Float2&							getNonDockInteractionSize() const noexcept;
				const fs::Float2&							getContentAreaSize() const noexcept;
				const fs::Float2&							getPreviousContentAreaSize() const noexcept;
				const fs::Float2&							getChildAt() const noexcept;
				const fs::Float2&							getNextChildOffset() const noexcept;
				const ControlType							getControlType() const noexcept;
				const wchar_t*								getText() const noexcept;
				const bool									isRootControl() const noexcept;
				const bool									isControlState(const ControlState controlState) const noexcept;
				const uint32								getViewportIndex() const noexcept;
				const uint32								getViewportIndexForChildren() const noexcept;
				const uint32								getViewportIndexForDocks() const noexcept;
				const std::vector<ControlData>&				getChildControlDataArray() const noexcept;
				const bool									hasChildWindow() const noexcept;
				DockDatum&									getDockDatum(const DockingMethod dockingMethod) noexcept;
				const DockDatum&							getDockDatum(const DockingMethod dockingMethod) const noexcept;
				const bool									isShowingInDock(const ControlData& dockedControlData) const noexcept;
				void										setDockSize(const DockingMethod dockingMethod, const fs::Float2& dockSize) noexcept;
				const fs::Float2							getDockSize(const DockingMethod dockingMethod) const noexcept;
				const fs::Float2							getDockOffsetSize() const noexcept;
				const fs::Float2							getDockPosition(const DockingMethod dockingMethod) const noexcept;
				const float									getHorzDockSizeSum() const noexcept;
				const float									getVertDockSizeSum() const noexcept;
				void										connectToDock(const uint64 dockControlHashKey) noexcept;
				void										disconnectFromDock() noexcept;
				const uint64								getDockControlHashKey() const noexcept;
				const bool									isDocking() const noexcept;
				const bool									isDockHosting() const noexcept;
				const bool									isResizable() const noexcept;
			
			public:
				const bool									hasChildWindowConnected(const uint64 childWindowHashKey) const noexcept;
				void										connectChildWindow(const uint64 childWindowHashKey) noexcept;
				void										disconnectChildWindow(const uint64 childWindowHashKey) noexcept;
				const std::unordered_map<uint64, bool>&		getChildWindowHashKeyMap() const noexcept;

			public:
				void										setControlState(const ControlState controlState) noexcept;
				void										swapDockingStateContext() noexcept;
			
			public:
				void										setParentHashKeyXXX(const uint64 parentHashKey) noexcept;
				void										setOffsetY_XXX(const float offsetY) noexcept;
				void										setViewportIndexXXX(const uint32 viewportIndex) noexcept;
				void										setViewportIndexForChildrenXXX(const uint32 viewportIndex) noexcept;
				void										setViewportIndexForDocksXXX(const uint32 viewportIndex) noexcept;

			public:
				uint8										_updateCount;
				fs::Float2									_displaySize;
				fs::Float2									_position; // In screen space, at left-top corner
				fs::Float2									_deltaPosition;
				fs::Float2									_displayOffset; // Used for scrolling child controls (of Window control)
				bool										_isFocusable;
				ResizingMask								_resizingMask;
				bool										_isDraggable;
				fs::Rect									_draggingConstraints; // MUST set all four values if want to limit dragging area
				uint64										_delegateHashKey; // Used for drag, resize and focus
				DockingControlType							_dockingControlType;
				DockingMethod								_lastDockingMethod;
				DockingMethod								_lastDockingMethodCandidate;
				std::wstring								_text;

				// [Window] ScrollBar type
				// [ScrollBar] Thumb at [0, 1]
				ControlValue								_internalValue;

			private:
				uint64										_hashKey;
				uint64										_parentHashKey;
				fs::Rect									_innerPadding; // For child controls
				fs::Float2									_displaySizeMin;
				fs::Float2									_interactionSize; // _nonDockInteractionSize + dock size
				fs::Float2									_nonDockInteractionSize; // Exluces dock area
				fs::Float2									_contentAreaSize; // Could be smaller or larger than _displaySize
				fs::Float2									_previousContentAreaSize;
				fs::Float2									_childAt; // In screen space, Next child control will be positioned according to this
				fs::Float2									_nextChildOffset; // Every new child sets this offset to calculate next _childAt
				ControlType									_controlType;
				ControlState								_controlState;
				uint32										_viewportIndex;
				uint32										_viewportIndexForChildren; // Used by window
				uint32										_viewportIndexForDocks;
				std::vector<ControlData>					_childControlDataArray;
				std::unordered_map<uint64, bool>			_childWindowHashKeyMap;
				DockDatum									_dockData[static_cast<uint32>(DockingMethod::COUNT)];
				uint64										_dockControlHashKey;
				DockingStateContext							_dokcingStateContext;
			};
			
			struct ParamPrepareControlData
			{
				fs::Rect			_innerPadding;
				fs::Float2			_initialDisplaySize;
				ResizingMask		_initialResizingMask;
				fs::Float2			_desiredPositionInParent			= fs::Float2::kNan;
				fs::Float2			_deltaInteractionSize				= fs::Float2::kZero;
				fs::Float2			_deltaInteractionSizeByDock			= fs::Float2::kZero;
				fs::Float2			_displaySizeMin						= fs::Float2(kControlDisplayMinWidth, kControlDisplayMinHeight);
				bool				_alwaysResetDisplaySize				= false;
				bool				_alwaysResetParent					= false;
				uint64				_parentHashKeyOverride				= 0;
				bool				_alwaysResetPosition				= true;
				bool				_ignoreMeForClientSize				= false;
				ViewportUsage		_viewportUsage						= ViewportUsage::Child;
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
				Dock,
				ShownInDock,

				TitleBarFocused,
				TitleBarOutOfFocus,

				TooltipBackground,

				ScrollBarTrack,
				ScrollBarThumb,

				LightFont,
				DarkFont,
				ShownInDockFont,
				
				COUNT
			};


		public:
																GuiContext(fs::SimpleRendering::GraphicDevice* const graphicDevice);
																~GuiContext();

		public:
			void												initialize(const char* const font);

		public:
			void												handleEvents(fs::Window::IWindow* const window);

		private:
			const bool											isInControlInternal(const fs::Float2& screenPosition, const fs::Float2& controlPosition, const fs::Float2& controlPositionOffset, const fs::Float2& interactionSize) const noexcept;
			const bool											isInControlInteractionArea(const fs::Float2& screenPosition, const ControlData& controlData) const noexcept;
			const bool											isInControlBorderArea(const fs::Float2& screenPosition, const ControlData& controlData, fs::Window::CursorType& outCursorType, ResizingMask& outResizingMask, ResizingMethod& outResizingMethod) const noexcept;
			const bool											shouldIgnoreInteraction(const fs::Float2& screenPosition, const ControlData& controlData) const noexcept;


#pragma region Next-states
		public:
			void												nextSameLine();
			void												nextControlSize(const fs::Float2& size, const bool force = false);
			void												nextNoAutoPositioned();
			// Only works if NoAutoPositioned!
			void												nextControlPosition(const fs::Float2& position);
			void												nextTooltip(const wchar_t* const tooltipText);

		private:
			void												resetNextStates();
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
			const bool											beginWindow(const wchar_t* const title, const WindowParam& windowParam);
			void												endWindow() { endControlInternal(ControlType::Window); }

			// [Button]
			// Return 'true' if clicked
			const bool											beginButton(const wchar_t* const text);
			void												endButton() { endControlInternal(ControlType::Button); }

			// [Label]
			void												pushLabel(const wchar_t* const text, const LabelParam& labelParam = LabelParam());

			// [Slider]
			// Return 'true' if value was changed
			const bool											beginSlider(const wchar_t* const name, const SliderParam& SliderParam, float& outValue);
			void												endSlider() { endControlInternal(ControlType::Slider); }

		private:
			// Returns size of titlebar
			fs::Float2											beginTitleBar(const wchar_t* const windowTitle, const fs::Float2& titleBarSize, const fs::Rect& innerPadding);
			void												endTitleBar() { endControlInternal(ControlType::TitleBar); }

			const bool											beginRoundButton(const wchar_t* const windowTitle, const fs::SimpleRendering::Color& color);
			void												endRoundButton() { endControlInternal(ControlType::RoundButton); }

			// [Tooltip]
			// Unique control
			void												pushTooltipWindow(const wchar_t* const tooltipText, const fs::Float2& position);

			// [ScrollBar]
			// Return 'true' if value was changed
			void												pushScrollBar(const ScrollBarType scrollBarType);

		private:
			void												renderDock(const ControlData& controlData, fs::SimpleRendering::ShapeFontRendererContext& shapeFontRendererContext);
			void												endControlInternal(const ControlType controlType);

		private:
			const ControlData&									getControlDataStackTopXXX() const noexcept;
			ControlData&										getControlDataStackTopXXX() noexcept;
			ControlData&										getControlData(const uint64 hashKey) noexcept;
			const ControlData&									getControlData(const uint64 hashKey) const noexcept;
			fs::Float4											getControlCenterPosition(const ControlData& controlData) const noexcept;
			const wchar_t*										generateControlKeyString(const ControlData& parentControlData, const wchar_t* const text, const ControlType controlType) const noexcept;
			const uint64										generateControlHashKeyXXX(const wchar_t* const text, const ControlType controlType) const noexcept;
			ControlData&										getControlData(const wchar_t* const text, const ControlType controlType, const wchar_t* const hashGenerationKeyOverride = nullptr) noexcept;
			void												calculateControlChildAt(ControlData& controlData) noexcept;
			const ControlData&									getParentWindowControlData() const noexcept;
			const ControlData&									getParentWindowControlData(const ControlData& controlData) const noexcept;
			const ControlData&									getParentWindowControlDataInternal(const uint64 hashKey) const noexcept;
#pragma endregion


#pragma region Before drawing controls
		private:
			void												prepareControlData(ControlData& controlData, const ParamPrepareControlData& paramPrepareControlData) noexcept;
			
			const bool											processClickControl(ControlData& controlData, const fs::SimpleRendering::Color& normalColor, const fs::SimpleRendering::Color& hoverColor, const fs::SimpleRendering::Color& pressedColor, fs::SimpleRendering::Color& outBackgroundColor) noexcept;
			const bool											processFocusControl(ControlData& controlData, const fs::SimpleRendering::Color& focusedColor, const fs::SimpleRendering::Color& nonFocusedColor, fs::SimpleRendering::Color& outBackgroundColor) noexcept;
			void												processShowOnlyControl(ControlData& controlData, fs::SimpleRendering::Color& outBackgroundColor, const bool doNotSetMouseInteractionDone = false) noexcept;
			const bool											processScrollableControl(ControlData& controlData, const fs::SimpleRendering::Color& normalColor, const fs::SimpleRendering::Color& dragColor, fs::SimpleRendering::Color& outBackgroundColor) noexcept;
			
			void												processControlInteractionInternal(ControlData& controlData, const bool doNotSetMouseInteractionDone = false) noexcept;
			void												processControlCommonInternal(ControlData& controlData) noexcept;
			void												processControlDocking(ControlData& controlData, const bool isDragging) noexcept;

			void												dock(const uint64 dockedControlHashKey, const uint64 dockControlHashKey) noexcept;
			void												undock(const uint64 dockedControlHashKey) noexcept;
			void												updateDockDatum(const uint64 dockControlHashKey, const bool dontUpdateWidthArray = false) noexcept;

			const bool											isInteractingInternal(const ControlData& controlData) const noexcept;
			
			// These functions must be called after process- functions
			const bool											isDraggingControl(const ControlData& controlData) const noexcept;
			const bool											isResizingControl(const ControlData& controlData) const noexcept;
			const bool											isControlHovered(const ControlData& controlData) const noexcept;
			const bool											isControlPressed(const ControlData& controlData) const noexcept;
			const bool											isControlClicked(const ControlData& controlData) const noexcept;
			const bool											isControlFocused(const ControlData& controlData) const noexcept;

			// RendererContext 고를 때 사용
			const bool											isAncestorFocused(const ControlData& controlData) const noexcept;
			const bool											isAncestorFocusedRecursiveXXX(const uint64 hashKey) const noexcept;
			const bool											isAncestorFocusedInclusiveXXX(const ControlData& controlData) const noexcept;

			const bool											isAncestor(const uint64 myHashKey, const uint64 ancestorCandidateHashKey) const noexcept;

			// Focus, Out-of-focus 색 정할 때 사용
			const bool											needToColorFocused(const ControlData& controlData) const noexcept;
			const bool											isDescendantFocused(const ControlData& controlData) const noexcept;
			const ControlData&									getClosestFocusableAncestorInclusiveInternal(const ControlData& controlData) const noexcept;

			const fs::SimpleRendering::Color&					getNamedColor(const NamedColor namedColor) const noexcept;
			fs::SimpleRendering::Color&							getNamedColor(const NamedColor namedColor) noexcept;
#pragma endregion


		public:
			void												render();

		private:
			void												resetStatesPerFrame();

		private:
			fs::SimpleRendering::GraphicDevice* const			_graphicDevice;

			fs::SimpleRendering::ShapeFontRendererContext		_shapeFontRendererContextBackground;
			fs::SimpleRendering::ShapeFontRendererContext		_shapeFontRendererContextForeground;
			fs::SimpleRendering::ShapeFontRendererContext		_shapeFontRendererContextTopMost;

			std::vector<D3D11_VIEWPORT>							_viewportArrayPerFrame;
			std::vector<D3D11_RECT>								_scissorRectangleArrayPerFrame;

			D3D11_VIEWPORT										_viewportFullScreen;
			D3D11_RECT											_scissorRectangleFullScreen;

		private:
			const ControlData									kNullControlData;
			ControlData											_rootControlData;
		
		private:
			std::vector<ControlStackData>						_controlStackPerFrame;

		private:
			mutable bool										_isMouseInteractionDonePerFrame;
			mutable uint64										_focusedControlHashKey;
			mutable uint64										_hoveredControlHashKey;
			mutable uint64										_pressedControlHashKey;
			mutable fs::Float2									_pressedControlInitialPosition;
			mutable uint64										_clickedControlHashKeyPerFrame;
			uint64												_hoverStartTimeMs;
			bool												_hoverStarted;
		

#pragma region Mouse Capture States
		private:
			mutable bool										_isDragBegun;
			mutable uint64										_draggedControlHashKey;
			mutable fs::Float2									_draggedControlInitialPosition;
		
		private:
			mutable bool										_isResizeBegun;
			mutable uint64										_resizedControlHashKey;
			mutable fs::Float2									_resizedControlInitialPosition;
			mutable fs::Float2									_resizedControlInitialDisplaySize;
			mutable ResizingMethod								_resizingMethod;
#pragma endregion
		
		private:
			std::unordered_map<uint64, ControlData>				_controlIdMap;


#pragma region Next-states
		private:
			bool												_nextSameLine;
			fs::Float2											_nextControlSize;
			bool												_nextSizingForced;
			bool												_nextNoAutoPositioned;
			fs::Float2											_nextControlPosition;
			const wchar_t*										_nextTooltipText;
#pragma endregion


#pragma region Mouse states
		private:
			fs::Float2											_mousePosition;
			fs::Float2											_mouseDownPosition;
			fs::Float2											_mouseUpPosition;
			bool												_mouseButtonDown;
			bool												_mouseDownUp;
			mutable fs::Window::CursorType						_cursorType; // per frame
#pragma endregion

		private:
			fs::Float2											_tooltipPosition;
			uint64												_tooltipParentWindowHashKey;
			const wchar_t*										_tooltipTextFinal;
			TaskWhenMouseUp										_taskWhenMouseUp;

		private:
			fs::SimpleRendering::Color							_namedColors[static_cast<uint32>(NamedColor::COUNT)];
		};
	}
}


#include <FsRenderingBase/Include/GuiContext.inl>


#endif // !FS_GUI_CONTEXT_H

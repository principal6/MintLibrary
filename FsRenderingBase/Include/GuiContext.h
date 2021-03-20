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
	namespace RenderingBase
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
			fs::RenderingBase::Color	_backgroundColor	= fs::RenderingBase::Color::kTransparent;
			fs::RenderingBase::Color	_fontColor			= fs::RenderingBase::Color::kTransparent;
			fs::Float2					_size				= fs::Float2::kZero;
			TextAlignmentHorz			_alignmentHorz		= TextAlignmentHorz::Center;
			TextAlignmentVert			_alignmentVert		= TextAlignmentVert::Middle;
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
		
		struct TextBoxParam
		{
			fs::Float2					_size				= fs::Float2(128.0f, 0.0f);
			TextAlignmentHorz			_alignmentHorz		= TextAlignmentHorz::Left;
			fs::RenderingBase::Color	_backgroundColor	= fs::RenderingBase::Color::kWhite;
			fs::RenderingBase::Color	_fontColor			= fs::RenderingBase::Color::kBlack;
		};

		struct ListViewParam
		{
			bool		_useScrollBar	= true;
		};

		struct ScrollBarTrackParam
		{
			fs::Float2			_size = fs::Float2(180, 100);
			fs::Float2			_positionInParent = fs::Float2(100, 100);
		};


		//             |        CASE 0       |        CASE 1       |
		//             |---------------------|---------------------|
		// hi00 i0 li0 |               ScrollBarType               |
		// hi01        |                MenuBarType                |
		// hi02 i1     | CaretAt             | ThumbAt             |
		// hi03        | CaretStete          |  ..                 |
		// hi04 i2 li1 | SelectionStart      | SelectedItemIndex   |
		// hi05        | SelectionLength     | IsToggled           |
		// hi06 i3     | TextDisplayOffset   | ItemSizeX           | == TitleBarSizeX
		// hi07        |  ..                 |  ..                 |
		// hi08 i4 li2 |                     | ItemSizeY           | == TitleBarSizeY
		// hi09        |                     |  ..                 |
		// hi10 i5     |                     |                     |
		// hi11        |                     |                     |
		// hi12 i6 li3 | InternalTimeMs      |                     |
		// hi13        |  ..                 |                     |
		// hi14 i7     |  ..                 |                     |
		// hi15        |  ..                 |                     |
		class ControlValue
		{
		public:
									ControlValue();
									~ControlValue()							= default;

		public:
			ControlValue&			operator=(const ControlValue& rhs)		= default;
			ControlValue&			operator=(ControlValue&& rhs) noexcept	= default;

		public:
			void					enableScrollBar(const ScrollBarType scrollBarType) noexcept;
			void					disableScrollBar(const ScrollBarType scrollBarType) noexcept;
			const bool				isScrollBarEnabled(const ScrollBarType scrollBarType) const noexcept;

		private:
			void					setCurrentScrollBarType(const ScrollBarType scrollBarType) noexcept;
			const ScrollBarType&	getCurrentScrollBarType() const noexcept;
		
		public:
			void					setCurrentMenuBarType(const MenuBarType menuBarType) noexcept;
			void					setThumbAt(const float thumbAt) noexcept;
			void					setSelectedItemIndex(const int16 itemIndex) noexcept;
			void					setIsToggled(const bool isToggled) noexcept;
			void					setItemSizeX(const float itemSizeX) noexcept;
			void					setItemSizeY(const float itemSizeY) noexcept;
			void					addItemSizeX(const float itemSizeX) noexcept;
			void					addItemSizeY(const float itemSizeY) noexcept;
			void					setInternalTimeMs(const uint64 internalTimeMs) noexcept;

		public:
			const MenuBarType&		getCurrentMenuBarType() const noexcept;
			const float				getThumbAt() const noexcept; // [Slider], [ScrollBar]
			int16&					getSelectedItemIndex() noexcept; // [ListView]
			const bool&				getIsToggled() const noexcept; // [CheckBox]
			const float				getItemSizeX() const noexcept;
			const float				getItemSizeY() const noexcept;
			const fs::Float2		getItemSize() const noexcept;
			uint16&					getCaretAt() noexcept;
			uint16&					getCaretState() noexcept;
			uint16&					getSelectionStart() noexcept;
			uint16&					getSelectionLength() noexcept;
			float&					getTextDisplayOffset() noexcept;
			uint64&					getInternalTimeMs() noexcept;

		private:
			static constexpr uint32 kSize64 = 4;
			union
			{
				struct
				{
					uint64	_lui[kSize64];
				};
				struct
				{
					int64	_li[kSize64];
				};
				struct
				{
					int32	_i[kSize64 * 2];
				};				
				struct
				{
					float	_f[kSize64 * 2];
				};
				struct
				{
					uint16	_hui[kSize64 * 4];
				};
				struct
				{
					int16	_hi[kSize64 * 4];
				};
				struct
				{
					int8	_c[kSize64 * 8];
				};
			};
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
		private:
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
			static constexpr fs::Rect					kWindowInnerPadding = fs::Rect(4.0f);
			static constexpr float						kScrollBarThickness = 8.0f;
			static constexpr fs::Rect					kTitleBarInnerPadding = fs::Rect(12.0f, 6.0f, 6.0f, 6.0f);
			static constexpr fs::Float2					kTitleBarBaseSize = fs::Float2(0.0f, fs::RenderingBase::kDefaultFontSize + kTitleBarInnerPadding.vert());
			static constexpr fs::Float2					kMenuBarBaseSize = fs::Float2(0.0f, fs::RenderingBase::kDefaultFontSize + 8.0f);
			static constexpr float						kMenuBarItemTextSpace = 24.0f;
			static constexpr float						kMenuItemSpaceLeft = 16.0f;
			static constexpr float						kMenuItemSpaceRight = 48.0f;
			static constexpr float						kHalfBorderThickness = 5.0f;
			static constexpr float						kSliderTrackThicknes = 6.0f;
			static constexpr float						kSliderThumbRadius = 8.0f;
			static constexpr float						kDockingInteractionShort = 30.0f;
			static constexpr float						kDockingInteractionLong = 40.0f;
			static constexpr float						kDockingInteractionDisplayBorderThickness = 2.0f;
			static constexpr float						kDockingInteractionOffset = 5.0f;
			static constexpr fs::Float2					kCheckBoxSize = fs::Float2(16.0f, 16.0f);
			static constexpr float						kMouseWheelScrollScale = -8.0f;
			static constexpr float						kTextBoxBackSpaceStride = 48.0f;
			static constexpr uint32						kTextBoxMaxTextLength = 2048;

		private:
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

			enum class NamedColor
			{
				NormalState,
				HoverState,
				PressedState,

				WindowFocused,
				WindowOutOfFocus,
				Dock,
				ShownInDock,
				HighlightColor,

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
			struct PrepareControlDataParam
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
				bool				_ignoreMeForContentAreaSize				= false;
				bool				_noIntervalForNextSibling			= false;
				ViewportUsage		_viewportUsage						= ViewportUsage::Child;
			};

			class ControlData
			{
			public:
															ControlData();
															ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType);
															ControlData(const uint64 hashKey, const uint64 parentHashKey, const ControlType controlType, const fs::Float2& size);
			
			public:
				void										clearPerFrameData() noexcept;
				void										updatePerFrameWithParent(const bool isNewData, const PrepareControlDataParam& prepareControlDataParam, ControlData& parent) noexcept;

			public:
				const uint64								getHashKey() const noexcept;
				const uint64								getParentHashKey() const noexcept;
				const fs::Rect&								getInnerPadding() const noexcept;
				fs::Float2									getClientSize() const noexcept;
				const float									getTopOffsetToClientArea() const noexcept;
				const fs::Float2&							getDisplaySizeMin() const noexcept;
				const float									getPureDisplayWidth() const noexcept;
				const float									getPureDisplayHeight() const noexcept;
				const fs::Float2&							getInteractionSize() const noexcept;
				const fs::Float2&							getNonDockInteractionSize() const noexcept;
				const fs::Float2&							getContentAreaSize() const noexcept;
				const fs::Float2&							getPreviousContentAreaSize() const noexcept;
				const fs::Float2&							getChildAt() const noexcept;
				const fs::Float2&							getNextChildOffset() const noexcept;
				const ControlType							getControlType() const noexcept;
				const bool									isTypeOf(const ControlType controlType) const noexcept;
				const wchar_t*								getText() const noexcept;
				const bool									isRootControl() const noexcept;
				const bool									isVisibleState(const VisibleState visibleState) const noexcept;
				const bool									isControlVisible() const noexcept;
				const uint32								getViewportIndex() const noexcept;
				const uint32								getViewportIndexForChildren() const noexcept;
				const uint32								getViewportIndexForDocks() const noexcept;
				const std::vector<uint64>&					getChildControlDataHashKeyArray() const noexcept;
				const std::vector<uint64>&					getPreviousChildControlDataHashKeyArray() const noexcept;
				const uint16								getPreviousChildControlCount() const noexcept;
				const uint16								getPreviousMaxChildControlCount() const noexcept;
				void										prepareChildControlDataHashKeyArray() noexcept;
				const bool									hasChildWindow() const noexcept;
				DockDatum&									getDockDatum(const DockingMethod dockingMethod) noexcept;
				const DockDatum&							getDockDatum(const DockingMethod dockingMethod) const noexcept;
				const bool									isShowingInDock(const ControlData& dockedControlData) const noexcept;
				void										setDockSize(const DockingMethod dockingMethod, const fs::Float2& dockSize) noexcept;
				const fs::Float2							getDockSize(const DockingMethod dockingMethod) const noexcept;
				const fs::Float2							getDockSizeIfHosting(const DockingMethod dockingMethod) const noexcept;
				const fs::Float2							getDockOffsetSize() const noexcept;
				const fs::Float2							getDockPosition(const DockingMethod dockingMethod) const noexcept;
				const float									getHorzDockSizeSum() const noexcept;
				const float									getVertDockSizeSum() const noexcept;
				const fs::Float2							getMenuBarThickness() const noexcept;
				void										connectToDock(const uint64 dockControlHashKey) noexcept;
				void										disconnectFromDock() noexcept;
				const uint64								getDockControlHashKey() const noexcept;
				const bool									isDocking() const noexcept;
				const bool									isDockHosting() const noexcept;
				const bool									isResizable() const noexcept;
				Rect										getControlRect() const noexcept;
				Rect										getControlPaddedRect() const noexcept;
			
			public:
				void										connectChildWindowIfNot(const ControlData& childWindowControlData) noexcept;
				void										disconnectChildWindow(const uint64 childWindowHashKey) noexcept;
				const std::unordered_map<uint64, bool>&		getChildWindowHashKeyMap() const noexcept;

			public:
				void										setVisibleState(const VisibleState visibleState) noexcept;
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
				bool										_isDraggable;
				bool										_isInteractableOutsideParent;
				ResizingMask								_resizingMask;
				fs::Rect									_draggingConstraints; // MUST set all four values if want to limit dragging area
				uint64										_delegateHashKey; // Used for drag, resize and focus
				DockingControlType							_dockingControlType;
				DockingMethod								_lastDockingMethod;
				DockingMethod								_lastDockingMethodCandidate;
				std::wstring								_text;
				ControlValue								_controlValue;

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
				VisibleState								_visibleState;
				uint32										_viewportIndex;
				uint32										_viewportIndexForChildren; // Used by window
				uint32										_viewportIndexForDocks;
				std::vector<uint64>							_childControlDataHashKeyArray;
				std::vector<uint64>							_previousChildControlDataHashKeyArray;
				uint16										_previousMaxChildControlCount;
				std::unordered_map<uint64, bool>			_childWindowHashKeyMap;
				DockDatum									_dockData[static_cast<uint32>(DockingMethod::COUNT)];
				uint64										_dockControlHashKey;
				DockingStateContext							_dokcingStateContext;
			};
			

		private:
			struct ControlStackData
			{
			public:
									ControlStackData() = default;
									ControlStackData(const ControlData& controlData);

			public:
				ControlType			_controlType;
				uint64				_hashKey;
			};

		
		public:
																GuiContext(fs::RenderingBase::GraphicDevice* const graphicDevice);
																~GuiContext();

		public:
			void												initialize(const char* const font);
			void												updateScreenSize(const fs::Float2& newScreenSize);

		public:
			void												receiveEventsFrom(fs::Window::IWindow* const window);

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
			void												nextControlSizeNonContrainedToParent();
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
			// CheckBox
			// TextBox
			// ListView
			// Menu
			// SpinBox
			// TreeView
			// Group RadioButton
			// ComboBox
			// Splitter

		public:
			void												testWindow(VisibleState& inoutVisibleState);
			void												testDockedWindow(VisibleState& inoutVisibleState);

		public:
			// [Window | Control with ID]
			// title is used as unique id for windows
			const bool											beginWindow(const wchar_t* const title, const WindowParam& windowParam, VisibleState& inoutVisibleState);
			void												endWindow() { endControlInternal(ControlType::Window); }

		private:
			void												dockWindowOnceInitially(ControlData& windowControlData, const DockingMethod dockingMethod, const fs::Float2& initialDockingSize);

		public:
			// [Button]
			// Return 'true' if clicked
			const bool											beginButton(const wchar_t* const text);
			void												endButton() { endControlInternal(ControlType::Button); }

			// [CheckBox]
			// Return 'true' if toggle state has changed
			const bool											beginCheckBox(const wchar_t* const text, bool& outIsChecked);
			void												endCheckBox() { endControlInternal(ControlType::CheckBox); }

			// [Label]
			void												pushLabel(const wchar_t* const name, const wchar_t* const text, const LabelParam& labelParam = LabelParam());

			// [Slider]
			// Return 'true' if value was changed
			const bool											beginSlider(const wchar_t* const name, const SliderParam& sliderParam, float& outValue);
			void												endSlider() { endControlInternal(ControlType::Slider); }

			// [TextBox]
			const bool											beginTextBox(const wchar_t* const name, const TextBoxParam& textBoxParam, std::wstring& outText);
			void												endTextBox() { endControlInternal(ControlType::TextBox); }

			// [ListView]
			const bool											beginListView(const wchar_t* const name, int16& outSelectedListItemIndex, const ListViewParam& listViewParam);
			void												endListView();

			// [ListItem]
			void												pushListItem(const wchar_t* const text);

			// [MenuBar]
			const bool											beginMenuBar(const wchar_t* const name);
			void												endMenuBar() { endControlInternal(ControlType::MenuBar); }

			// [MenuBarItem]
			const bool											beginMenuBarItem(const wchar_t* const text);
			void												endMenuBarItem() { endControlInternal(ControlType::MenuBarItem); }

			// [MenuItem]
			const bool											beginMenuItem(const wchar_t* const text);
			void												endMenuItem() { endControlInternal(ControlType::MenuItem); }


		private:
			// Returns size of titlebar
			fs::Float2											beginTitleBar(const wchar_t* const windowTitle, const fs::Float2& titleBarSize, const fs::Rect& innerPadding, VisibleState& inoutParentVisibleState);
			void												endTitleBar() { endControlInternal(ControlType::TitleBar); }

			const bool											pushRoundButton(const wchar_t* const windowTitle, const fs::RenderingBase::Color& color);

			// [Tooltip]
			// Unique control
			void												pushTooltipWindow(const wchar_t* const tooltipText, const fs::Float2& position);

			// [ScrollBar]
			// Return 'true' if value was changed
			void												pushScrollBar(const ScrollBarType scrollBarType);

			ControlData&										pushScrollBarTrack(const ScrollBarType scrollBarType, const ScrollBarTrackParam& scrollBarTrackParam, bool& outHasExtraSize);
			void												pushScrollBarThumb(const ScrollBarType scrollBarType, const float visibleLength, const float totalLength, const ControlData& scrollBarTrack, fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext);

		private:
			void												processDock(const ControlData& controlData, fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext);
			void												endControlInternal(const ControlType controlType);
			void												pushScissorRectangleForMe(ControlData& controlData, const D3D11_RECT& scissorRectangle);
			void												pushScissorRectangleForDocks(ControlData& controlData, const D3D11_RECT& scissorRectangle);
			void												pushScissorRectangleForChildren(ControlData& controlData, const D3D11_RECT& scissorRectangle);

		private:
			const ControlData&									getControlStackTopXXX() const noexcept;
			ControlData&										getControlStackTopXXX() noexcept;
			ControlData&										getControlData(const uint64 hashKey) noexcept;
			const ControlData&									getControlData(const uint64 hashKey) const noexcept;
			fs::Float4											getControlCenterPosition(const ControlData& controlData) const noexcept;
			fs::Float2											getControlPositionInParentSpace(const ControlData& controlData) const noexcept;
			const wchar_t*										generateControlKeyString(const wchar_t* const name, const ControlType controlType) const noexcept;
			const wchar_t*										generateControlKeyString(const ControlData& parentControlData, const wchar_t* const name, const ControlType controlType) const noexcept;
			const uint64										generateControlHashKeyXXX(const wchar_t* const text, const ControlType controlType) const noexcept;
			ControlData&										createOrGetControlData(const wchar_t* const text, const ControlType controlType, const wchar_t* const hashGenerationKeyOverride = nullptr) noexcept;
			const ControlData&									getParentWindowControlData() const noexcept;
			const ControlData&									getParentWindowControlData(const ControlData& controlData) const noexcept;
			const ControlData&									getParentWindowControlDataInternal(const uint64 hashKey) const noexcept;
#pragma endregion


		public:
			const bool											isControlClicked() const noexcept;
			const bool											isControlPressed() const noexcept;
			const bool											isFocusedControlTextBox() const noexcept;

		private:
			void												setControlFocused(const ControlData& control) noexcept;


#pragma region Before drawing controls
		private:
			void												prepareControlData(ControlData& controlData, const PrepareControlDataParam& prepareControlDataParam) noexcept;
			void												calculateControlChildAt(ControlData& controlData) noexcept;

			const bool											processClickControl(ControlData& controlData, const fs::RenderingBase::Color& normalColor, const fs::RenderingBase::Color& hoverColor, const fs::RenderingBase::Color& pressedColor, fs::RenderingBase::Color& outBackgroundColor) noexcept;
			const bool											processFocusControl(ControlData& controlData, const fs::RenderingBase::Color& focusedColor, const fs::RenderingBase::Color& nonFocusedColor, fs::RenderingBase::Color& outBackgroundColor) noexcept;
			void												processShowOnlyControl(ControlData& controlData, fs::RenderingBase::Color& outBackgroundColor, const bool doNotSetMouseInteractionDone = false) noexcept;
			const bool											processScrollableControl(ControlData& controlData, const fs::RenderingBase::Color& normalColor, const fs::RenderingBase::Color& dragColor, fs::RenderingBase::Color& outBackgroundColor) noexcept;
			const bool											processToggleControl(ControlData& controlData, const fs::RenderingBase::Color& normalColor, const fs::RenderingBase::Color& hoverColor, const fs::RenderingBase::Color& toggledColor, fs::RenderingBase::Color& outBackgroundColor) noexcept;
			
			void												processControlInteractionInternal(ControlData& controlData, const bool doNotSetMouseInteractionDone = false) noexcept;
			void												processControlCommon(ControlData& controlData) noexcept;
			void												checkControlResizing(ControlData& controlData) noexcept;
			void												checkControlHoveringForTooltip(ControlData& controlData) noexcept;
			void												processControlResizingInternal(ControlData& controlData) noexcept;
			void												processControlDraggingInternal(ControlData& controlData) noexcept;
			void												processControlDockingInternal(ControlData& controlData) noexcept;

			void												dock(const uint64 dockedControlHashKey, const uint64 dockControlHashKey) noexcept;
			void												undock(const uint64 dockedControlHashKey) noexcept;
			void												updateDockDatum(const uint64 dockControlHashKey, const bool dontUpdateWidthArray = false) noexcept;

			const bool											isInteractingInternal(const ControlData& controlData) const noexcept;
			
			// These functions must be called after process- functions
			const bool											isControlBeingDragged(const ControlData& controlData) const noexcept;
			const bool											isControlBeingResized(const ControlData& controlData) const noexcept;
			const bool											isControlHovered(const ControlData& controlData) const noexcept;
			const bool											isControlPressed(const ControlData& controlData) const noexcept;
			const bool											isControlClicked(const ControlData& controlData) const noexcept;
			const bool											isControlFocused(const ControlData& controlData) const noexcept;

			// RendererContext 고를 때 사용
			const bool											isAncestorControlFocused(const ControlData& controlData) const noexcept;
			const bool											isAncestorControlPressed(const ControlData& controlData) const noexcept;
			const bool											isAncestorControlTargetRecursiveXXX(const uint64 hashKey, const uint64 targetHashKey) const noexcept;
			const bool											isAncestorControlFocusedInclusiveXXX(const ControlData& controlData) const noexcept;

			const bool											isAncestorControlInclusive(const ControlData& controlData, const uint64 ancestorCandidateHashKey) const noexcept;
			const bool											isAncestorControlRecursiveXXX(const uint64 currentControlHashKey, const uint64 ancestorCandidateHashKey) const noexcept;
			const bool											isDescendantControlInclusive(const ControlData& controlData, const uint64 descendantCandidateHashKey) const noexcept;
			const bool											isDescendantControlRecursiveXXX(const uint64 currentControlHashKey, const uint64 descendantCandidateHashKey) const noexcept;

			// Focus, Out-of-focus 색 정할 때 사용
			const bool											needToColorFocused(const ControlData& controlData) const noexcept;
			const bool											isDescendantControlFocusedInclusive(const ControlData& controlData) const noexcept;
			const bool											isDescendantControlHoveredInclusive(const ControlData& controlData) const noexcept;
			const bool											isDescendantControlPressedInclusive(const ControlData& controlData) const noexcept;
			const bool											isDescendantControlPressed(const ControlData& controlData) const noexcept;
			const bool											isDescendantControlHovered(const ControlData& controlData) const noexcept;
			const ControlData&									getClosestFocusableAncestorControlInclusive(const ControlData& controlData) const noexcept;

			const fs::RenderingBase::Color&						getNamedColor(const NamedColor namedColor) const noexcept;
			fs::RenderingBase::Color&							getNamedColor(const NamedColor namedColor) noexcept;

			const float											getMouseWheelScroll(const ControlData& scrollParentControlData) const noexcept;
			const float											calculateTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept;
			const uint32										calculateIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept;
#pragma endregion


		public:
			void												render();

		private:
			void												resetPerFrameStates();

		private:
			fs::RenderingBase::GraphicDevice* const				_graphicDevice;

			float												_fontSize;
			fs::RenderingBase::ShapeFontRendererContext			_shapeFontRendererContextBackground;
			fs::RenderingBase::ShapeFontRendererContext			_shapeFontRendererContextForeground;
			fs::RenderingBase::ShapeFontRendererContext			_shapeFontRendererContextTopMost;

			std::vector<D3D11_VIEWPORT>							_viewportArrayPerFrame;
			std::vector<D3D11_RECT>								_scissorRectangleArrayPerFrame;

			D3D11_VIEWPORT										_viewportFullScreen;
			D3D11_RECT											_scissorRectangleFullScreen;

			int8												_updateScreenSizeCounter;

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
			fs::Float2											_nextDesiredControlSize;
			bool												_nextSizingForced;
			bool												_nextControlSizeNonContrainedToParent;
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
			bool												_mouseButtonDownFirst;
			bool												_mouseDownUp;
			mutable float										_mouseWheel;
			mutable fs::Window::CursorType						_cursorType; // per frame
#pragma endregion

#pragma region Key Character Input
		private:
			uint32												_caretBlinkIntervalMs;
			wchar_t												_wcharInput;
			wchar_t												_wcharInputCandiate;
			fs::Window::EventData::KeyCode						_keyCode;
#pragma endregion

		private:
			fs::Float2											_tooltipPosition;
			uint64												_tooltipParentWindowHashKey;
			const wchar_t*										_tooltipTextFinal;
			TaskWhenMouseUp										_taskWhenMouseUp;

		private:
			fs::RenderingBase::Color							_namedColors[static_cast<uint32>(NamedColor::COUNT)];
		};
	}
}


#include <FsRenderingBase/Include/GuiContext.inl>


#endif // !FS_GUI_CONTEXT_H

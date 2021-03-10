#include <stdafx.h>
#include <FsRenderingBase/Include/GuiContext.h>

#include <FsContainer/Include/StringUtil.hpp>

#include <FsRenderingBase/Include/GraphicDevice.h>

#include <FsPlatform/Include/WindowsWindow.h>

#include <FsLibrary/Profiler/ScopedCpuProfiler.h>

#include <functional>


namespace fs
{
	namespace Gui
	{
		FS_INLINE void constraintInnerRect(D3D11_RECT& targetInnerRect, const D3D11_RECT& outerRect)
		{
			targetInnerRect.left = fs::max(targetInnerRect.left, outerRect.left);
			targetInnerRect.right = fs::min(targetInnerRect.right, outerRect.right);
			targetInnerRect.top = fs::max(targetInnerRect.top, outerRect.top);
			targetInnerRect.bottom = fs::min(targetInnerRect.bottom, outerRect.bottom);

			// Rect Size 가 음수가 되지 않도록 방지!! (중요)
			targetInnerRect.right = fs::max(targetInnerRect.left, targetInnerRect.right);
			targetInnerRect.bottom = fs::max(targetInnerRect.top, targetInnerRect.bottom);
		}

		FS_INLINE fs::Float2 getControlLeftCenterPosition(const GuiContext::ControlData& controlData)
		{
			return fs::Float2(controlData._position._x, controlData._position._y + controlData._displaySize._y * 0.5f);
		};

		FS_INLINE fs::Float2 getControlRightCenterPosition(const GuiContext::ControlData& controlData)
		{
			return fs::Float2(controlData._position._x + controlData._displaySize._x, controlData._position._y + controlData._displaySize._y * 0.5f);
		};


		GuiContext::GuiContext(fs::RenderingBase::GraphicDevice* const graphicDevice)
			: _graphicDevice{ graphicDevice }
			, _fontSize{ 0.0f }
			, _shapeFontRendererContextBackground{ _graphicDevice }
			, _shapeFontRendererContextForeground{ _graphicDevice }
			, _shapeFontRendererContextTopMost{ _graphicDevice }
			, _focusedControlHashKey{ 0 }
			, _hoveredControlHashKey{ 0 }
			, _hoverStartTimeMs{ 0 }
			, _hoverStarted{ false }
			, _isDragBegun{ false }
			, _draggedControlHashKey{ 0 }
			, _isResizeBegun{ false }
			, _resizedControlHashKey{ 0 }
			, _resizingMethod{ ResizingMethod::ResizeOnly }
			, _mouseButtonDown{ false }
			, _mouseDownUp{ false }
			, _cursorType{ fs::Window::CursorType::Arrow }
			, _caretBlinkIntervalMs{ 0 }
			, _wcharInput{ L'\0'}
			, _wcharInputCandiate{ L'\0'}
			, _keyCode{ fs::Window::EventData::KeyCode::NONE }
			, _tooltipTextFinal{ nullptr }
		{
			getNamedColor(NamedColor::NormalState) = fs::RenderingBase::Color(45, 47, 49);
			getNamedColor(NamedColor::HoverState) = getNamedColor(NamedColor::NormalState).addedRgb(0.25f);
			getNamedColor(NamedColor::PressedState) = fs::RenderingBase::Color(80, 100, 120);

			getNamedColor(NamedColor::WindowFocused) = fs::RenderingBase::Color(3, 5, 7);
			getNamedColor(NamedColor::WindowOutOfFocus) = getNamedColor(NamedColor::WindowFocused);
			getNamedColor(NamedColor::Dock) = getNamedColor(NamedColor::NormalState);
			getNamedColor(NamedColor::ShownInDock) = fs::RenderingBase::Color(23, 25, 27);
			getNamedColor(NamedColor::HighlightColor) = fs::RenderingBase::Color(100, 180, 255);

			getNamedColor(NamedColor::TitleBarFocused) = getNamedColor(NamedColor::WindowFocused);
			getNamedColor(NamedColor::TitleBarOutOfFocus) = getNamedColor(NamedColor::WindowOutOfFocus);

			getNamedColor(NamedColor::TooltipBackground) = fs::RenderingBase::Color(200, 255, 220);

			getNamedColor(NamedColor::ScrollBarTrack) = fs::RenderingBase::Color(80, 82, 84);
			getNamedColor(NamedColor::ScrollBarThumb) = getNamedColor(NamedColor::ScrollBarTrack).addedRgb(0.25f);

			getNamedColor(NamedColor::LightFont) = fs::RenderingBase::Color(233, 235, 237);
			getNamedColor(NamedColor::DarkFont) = getNamedColor(NamedColor::LightFont).addedRgb(-0.75f);
			getNamedColor(NamedColor::ShownInDockFont) = getNamedColor(NamedColor::HighlightColor);
		}

		GuiContext::~GuiContext()
		{
			__noop;
		}

		void GuiContext::initialize(const char* const font)
		{
			_fontSize = static_cast<float>(_graphicDevice->getFontRendererContext().getFontSize());
			const fs::RenderingBase::FontRendererContext::FontData& fontData = _graphicDevice->getFontRendererContext().getFontData();
			if (_shapeFontRendererContextBackground.initializeFontData(fontData) == false)
			{
				FS_ASSERT("김장원", false, "ShapeFontRendererContext::initializeFont() 에 실패했습니다!");
			}

			if (_shapeFontRendererContextForeground.initializeFontData(fontData) == false)
			{
				FS_ASSERT("김장원", false, "ShapeFontRendererContext::initializeFont() 에 실패했습니다!");
			}

			if (_shapeFontRendererContextTopMost.initializeFontData(fontData) == false)
			{
				FS_ASSERT("김장원", false, "ShapeFontRendererContext::initializeFont() 에 실패했습니다!");
			}

			_shapeFontRendererContextBackground.initializeShaders();
			_shapeFontRendererContextBackground.setUseMultipleViewports();
			
			_shapeFontRendererContextForeground.initializeShaders();
			_shapeFontRendererContextForeground.setUseMultipleViewports();
			
			_shapeFontRendererContextTopMost.initializeShaders();
			_shapeFontRendererContextTopMost.setUseMultipleViewports();

			_caretBlinkIntervalMs = _graphicDevice->getWindow()->getCaretBlinkIntervalMs();
			
			const fs::Float2& windowSize = fs::Float2(_graphicDevice->getWindowSize());
			_rootControlData = ControlData(1, 0, fs::Gui::ControlType::ROOT, windowSize);

			// Full-screen Viewport & ScissorRectangle
			_viewportFullScreen = _graphicDevice->getFullScreenViewport();
			_scissorRectangleFullScreen.left = static_cast<LONG>(0);
			_scissorRectangleFullScreen.top = static_cast<LONG>(0);
			_scissorRectangleFullScreen.right = static_cast<LONG>(_rootControlData._displaySize._x);
			_scissorRectangleFullScreen.bottom = static_cast<LONG>(_rootControlData._displaySize._y);

			resetNextStates();
			resetStatesPerFrame();
		}

		void GuiContext::handleEvents(fs::Window::IWindow* const window)
		{
			// 초기화
			_mouseDownUp = false;
			_mouseButtonDownFirst = false;

			fs::Window::WindowsWindow* const windowsWindow = static_cast<fs::Window::WindowsWindow*>(window);
			if (windowsWindow->hasEvent() == true)
			{
				const fs::Window::EventData& eventData = windowsWindow->peekEvent();
				if (eventData._type == fs::Window::EventType::MouseMove)
				{
					_mousePosition = eventData._value.getMousePosition();
				}
				else if (eventData._type == fs::Window::EventType::MouseDown)
				{
					_mouseDownPosition = eventData._value.getMousePosition();
					_mouseButtonDown = true;
					_mouseButtonDownFirst = true;
				}
				else if (eventData._type == fs::Window::EventType::MouseUp)
				{
					if (_taskWhenMouseUp.isSet())
					{
						updateDockDatum(_taskWhenMouseUp.getUpdateDockDatum());
					}

					_mouseUpPosition = eventData._value.getMousePosition();
					if (_mouseButtonDown == true)
					{
						_mouseDownUp = true;
					}
					_mouseButtonDown = false;
				}
				else if (eventData._type == fs::Window::EventType::MouseWheel)
				{
					_mouseWheel = eventData._value.getMouseWheel();
				}
				else if (eventData._type == fs::Window::EventType::KeyStroke)
				{
					_wcharInputCandiate = L'\0';

					_wcharInput = eventData._value.getInputWchar();
				}
				else if (eventData._type == fs::Window::EventType::KeyStrokeCandidate)
				{
					_wcharInputCandiate = eventData._value.getInputWchar();
				}
				else if (eventData._type == fs::Window::EventType::KeyDown)
				{
					_keyCode = eventData._value.getKeyCode();
				}
			}
		}

		const bool GuiContext::isInControlInternal(const fs::Float2& screenPosition, const fs::Float2& controlPosition, const fs::Float2& controlPositionOffset, const fs::Float2& interactionSize) const noexcept
		{
			const fs::Float2 max = controlPosition + controlPositionOffset + interactionSize;
			if (controlPosition._x + controlPositionOffset._x <= screenPosition._x && screenPosition._x <= max._x &&
				controlPosition._y + controlPositionOffset._y <= screenPosition._y && screenPosition._y <= max._y)
			{
				return true;
			}
			return false;
		}

		const bool GuiContext::isInControlInteractionArea(const fs::Float2& screenPosition, const ControlData& controlData) const noexcept
		{
			if (controlData.isDockHosting() == true)
			{
				fs::Float2 positionOffset;

				const DockDatum& dockDatumTopSide = controlData.getDockDatum(DockingMethod::TopSide);
				const DockDatum& dockDatumLeftSide = controlData.getDockDatum(DockingMethod::LeftSide);
				if (dockDatumTopSide.hasDockedControls() == true)
				{
					positionOffset._y = controlData.getDockSize(DockingMethod::TopSide)._y;
				}
				if (dockDatumLeftSide.hasDockedControls() == true)
				{
					positionOffset._x = controlData.getDockSize(DockingMethod::LeftSide)._x;
				}

				return isInControlInternal(screenPosition, controlData._position, positionOffset, controlData.getNonDockInteractionSize());
			}
			return isInControlInternal(screenPosition, controlData._position, fs::Float2::kZero, controlData.getInteractionSize());
		}

		const bool GuiContext::isInControlBorderArea(const fs::Float2& screenPosition, const ControlData& controlData, fs::Window::CursorType& outCursorType, ResizingMask& outResizingMask, ResizingMethod& outResizingMethod) const noexcept
		{
			const fs::Float2 extendedPosition = controlData._position - fs::Float2(kHalfBorderThickness);
			const fs::Float2 extendedInteractionSize = controlData.getInteractionSize() + fs::Float2(kHalfBorderThickness * 2.0f);
			const fs::Float2 originalMax = controlData._position + controlData.getInteractionSize();
			if (isInControlInternal(screenPosition, extendedPosition, fs::Float2::kZero, extendedInteractionSize) == true)
			{
				outResizingMask.setAllFalse();

				outResizingMask._left = (screenPosition._x <= controlData._position._x + kHalfBorderThickness);
				outResizingMask._right = (originalMax._x - kHalfBorderThickness <= screenPosition._x);
				outResizingMask._top = (screenPosition._y <= controlData._position._y + kHalfBorderThickness);
				outResizingMask._bottom = (originalMax._y - kHalfBorderThickness <= screenPosition._y);
				
				const bool leftOrRight = outResizingMask._left || outResizingMask._right;
				const bool topOrBottom = outResizingMask._top || outResizingMask._bottom;
				const bool topLeftOrBottomRight = (outResizingMask.topLeft() || outResizingMask.bottomRight());
				const bool bottomLeftOrTopRight = (outResizingMask.bottomLeft() || outResizingMask.topRight());
				
				outResizingMethod = ResizingMethod::ResizeOnly;
				if (outResizingMask._left == true)
				{
					outResizingMethod = ResizingMethod::RepositionHorz;
				}
				if (outResizingMask._top == true)
				{
					outResizingMethod = ResizingMethod::RepositionVert;

					if (outResizingMask._left == true)
					{
						outResizingMethod = ResizingMethod::RepositionBoth;
					}
				}

				if (topLeftOrBottomRight == true)
				{
					outCursorType = fs::Window::CursorType::SizeLeftTilted;
				}
				else if (bottomLeftOrTopRight == true)
				{
					outCursorType = fs::Window::CursorType::SizeRightTilted;
				}
				else if (leftOrRight == true)
				{
					outCursorType = fs::Window::CursorType::SizeHorz;
				}
				else if (topOrBottom == true)
				{
					outCursorType = fs::Window::CursorType::SizeVert;
				}

				const bool result = (leftOrRight || topOrBottom);
				return result;
			}
			return false;
		}

		const bool GuiContext::shouldIgnoreInteraction(const fs::Float2& screenPosition, const ControlData& controlData) const noexcept
		{
			const ControlType controlType = controlData.getControlType();
			const ControlData& parentControlData = getControlData(controlData.getParentHashKey());
			if (controlType == ControlType::Window || parentControlData.hasChildWindow() == false)
			{
				return false;
			}

			// ParentControlData 가 Root 거나 Window 일 때만 여기에 온다.
			const auto& childWindowHashKeyMap = parentControlData.getChildWindowHashKeyMap();
			for (const auto& iter : childWindowHashKeyMap)
			{
				const ControlData& childWindowControlData = getControlData(iter.first);
				if (isInControlInteractionArea(screenPosition, childWindowControlData) == true)
				{
					return true;
				}
			}
			return false;
		}

		void GuiContext::testWindow()
		{
			fs::Gui::WindowParam windowParam;
			windowParam._size = fs::Float2(500.0f, 500.0f);
			windowParam._position = fs::Float2(200.0f, 50.0f);
			windowParam._scrollBarType = fs::Gui::ScrollBarType::Both;
			if (beginWindow(L"TestWindow", windowParam) == true)
			{
				if (beginMenuBar(L"메뉴테스트1") == true)
				{
					if (beginMenuBarItem(L"파일") == true)
					{
						if (beginMenuItem(L"새로 만들기") == true)
						{
							if (beginMenuItem(L"T") == true)
							{
								endMenuItem();
							}

							if (beginMenuItem(L"TT") == true)
							{
								endMenuItem();
							}

							endMenuItem();
						}

						if (beginMenuItem(L"불러오기") == true)
						{
							if (beginMenuItem(L"ABC") == true)
							{
								if (beginMenuItem(L"Nest") == true)
								{
									endMenuItem();
								}

								endMenuItem();
							}

							if (beginMenuItem(L"DEF") == true)
							{
								endMenuItem();
							}

							endMenuItem();
						}

						if (beginMenuItem(L"내보내기") == true)
						{
							endMenuItem();
						}

						endMenuBarItem();
					}

					if (beginMenuBarItem(L"도움말") == true)
					{
						endMenuBarItem();
					}

					endMenuBar();
				}

				nextTooltip(L"툴팁 테스트!");

				if (beginButton(L"테스트") == true)
				{
					endButton();
				}

				{
					bool isChecked = false;
					if (beginCheckBox(L"체크박스", isChecked) == true)
					{
						endCheckBox();
					}
				}

				{
					fs::Gui::SliderParam sliderParam;
					sliderParam._size._y = 32.0f;
					float value = 0.0f;
					if (beginSlider(L"Slider0", sliderParam, value) == true)
					{
						endSlider();
					}
				}


				nextSameLine();
				if (beginButton(L"테스트A") == true)
				{
					endButton();
				}

				nextSameLine();
				if (beginButton(L"테스트B") == true)
				{
					endButton();
				}

				if (beginButton(L"테스트2") == true)
				{
					endButton();
				}

				if (beginButton(L"테스트3") == true)
				{
					endButton();
				}

				static std::wstring textBoxContent;
				{
					fs::Gui::TextBoxParam textBoxParam;
					textBoxParam._size._x = 240.0f;
					textBoxParam._size._y = 24.0f;
					textBoxParam._alignmentHorz = fs::Gui::TextAlignmentHorz::Center;
					if (beginTextBox(L"TextBox", textBoxParam, textBoxContent) == true)
					{
						endTextBox();
					}
				}

				fs::Gui::ListViewParam listViewParam;
				int16 listViewSelectedItemIndex = 0;
				if (beginListView(L"리스트뷰", listViewSelectedItemIndex, listViewParam) == true)
				{
					pushListItem(L"아이템1");
					pushListItem(L"아이템2");
					pushListItem(L"아이템3");
					pushListItem(L"아이템4");
					pushListItem(L"아이템5");
					pushListItem(L"아이템6");

					endListView();
				}

				{
					fs::Gui::WindowParam testWindowParam;
					testWindowParam._size = fs::Float2(200.0f, 240.0f);
					testWindowParam._scrollBarType = fs::Gui::ScrollBarType::Both;
					testWindowParam._initialDockingMethod = fs::Gui::DockingMethod::BottomSide;
					if (beginWindow(L"1ST", testWindowParam))
					{
						if (beginButton(L"테스트!!") == true)
						{
							endButton();
						}

						endWindow();
					}
				}

				{
					fs::Gui::WindowParam testWindowParam;
					testWindowParam._size = fs::Float2(100.0f, 100.0f);
					testWindowParam._position._x = 10.0f;
					testWindowParam._position._y = 60.0f;
					testWindowParam._initialDockingMethod = fs::Gui::DockingMethod::BottomSide;
					if (beginWindow(L"2NDDD", testWindowParam))
					{
						if (beginButton(L"YEAH") == true)
						{
							endButton();
						}

						endWindow();
					}
				}

				endWindow();
			}
		}

		void GuiContext::testDockedWindow()
		{
			fs::Gui::WindowParam windowParam;
			windowParam._size = fs::Float2(300.0f, 400.0f);
			windowParam._position = fs::Float2(20.0f, 50.0f);
			windowParam._initialDockingMethod = fs::Gui::DockingMethod::RightSide;
			windowParam._initialDockingSize._x = 240.0f;
			if (beginWindow(L"TestDockedWindow", windowParam) == true)
			{
				if (beginButton(L"버튼이요") == true)
				{
					bool a = true;
					endButton();
				}

				nextSameLine();

				nextTooltip(L"Button B Toolip!!");

				if (beginButton(L"Button B") == true)
				{
					endButton();
				}

				if (beginButton(L"Another") == true)
				{
					endButton();
				}

				pushLabel(L"A label!");

				nextSameLine();

				if (beginButton(L"Fourth") == true)
				{
					endButton();
				}

				endWindow();
			}
		}

		const bool GuiContext::beginWindow(const wchar_t* const title, const WindowParam& windowParam)
		{
			static constexpr ControlType controlType = ControlType::Window;
			const float windowInnerPadding = 4.0f;
			
			fs::Float2 titleBarSize = kTitleBarBaseSize;

			const float titleWidth = _shapeFontRendererContextForeground.calculateTextWidth(title, fs::StringUtil::wcslen(title));
			ControlData& windowControlData = getControlData(title, controlType);
			windowControlData._dockingControlType = DockingControlType::DockerDock;
			windowControlData._isFocusable = true;


			// Initial docking
			if (windowControlData._updateCount == 2 && windowParam._initialDockingMethod != DockingMethod::COUNT)
			{
				windowControlData._lastDockingMethodCandidate = windowParam._initialDockingMethod;

				ControlData& parentControlData = getControlData(windowControlData.getParentHashKey());
				if (windowParam._initialDockingMethod == DockingMethod::LeftSide || windowParam._initialDockingMethod == DockingMethod::RightSide)
				{
					parentControlData.setDockSize(windowParam._initialDockingMethod, fs::Float2(windowParam._initialDockingSize._x, parentControlData._displaySize._y));
				}
				else
				{
					parentControlData.setDockSize(windowParam._initialDockingMethod, fs::Float2(parentControlData._displaySize._x, windowParam._initialDockingSize._y));
				}

				dock(windowControlData.getHashKey(), parentControlData.getHashKey());
			}


			PrepareControlDataParam prepareControlDataParam;
			{
				prepareControlDataParam._initialDisplaySize = windowParam._size;
				prepareControlDataParam._initialResizingMask.setAllTrue();
				prepareControlDataParam._desiredPositionInParent = windowParam._position;
				prepareControlDataParam._innerPadding = fs::Rect(windowInnerPadding);
				prepareControlDataParam._displaySizeMin._x = titleWidth + kTitleBarInnerPadding.horz() + kDefaultRoundButtonRadius * 2.0f;
				prepareControlDataParam._displaySizeMin._y = kTitleBarBaseSize._y + 16.0f;
				prepareControlDataParam._alwaysResetPosition = false;
				prepareControlDataParam._viewportUsage = ViewportUsage::Parent; // ROOT

				if (windowControlData.isDockHosting() == true)
				{
					prepareControlDataParam._deltaInteractionSizeByDock._x = -windowControlData.getHorzDockSizeSum();
					prepareControlDataParam._deltaInteractionSizeByDock._y = -windowControlData.getVertDockSizeSum();
				}
				else
				{
					prepareControlDataParam._deltaInteractionSizeByDock.setZero();
				}
			}
			nextNoAutoPositioned();
			prepareControlData(windowControlData, prepareControlDataParam);

			const ControlData& parentControlData = getControlData(windowControlData.getParentHashKey());
			const bool isParentAlsoWindow = parentControlData.getControlType() == ControlType::Window;
			if (isParentAlsoWindow == true)
			{
				windowControlData._position += parentControlData._deltaPosition;
				windowControlData._deltaPosition = parentControlData._deltaPosition; // 계층 가장 아래 Window 까지 전파되도록
			}

			fs::RenderingBase::Color color;
			const bool isFocused = processFocusControl(windowControlData, getNamedColor(NamedColor::WindowFocused), getNamedColor(NamedColor::WindowOutOfFocus), color);
			const bool isAncestorFocused_ = isAncestorFocused(windowControlData);
			fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext = (isFocused || isAncestorFocused_) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;

			// Viewport & Scissor rectangle
			{
				// ScrollBar state
				const ScrollBarType scrollBarState = windowControlData._controlValue.getCurrentScrollBarType();
				const bool hasScrollBarVert = (scrollBarState == ScrollBarType::Both || scrollBarState == ScrollBarType::Vert);
				const bool hasScrollBarHorz = (scrollBarState == ScrollBarType::Both || scrollBarState == ScrollBarType::Horz);

				// Viewport & ScissorRectangle for me !!!
				{
					windowControlData.setViewportIndexXXX(static_cast<uint32>(_viewportArrayPerFrame.size()));

					D3D11_RECT scissorRectangleForMe;
					scissorRectangleForMe.left = static_cast<LONG>(windowControlData._position._x);
					scissorRectangleForMe.top = static_cast<LONG>(windowControlData._position._y - kTitleBarBaseSize._y);
					scissorRectangleForMe.right = static_cast<LONG>(windowControlData._position._x + windowControlData._displaySize._x);
					scissorRectangleForMe.bottom = static_cast<LONG>(windowControlData._position._y + windowControlData._displaySize._y);

					if (isParentAlsoWindow == true)
					{
						const D3D11_RECT& parentScissorRectangle = _scissorRectangleArrayPerFrame[parentControlData.getViewportIndexForDocks()];
						constraintInnerRect(scissorRectangleForMe, parentScissorRectangle);
					}
					_scissorRectangleArrayPerFrame.emplace_back(scissorRectangleForMe);
					_viewportArrayPerFrame.emplace_back(_viewportFullScreen);
				}

				if (isParentAlsoWindow == true)
				{
					if (windowControlData.isDocking() == true)
					{
						windowControlData.setViewportIndexXXX(parentControlData.getViewportIndexForDocks());
					}
					else
					{
						windowControlData.setViewportIndexXXX(parentControlData.getViewportIndex());
					}
				}

				// Viewport & ScissorRectangle for docks !!!
				{
					windowControlData.setViewportIndexForDocksXXX(static_cast<uint32>(_viewportArrayPerFrame.size()));

					D3D11_RECT scissorRectangleForDocks;
					scissorRectangleForDocks.left = static_cast<LONG>(windowControlData._position._x + prepareControlDataParam._innerPadding.left());
					scissorRectangleForDocks.top = static_cast<LONG>(windowControlData._position._y + prepareControlDataParam._innerPadding.top() + kTitleBarBaseSize._y);
					scissorRectangleForDocks.right = static_cast<LONG>(windowControlData._position._x + windowControlData._displaySize._x - prepareControlDataParam._innerPadding.horz());
					scissorRectangleForDocks.bottom = static_cast<LONG>(windowControlData._position._y + windowControlData._displaySize._y - prepareControlDataParam._innerPadding.vert());
					
					if (isParentAlsoWindow == true)
					{
						const D3D11_RECT& parentScissorRectangle = _scissorRectangleArrayPerFrame[parentControlData.getViewportIndexForDocks()];
						constraintInnerRect(scissorRectangleForDocks, parentScissorRectangle);

					}
					_scissorRectangleArrayPerFrame.emplace_back(scissorRectangleForDocks);
					_viewportArrayPerFrame.emplace_back(_viewportFullScreen);
				}

				// Viewport & ScissorRectangle for child controls !!!
				{
					windowControlData.setViewportIndexForChildrenXXX(static_cast<uint32>(_viewportArrayPerFrame.size()));

					const DockDatum& dockDatumTopSide = windowControlData.getDockDatum(DockingMethod::TopSide);
					const DockDatum& dockDatumBottomSide = windowControlData.getDockDatum(DockingMethod::BottomSide);
					const DockDatum& dockDatumLeftSide = windowControlData.getDockDatum(DockingMethod::LeftSide);
					const DockDatum& dockDatumRightSide = windowControlData.getDockDatum(DockingMethod::RightSide);
					D3D11_RECT scissorRectangleForChildren;
					scissorRectangleForChildren.top = static_cast<LONG>(windowControlData._position._y + prepareControlDataParam._innerPadding.top() + kTitleBarBaseSize._y);
					if (dockDatumTopSide.hasDockedControls() == true)
					{
						scissorRectangleForChildren.top += static_cast<LONG>(windowControlData.getDockSize(DockingMethod::TopSide)._y);
					}
					scissorRectangleForChildren.left = static_cast<LONG>(windowControlData._position._x + prepareControlDataParam._innerPadding.left());
					if (dockDatumLeftSide.hasDockedControls() == true)
					{
						scissorRectangleForChildren.left += static_cast<LONG>(windowControlData.getDockSize(DockingMethod::LeftSide)._x);
					}
					scissorRectangleForChildren.right = static_cast<LONG>(windowControlData._position._x + windowControlData._displaySize._x - prepareControlDataParam._innerPadding.horz() - ((hasScrollBarVert == true) ? kScrollBarThickness : 0.0f));
					if (dockDatumRightSide.hasDockedControls() == true)
					{
						scissorRectangleForChildren.right -= static_cast<LONG>(windowControlData.getDockSize(DockingMethod::RightSide)._x);
					}
					scissorRectangleForChildren.bottom = static_cast<LONG>(windowControlData._position._y + windowControlData._displaySize._y - prepareControlDataParam._innerPadding.vert() - ((hasScrollBarHorz == true) ? kScrollBarThickness : 0.0f));
					if (dockDatumBottomSide.hasDockedControls() == true)
					{
						scissorRectangleForChildren.bottom -= static_cast<LONG>(windowControlData.getDockSize(DockingMethod::BottomSide)._y);
					}

					if (isParentAlsoWindow == true)
					{
						const D3D11_RECT& parentScissorRectangle = _scissorRectangleArrayPerFrame[parentControlData.getViewportIndex()];
						constraintInnerRect(scissorRectangleForChildren, parentScissorRectangle);
					}
					_scissorRectangleArrayPerFrame.emplace_back(scissorRectangleForChildren);
					_viewportArrayPerFrame.emplace_back(_viewportFullScreen);
				}
			}

			
			bool isOpen = windowControlData.isControlState(ControlState::VisibleOpen);
			if (windowControlData.isDocking() == true)
			{
				const ControlData& dockControlData = getControlData(windowControlData.getDockControlHashKey());
				const bool isShownInDock = dockControlData.isShowingInDock(windowControlData);
				isOpen = isShownInDock;
			}
			if (isOpen == true)
			{
				shapeFontRendererContext.setViewportIndex(windowControlData.getViewportIndex());
				shapeFontRendererContext.setColor(color);

				const fs::Float4& windowCenterPosition = getControlCenterPosition(windowControlData);
				shapeFontRendererContext.setPosition(windowCenterPosition + fs::Float4(0, titleBarSize._y * 0.5f, 0, 0));
				if (windowControlData.isDocking() == true)
				{
					fs::RenderingBase::Color inDockColor = getNamedColor(NamedColor::ShownInDock);
					inDockColor.a(color.a());
					shapeFontRendererContext.setColor(inDockColor);
					shapeFontRendererContext.drawRectangle(windowControlData._displaySize - fs::Float2(0, titleBarSize._y), 0.0f, 0.0f);
				}
				else
				{
					shapeFontRendererContext.drawHalfRoundedRectangle(windowControlData._displaySize - fs::Float2(0, titleBarSize._y), (kDefaultRoundnessInPixel * 2.0f / windowControlData._displaySize.minElement()), 0.0f);
				}

				renderDock(windowControlData, shapeFontRendererContext);
				
				_controlStackPerFrame.emplace_back(ControlStackData(windowControlData));
			}

			// 중요
			nextNoAutoPositioned();

			titleBarSize._x = windowControlData._displaySize._x;
			beginTitleBar(title, titleBarSize, kTitleBarInnerPadding);
			endTitleBar();

			if (windowParam._scrollBarType != ScrollBarType::None)
			{
				pushScrollBar(windowParam._scrollBarType);
			}
			
			return isOpen;
		}

		const bool GuiContext::beginButton(const wchar_t* const text)
		{
			static constexpr ControlType controlType = ControlType::Button;
			
			ControlData& controlData = getControlData(text, controlType);
			
			PrepareControlDataParam prepareControlDataParam;
			{
				const float textWidth = _shapeFontRendererContextBackground.calculateTextWidth(text, fs::StringUtil::wcslen(text));
				prepareControlDataParam._initialDisplaySize = fs::Float2(textWidth + 24, _fontSize + 12);
			}
			prepareControlData(controlData, prepareControlDataParam);
		
			fs::RenderingBase::Color color;
			const bool isClicked = processClickControl(controlData, getNamedColor(NamedColor::NormalState), getNamedColor(NamedColor::HoverState), getNamedColor(NamedColor::PressedState), color);
		
			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused_ == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;
			const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setColor(color);
			shapeFontRendererContext.setPosition(controlCenterPosition);
			const fs::Float2& displaySize = controlData._displaySize;
			shapeFontRendererContext.drawRoundedRectangle(displaySize, (kDefaultRoundnessInPixel * 2.0f / displaySize.minElement()), 0.0f, 0.0f);

			shapeFontRendererContext.setTextColor(getNamedColor(NamedColor::LightFont) * fs::RenderingBase::Color(1.0f, 1.0f, 1.0f, color.a()));
			shapeFontRendererContext.drawDynamicText(text, controlCenterPosition, fs::RenderingBase::TextRenderDirectionHorz::Centered, fs::RenderingBase::TextRenderDirectionVert::Centered, kFontScaleB);

			if (isClicked == true)
			{
				_controlStackPerFrame.emplace_back(ControlStackData(controlData));
			}

			return isClicked;
		}

		const bool GuiContext::beginCheckBox(const wchar_t* const text, bool& outIsChecked)
		{
			static constexpr ControlType controlType = ControlType::CheckBox;

			ControlData& controlData = getControlData(text, controlType);
			PrepareControlDataParam prepareControlDataParam;
			{
				prepareControlDataParam._initialDisplaySize = kCheckBoxSize;
			}
			prepareControlData(controlData, prepareControlDataParam);

			// Toggle control
			fs::RenderingBase::Color color;
			const bool isClicked = processToggleControl(controlData, getNamedColor(NamedColor::NormalState), getNamedColor(NamedColor::NormalState), getNamedColor(NamedColor::HighlightColor), color);

			outIsChecked = controlData._controlValue.getIsToggled();

			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused_ == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;
			const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setColor(color);
			shapeFontRendererContext.setPosition(controlCenterPosition);
			const fs::Float2& displaySize = controlData._displaySize;
			shapeFontRendererContext.drawRoundedRectangle(displaySize, (kDefaultRoundnessInPixel / displaySize.minElement()), 0.0f, 0.0f);

			if (outIsChecked == true)
			{
				shapeFontRendererContext.setColor(getNamedColor(NamedColor::LightFont));
				fs::Float2 p0 = fs::Float2(controlCenterPosition._x - 1.0f, controlCenterPosition._y + 4.0f);
				shapeFontRendererContext.drawLine(p0, p0 + fs::Float2(-4.0f, -5.0f), 2.0f);

				shapeFontRendererContext.drawLine(p0, p0 + fs::Float2(+7.0f, -8.0f), 2.0f);
			}

			shapeFontRendererContext.setTextColor(getNamedColor(NamedColor::LightFont) * fs::RenderingBase::Color(1.0f, 1.0f, 1.0f, color.a()));
			shapeFontRendererContext.drawDynamicText(text, controlCenterPosition + fs::Float4(kCheckBoxSize._x * 0.75f, 0.0f, 0.0f, 0.0f), fs::RenderingBase::TextRenderDirectionHorz::Rightward, fs::RenderingBase::TextRenderDirectionVert::Centered, kFontScaleB);

			if (isClicked == true)
			{
				_controlStackPerFrame.emplace_back(ControlStackData(controlData));
			}

			return isClicked;
		}

		void GuiContext::pushLabel(const wchar_t* const text, const LabelParam& labelParam)
		{
			static constexpr ControlType controlType = ControlType::Label;

			const float textWidth = _shapeFontRendererContextBackground.calculateTextWidth(text, fs::StringUtil::wcslen(text));
			ControlData& controlData = getControlData(text, controlType);

			PrepareControlDataParam prepareControlDataParam;
			{
				prepareControlDataParam._initialDisplaySize = (labelParam._size == fs::Float2::kZero) ? fs::Float2(textWidth + 24, _fontSize + 12) : labelParam._size;
			}
			prepareControlData(controlData, prepareControlDataParam);
			
			fs::RenderingBase::Color colorWithAlpha = fs::RenderingBase::Color(255, 255, 255);
			processShowOnlyControl(controlData, colorWithAlpha);

			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused_ == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;
			const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setColor(labelParam._backgroundColor);
			shapeFontRendererContext.setPosition(controlCenterPosition);
			const fs::Float2& displaySize = controlData._displaySize;
			shapeFontRendererContext.drawRectangle(displaySize, 0.0f, 0.0f);

			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setTextColor((labelParam._fontColor.isTransparent() == true) ? getNamedColor(NamedColor::LightFont) * colorWithAlpha : labelParam._fontColor);

			fs::Float4 textPosition = controlCenterPosition;
			fs::RenderingBase::TextRenderDirectionHorz textRenderDirectionHorz = fs::RenderingBase::TextRenderDirectionHorz::Centered;
			fs::RenderingBase::TextRenderDirectionVert textRenderDirectionVert = fs::RenderingBase::TextRenderDirectionVert::Centered;
			if (labelParam._alignmentHorz != TextAlignmentHorz::Center)
			{
				if (labelParam._alignmentHorz == TextAlignmentHorz::Left)
				{
					textPosition._x = controlData._position._x;
					textRenderDirectionHorz = fs::RenderingBase::TextRenderDirectionHorz::Rightward;
				}
				else
				{
					textPosition._x = controlData._position._x + controlData._displaySize._x;
					textRenderDirectionHorz = fs::RenderingBase::TextRenderDirectionHorz::Leftward;
				}
			}
			if (labelParam._alignmentVert != TextAlignmentVert::Middle)
			{
				if (labelParam._alignmentVert == TextAlignmentVert::Top)
				{
					textPosition._y = controlData._position._y;
					textRenderDirectionVert = fs::RenderingBase::TextRenderDirectionVert::Downward;
				}
				else
				{
					textPosition._y = controlData._position._y + controlData._displaySize._y;
					textRenderDirectionVert = fs::RenderingBase::TextRenderDirectionVert::Upward;
				}
			}
			shapeFontRendererContext.drawDynamicText(text, textPosition, textRenderDirectionHorz, textRenderDirectionVert, kFontScaleB);
		}

		const bool GuiContext::beginSlider(const wchar_t* const name, const SliderParam& sliderParam, float& outValue)
		{
			static constexpr ControlType trackControlType = ControlType::Slider;

			bool isChanged = false;
			const float sliderValidLength = sliderParam._size._x - kSliderThumbRadius * 2.0f;
			ControlData& trackControlData = getControlData(name, trackControlType);
			
			PrepareControlDataParam prepareControlDataParamForTrack;
			{
				prepareControlDataParamForTrack._initialDisplaySize._x = sliderParam._size._x;
				prepareControlDataParamForTrack._initialDisplaySize._y = (0.0f == sliderParam._size._y) ? kSliderThumbRadius * 2.0f : sliderParam._size._y;
			}
			prepareControlData(trackControlData, prepareControlDataParamForTrack);
			
			fs::RenderingBase::Color trackbColor = getNamedColor(NamedColor::HoverState);
			processShowOnlyControl(trackControlData, trackbColor, true);

			{
				static constexpr ControlType thumbControlType = ControlType::SliderThumb;

				const ControlData& parentWindowControlData = getParentWindowControlData(trackControlData);

				ControlData& thumbControlData = getControlData(name, thumbControlType);
				thumbControlData._position._x = trackControlData._position._x + trackControlData._controlValue.getThumbAt() * sliderValidLength;
				thumbControlData._position._y = trackControlData._position._y + trackControlData._displaySize._y * 0.5f - thumbControlData._displaySize._y * 0.5f;
				thumbControlData._isDraggable = true;
				thumbControlData._draggingConstraints.top(thumbControlData._position._y);
				thumbControlData._draggingConstraints.bottom(thumbControlData._draggingConstraints.top());
				thumbControlData._draggingConstraints.left(trackControlData._position._x);
				thumbControlData._draggingConstraints.right(thumbControlData._draggingConstraints.left() + sliderValidLength);

				PrepareControlDataParam prepareControlDataParamForThumb;
				{
					prepareControlDataParamForThumb._initialDisplaySize._x = kSliderThumbRadius * 2.0f;
					prepareControlDataParamForThumb._initialDisplaySize._y = kSliderThumbRadius * 2.0f;
					prepareControlDataParamForThumb._alwaysResetPosition = false;
					prepareControlDataParamForThumb._desiredPositionInParent = trackControlData._position - parentWindowControlData._position;
				}
				nextNoAutoPositioned();
				prepareControlData(thumbControlData, prepareControlDataParamForThumb);
				
				fs::RenderingBase::Color thumbColor;
				processScrollableControl(thumbControlData, getNamedColor(NamedColor::HighlightColor), getNamedColor(NamedColor::HighlightColor).addedRgb(0.125f), thumbColor);

				const float thumbAt = (thumbControlData._position._x - trackControlData._position._x) / sliderValidLength;
				if (trackControlData._controlValue.getThumbAt() != thumbAt)
				{
					_controlStackPerFrame.emplace_back(ControlStackData(trackControlData));

					isChanged = true;
				}
				trackControlData._controlValue.setThumbAt(thumbAt);
				outValue = thumbAt;

				const bool isAncestorFocused_ = isAncestorFocused(trackControlData);
				fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused_ == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;

				// Draw track
				{
					const float trackRadius = kSliderTrackThicknes * 0.5f;
					const float trackRectLength = sliderParam._size._x - trackRadius * 2.0f;
					const float trackRectLeftLength = thumbAt * sliderValidLength;
					const float trackRectRightLength = trackRectLength - trackRectLeftLength;

					const fs::Float4& trackCenterPosition = getControlCenterPosition(trackControlData);
					fs::Float4 trackRenderPosition = trackCenterPosition - fs::Float4(trackRectLength * 0.5f, 0.0f, 0.0f, 0.0f);

					// Left(or Upper) half circle
					shapeFontRendererContext.setViewportIndex(thumbControlData.getViewportIndex());
					shapeFontRendererContext.setColor(getNamedColor(NamedColor::HighlightColor));
					shapeFontRendererContext.setPosition(trackRenderPosition);
					shapeFontRendererContext.drawHalfCircle(trackRadius, +fs::Math::kPiOverTwo);

					// Left rect
					trackRenderPosition._x += trackRectLeftLength * 0.5f;
					shapeFontRendererContext.setPosition(trackRenderPosition);
					shapeFontRendererContext.drawRectangle(fs::Float2(trackRectLeftLength, kSliderTrackThicknes), 0.0f, 0.0f);
					trackRenderPosition._x += trackRectLeftLength * 0.5f;

					// Right rect
					shapeFontRendererContext.setColor(trackbColor);
					trackRenderPosition._x += trackRectRightLength * 0.5f;
					shapeFontRendererContext.setPosition(trackRenderPosition);
					shapeFontRendererContext.drawRectangle(fs::Float2(trackRectRightLength, kSliderTrackThicknes), 0.0f, 0.0f);
					trackRenderPosition._x += trackRectRightLength * 0.5f;

					// Right(or Lower) half circle
					shapeFontRendererContext.setPosition(trackRenderPosition);
					shapeFontRendererContext.drawHalfCircle(trackRadius, -fs::Math::kPiOverTwo);
				}

				// Draw thumb
				{
					const fs::Float4& thumbCenterPosition = getControlCenterPosition(thumbControlData);
					shapeFontRendererContext.setPosition(thumbCenterPosition);
					shapeFontRendererContext.setColor(fs::RenderingBase::Color::kWhite.scaledA(thumbColor.a()));
					shapeFontRendererContext.drawCircle(kSliderThumbRadius);
					shapeFontRendererContext.setColor(thumbColor);
					shapeFontRendererContext.drawCircle(kSliderThumbRadius - 2.0f);
				}
			}
			
			return isChanged;
		}

		const bool GuiContext::beginTextBox(const wchar_t* const name, const TextBoxParam& textBoxParam, std::wstring& outText)
		{
			static constexpr ControlType controlType = ControlType::TextBox;
			
			ControlData& controlData = getControlData(name, controlType);
			controlData._isFocusable = true;

			PrepareControlDataParam prepareControlDataParam;
			{
				prepareControlDataParam._initialDisplaySize._x = textBoxParam._size._x;
				prepareControlDataParam._initialDisplaySize._y = fs::max(_fontSize, textBoxParam._size._y);
			}
			prepareControlData(controlData, prepareControlDataParam);

			fs::RenderingBase::Color color;
			const bool isFocused = processFocusControl(controlData, textBoxParam._backgroundColor, textBoxParam._backgroundColor.addedRgb(-0.125f), color);
			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused_ == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;
			
			const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);

			// Viewport & Scissor rectangle
			{
				controlData.setViewportIndexXXX(static_cast<uint32>(_viewportArrayPerFrame.size()));

				D3D11_RECT scissorRectangleForMe;
				scissorRectangleForMe.left = static_cast<LONG>(controlData._position._x);
				scissorRectangleForMe.top = static_cast<LONG>(controlData._position._y);
				scissorRectangleForMe.right = static_cast<LONG>(scissorRectangleForMe.left + controlData._displaySize._x);
				scissorRectangleForMe.bottom = static_cast<LONG>(scissorRectangleForMe.top + controlData._displaySize._y);

				const ControlData& parentControlData = getControlData(controlData.getParentHashKey());
				{
					const D3D11_RECT& parentScissorRectangle = _scissorRectangleArrayPerFrame[parentControlData.getViewportIndexForChildren()];
					constraintInnerRect(scissorRectangleForMe, parentScissorRectangle);
				}

				_scissorRectangleArrayPerFrame.emplace_back(scissorRectangleForMe);
				_viewportArrayPerFrame.emplace_back(_viewportFullScreen);
			}


			const wchar_t inputCandidate[2]{ _wcharInputCandiate, L'\0' };
			const float inputCandidateWidth = (32 <= _wcharInputCandiate) ? shapeFontRendererContext.calculateTextWidth(inputCandidate, 1) : 0.0f;
			fs::Float4 textRenderOffset;
			if (controlData._controlValue.getTextDisplayOffset() == 0)
			{
				const float fullTextWidth = shapeFontRendererContext.calculateTextWidth(outText.c_str(), static_cast<uint32>(outText.length()));
				if (textBoxParam._alignmentHorz == TextAlignmentHorz::Center)
				{
					textRenderOffset._x = (controlData._displaySize._x - fullTextWidth - inputCandidateWidth) * 0.5f;
				}
				else if (textBoxParam._alignmentHorz == TextAlignmentHorz::Right)
				{
					textRenderOffset._x = controlData._displaySize._x - fullTextWidth - inputCandidateWidth;
				}
			}

			// Input 처리
			if (isFocused == true)
			{
				static bool isFirstCalled = true;
				static std::wstring errorMessage;
				if (isFirstCalled == true)
				{
					errorMessage.resize(1024);
					swprintf_s(errorMessage.data(), 1024, L"텍스트 길이가 %d 자를 넘을 수 없습니다!", kTextBoxMaxTextLength);
					isFirstCalled = false;
				}
				static std::function fnRefreshCaret = [](const uint64 currentTimeMs, uint16& caretState, uint64& lastCaretBlinkTimeMs)
				{
					lastCaretBlinkTimeMs = currentTimeMs;
					caretState = 0;
				};
				static std::function fnEraseSelection = [&]()
				{
					const uint16 selectionLength = controlData._controlValue.getSelectionLength();
					const uint16 selectionStart = controlData._controlValue.getSelectionStart();
					outText.erase(selectionStart, selectionLength);

					const uint16 textLength = static_cast<uint16>(outText.length());
					uint16& caretAt = controlData._controlValue.getCaretAt();
					caretAt = fs::min(static_cast<uint16>(caretAt - selectionLength), textLength);

					controlData._controlValue.getSelectionLength() = 0;
				};
				static std::function fnInsertWchar = [&](uint16& caretAt, const wchar_t input)
				{
					if (outText.length() < kTextBoxMaxTextLength)
					{
						outText.insert(outText.begin() + caretAt, input);

						++caretAt;
					}
					else
					{
						_graphicDevice->getWindow()->showMessageBox(L"오류", errorMessage.c_str(), fs::Window::MessageBoxType::Error);
					}
				};
				static std::function fnInsertWstring = [&](uint16& caretAt, const std::wstring& input)
				{
					const uint32 oldLength = static_cast<uint32>(outText.length());
					if (oldLength < kTextBoxMaxTextLength)
					{
						uint32 deltaLength = static_cast<uint32>(input.length());
						if (kTextBoxMaxTextLength < oldLength + input.length())
						{
							_graphicDevice->getWindow()->showMessageBox(L"오류", errorMessage.c_str(), fs::Window::MessageBoxType::Error);

							deltaLength = kTextBoxMaxTextLength - oldLength;
						}
						outText.insert(caretAt, input.substr(0, deltaLength));

						caretAt += static_cast<uint16>(deltaLength);
					}
					else
					{
						_graphicDevice->getWindow()->showMessageBox(L"오류", errorMessage.c_str(), fs::Window::MessageBoxType::Error);
					}
				};
				static std::function fnProcessSelection = [&](const uint16 oldCaretAt, const uint16 caretAt)
				{
					uint16& selectionStart = controlData._controlValue.getSelectionStart();
					uint16& selectionLength = controlData._controlValue.getSelectionLength();

					if (selectionLength == 0)
					{
						// 새 Selection
						selectionStart = fs::min(caretAt, oldCaretAt);
						selectionLength = fs::max(caretAt, oldCaretAt) - selectionStart;
					}
					else
					{
						// 기존에 Selection 있음
						const bool isLeftWard = caretAt < oldCaretAt;
						const uint16 oldSelectionStart = selectionStart;
						const uint16 oldSelectionEnd = selectionStart + selectionLength;
						const bool fromHead = (oldSelectionStart == oldCaretAt);
						if (((oldSelectionEnd == oldCaretAt) && (caretAt < oldSelectionStart)) || ((oldSelectionStart == oldCaretAt && oldSelectionEnd < caretAt)))
						{
							// 새 caretAt 위치가 급격히 달라진 경우
							if (caretAt < oldSelectionStart)
							{
								selectionStart = caretAt;
								selectionLength = oldSelectionStart - caretAt;
							}
							else
							{
								selectionStart = oldSelectionEnd;
								selectionLength = caretAt - selectionStart;
							}
						}
						else
						{
							if (fromHead == true)
							{
								// from Head
								const uint16 selectionEnd = oldSelectionEnd;
								selectionStart = (isLeftWard == true) ? fs::min(selectionStart, caretAt) : fs::max(selectionStart, caretAt);
								selectionLength = selectionEnd - selectionStart;
							}
							else
							{
								// from Tail
								const uint16 selectionEnd = (isLeftWard == true) ? fs::min(oldSelectionEnd, caretAt) : fs::max(oldSelectionEnd, caretAt);
								selectionStart = fs::min(selectionStart, caretAt);
								selectionLength = selectionEnd - selectionStart;
							}
						}
					}
				};


				const uint64 currentTimeMs = fs::Profiler::getCurrentTimeMs();
				uint16& caretAt = controlData._controlValue.getCaretAt();
				uint16& caretState = controlData._controlValue.getCaretState();
				uint64& lastCaretBlinkTimeMs = controlData._controlValue.getInternalTimeMs();
				if (lastCaretBlinkTimeMs + _caretBlinkIntervalMs < currentTimeMs)
				{
					lastCaretBlinkTimeMs = currentTimeMs;

					caretState ^= 1;
				}

				const bool isShiftKeyDown = _graphicDevice->getWindow()->isKeyDown(fs::Window::EventData::KeyCode::Shift);
				const bool isControlKeyDown = _graphicDevice->getWindow()->isKeyDown(fs::Window::EventData::KeyCode::Control);


				// 마우스 입력 처리
				const bool isMouseLeftDown = _graphicDevice->getWindow()->isMouseDown(fs::Window::EventData::MouseButton::Left);
				const bool isMouseLeftDownFirst = _graphicDevice->getWindow()->isMouseDownFirst(fs::Window::EventData::MouseButton::Left);
				if (isMouseLeftDown == true || isMouseLeftDownFirst == true)
				{
					const uint16 textLength = static_cast<uint16>(outText.length());
					const float textDisplayOffset = controlData._controlValue.getTextDisplayOffset();
					const float positionInText = _mousePosition._x - controlData._position._x + textDisplayOffset - textRenderOffset._x;
					const uint32 newCaretAt = shapeFontRendererContext.calculateIndexFromPositionInText(outText.c_str(), textLength, positionInText);
					if (isMouseLeftDownFirst == true)
					{
						caretAt = newCaretAt;

						controlData._controlValue.getSelectionLength() = 0;
						controlData._controlValue.getSelectionStart() = caretAt;
					}
					else
					{
						if (newCaretAt != caretAt)
						{
							fnProcessSelection(caretAt, newCaretAt);

							caretAt = newCaretAt;
						}
					}

					_wcharInput = 0;
					_keyCode = fs::Window::EventData::KeyCode::NONE;
				}
				else
				{
					const uint16 oldCaretAt = controlData._controlValue.getCaretAt();
					if (32 <= _wcharInputCandiate)
					{
						const uint16 selectionLength = controlData._controlValue.getSelectionLength();
						if (0 < selectionLength)
						{
							fnEraseSelection();
						}
					}
					else if (_wcharInput != L'\0')
					{
						// 글자 입력 or 키 입력

						if (32 <= _wcharInput)
						{
							const uint16 selectionLength = controlData._controlValue.getSelectionLength();
							if (0 < selectionLength)
							{
								fnEraseSelection();
							}

							fnInsertWchar(caretAt, _wcharInput);
						}
						else
						{
							const uint16 selectionLength = controlData._controlValue.getSelectionLength();

							if (_wcharInput == VK_BACK) // BackSpace
							{
								const uint16 selectionLength = controlData._controlValue.getSelectionLength();
								if (0 < selectionLength)
								{
									fnEraseSelection();
								}
								else
								{
									if (outText.empty() == false && 0 < caretAt)
									{
										outText.erase(outText.begin() + caretAt - 1);

										caretAt = fs::max(caretAt - 1, 0);
									}
								}
							}
							else if (isControlKeyDown == true && _wcharInput == 0x01) // Ctrl + A
							{
								controlData._controlValue.getSelectionStart() = 0;
								controlData._controlValue.getSelectionLength() = static_cast<uint16>(outText.length());
								caretAt = controlData._controlValue.getSelectionLength();
							}
							else if (isControlKeyDown == true && _wcharInput == 0x03) // Ctrl + C
							{
								if (0 < selectionLength)
								{
									const uint16 selectionStart = controlData._controlValue.getSelectionStart();
									_graphicDevice->getWindow()->textToClipboard(&outText[selectionStart], selectionLength);
								}
							}
							else if (isControlKeyDown == true && _wcharInput == 0x16) // Ctrl + V
							{
								std::wstring fromClipboard;
								_graphicDevice->getWindow()->textFromClipboard(fromClipboard);

								if (fromClipboard.empty() == false)
								{
									if (0 < selectionLength)
									{
										fnEraseSelection();
									}

									fnInsertWstring(caretAt, fromClipboard);
								}
							}
							else if (isControlKeyDown == true && _wcharInput == 0x18) // Ctrl + X
							{
								if (0 < selectionLength)
								{
									const uint16 selectionStart = controlData._controlValue.getSelectionStart();
									_graphicDevice->getWindow()->textToClipboard(&outText[selectionStart], selectionLength);

									fnEraseSelection();
								}
							}
						}

						_wcharInput = L'\0';
					}
					else
					{
						// 키 눌림 처리

						const uint16 textLength = static_cast<uint16>(outText.length());

						if (_keyCode == fs::Window::EventData::KeyCode::Left)
						{
							caretAt = fs::max(caretAt - 1, 0);
						}
						else if (_keyCode == fs::Window::EventData::KeyCode::Right)
						{
							caretAt = fs::min(caretAt + 1, static_cast<int32>(textLength));
						}
						else if (_keyCode == fs::Window::EventData::KeyCode::Delete)
						{
							const uint16 selectionLength = controlData._controlValue.getSelectionLength();
							if (0 < selectionLength)
							{
								fnEraseSelection();
							}
							else
							{
								if (0 < textLength && caretAt < textLength)
								{
									outText.erase(outText.begin() + caretAt);

									caretAt = fs::min(caretAt, textLength);
								}
							}
						}
						else if (_keyCode == fs::Window::EventData::KeyCode::Home)
						{
							caretAt = 0;

							float& textDisplayOffset = controlData._controlValue.getTextDisplayOffset();
							textDisplayOffset = 0.0f;
						}
						else if (_keyCode == fs::Window::EventData::KeyCode::End)
						{
							caretAt = textLength;

							float& textDisplayOffset = controlData._controlValue.getTextDisplayOffset();
							const float textWidth = shapeFontRendererContext.calculateTextWidth(outText.c_str(), textLength);
							textDisplayOffset = fs::max(0.0f, textWidth - controlData._displaySize._x);
						}
					}

					// Caret 위치가 바뀐 경우 refresh
					if (oldCaretAt != caretAt)
					{
						fnRefreshCaret(currentTimeMs, caretState, lastCaretBlinkTimeMs);

						// Selection
						if (isShiftKeyDown == true && _keyCode != fs::Window::EventData::KeyCode::NONE)
						{
							fnProcessSelection(oldCaretAt, caretAt);
						}
					}

					if (isShiftKeyDown == false && _keyCode != fs::Window::EventData::KeyCode::NONE && _keyCode != fs::Window::EventData::KeyCode::Control && _keyCode != fs::Window::EventData::KeyCode::Alt)
					{
						// Selection 해제
						uint16& selectionLength = controlData._controlValue.getSelectionLength();
						selectionLength = 0;
					}
				}
			}

			// Caret 의 렌더링 위치가 TextBox 를 벗어나는 경우 처리!!
			const uint16 textLength = static_cast<uint16>(outText.length());
			const uint16 caretAt = controlData._controlValue.getCaretAt();
			float& textDisplayOffset = controlData._controlValue.getTextDisplayOffset();
			const float textWidthTillCaret = shapeFontRendererContext.calculateTextWidth(outText.c_str(), fs::min(caretAt, textLength));
			{
				const float deltaTextDisplayOffsetRight = (textWidthTillCaret + inputCandidateWidth - textDisplayOffset) - controlData._displaySize._x;
				if (0.0f < deltaTextDisplayOffsetRight)
				{
					textDisplayOffset += deltaTextDisplayOffsetRight;
				}

				const float deltaTextDisplayOffsetLeft = (textWidthTillCaret + inputCandidateWidth - textDisplayOffset);
				if (deltaTextDisplayOffsetLeft < 0.0f)
				{
					textDisplayOffset -= kTextBoxBackSpaceStride;

					textDisplayOffset = fs::max(textDisplayOffset, 0.0f);
				}

				if (textWidthTillCaret + inputCandidateWidth < controlData._displaySize._x)
				{
					textDisplayOffset = 0.0f;
				}
			}
			

			// Draw background
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setColor(color);
			shapeFontRendererContext.setPosition(controlCenterPosition);
			shapeFontRendererContext.drawRoundedRectangle(controlData._displaySize, (kDefaultRoundnessInPixel / controlData._displaySize.minElement()), 0.0f, 0.0f);


			// Text 및 Caret 렌더링
			const uint16 caretState = controlData._controlValue.getCaretState();
			const fs::Float2& controlLeftCenterPosition = getControlLeftCenterPosition(controlData);
			const fs::Float4 textRenderPosition = fs::Float4(controlLeftCenterPosition._x - textDisplayOffset, controlLeftCenterPosition._y, 0, 0);
			if (isFocused == true && 32 <= _wcharInputCandiate)
			{
				// InputCandidate 렌더링 필요!

				// Text 렌더링 (Caret 이전)
				if (outText.empty() == false)
				{
					shapeFontRendererContext.setTextColor(textBoxParam._fontColor);
					shapeFontRendererContext.drawDynamicText(outText.c_str(), caretAt, textRenderPosition + textRenderOffset, fs::RenderingBase::TextRenderDirectionHorz::Rightward, fs::RenderingBase::TextRenderDirectionVert::Centered);
				}

				// Input Candidate 렌더링
				shapeFontRendererContext.setTextColor(textBoxParam._fontColor);
				shapeFontRendererContext.drawDynamicText(inputCandidate, fs::Float4(controlLeftCenterPosition._x + textWidthTillCaret - textDisplayOffset, controlLeftCenterPosition._y, 0, 0) + textRenderOffset, fs::RenderingBase::TextRenderDirectionHorz::Rightward, fs::RenderingBase::TextRenderDirectionVert::Centered);

				// Text 렌더링 (Caret 이후)
				if (outText.empty() == false)
				{
					shapeFontRendererContext.setTextColor(textBoxParam._fontColor);
					shapeFontRendererContext.drawDynamicText(outText.c_str() + caretAt, textLength - caretAt, textRenderPosition + fs::Float4(textWidthTillCaret + inputCandidateWidth, 0, 0, 0) + textRenderOffset, fs::RenderingBase::TextRenderDirectionHorz::Rightward, fs::RenderingBase::TextRenderDirectionVert::Centered);
				}

				// Caret 렌더링 (Input Candidate 의 바로 뒤에!)
				if (0 == caretState)
				{
					const float caretHeight = _fontSize;
					const fs::Float2& p0 = fs::Float2(controlLeftCenterPosition._x + textWidthTillCaret + inputCandidateWidth - textDisplayOffset + textRenderOffset._x, controlLeftCenterPosition._y - caretHeight * 0.5f);
					shapeFontRendererContext.setColor(fs::RenderingBase::Color::kBlack);
					shapeFontRendererContext.drawLine(p0, p0 + fs::Float2(0.0f, caretHeight), 2.0f);
				}
			}
			else
			{
				// InputCandidate 렌더링 불필요

				// Text 전체 렌더링
				if (outText.empty() == false)
				{
					shapeFontRendererContext.setTextColor(textBoxParam._fontColor);
					shapeFontRendererContext.drawDynamicText(outText.c_str(), textRenderPosition + textRenderOffset, fs::RenderingBase::TextRenderDirectionHorz::Rightward, fs::RenderingBase::TextRenderDirectionVert::Centered);
				}

				// Caret 렌더링
				if (isFocused == true && caretState == 0)
				{
					const float caretHeight = _fontSize;
					const fs::Float2& p0 = fs::Float2(controlLeftCenterPosition._x + textWidthTillCaret - textDisplayOffset + textRenderOffset._x, controlLeftCenterPosition._y - caretHeight * 0.5f);
					shapeFontRendererContext.setColor(fs::RenderingBase::Color::kBlack);
					shapeFontRendererContext.drawLine(p0, p0 + fs::Float2(0.0f, caretHeight), 2.0f);
				}
			}

			// Selection 렌더링
			if (isFocused == true)
			{
				const uint16 selectionStart = controlData._controlValue.getSelectionStart();
				const uint16 selectionLength = controlData._controlValue.getSelectionLength();
				const uint16 selectionEnd = selectionStart + selectionLength;
				if (0 < selectionLength)
				{
					const float textWidthTillSelectionStart = shapeFontRendererContext.calculateTextWidth(outText.c_str(), selectionStart);
					const float textWidthTillSelectionEnd = shapeFontRendererContext.calculateTextWidth(outText.c_str(), selectionEnd);
					const float textWidthSelection = textWidthTillSelectionEnd - textWidthTillSelectionStart;

					const fs::Float4 selectionRenderPosition = fs::Float4(controlLeftCenterPosition._x - textDisplayOffset + textWidthTillSelectionStart + textWidthSelection * 0.5f, controlLeftCenterPosition._y, 0, 0);
					shapeFontRendererContext.setPosition(selectionRenderPosition + textRenderOffset);
					shapeFontRendererContext.setColor(getNamedColor(NamedColor::HighlightColor).addedRgb(-0.375f).scaledA(0.25f));
					shapeFontRendererContext.drawRectangle(fs::Float2(textWidthSelection, _fontSize * 1.25f), 0.0f, 0.0f);
				}
			}

			return false;
		}

		const bool GuiContext::beginListView(const wchar_t* const name, int16& outSelectedListItemIndex, const ListViewParam& listViewParam)
		{
			static constexpr ControlType controlType = ControlType::ListView;
			
			ControlData& controlData = getControlData(name, controlType);
			controlData._isFocusable = false;

			PrepareControlDataParam prepareControlDataParam;
			{
				prepareControlDataParam._initialDisplaySize._x = 160.0f;
				prepareControlDataParam._initialDisplaySize._y = 100.0f;
			}
			prepareControlData(controlData, prepareControlDataParam);
			
			const uint32 previousChildCount = static_cast<uint32>(controlData.getPreviousChildControlDataHashKeyArray().size());
			if (previousChildCount == 0)
			{
				controlData._controlValue.setSelectedItemIndex(-1);
			}
			outSelectedListItemIndex = controlData._controlValue.getSelectedItemIndex();

			fs::RenderingBase::Color color = getNamedColor(NamedColor::LightFont);
			processShowOnlyControl(controlData, color, true);
			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			//const bool isDescendantFocused_ = isDescendantFocused(controlData);
			fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused_ == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;

			const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);

			// Viewport & Scissor rectangle for me
			{
				controlData.setViewportIndexXXX(static_cast<uint32>(_viewportArrayPerFrame.size()));

				D3D11_RECT scissorRectangleForMe;
				scissorRectangleForMe.left = static_cast<LONG>(controlData._position._x);
				scissorRectangleForMe.top = static_cast<LONG>(controlData._position._y);
				scissorRectangleForMe.right = static_cast<LONG>(scissorRectangleForMe.left + controlData._displaySize._x);
				scissorRectangleForMe.bottom = static_cast<LONG>(scissorRectangleForMe.top + controlData._displaySize._y);

				const ControlData& parentControlData = getControlData(controlData.getParentHashKey());
				{
					const D3D11_RECT& parentScissorRectangle = _scissorRectangleArrayPerFrame[parentControlData.getViewportIndexForChildren()];
					constraintInnerRect(scissorRectangleForMe, parentScissorRectangle);
				}

				_scissorRectangleArrayPerFrame.emplace_back(scissorRectangleForMe);
				_viewportArrayPerFrame.emplace_back(_viewportFullScreen);
			}

			// Viewport & Scissor rectangle for children
			{
				const float halfRoundnessInPixel = kDefaultRoundnessInPixel * 0.5f;
				const float quarterRoundnessInPixel = halfRoundnessInPixel * 0.5f;

				controlData.setViewportIndexForChildrenXXX(static_cast<uint32>(_viewportArrayPerFrame.size()));

				D3D11_RECT scissorRectangleForMe;
				scissorRectangleForMe.left = static_cast<LONG>(controlData._position._x + quarterRoundnessInPixel);
				scissorRectangleForMe.top = static_cast<LONG>(controlData._position._y);
				scissorRectangleForMe.right = static_cast<LONG>(scissorRectangleForMe.left + controlData._displaySize._x - halfRoundnessInPixel);
				scissorRectangleForMe.bottom = static_cast<LONG>(scissorRectangleForMe.top + controlData._displaySize._y);

				const ControlData& parentControlData = getControlData(controlData.getParentHashKey());
				{
					const D3D11_RECT& parentScissorRectangle = _scissorRectangleArrayPerFrame[parentControlData.getViewportIndexForChildren()];
					constraintInnerRect(scissorRectangleForMe, parentScissorRectangle);
				}

				_scissorRectangleArrayPerFrame.emplace_back(scissorRectangleForMe);
				_viewportArrayPerFrame.emplace_back(_viewportFullScreen);
			}

			// Background 렌더링
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setColor(color);
			shapeFontRendererContext.setPosition(controlCenterPosition);
			shapeFontRendererContext.drawRoundedRectangle(controlData._displaySize, (kDefaultRoundnessInPixel / controlData._displaySize.minElement()), 0.0f, 0.0f);
			
			_controlStackPerFrame.emplace_back(ControlStackData(controlData));

			if (listViewParam._useScrollBar == true)
			{
				controlData._controlValue.setCurrentScrollBarType(fs::Gui::ScrollBarType::Vert);
			}
			else
			{
				controlData._controlValue.setCurrentScrollBarType(fs::Gui::ScrollBarType::None);
			}

			return true;
		}

		void GuiContext::endListView()
		{
			ControlData& controlData = getControlDataStackTopXXX();
			const fs::Gui::ScrollBarType scrollBarType = controlData._controlValue.getCurrentScrollBarType();
			if (scrollBarType == fs::Gui::ScrollBarType::Vert)
			{
				pushScrollBar(fs::Gui::ScrollBarType::Vert);
			}

			endControlInternal(ControlType::ListView);
		}

		void GuiContext::pushListItem(const wchar_t* const text)
		{
			static constexpr ControlType controlType = ControlType::ListItem;
			
			ControlData& controlData = getControlData(text, controlType);
			controlData._isFocusable = false;

			PrepareControlDataParam prepareControlDataParam;
			{
				prepareControlDataParam._initialDisplaySize._x = 160.0f;
				prepareControlDataParam._initialDisplaySize._y = _fontSize + 12.0f;
				prepareControlDataParam._innerPadding.left(prepareControlDataParam._initialDisplaySize._y * 0.25f);
				prepareControlDataParam._noIntervalForNextSibling = true;
				prepareControlDataParam._viewportUsage = fs::Gui::ViewportUsage::Child;
			}
			prepareControlData(controlData, prepareControlDataParam);

			ControlData& parentControlData = getControlData(controlData.getParentHashKey());
			const int16 parentSelectedItemIndex = parentControlData._controlValue.getSelectedItemIndex();
			const int16 myIndex = static_cast<int16>(parentControlData.getChildControlDataHashKeyArray().size() - 1);
			fs::RenderingBase::Color inputColor;
			if (parentSelectedItemIndex == myIndex)
			{
				inputColor = getNamedColor(NamedColor::HighlightColor);
			}
			else
			{
				inputColor = getNamedColor(NamedColor::LightFont);
			}

			fs::RenderingBase::Color finalColor;
			const bool isClicked = processClickControl(controlData, inputColor, inputColor, inputColor, finalColor);
			if (isClicked == true)
			{
				parentControlData._controlValue.setSelectedItemIndex(myIndex);
			}
			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused_ == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;

			const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);

			// Background 렌더링
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setColor(finalColor);
			shapeFontRendererContext.setPosition(controlCenterPosition);
			shapeFontRendererContext.drawRoundedRectangle(controlData._displaySize, (kDefaultRoundnessInPixel / controlData._displaySize.minElement()), 0.0f, 0.0f);

			const fs::Float2& controlLeftCenterPosition = getControlLeftCenterPosition(controlData);
			shapeFontRendererContext.setTextColor(getNamedColor(NamedColor::DarkFont));
			shapeFontRendererContext.drawDynamicText(text, fs::Float4(controlLeftCenterPosition._x + controlData.getInnerPadding().left(), controlLeftCenterPosition._y, 0, 0),
				fs::RenderingBase::TextRenderDirectionHorz::Rightward, fs::RenderingBase::TextRenderDirectionVert::Centered);
		}

		const bool GuiContext::beginMenuBar(const wchar_t* const name)
		{
			static constexpr ControlType controlType = ControlType::MenuBar;

			ControlData& controlData = getControlData(name, controlType);
			ControlData& parentControlData = getControlData(controlData.getParentHashKey());
			const ControlType parentControlType = parentControlData.getControlType();
			const bool isParentControlRoot = (parentControlType == ControlType::ROOT);
			const bool isParentControlWindow = (parentControlType == ControlType::Window);
			if (isParentControlRoot == false && isParentControlWindow == false)
			{
				FS_LOG_ERROR("김장원", "MenuBar 는 Window 나 Root 컨트롤의 자식으로만 사용할 수 있습니다!");
				return false;
			}
			parentControlData._controlValue.setCurrentMenuBarType(MenuBarType::Top);

			PrepareControlDataParam prepareControlDataParam;
			{
				prepareControlDataParam._alwaysResetDisplaySize = true;
				prepareControlDataParam._initialDisplaySize._x = parentControlData._displaySize._x;
				prepareControlDataParam._initialDisplaySize._y = kMenuBarBaseSize._y;
				prepareControlDataParam._desiredPositionInParent._x = 0.0f;
				prepareControlDataParam._desiredPositionInParent._y = (isParentControlWindow == true) ? kTitleBarBaseSize._y : 0.0f;
				prepareControlDataParam._viewportUsage = ViewportUsage::Parent;
			}
			nextNoAutoPositioned();
			prepareControlData(controlData, prepareControlDataParam);

			if (_pressedControlHashKey != 0 && isDescendantInclusive(controlData, _pressedControlHashKey) == false
				&& isInControlInternal(_mousePosition, controlData._position, fs::Float2::kZero, fs::Float2(controlData._controlValue.getItemSizeX(), controlData.getInteractionSize()._y)) == false)
			{
				controlData._controlValue.setIsToggled(false);
			}
			controlData._controlValue.setItemSizeX(0.0f);

			const bool isToggled = controlData._controlValue.getIsToggled();
			const uint32 previousChildCount = static_cast<uint32>(controlData.getPreviousChildControlDataHashKeyArray().size());
			if (previousChildCount == 0 || isToggled == false)
			{
				controlData._controlValue.setSelectedItemIndex(-1);
			}

			fs::RenderingBase::Color color = getNamedColor(NamedColor::NormalState);
			processShowOnlyControl(controlData, color, true);
			if (isParentControlRoot == true)
			{
				color.a(1.0f);
			}

			fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext = _shapeFontRendererContextTopMost;

			const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);

			// Background 렌더링
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setColor(color);
			shapeFontRendererContext.setPosition(controlCenterPosition);
			shapeFontRendererContext.drawRoundedRectangle(controlData._displaySize, 0.0f, 0.0f, 0.0f);

			_controlStackPerFrame.emplace_back(ControlStackData(controlData));

			return true;
		}

		const bool GuiContext::beginMenuBarItem(const wchar_t* const text)
		{
			static constexpr ControlType controlType = ControlType::MenuBarItem;

			ControlData& controlData = getControlData(text, controlType);
			ControlData& parentControlData = getControlData(controlData.getParentHashKey());
			const ControlType parentControlType = parentControlData.getControlType();
			if (parentControlType != ControlType::MenuBar)
			{
				FS_LOG_ERROR("김장원", "MenuBarItem 은 MenuBar 컨트롤의 자식으로만 사용할 수 있습니다!");
				return false;
			}
			parentControlData._controlValue.setCurrentMenuBarType(MenuBarType::Top);

			const uint32 textLength = fs::StringUtil::wcslen(text);
			const float textWidth = _shapeFontRendererContextTopMost.calculateTextWidth(text, textLength);

			PrepareControlDataParam prepareControlDataParam;
			{
				prepareControlDataParam._initialDisplaySize._x = textWidth + kMenuBarItemTextSpace;
				prepareControlDataParam._initialDisplaySize._y = kMenuBarBaseSize._y;
				prepareControlDataParam._desiredPositionInParent._x = parentControlData._controlValue.getItemSizeX();
				prepareControlDataParam._desiredPositionInParent._y = 0.0f;
				prepareControlDataParam._viewportUsage = ViewportUsage::Parent;
			}
			nextNoAutoPositioned();
			prepareControlData(controlData, prepareControlDataParam);
			parentControlData._controlValue.addItemSizeX(controlData._displaySize._x);
			controlData._controlValue.setItemSizeY(0.0f);

			const int16 parentSelectedItemIndex = parentControlData._controlValue.getSelectedItemIndex();
			const int16 myIndex = static_cast<int16>(parentControlData.getChildControlDataHashKeyArray().size() - 1);
			const fs::RenderingBase::Color& normalColor = (parentSelectedItemIndex == myIndex) ? getNamedColor(NamedColor::PressedState) : getNamedColor(NamedColor::NormalState);
			const fs::RenderingBase::Color& hoverColor = (parentSelectedItemIndex == myIndex) ? getNamedColor(NamedColor::PressedState) : getNamedColor(NamedColor::HoverState);
			const fs::RenderingBase::Color& pressedColor = (parentSelectedItemIndex == myIndex) ? getNamedColor(NamedColor::PressedState) : getNamedColor(NamedColor::PressedState);
			fs::RenderingBase::Color finalColor;
			const bool isClicked = processClickControl(controlData, normalColor, hoverColor, pressedColor, finalColor);
			const bool& isParentControlToggled = parentControlData._controlValue.getIsToggled();
			if (isClicked == true)
			{	
				parentControlData._controlValue.setIsToggled(!isParentControlToggled);
				parentControlData._controlValue.setSelectedItemIndex((isParentControlToggled == true) ? myIndex : -1);
			}

			if (isControlHovered(controlData) == true && isParentControlToggled == true)
			{
				parentControlData._controlValue.setSelectedItemIndex(myIndex);
			}
			const bool isMeSelected = (parentControlData._controlValue.getSelectedItemIndex() == myIndex);
			
			fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext = _shapeFontRendererContextTopMost;

			const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);

			// Background 렌더링
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setColor(finalColor);
			shapeFontRendererContext.setPosition(controlCenterPosition);
			shapeFontRendererContext.drawRoundedRectangle(controlData._displaySize, 0.0f, 0.0f, 0.0f);

			// Text 렌더링
			const fs::Float2& controlLeftCenterPosition = getControlLeftCenterPosition(controlData);
			shapeFontRendererContext.setTextColor(getNamedColor(NamedColor::LightFont));
			shapeFontRendererContext.drawDynamicText(text, fs::Float4(controlLeftCenterPosition._x + controlData.getInnerPadding().left() + controlData._displaySize._x * 0.5f,
				controlLeftCenterPosition._y, 0, 0), fs::RenderingBase::TextRenderDirectionHorz::Centered, fs::RenderingBase::TextRenderDirectionVert::Centered);

			const bool result = (isClicked || isMeSelected);
			if (result == true)
			{
				_controlStackPerFrame.emplace_back(ControlStackData(controlData));
			}
			return result;
		}

		const bool GuiContext::beginMenuItem(const wchar_t* const text)
		{
			static constexpr ControlType controlType = ControlType::MenuItem;

			ControlData& controlData = getControlData(text, controlType);
			controlData._isInteractableOutsideParent = true;
			
			ControlData& parentControlData = getControlData(controlData.getParentHashKey());
			const ControlType parentControlType = parentControlData.getControlType();
			const bool isParentControlMenuItem = (parentControlType == ControlType::MenuItem);
			if (parentControlType != ControlType::MenuBarItem && isParentControlMenuItem == false)
			{
				FS_LOG_ERROR("김장원", "MenuItem 은 MenuBarItem 이나 MenuItem 컨트롤의 자식으로만 사용할 수 있습니다!");
				return false;
			}

			const uint32 textLength = fs::StringUtil::wcslen(text);
			const float textWidth = _shapeFontRendererContextTopMost.calculateTextWidth(text, textLength);

			PrepareControlDataParam prepareControlDataParam;
			{
				prepareControlDataParam._alwaysResetDisplaySize = true;
				prepareControlDataParam._initialDisplaySize._x = parentControlData._controlValue.getItemSizeX();
				prepareControlDataParam._initialDisplaySize._y = kMenuBarBaseSize._y;
				prepareControlDataParam._innerPadding.left(kMenuItemSpaceLeft);
				prepareControlDataParam._desiredPositionInParent._x = (isParentControlMenuItem == true) ? parentControlData._displaySize._x : 0.0f;
				prepareControlDataParam._desiredPositionInParent._y = parentControlData._controlValue.getItemSizeY() + ((isParentControlMenuItem == true) ? 0.0f : prepareControlDataParam._initialDisplaySize._y);
			}
			nextNoAutoPositioned();
			nextControlSizeNonContrainedToParent();
			prepareControlData(controlData, prepareControlDataParam);
			parentControlData._controlValue.setItemSizeX(fs::max(parentControlData._controlValue.getItemSizeX(), textWidth + kMenuItemSpaceRight));
			parentControlData._controlValue.addItemSizeY(controlData._displaySize._y);
			controlData._controlValue.setItemSizeY(0.0f);

			const bool isDescendantHovered_ = isDescendantHovered(controlData);
			const fs::RenderingBase::Color& normalColor = getNamedColor((isDescendantHovered_ == true) ? NamedColor::HoverState : NamedColor::NormalState);
			const fs::RenderingBase::Color& hoverColor = getNamedColor(NamedColor::HoverState);
			const fs::RenderingBase::Color& pressedColor = getNamedColor(NamedColor::PressedState);
			fs::RenderingBase::Color finalColor;
			const bool isClicked = processClickControl(controlData, normalColor, hoverColor, pressedColor, finalColor);
			const bool isHovered = isControlHovered(controlData);
			const bool& isToggled = controlData._controlValue.getIsToggled();
			const uint64 internalTimeMs = controlData._controlValue.getInternalTimeMs();
			const int16 myIndex = static_cast<int16>(parentControlData.getChildControlDataHashKeyArray().size() - 1);
			if (isHovered == true)
			{
				parentControlData._controlValue.setSelectedItemIndex(myIndex);
			}
			else if (isHovered == false && isDescendantHovered_  == false && isToggled == true)
			{
				parentControlData._controlValue.setSelectedItemIndex(-1);
			}
			controlData._controlValue.setIsToggled((parentControlData._controlValue.getSelectedItemIndex() == myIndex));

			fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext = _shapeFontRendererContextTopMost;
			const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);
			
			// Background 렌더링
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setColor(finalColor);
			shapeFontRendererContext.setPosition(controlCenterPosition);
			shapeFontRendererContext.drawRoundedRectangle(controlData._displaySize, 0.0f, 0.0f, 0.0f);

			// 삼각형 렌더링
			const uint16 previousMaxChildCount = controlData.getPreviousMaxChildControlCount();
			if (0 < previousMaxChildCount)
			{
				const fs::Float2& controlRightCenterPosition = getControlRightCenterPosition(controlData);
				fs::Float2 a = controlRightCenterPosition + fs::Float2(-14, -5);
				fs::Float2 b = controlRightCenterPosition + fs::Float2( -4,  0);
				fs::Float2 c = controlRightCenterPosition + fs::Float2(-14, +5);
				shapeFontRendererContext.setColor(getNamedColor((isToggled == true) ? NamedColor::HighlightColor : NamedColor::LightFont));
				shapeFontRendererContext.drawSolidTriangle(a, b, c);
			}

			// Text 렌더링
			const fs::Float2& controlLeftCenterPosition = getControlLeftCenterPosition(controlData);
			shapeFontRendererContext.setTextColor(getNamedColor(NamedColor::LightFont));
			shapeFontRendererContext.drawDynamicText(text, fs::Float4(controlLeftCenterPosition._x + controlData.getInnerPadding().left(),
				controlLeftCenterPosition._y, 0, 0), fs::RenderingBase::TextRenderDirectionHorz::Rightward, fs::RenderingBase::TextRenderDirectionVert::Centered);

			// (previousMaxChildCount) 최초 업데이트 시 Child 가 다 등록되어야 하므로 controlData._updateCount 를 이용한다.
			const bool result = (isToggled || isClicked || controlData._updateCount <= 1);
			if (result == true)
			{
				_controlStackPerFrame.emplace_back(ControlStackData(controlData));
			}
			return result;
		}

		void GuiContext::pushScrollBar(const ScrollBarType scrollBarType)
		{
			static constexpr ControlType trackControlType = ControlType::ScrollBar;
			static std::function fnCalculatePureWindowWidth = [this](const ControlData& parentControlData, const ScrollBarType& scrollBarState)
			{
				const fs::Float2& menuBarThicknes = parentControlData.getMenuBarThickness();
				return fs::max(
					0.0f, 
					parentControlData._displaySize._x - parentControlData.getHorzDockSizeSum() - parentControlData.getInnerPadding().horz() 
						- ((scrollBarState == ScrollBarType::Both || scrollBarState == ScrollBarType::Vert) ? kScrollBarThickness * 2.0f : 0.0f) - menuBarThicknes._x
				);
			};
			static std::function fnCalculatePureWindowHeight = [this](const ControlData& parentControlData, const ScrollBarType& scrollBarState)
			{
				const float titleBarHeight = (fs::Gui::ControlType::Window == parentControlData.getControlType()) ? kTitleBarBaseSize._y : 0.0f;
				const fs::Float2& menuBarThicknes = parentControlData.getMenuBarThickness();
				return fs::max(
					0.0f, 
					parentControlData._displaySize._y - parentControlData.getVertDockSizeSum() - titleBarHeight - parentControlData.getInnerPadding().vert() 
						- ((scrollBarState == ScrollBarType::Both || scrollBarState == ScrollBarType::Horz) ? kScrollBarThickness * 2.0f : 0.0f) - menuBarThicknes._y
				);
			};

			ControlData& parentControlData = getControlDataStackTopXXX();
			const ControlType parentControlType = parentControlData.getControlType();
			const ScrollBarType& parentControlCurrentScrollBarType = parentControlData._controlValue.getCurrentScrollBarType();
			const bool isAncestorFocused = isAncestorFocusedInclusiveXXX(parentControlData);
			const fs::Float2& parentControlPreviousContentAreaSize = parentControlData.getPreviousContentAreaSize();
			const fs::Float2& menuBarThicknes = parentControlData.getMenuBarThickness();

			// Vertical Track
			if (scrollBarType == ScrollBarType::Vert || scrollBarType == ScrollBarType::Both)
			{
				ControlData& trackControlData = getControlData(generateControlKeyString(parentControlData, L"ScrollBarVertTrack", trackControlType), trackControlType);

				PrepareControlDataParam prepareControlDataParamForTrack;
				{
					prepareControlDataParamForTrack._initialDisplaySize._x = kScrollBarThickness;
					prepareControlDataParamForTrack._initialDisplaySize._y = fnCalculatePureWindowHeight(parentControlData, parentControlCurrentScrollBarType);

					const float titleBarOffsetX = (fs::Gui::ControlType::Window == parentControlType) ? kHalfBorderThickness * 2.0f : kScrollBarThickness * 0.5f;
					const float titleBarHeight = (fs::Gui::ControlType::Window == parentControlType) ? kTitleBarBaseSize._y : 0.0f;
					prepareControlDataParamForTrack._desiredPositionInParent._x = parentControlData._displaySize._x - titleBarOffsetX;
					prepareControlDataParamForTrack._desiredPositionInParent._y = titleBarHeight + parentControlData.getInnerPadding().top() + menuBarThicknes._y;
					if (parentControlData.isDockHosting() == true)
					{
						const DockDatum& dockDatumRightSide = parentControlData.getDockDatum(DockingMethod::RightSide);
						if (dockDatumRightSide.hasDockedControls() == true)
						{
							prepareControlDataParamForTrack._desiredPositionInParent._x -= parentControlData.getDockSize(DockingMethod::RightSide)._x;
						}

						const DockDatum& dockDatumTopSide = parentControlData.getDockDatum(DockingMethod::TopSide);
						if (dockDatumTopSide.hasDockedControls() == true)
						{
							prepareControlDataParamForTrack._desiredPositionInParent._y += parentControlData.getDockSize(DockingMethod::TopSide)._y;
						}
					}

					prepareControlDataParamForTrack._parentHashKeyOverride = parentControlData.getHashKey();
					prepareControlDataParamForTrack._alwaysResetDisplaySize = true;
					prepareControlDataParamForTrack._alwaysResetPosition = true;
					prepareControlDataParamForTrack._ignoreMeForContentAreaSize = true;
					prepareControlDataParamForTrack._viewportUsage = ViewportUsage::Parent;
				}
				nextNoAutoPositioned();
				prepareControlData(trackControlData, prepareControlDataParamForTrack);

				fs::RenderingBase::Color trackColor = getNamedColor(NamedColor::ScrollBarTrack);
				processShowOnlyControl(trackControlData, trackColor, true);

				const float parentWindowPureDisplayHeight = fnCalculatePureWindowHeight(parentControlData, parentControlCurrentScrollBarType);
				const float extraSize = parentControlPreviousContentAreaSize._y - parentWindowPureDisplayHeight;
				if (0.0f <= extraSize)
				{
					// Update parent control current scrollbar type
					if (parentControlCurrentScrollBarType != ScrollBarType::Both)
					{
						parentControlData._controlValue.setCurrentScrollBarType((parentControlCurrentScrollBarType == ScrollBarType::Horz) ? ScrollBarType::Both : ScrollBarType::Vert);
					}

					// Rendering track
					const float radius = kScrollBarThickness * 0.5f;
					fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;
					{
						const float rectLength = trackControlData._displaySize._y - radius * 2.0f;
						shapeFontRendererContext.setViewportIndex(trackControlData.getViewportIndex());
						shapeFontRendererContext.setColor(trackColor);

						fs::Float4 trackRenderPosition = fs::Float4(trackControlData._position._x, trackControlData._position._y + radius, 0.0f, 1.0f);

						// Upper half circle
						shapeFontRendererContext.setPosition(trackRenderPosition);
						shapeFontRendererContext.drawHalfCircle(radius, 0.0f);

						// Rect
						if (0.0f < rectLength)
						{
							trackRenderPosition._y += rectLength * 0.5f;
							shapeFontRendererContext.setPosition(trackRenderPosition);
							shapeFontRendererContext.drawRectangle(trackControlData._displaySize - fs::Float2(0.0f, radius * 2.0f), 0.0f, 0.0f);
						}

						// Lower half circle
						if (0.0f < rectLength)
						{
							trackRenderPosition._y += rectLength * 0.5f;
						}
						shapeFontRendererContext.setPosition(trackRenderPosition);
						shapeFontRendererContext.drawHalfCircle(radius, fs::Math::kPi);
					}


					// Thumb
					const float thumbSizeRatio = (parentWindowPureDisplayHeight / parentControlPreviousContentAreaSize._y);
					const float thumbSize = parentWindowPureDisplayHeight * thumbSizeRatio - radius * 2.0f;
					{
						static constexpr ControlType thumbControlType = ControlType::ScrollBarThumb;

						const float trackRemnantSize = std::abs(trackControlData._displaySize._y - thumbSize);
						ControlData& thumbControlData = getControlData(generateControlKeyString(parentControlData, L"ScrollBarVertThumb", thumbControlType), thumbControlType);

						PrepareControlDataParam prepareControlDataParamForThumb;
						{
							prepareControlDataParamForThumb._initialDisplaySize._x = kScrollBarThickness;
							prepareControlDataParamForThumb._initialDisplaySize._y = thumbSize;
							
							prepareControlDataParamForThumb._desiredPositionInParent._x = prepareControlDataParamForTrack._desiredPositionInParent._x - kScrollBarThickness * 0.5f;
							prepareControlDataParamForThumb._desiredPositionInParent._y = prepareControlDataParamForTrack._desiredPositionInParent._y;

							prepareControlDataParamForThumb._parentHashKeyOverride = parentControlData.getHashKey();
							prepareControlDataParamForThumb._alwaysResetDisplaySize = true;
							prepareControlDataParamForThumb._ignoreMeForContentAreaSize = true;
							prepareControlDataParamForThumb._viewportUsage = ViewportUsage::Parent;

							thumbControlData._isDraggable = true;
							thumbControlData._draggingConstraints.left(parentControlData._position._x + prepareControlDataParamForThumb._desiredPositionInParent._x);
							thumbControlData._draggingConstraints.right(thumbControlData._draggingConstraints.left());
							thumbControlData._draggingConstraints.top(trackControlData._position._y);
							thumbControlData._draggingConstraints.bottom(trackControlData._position._y + trackRemnantSize);
						}
						nextNoAutoPositioned();
						prepareControlData(thumbControlData, prepareControlDataParamForThumb);

						// @중요
						// Calculate position from internal value
						thumbControlData._position._y = parentControlData._position._y + prepareControlDataParamForTrack._desiredPositionInParent._y + (thumbControlData._controlValue.getThumbAt() * trackRemnantSize);

						fs::RenderingBase::Color thumbColor;
						processScrollableControl(thumbControlData, getNamedColor(NamedColor::ScrollBarThumb), getNamedColor(NamedColor::ScrollBarThumb).scaledRgb(1.25f), thumbColor);

						const float mouseWheelScroll = getMouseWheelScroll(parentControlData);
						const float thumbAtRatio = (trackRemnantSize < 1.0f) ? 0.0f : fs::Math::saturate((thumbControlData._position._y - thumbControlData._draggingConstraints.top() + mouseWheelScroll) / trackRemnantSize);
						thumbControlData._controlValue.setThumbAt(thumbAtRatio);
						parentControlData._displayOffset._y = -thumbAtRatio * (parentControlPreviousContentAreaSize._y - trackControlData._displaySize._y); // Scrolling!

						// Rendering thumb
						{
							const float rectLength = thumbSize - radius * 2.0f;
							shapeFontRendererContext.setViewportIndex(thumbControlData.getViewportIndex());
							shapeFontRendererContext.setColor(thumbColor);

							fs::Float4 thumbRenderPosition = fs::Float4(thumbControlData._position._x + radius, thumbControlData._position._y + radius, 0.0f, 1.0f);
							
							// Upper half circle
							shapeFontRendererContext.setPosition(thumbRenderPosition);
							shapeFontRendererContext.drawHalfCircle(radius, 0.0f);

							// Rect
							if (0.0f < rectLength)
							{
								thumbRenderPosition._y += rectLength * 0.5f;
								shapeFontRendererContext.setPosition(thumbRenderPosition);
								shapeFontRendererContext.drawRectangle(thumbControlData._displaySize - fs::Float2(0.0f, radius * 2.0f), 0.0f, 0.0f);
							}

							// Lower half circle
							if (0.0f < rectLength)
							{
								thumbRenderPosition._y += rectLength * 0.5f;
							}
							shapeFontRendererContext.setPosition(thumbRenderPosition);
							shapeFontRendererContext.drawHalfCircle(radius, fs::Math::kPi);
						}
					}
				}
				else
				{
					parentControlData._controlValue.setCurrentScrollBarType((parentControlCurrentScrollBarType == ScrollBarType::Vert || parentControlCurrentScrollBarType == ScrollBarType::None) ? ScrollBarType::None : ScrollBarType::Horz);
				}
			}

			// Horizontal Track
			if (scrollBarType == ScrollBarType::Horz || scrollBarType == ScrollBarType::Both)
			{
				ControlData& trackControlData = getControlData(generateControlKeyString(parentControlData, L"ScrollBarHorzTrack", trackControlType), trackControlType);
				
				PrepareControlDataParam prepareControlDataParamForTrack;
				{
					prepareControlDataParamForTrack._initialDisplaySize._x = fnCalculatePureWindowWidth(parentControlData, parentControlCurrentScrollBarType);
					prepareControlDataParamForTrack._initialDisplaySize._y = kScrollBarThickness;

					prepareControlDataParamForTrack._desiredPositionInParent._x = parentControlData.getInnerPadding().left() + menuBarThicknes._x;
					prepareControlDataParamForTrack._desiredPositionInParent._y = parentControlData._displaySize._y - kHalfBorderThickness * 2.0f;
					if (parentControlData.isDockHosting() == true)
					{
						const DockDatum& dockDatumLeftSide = parentControlData.getDockDatum(DockingMethod::LeftSide);
						if (dockDatumLeftSide.hasDockedControls() == true)
						{
							prepareControlDataParamForTrack._desiredPositionInParent._x += parentControlData.getDockSize(DockingMethod::LeftSide)._x;
						}

						const DockDatum& dockDatumBottomSide = parentControlData.getDockDatum(DockingMethod::BottomSide);
						if (dockDatumBottomSide.hasDockedControls() == true)
						{
							prepareControlDataParamForTrack._desiredPositionInParent._y -= parentControlData.getDockSize(DockingMethod::BottomSide)._y;
						}
					}
					
					prepareControlDataParamForTrack._parentHashKeyOverride = parentControlData.getHashKey();
					prepareControlDataParamForTrack._alwaysResetDisplaySize = true;
					prepareControlDataParamForTrack._alwaysResetPosition = true;
					prepareControlDataParamForTrack._ignoreMeForContentAreaSize = true;
					prepareControlDataParamForTrack._viewportUsage = ViewportUsage::Parent;
				}
				nextNoAutoPositioned();
				prepareControlData(trackControlData, prepareControlDataParamForTrack);

				fs::RenderingBase::Color trackColor = getNamedColor(NamedColor::ScrollBarTrack);
				processShowOnlyControl(trackControlData, trackColor, true);

				const float parentWindowPureDisplayWidth = fnCalculatePureWindowWidth(parentControlData, parentControlCurrentScrollBarType);
				const float extraSize = parentControlPreviousContentAreaSize._x - parentWindowPureDisplayWidth;
				if (0.0f <= extraSize)
				{
					// Update parent window scrollbar state
					if (parentControlCurrentScrollBarType != ScrollBarType::Both)
					{
						parentControlData._controlValue.setCurrentScrollBarType((parentControlCurrentScrollBarType == ScrollBarType::Vert) ? ScrollBarType::Both : ScrollBarType::Horz);
					}

					// Rendering track
					const float radius = kScrollBarThickness * 0.5f;
					fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;
					{
						const float rectLength = trackControlData._displaySize._x - radius * 2.0f;
						shapeFontRendererContext.setViewportIndex(trackControlData.getViewportIndex());
						shapeFontRendererContext.setColor(trackColor);

						fs::Float4 trackRenderPosition = fs::Float4(trackControlData._position._x + radius, trackControlData._position._y, 0.0f, 1.0f);

						// Left half circle
						shapeFontRendererContext.setPosition(trackRenderPosition);
						shapeFontRendererContext.drawHalfCircle(radius, +fs::Math::kPiOverTwo);

						// Rect
						if (0.0f < rectLength)
						{
							trackRenderPosition._x += rectLength * 0.5f;
							shapeFontRendererContext.setPosition(trackRenderPosition);
							shapeFontRendererContext.drawRectangle(trackControlData._displaySize - fs::Float2(radius * 2.0f, 0.0f), 0.0f, 0.0f);
						}

						// Right half circle
						if (0.0f < rectLength)
						{
							trackRenderPosition._x += rectLength * 0.5f;
						}
						shapeFontRendererContext.setPosition(trackRenderPosition);
						shapeFontRendererContext.drawHalfCircle(radius, -fs::Math::kPiOverTwo);
					}


					// Thumb
					const float thumbSizeRatio = (parentWindowPureDisplayWidth / parentControlPreviousContentAreaSize._x);
					const float thumbSize = parentWindowPureDisplayWidth * thumbSizeRatio - radius * 2.0f;
					{
						static constexpr ControlType thumbControlType = ControlType::ScrollBarThumb;

						ControlData& thumbControlData = getControlData(generateControlKeyString(parentControlData, L"ScrollBarHorzThumb", thumbControlType), thumbControlType);
						const float trackRemnantSize = std::abs(trackControlData._displaySize._x - thumbSize);

						PrepareControlDataParam prepareControlDataParamForThumb;
						{
							prepareControlDataParamForThumb._initialDisplaySize._x = thumbSize;
							prepareControlDataParamForThumb._initialDisplaySize._y = kScrollBarThickness;

							prepareControlDataParamForThumb._desiredPositionInParent._x = prepareControlDataParamForTrack._desiredPositionInParent._x;
							prepareControlDataParamForThumb._desiredPositionInParent._y = prepareControlDataParamForTrack._desiredPositionInParent._y - kScrollBarThickness * 0.5f;
							
							prepareControlDataParamForThumb._parentHashKeyOverride = parentControlData.getHashKey();
							prepareControlDataParamForThumb._alwaysResetDisplaySize = true;
							prepareControlDataParamForThumb._alwaysResetPosition = false; // 중요!
							prepareControlDataParamForThumb._ignoreMeForContentAreaSize = true;
							prepareControlDataParamForThumb._viewportUsage = ViewportUsage::Parent;

							thumbControlData._isDraggable = true;
							thumbControlData._draggingConstraints.left(trackControlData._position._x);
							thumbControlData._draggingConstraints.right(trackControlData._position._x + trackRemnantSize);
							thumbControlData._draggingConstraints.top(parentControlData._position._y + prepareControlDataParamForThumb._desiredPositionInParent._y);
							thumbControlData._draggingConstraints.bottom(thumbControlData._draggingConstraints.top());
						}
						nextNoAutoPositioned();
						prepareControlData(thumbControlData, prepareControlDataParamForThumb);

						// @중요
						// Calculate position from internal value
						thumbControlData._position._x = parentControlData._position._x + prepareControlDataParamForTrack._desiredPositionInParent._x + (thumbControlData._controlValue.getThumbAt() * trackRemnantSize);

						fs::RenderingBase::Color thumbColor;
						processScrollableControl(thumbControlData, getNamedColor(NamedColor::ScrollBarThumb), getNamedColor(NamedColor::ScrollBarThumb).scaledRgb(1.25f), thumbColor);

						const float thumbAtRatio = (trackRemnantSize < 1.0f) ? 0.0f : fs::Math::saturate((thumbControlData._position._x - thumbControlData._draggingConstraints.left()) / trackRemnantSize);
						thumbControlData._controlValue.setThumbAt(thumbAtRatio);
						parentControlData._displayOffset._x = -thumbAtRatio * (parentControlPreviousContentAreaSize._x - trackControlData._displaySize._x + ((scrollBarType == ScrollBarType::Both) ? kScrollBarThickness : 0.0f)); // Scrolling!

						// Rendering thumb
						{
							const float rectLength = thumbSize - radius * 2.0f;
							shapeFontRendererContext.setViewportIndex(thumbControlData.getViewportIndex());
							shapeFontRendererContext.setColor(thumbColor);
							
							fs::Float4 thumbRenderPosition = fs::Float4(thumbControlData._position._x + radius, thumbControlData._position._y + radius, 0.0f, 1.0f);

							// Left half circle
							shapeFontRendererContext.setPosition(thumbRenderPosition);
							shapeFontRendererContext.drawHalfCircle(radius, +fs::Math::kPiOverTwo);

							// Rect
							if (0.0f < rectLength)
							{
								thumbRenderPosition._x += rectLength * 0.5f;
								shapeFontRendererContext.setPosition(thumbRenderPosition);
								shapeFontRendererContext.drawRectangle(thumbControlData._displaySize - fs::Float2(radius * 2.0f, 0.0f), 0.0f, 0.0f);
							}

							// Right half circle
							if (0.0f < rectLength)
							{
								thumbRenderPosition._x += rectLength * 0.5f;
							}
							shapeFontRendererContext.setPosition(thumbRenderPosition);
							shapeFontRendererContext.drawHalfCircle(radius, -fs::Math::kPiOverTwo);
						}
					}
				}
				else
				{
					parentControlData._controlValue.setCurrentScrollBarType((parentControlCurrentScrollBarType == ScrollBarType::Horz || parentControlCurrentScrollBarType == ScrollBarType::None) ? ScrollBarType::None : ScrollBarType::Vert);
				}
			}
		}

		void GuiContext::renderDock(const ControlData& controlData, fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext)
		{
			if (controlData._dockingControlType == DockingControlType::Dock || controlData._dockingControlType == DockingControlType::DockerDock)
			{
				for (DockingMethod dockingMethodIter = static_cast<DockingMethod>(0); dockingMethodIter != DockingMethod::COUNT; dockingMethodIter = static_cast<DockingMethod>(static_cast<uint32>(dockingMethodIter) + 1))
				{
					const DockDatum& dockDatum = controlData.getDockDatum(dockingMethodIter);
					if (dockDatum.hasDockedControls() == true)
					{
						const fs::Float2& dockSize = controlData.getDockSize(dockingMethodIter);
						const fs::Float2& dockPosition = controlData.getDockPosition(dockingMethodIter);

						shapeFontRendererContext.setViewportIndex(controlData.getViewportIndexForDocks());
						
						shapeFontRendererContext.setColor(getNamedColor(NamedColor::Dock));
						shapeFontRendererContext.setPosition(fs::Float4(dockPosition._x + controlData.getDockSize(dockingMethodIter)._x * 0.5f, dockPosition._y + controlData.getDockSize(dockingMethodIter)._y * 0.5f, 0, 0));

						shapeFontRendererContext.drawRectangle(controlData.getDockSize(dockingMethodIter), 0.0f, 0.0f);
					}
				}
			}
		}

		void GuiContext::endControlInternal(const ControlType controlType)
		{
			FS_ASSERT("김장원", _controlStackPerFrame.back()._controlType == controlType, "begin 과 end 의 ControlType 이 다릅니다!!!");
			_controlStackPerFrame.pop_back();
		}

		fs::Float2 GuiContext::beginTitleBar(const wchar_t* const windowTitle, const fs::Float2& titleBarSize, const fs::Rect& innerPadding)
		{
			static constexpr ControlType controlType = ControlType::TitleBar;

			ControlData& controlData = getControlData(windowTitle, controlType);
			controlData._isDraggable = true;
			controlData._delegateHashKey = controlData.getParentHashKey();
			
			ControlData& parentControlData = getControlData(controlData.getParentHashKey());
			const bool isParentControlDocking = parentControlData.isDocking();
			PrepareControlDataParam prepareControlDataParam;
			{
				if (isParentControlDocking == true)
				{
					const ControlData& dockControlData = getControlData(parentControlData.getDockControlHashKey());
					const DockDatum& parentDockDatum = dockControlData.getDockDatum(parentControlData._lastDockingMethod);
					const int32 dockedControlIndex = parentDockDatum.getDockedControlIndex(parentControlData.getHashKey());
					const float textWidth = _shapeFontRendererContextTopMost.calculateTextWidth(windowTitle, fs::StringUtil::wcslen(windowTitle));
					const fs::Float2& displaySizeOverride = fs::Float2(textWidth + 16.0f, controlData._displaySize._y);
					prepareControlDataParam._initialDisplaySize = displaySizeOverride;
					prepareControlDataParam._desiredPositionInParent._x = parentDockDatum.getDockedControlTitleBarOffset(dockedControlIndex);
					prepareControlDataParam._desiredPositionInParent._y = 0.0f;
				}
				else
				{
					prepareControlDataParam._initialDisplaySize = titleBarSize;
					prepareControlDataParam._deltaInteractionSize = fs::Float2(-innerPadding.right() - kDefaultRoundButtonRadius * 2.0f, 0.0f);
				}
				prepareControlDataParam._alwaysResetDisplaySize = true;
				prepareControlDataParam._alwaysResetPosition = true;
				prepareControlDataParam._viewportUsage = ViewportUsage::Parent;
			}
			prepareControlData(controlData, prepareControlDataParam);
			
			fs::RenderingBase::Color titleBarColor;
			const bool isFocused = processFocusControl(controlData, getNamedColor(NamedColor::TitleBarFocused), getNamedColor(NamedColor::TitleBarOutOfFocus), titleBarColor);
			if (isParentControlDocking == true)
			{
				if (isControlPressed(controlData) == true)
				{
					ControlData& dockControlData = getControlData(parentControlData.getDockControlHashKey());
					DockDatum& dockDatum = dockControlData.getDockDatum(parentControlData._lastDockingMethod);
					dockDatum._dockedControlIndexShown = dockDatum.getDockedControlIndex(parentControlData.getHashKey());
				}
			}

			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused_ == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());

			const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);
			shapeFontRendererContext.setPosition(controlCenterPosition);
			if (isParentControlDocking == true)
			{
				const ControlData& dockControlData = getControlData(parentControlData.getDockControlHashKey());
				const bool isParentControlShownInDock = dockControlData.isShowingInDock(parentControlData);
				if (isControlHovered(controlData) == true)
				{
					shapeFontRendererContext.setColor(((isParentControlShownInDock == true) ? getNamedColor(NamedColor::ShownInDock) : getNamedColor(NamedColor::ShownInDock).addedRgb(32)));
				}
				else
				{
					shapeFontRendererContext.setColor(((isParentControlShownInDock == true) ? getNamedColor(NamedColor::ShownInDock) : getNamedColor(NamedColor::ShownInDock).addedRgb(16)));
				}

				shapeFontRendererContext.drawRectangle(controlData._displaySize, 0.0f, 0.0f);
			}
			else
			{
				shapeFontRendererContext.setColor(titleBarColor);

				shapeFontRendererContext.drawHalfRoundedRectangle(controlData._displaySize, (kDefaultRoundnessInPixel * 2.0f / controlData._displaySize.minElement()), fs::Math::kPi);

				shapeFontRendererContext.setColor(fs::RenderingBase::Color(127, 127, 127));
				shapeFontRendererContext.drawLine(controlData._position + fs::Float2(0.0f, titleBarSize._y), controlData._position + fs::Float2(controlData._displaySize._x, titleBarSize._y), 1.0f);
			}

			const fs::Float4& titleBarTextPosition = fs::Float4(controlData._position._x, controlData._position._y, 0.0f, 1.0f) + fs::Float4(innerPadding.left(), titleBarSize._y * 0.5f, 0.0f, 0.0f);
			const bool needToColorFocused_ = needToColorFocused(parentControlData);
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			if (isParentControlDocking == true)
			{
				const ControlData& dockControlData = getControlData(parentControlData.getDockControlHashKey());
				const bool isParentControlShownInDock = dockControlData.isShowingInDock(parentControlData);

				shapeFontRendererContext.setTextColor((isParentControlShownInDock == true) ? getNamedColor(NamedColor::ShownInDockFont) : getNamedColor(NamedColor::LightFont));
			}
			else
			{
				shapeFontRendererContext.setTextColor((needToColorFocused_ == true) ? getNamedColor(NamedColor::LightFont) : getNamedColor(NamedColor::DarkFont));
			}
			shapeFontRendererContext.drawDynamicText(windowTitle, titleBarTextPosition, fs::RenderingBase::TextRenderDirectionHorz::Rightward, fs::RenderingBase::TextRenderDirectionVert::Centered, 0.9375f);

			_controlStackPerFrame.emplace_back(ControlStackData(controlData));

			// Close button
			if (parentControlData.isDocking() == false)
			{
				// 중요
				nextNoAutoPositioned();
				nextControlPosition(fs::Float2(titleBarSize._x - kDefaultRoundButtonRadius * 2.0f - innerPadding.right(), (titleBarSize._y - kDefaultRoundButtonRadius * 2.0f) * 0.5f));

				beginRoundButton(windowTitle, fs::RenderingBase::Color(1.0f, 0.375f, 0.375f));
				endRoundButton();
			}

			// Window Offset 재조정!!
			parentControlData.setOffsetY_XXX(titleBarSize._y + parentControlData.getInnerPadding().top());

			return titleBarSize;
		}

		const bool GuiContext::beginRoundButton(const wchar_t* const windowTitle, const fs::RenderingBase::Color& color)
		{
			static constexpr ControlType controlType = ControlType::RoundButton;

			const ControlData& parentWindowData = getParentWindowControlData();

			const float radius = kDefaultRoundButtonRadius;
			ControlData& controlData = getControlData(windowTitle, controlType);

			PrepareControlDataParam prepareControlDataParam;
			{
				prepareControlDataParam._parentHashKeyOverride = parentWindowData.getHashKey();
				prepareControlDataParam._initialDisplaySize = fs::Float2(radius * 2.0f);
				prepareControlDataParam._viewportUsage = ViewportUsage::Parent;
			}
			prepareControlData(controlData, prepareControlDataParam);
			
			fs::RenderingBase::Color controlColor;
			const bool isClicked = processClickControl(controlData, color, color.scaledRgb(1.5f), color.scaledRgb(0.75f), controlColor);

			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused_ == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;

			const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setColor(controlColor);
			shapeFontRendererContext.setPosition(controlCenterPosition);
			shapeFontRendererContext.drawCircle(radius);

			_controlStackPerFrame.emplace_back(ControlStackData(controlData));

			return isClicked;
		}

		void GuiContext::pushTooltipWindow(const wchar_t* const tooltipText, const fs::Float2& position)
		{
			static constexpr ControlType controlType = ControlType::TooltipWindow;
			static constexpr float kTooltipFontScale = kFontScaleC;
			const float tooltipWindowPadding = 8.0f;

			ControlData& controlData = getControlData(tooltipText, controlType, L"TooltipWindow");

			PrepareControlDataParam prepareControlDataParam;
			{
				const float tooltipTextWidth = _shapeFontRendererContextForeground.calculateTextWidth(tooltipText, fs::StringUtil::wcslen(tooltipText)) * kTooltipFontScale;
				prepareControlDataParam._initialDisplaySize = fs::Float2(tooltipTextWidth + tooltipWindowPadding * 2.0f, _fontSize * kTooltipFontScale + tooltipWindowPadding);
				prepareControlDataParam._desiredPositionInParent = position;
				prepareControlDataParam._alwaysResetParent = true;
				prepareControlDataParam._alwaysResetDisplaySize = true;
				prepareControlDataParam._alwaysResetPosition = true;
				prepareControlDataParam._parentHashKeyOverride = _tooltipParentWindowHashKey; // ROOT
				prepareControlDataParam._viewportUsage = ViewportUsage::Parent;
			}
			nextNoAutoPositioned();
			prepareControlData(controlData, prepareControlDataParam);
			
			fs::RenderingBase::Color dummyColor;
			processShowOnlyControl(controlData, dummyColor);

			fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext = _shapeFontRendererContextTopMost;

			//// Viewport & Scissor rectangle
			//{
			//	controlData.setViewportIndexXXX(static_cast<uint32>(_viewportArrayPerFrame.size()));
			//
			//	D3D11_RECT scissorRectangleForMe;
			//	scissorRectangleForMe.left = static_cast<LONG>(controlData._position._x);
			//	scissorRectangleForMe.top = static_cast<LONG>(controlData._position._y);
			//	scissorRectangleForMe.right = static_cast<LONG>(scissorRectangleForMe.left + controlData._displaySize._x);
			//	scissorRectangleForMe.bottom = static_cast<LONG>(scissorRectangleForMe.top + controlData._displaySize._y);
			//	_scissorRectangleArrayPerFrame.emplace_back(scissorRectangleForMe);
			//	_viewportArrayPerFrame.emplace_back(_viewportFullScreen);
			//}

			{
				const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);
				shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
				shapeFontRendererContext.setColor(getNamedColor(NamedColor::TooltipBackground));
				shapeFontRendererContext.setPosition(controlCenterPosition);
				shapeFontRendererContext.drawRoundedRectangle(controlData._displaySize, (kDefaultRoundnessInPixel / controlData._displaySize.minElement()) * 0.75f, 0.0f, 0.0f);

				const fs::Float4& textPosition = fs::Float4(controlData._position._x, controlData._position._y, 0.0f, 1.0f) + fs::Float4(tooltipWindowPadding, prepareControlDataParam._initialDisplaySize._y * 0.5f, 0.0f, 0.0f);
				shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
				shapeFontRendererContext.setTextColor(getNamedColor(NamedColor::DarkFont));
				shapeFontRendererContext.drawDynamicText(tooltipText, textPosition, fs::RenderingBase::TextRenderDirectionHorz::Rightward, fs::RenderingBase::TextRenderDirectionVert::Centered, kTooltipFontScale);
			}
		}

		const wchar_t* GuiContext::generateControlKeyString(const ControlData& parentControlData, const wchar_t* const text, const ControlType controlType) const noexcept
		{
			static std::wstring hashKeyWstring;
			hashKeyWstring.clear();
			hashKeyWstring.append(std::to_wstring(parentControlData.getHashKey()));
			hashKeyWstring.append(text);
			hashKeyWstring.append(std::to_wstring(static_cast<uint16>(controlType)));
			return hashKeyWstring.c_str();
		}

		const uint64 GuiContext::generateControlHashKeyXXX(const wchar_t* const text, const ControlType controlType) const noexcept
		{
			static std::wstring hashKeyWstring;
			hashKeyWstring.clear();
			hashKeyWstring.append(text);
			hashKeyWstring.append(std::to_wstring(static_cast<uint16>(controlType)));
			return fs::StringUtil::hashRawString64(hashKeyWstring.c_str());
		}

		GuiContext::ControlData& GuiContext::getControlData(const wchar_t* const text, const ControlType controlType, const wchar_t* const hashGenerationKeyOverride) noexcept
		{
			const uint64 hashKey = generateControlHashKeyXXX((hashGenerationKeyOverride == nullptr) ? text : hashGenerationKeyOverride, controlType);
			auto found = _controlIdMap.find(hashKey);
			if (found == _controlIdMap.end())
			{
				const ControlData& stackTopControlData = getControlDataStackTopXXX();
				ControlData newControlData{ hashKey, stackTopControlData.getHashKey(), controlType };
				newControlData._text = text;

				_controlIdMap[hashKey] = newControlData;
			}

			ControlData& controlData = _controlIdMap[hashKey];
			if (controlData._updateCount < 3)
			{
				++controlData._updateCount;
			}
			return controlData;
		}

		void GuiContext::calculateControlChildAt(ControlData& controlData) noexcept
		{
			const MenuBarType currentMenuBarType = controlData._controlValue.getCurrentMenuBarType();
			fs::Float2& controlChildAt = const_cast<fs::Float2&>(controlData.getChildAt());
			controlChildAt = controlData._position + controlData._displayOffset + 
				((_nextNoAutoPositioned == false) 
					? fs::Float2(controlData.getInnerPadding().left(), controlData.getInnerPadding().top())
					: fs::Float2::kZero) +
				fs::Float2(0.0f, (MenuBarType::None != currentMenuBarType) ? kMenuBarBaseSize._y : 0.0f);
		}

		const GuiContext::ControlData& GuiContext::getParentWindowControlData() const noexcept
		{
			FS_ASSERT("김장원", _controlStackPerFrame.empty() == false, "Control 스택이 비어있을 때 호출되면 안 됩니다!!!");

			return getParentWindowControlData(getControlData(_controlStackPerFrame.back()._hashKey));
		}

		const GuiContext::ControlData& GuiContext::getParentWindowControlData(const ControlData& controlData) const noexcept
		{
			return getParentWindowControlDataInternal(controlData.getParentHashKey());
		}

		const GuiContext::ControlData& GuiContext::getParentWindowControlDataInternal(const uint64 hashKey) const noexcept
		{
			if (hashKey == 0)
			{
				return _rootControlData;
			}

			const ControlData& controlData = getControlData(hashKey);
			if (controlData.getControlType() == ControlType::Window)
			{
				return controlData;
			}

			return getParentWindowControlDataInternal(controlData.getParentHashKey());
		}

		void GuiContext::prepareControlData(ControlData& controlData, const PrepareControlDataParam& prepareControlDataParam) noexcept
		{
			const bool isNewData = controlData._displaySize.isNan();

			controlData.clearPerFrameData();
			if ((isNewData == true) || (prepareControlDataParam._alwaysResetParent == true))
			{
				const ControlData& stackTopControlData = getControlDataStackTopXXX();
				const uint64 parentHashKey = (prepareControlDataParam._parentHashKeyOverride == 0) ? stackTopControlData.getHashKey() : prepareControlDataParam._parentHashKeyOverride;
				controlData.setParentHashKeyXXX(parentHashKey);

				if (isNewData == true)
				{
					controlData._resizingMask = prepareControlDataParam._initialResizingMask;
				}
			}

			// Child ControlData array
			{
				controlData.prepareChildControlDataHashKeyArray();
			}

			ControlData& parentControlData = getControlData(controlData.getParentHashKey());
			{
				std::vector<uint64>& parentChildControlDataHashKeyArray = const_cast<std::vector<uint64>&>(parentControlData.getChildControlDataHashKeyArray());
				parentChildControlDataHashKeyArray.emplace_back(controlData.getHashKey());
			}

			// Child window
			const ControlType controlType = controlData.getControlType();
			if (controlType == ControlType::Window)
			{
				const uint64 controlHashKey = controlData.getHashKey();
				if (parentControlData.hasChildWindowConnected(controlHashKey) == false)
				{
					parentControlData.connectChildWindow(controlHashKey);
				}
			}

			// Inner padding
			{
				fs::Rect& controlInnerPadding = const_cast<fs::Rect&>(controlData.getInnerPadding());
				controlInnerPadding = prepareControlDataParam._innerPadding;
			}

			// Display size, Display size min
			if (isNewData == true || prepareControlDataParam._alwaysResetDisplaySize == true)
			{
				const float maxDisplaySizeX = parentControlData._displaySize._x - ((_nextNoAutoPositioned == false) ? (parentControlData.getInnerPadding().left() * 2.0f) : 0.0f);

				if (_nextControlSizeNonContrainedToParent == true)
				{
					controlData._displaySize._x = (_nextControlSize._x <= 0.0f) ? prepareControlDataParam._initialDisplaySize._x : _nextControlSize._x;
					controlData._displaySize._y = (_nextControlSize._y <= 0.0f) ? prepareControlDataParam._initialDisplaySize._y : _nextControlSize._y;
				}
				else
				{
					controlData._displaySize._x = (_nextControlSize._x <= 0.0f) ? fs::min(maxDisplaySizeX, prepareControlDataParam._initialDisplaySize._x) :
						((_nextSizingForced == true) ? _nextControlSize._x : fs::min(maxDisplaySizeX, _nextControlSize._x));
					controlData._displaySize._y = (_nextControlSize._y <= 0.0f) ? prepareControlDataParam._initialDisplaySize._y :
						((_nextSizingForced == true) ? _nextControlSize._y : fs::max(prepareControlDataParam._initialDisplaySize._y, _nextControlSize._y));
				}

				fs::Float2& controlDisplaySizeMin = const_cast<fs::Float2&>(controlData.getDisplaySizeMin());
				controlDisplaySizeMin = prepareControlDataParam._displaySizeMin;
			}

			// Interaction size!!!
			{
				fs::Float2& controlInteractionSize = const_cast<fs::Float2&>(controlData.getInteractionSize());
				fs::Float2& controlContentInteractionSize = const_cast<fs::Float2&>(controlData.getNonDockInteractionSize());
				controlInteractionSize = controlData._displaySize + prepareControlDataParam._deltaInteractionSize;
				controlContentInteractionSize = controlInteractionSize + prepareControlDataParam._deltaInteractionSizeByDock;
			}

			// Position, Parent offset, Parent child at, Parent content area size
			if (_nextNoAutoPositioned == false)
			{
				// Auto-positioned

				fs::Float2& parentControlChildAt = const_cast<fs::Float2&>(parentControlData.getChildAt());
				fs::Float2& parentControlNextChildOffset = const_cast<fs::Float2&>(parentControlData.getNextChildOffset());
				const float parentControlPreviousNextChildOffsetX = parentControlNextChildOffset._x;
				if (_nextSameLine == true)
				{
					parentControlChildAt._x += (parentControlNextChildOffset._x + kDefaultIntervalX);

					parentControlNextChildOffset = controlData._displaySize;
				}
				else
				{
					parentControlChildAt._x = parentControlData._position._x + parentControlData.getInnerPadding().left() + parentControlData._displayOffset._x; // @중요
					
					if (parentControlData.isDockHosting() == true)
					{
						const DockDatum& dockDatumTopSide = parentControlData.getDockDatum(DockingMethod::TopSide);
						if (controlType != ControlType::Window && dockDatumTopSide.hasDockedControls() == true)
						{
							// 맨 처음 Child Control 만 내려주면 된다!!
							const float offsetY = parentControlData.getDockSize(DockingMethod::TopSide)._y + parentControlData.getInnerPadding().top();
							if (parentControlChildAt._y < offsetY)
							{
								parentControlChildAt._y += offsetY;
							}
						}

						const DockDatum& dockDatumLeftSide = parentControlData.getDockDatum(DockingMethod::LeftSide);
						if (dockDatumLeftSide.hasDockedControls() == true)
						{
							parentControlChildAt._x += parentControlData.getDockSize(DockingMethod::LeftSide)._x;
						}
					}

					parentControlChildAt._y += parentControlNextChildOffset._y;

					parentControlNextChildOffset = controlData._displaySize;
				}

				const bool addIntervalY = (_nextNoAutoPositioned == false && prepareControlDataParam._noIntervalForNextSibling == false);
				if (addIntervalY == true)
				{
					parentControlNextChildOffset._y += kDefaultIntervalY;
				}

				// Parent content area size
				fs::Float2& parentControlContentAreaSize = const_cast<fs::Float2&>(parentControlData.getContentAreaSize());
				if (prepareControlDataParam._ignoreMeForContentAreaSize == false)
				{
					if (true == _nextSameLine)
					{
						if (parentControlContentAreaSize._x == 0.0f)
						{
							// 최초 _nextSameLine 시 바로 왼쪽 컨트롤의 크기도 추가해줘야 한다!
							parentControlContentAreaSize._x = parentControlPreviousNextChildOffsetX;
						}
						parentControlContentAreaSize._x += controlData._displaySize._x + kDefaultIntervalX;
					}

					parentControlContentAreaSize._y += (true == _nextSameLine) ? 0.0f : controlData._displaySize._y;
					parentControlContentAreaSize._y += (true == addIntervalY) ? kDefaultIntervalY : 0.0f;
				}

				controlData._position = parentControlChildAt;
			}
			else
			{
				// NO Auto-positioned

				if (prepareControlDataParam._alwaysResetPosition == true || isNewData == true)
				{
					controlData._position = parentControlData._position;

					if (prepareControlDataParam._desiredPositionInParent.isNan() == true)
					{
						controlData._position += _nextControlPosition;
					}
					else
					{
						controlData._position += prepareControlDataParam._desiredPositionInParent;
					}
				}
			}

			// Drag constraints 적용! (Dragging 이 아닐 때도 Constraint 가 적용되어야 함.. 예를 들면 resizing 중에!)
			if (controlData._isDraggable == true && controlData._draggingConstraints.isNan() == false)
			{
				controlData._position._x = fs::min(fs::max(controlData._draggingConstraints.left(), controlData._position._x), controlData._draggingConstraints.right());
				controlData._position._y = fs::min(fs::max(controlData._draggingConstraints.top(), controlData._position._y), controlData._draggingConstraints.bottom());
			}

			// State
			{
				if (controlType == ControlType::Window)
				{
					controlData.setControlState(ControlState::VisibleOpen);
				}
				else
				{
					controlData.setControlState(ControlState::Visible);
				}
			}

			// Child at
			calculateControlChildAt(controlData);

			// Viewport index
			{
				switch (prepareControlDataParam._viewportUsage)
				{
				case fs::Gui::ViewportUsage::Parent:
					controlData.setViewportIndexXXX(parentControlData.getViewportIndex());
					break;
				case fs::Gui::ViewportUsage::ParentDock:
					controlData.setViewportIndexXXX(parentControlData.getViewportIndexForDocks());
					break;
				case fs::Gui::ViewportUsage::Child:
					controlData.setViewportIndexXXX(parentControlData.getViewportIndexForChildren());
					break;
				default:
					break;
				}
			}
		}

		const bool GuiContext::processClickControl(ControlData& controlData, const fs::RenderingBase::Color& normalColor, const fs::RenderingBase::Color& hoverColor, const fs::RenderingBase::Color& pressedColor, fs::RenderingBase::Color& outBackgroundColor) noexcept
		{
			processControlInteractionInternal(controlData);

			outBackgroundColor = normalColor;

			const bool isClicked = isControlClicked(controlData);
			if (isControlHovered(controlData) == true)
			{
				outBackgroundColor = hoverColor;
			}
			else if (isControlPressed(controlData) == true || isClicked == true)
			{
				outBackgroundColor = pressedColor;
			}

			if (needToColorFocused(controlData) == false)
			{
				// Out-of-focus alpha
				outBackgroundColor.scaleA(kDefaultOutOfFocusAlpha);
			}

			processControlCommonInternal(controlData);

			return isClicked;
		}

		const bool GuiContext::processFocusControl(ControlData& controlData, const fs::RenderingBase::Color& focusedColor, const fs::RenderingBase::Color& nonFocusedColor, fs::RenderingBase::Color& outBackgroundColor) noexcept
		{
			processControlInteractionInternal(controlData, true);

			const uint64 controlHashKey = (0 != controlData._delegateHashKey) ? controlData._delegateHashKey : controlData.getHashKey();

			// Check new focus
			if (_draggedControlHashKey == 0 && _resizedControlHashKey == 0 && controlData._isFocusable == true &&
				(_mouseButtonDownFirst == true && (isControlPressed(controlData) == true || isControlClicked(controlData) == true)))
			{
				// Focus entered
				_focusedControlHashKey = controlHashKey;
			}

			if (needToColorFocused(controlData) == true)
			{
				// Focused

				outBackgroundColor = focusedColor;
				outBackgroundColor.a(kDefaultFocusedAlpha);
			}
			else
			{
				// Out of focus

				outBackgroundColor = nonFocusedColor;
				outBackgroundColor.a(kDefaultOutOfFocusAlpha);
			}

			processControlCommonInternal(controlData);

			return isControlFocused(controlData);
		}

		void GuiContext::processShowOnlyControl(ControlData& controlData, fs::RenderingBase::Color& outBackgroundColor, const bool doNotSetMouseInteractionDone) noexcept
		{
			processControlInteractionInternal(controlData, doNotSetMouseInteractionDone);

			if (needToColorFocused(controlData) == true)
			{
				outBackgroundColor.scaleA(kDefaultFocusedAlpha);
			}
			else
			{
				outBackgroundColor.scaleA(kDefaultOutOfFocusAlpha);
			}

			processControlCommonInternal(controlData);
		}

		const bool GuiContext::processScrollableControl(ControlData& controlData, const fs::RenderingBase::Color& normalColor, const fs::RenderingBase::Color& dragColor, fs::RenderingBase::Color& outBackgroundColor) noexcept
		{
			processControlInteractionInternal(controlData);

			outBackgroundColor = normalColor;

			const bool isHovered = isControlHovered(controlData);
			const bool isPressed = isControlPressed(controlData);
			const bool isDragging = isDraggingControl(controlData);
			if (isHovered == true || isPressed == true || isDragging == true)
			{
				outBackgroundColor = dragColor;
			}

			if (needToColorFocused(controlData) == false)
			{
				outBackgroundColor.scaleA(kDefaultOutOfFocusAlpha);
			}

			processControlCommonInternal(controlData);

			return isPressed;
		}
		
		const bool GuiContext::processToggleControl(ControlData& controlData, const fs::RenderingBase::Color& normalColor, const fs::RenderingBase::Color& hoverColor, const fs::RenderingBase::Color& toggledColor, fs::RenderingBase::Color& outBackgroundColor) noexcept
		{
			processControlInteractionInternal(controlData);

			const bool isClicked = isControlClicked(controlData);
			if (isClicked == true)
			{
				controlData._controlValue.setIsToggled(!controlData._controlValue.getIsToggled());
			}

			const bool isToggled = controlData._controlValue.getIsToggled();
			const bool isHovered = isControlHovered(controlData);
			outBackgroundColor = (isToggled == true) ? toggledColor : (isHovered == true) ? hoverColor : normalColor;

			if (needToColorFocused(controlData) == false)
			{
				outBackgroundColor.scaleA(kDefaultOutOfFocusAlpha);
			}

			processControlCommonInternal(controlData);
			return isClicked;
		}

		void GuiContext::processControlInteractionInternal(ControlData& controlData, const bool doNotSetMouseInteractionDone) noexcept
		{
			static std::function fnResetHoverDataIfMe = [&](const uint64 controlHashKey) 
			{
				if (controlHashKey == _hoveredControlHashKey)
				{
					_hoveredControlHashKey = 0;
					_hoverStartTimeMs = fs::Profiler::getCurrentTimeMs();
					_tooltipPosition.setZero();
					_tooltipParentWindowHashKey = 0;
				}
			};

			static std::function fnResetPressDataIfMe = [&](const uint64 controlHashKey) 
			{
				if (controlHashKey == _pressedControlHashKey)
				{
					_pressedControlHashKey = 0;
					_pressedControlInitialPosition.setZero();
				}
			};

			const uint64 controlHashKey = controlData.getHashKey();
			if (isInteractingInternal(controlData) == false)
			{
				fnResetHoverDataIfMe(controlHashKey);
				fnResetPressDataIfMe(controlHashKey);
				return;
			}

			const ControlData& parentControlData = getControlData(controlData.getParentHashKey());
			if (_isMouseInteractionDonePerFrame == true)
			{
				fnResetHoverDataIfMe(controlHashKey);
				fnResetPressDataIfMe(controlHashKey);
				return;
			}

			const bool shouldIgnoreInteraction_ = shouldIgnoreInteraction(_mousePosition, controlData);
			const bool isMouseInParentInteractionArea = isInControlInteractionArea(_mousePosition, parentControlData);
			const bool isMouseInInteractionArea = isInControlInteractionArea(_mousePosition, controlData);
			if ((controlData._isInteractableOutsideParent == true || isMouseInParentInteractionArea == true) && isMouseInInteractionArea == true && shouldIgnoreInteraction_ == false)
			{
				// Hovered (at least)

				if (doNotSetMouseInteractionDone == false)
				{
					_isMouseInteractionDonePerFrame = true;
				}

				const bool isMouseDownInInteractionArea = isInControlInteractionArea(_mouseDownPosition, controlData);

				if (_hoveredControlHashKey != controlHashKey && controlData._isFocusable == false)
				{
					fnResetHoverDataIfMe(_hoveredControlHashKey);

					_hoveredControlHashKey = controlHashKey;
					if (_hoverStarted == false)
					{
						_hoverStarted = true;
					}
				}

				// Click Event 가 발생했을 때도 Pressed 상태 유지!
				if (_mouseDownUp == false && _mouseButtonDown == false)
				{
					fnResetPressDataIfMe(controlHashKey);
				}

				// Pressed (Mouse down)
				if (_mouseButtonDown == true && isMouseDownInInteractionArea == true)
				{
					fnResetHoverDataIfMe(controlHashKey);

					if (_pressedControlHashKey != controlHashKey)
					{
						_pressedControlHashKey = controlHashKey;

						_pressedControlInitialPosition = controlData._position;
					}
				}

				// Clicked (only in interaction area)
				if (_mouseDownUp == true && isMouseDownInInteractionArea == true)
				{
					if (_pressedControlHashKey == controlHashKey)
					{
						_clickedControlHashKeyPerFrame = controlHashKey;
					}
				}
			}
			else
			{
				// Not interacting

				fnResetHoverDataIfMe(controlHashKey);
				fnResetPressDataIfMe(controlHashKey);
			}
		}

		void GuiContext::processControlCommonInternal(ControlData& controlData) noexcept
		{
			if (_resizedControlHashKey == 0 && isInteractingInternal(controlData) == true)
			{
				fs::Window::CursorType newCursorType;
				ResizingMask resizingMask;
				const bool isResizable = controlData.isResizable();
				if (controlData.isResizable() == true && isInControlBorderArea(_mousePosition, controlData, newCursorType, resizingMask, _resizingMethod) == true)
				{
					if (controlData._resizingMask.overlaps(resizingMask) == true)
					{
						_cursorType = newCursorType;

						_isMouseInteractionDonePerFrame = true;
					}
				}
			}

			const bool isHovered = isControlHovered(controlData);
			if (_nextTooltipText != nullptr && isHovered == true && _hoverStartTimeMs + 1000 < fs::Profiler::getCurrentTimeMs())
			{
				if (_hoverStarted == true)
				{
					_tooltipTextFinal = _nextTooltipText;
					_tooltipPosition = _mousePosition;
					_tooltipParentWindowHashKey = getParentWindowControlData(controlData).getHashKey();
					
					_hoverStarted = false;
				}
			}

			ControlData& changeTargetControlData = (controlData._delegateHashKey == 0) ? controlData : getControlData(controlData._delegateHashKey);
			const bool isResizing = isResizingControl(changeTargetControlData);
			const bool isDragging = isDraggingControl(controlData);
			const fs::Float2 mouseDeltaPosition = _mousePosition - _mouseDownPosition;
			if (isResizing == true)
			{
				if (_isResizeBegun == true)
				{
					_resizedControlInitialPosition = changeTargetControlData._position;
					_resizedControlInitialDisplaySize = changeTargetControlData._displaySize;
					
					_isResizeBegun = false;
				}

				fs::Float2& changeTargetControlDisplaySize = const_cast<fs::Float2&>(changeTargetControlData._displaySize);

				const float flipHorz = (_resizingMethod == ResizingMethod::RepositionHorz || _resizingMethod == ResizingMethod::RepositionBoth) ? -1.0f : +1.0f;
				const float flipVert = (_resizingMethod == ResizingMethod::RepositionVert || _resizingMethod == ResizingMethod::RepositionBoth) ? -1.0f : +1.0f;
				if (_cursorType != fs::Window::CursorType::SizeVert)
				{
					const float newPositionX = _resizedControlInitialPosition._x - mouseDeltaPosition._x * flipHorz;
					const float newDisplaySizeX = _resizedControlInitialDisplaySize._x + mouseDeltaPosition._x * flipHorz;

					if (changeTargetControlData.getDisplaySizeMin()._x + changeTargetControlData.getHorzDockSizeSum() < newDisplaySizeX)
					{
						if (flipHorz < 0.0f)
						{
							changeTargetControlData._position._x = newPositionX;
						}
						changeTargetControlDisplaySize._x = newDisplaySizeX;
					}
				}
				if (_cursorType != fs::Window::CursorType::SizeHorz)
				{
					const float newPositionY = _resizedControlInitialPosition._y - mouseDeltaPosition._y * flipVert;
					const float newDisplaySizeY = _resizedControlInitialDisplaySize._y + mouseDeltaPosition._y * flipVert;

					if (changeTargetControlData.getDisplaySizeMin()._y + changeTargetControlData.getVertDockSizeSum() < newDisplaySizeY)
					{
						if (flipVert < 0.0f)
						{
							changeTargetControlData._position._y = newPositionY;
						}
						changeTargetControlDisplaySize._y = newDisplaySizeY;
					}
				}

				if (changeTargetControlData.isDocking() == true)
				{
					// 내가 Docking 중인 컨트롤이라면 Dock Control 의 Dock 크기도 같이 변경해줘야 한다.

					const uint64 dockControlHashKey = changeTargetControlData.getDockControlHashKey();
					ControlData& dockControlData = getControlData(dockControlHashKey);
					dockControlData.setDockSize(changeTargetControlData._lastDockingMethod, changeTargetControlDisplaySize);
					updateDockDatum(dockControlHashKey);
				}
				else if (changeTargetControlData._dockingControlType == DockingControlType::Dock || changeTargetControlData._dockingControlType == DockingControlType::DockerDock)
				{
					// 내가 DockHosting 중일 수 있음

					updateDockDatum(changeTargetControlData.getHashKey());
				}

				_isMouseInteractionDonePerFrame = true;
			}
			else if (isDragging == true)
			{
				if (_isDragBegun == true)
				{
					_draggedControlInitialPosition = changeTargetControlData._position;

					_isDragBegun = false;
				}

				const fs::Float2 originalPosition = changeTargetControlData._position;
				if (changeTargetControlData._draggingConstraints.isNan() == true)
				{
					changeTargetControlData._position = _draggedControlInitialPosition + mouseDeltaPosition;
				}
				else
				{
					const fs::Float2& naivePosition = _draggedControlInitialPosition + mouseDeltaPosition;
					changeTargetControlData._position._x = fs::min(fs::max(changeTargetControlData._draggingConstraints.left(), naivePosition._x), changeTargetControlData._draggingConstraints.right());
					changeTargetControlData._position._y = fs::min(fs::max(changeTargetControlData._draggingConstraints.top(), naivePosition._y), changeTargetControlData._draggingConstraints.bottom());
				}

				if (changeTargetControlData.isDocking() == true)
				{
					// Docking 중이었으면 마우스로 바로 옮길 수 없도록!! (Dock 에 좀 더 오래 붙어있도록)

					changeTargetControlData._position = originalPosition;

					ControlData& dockControlData = getControlData(changeTargetControlData.getDockControlHashKey());
					DockDatum& dockDatum = dockControlData.getDockDatum(changeTargetControlData._lastDockingMethod);
					const fs::Float2& dockSize = dockControlData.getDockSize(changeTargetControlData._lastDockingMethod);
					const fs::Float2& dockPosition = dockControlData.getDockPosition(changeTargetControlData._lastDockingMethod);
					const fs::Rect dockRect{ dockPosition, dockSize };
					bool needToDisconnectFromDock = true;
					if (dockRect.contains(_mousePosition) == true)
					{
						needToDisconnectFromDock = false;

						const fs::Rect dockTitleBarAreaRect{ dockPosition, fs::Float2(dockSize._x, kTitleBarBaseSize._y) };
						if (dockTitleBarAreaRect.contains(_mousePosition) == true)
						{
							const float titleBarOffset = _mousePosition._x - dockTitleBarAreaRect.left();
							const int32 targetDockedControlindex = dockDatum.getDockedControlIndexByMousePosition(titleBarOffset);
							if (0 <= targetDockedControlindex)
							{
								const int32 originalDockedControlIndex = dockDatum.getDockedControlIndex(changeTargetControlData.getHashKey());
								if (originalDockedControlIndex != targetDockedControlindex)
								{
									dockDatum.swapDockedControlsXXX(originalDockedControlIndex, targetDockedControlindex);
									dockDatum._dockedControlIndexShown = targetDockedControlindex;

									_taskWhenMouseUp.setUpdateDockDatum(dockControlData.getHashKey());
									updateDockDatum(dockControlData.getHashKey(), true);
								}
							}
							else
							{
								needToDisconnectFromDock = true;
							}
						}
					}

					if (needToDisconnectFromDock == true)
					{
						// 마우스가 dockRect 를 벗어나야 옮길 수 있다!

						undock(changeTargetControlData.getHashKey());
					}
				}
				else
				{
					// Set delta position
					changeTargetControlData._deltaPosition = changeTargetControlData._position - originalPosition;
				}
				
				_isMouseInteractionDonePerFrame = true;
			}
			
			processControlDocking(changeTargetControlData, isDragging);

			resetNextStates();
		}

		void GuiContext::processControlDocking(ControlData& controlData, const bool isDragging) noexcept
		{
			static constexpr fs::RenderingBase::Color color = fs::RenderingBase::Color(100, 110, 160);
			static std::function fnRenderDockingBox = [&](const fs::Rect& boxRect, const ControlData& parentControlData)
			{
				const fs::Float4& parentControlCenterPosition = getControlCenterPosition(parentControlData);
				fs::Float4 renderPosition = parentControlCenterPosition;
				renderPosition._x = boxRect.center()._x;
				renderPosition._y = boxRect.center()._y;
				_shapeFontRendererContextTopMost.setViewportIndex(parentControlData.getViewportIndex());

				const bool isMouseInBoxRect = boxRect.contains(_mousePosition);
				_shapeFontRendererContextTopMost.setColor(((isMouseInBoxRect == true) ? color.scaledRgb(1.5f) : color));
				_shapeFontRendererContextTopMost.setPosition(renderPosition);
				_shapeFontRendererContextTopMost.drawRectangle(boxRect.size(), kDockingInteractionDisplayBorderThickness, 0.0f);
			};
			static std::function fnRenderPreview = [&](const fs::Rect& previewRect)
			{
				_shapeFontRendererContextTopMost.setViewportIndex(0);
				_shapeFontRendererContextTopMost.setColor(color.scaledA(0.5f));
				_shapeFontRendererContextTopMost.setPosition(fs::Float4(previewRect.center()._x, previewRect.center()._y, 0.0f, 1.0f));
				_shapeFontRendererContextTopMost.drawRectangle(previewRect.size(), 0.0f, 0.0f);
			};

			ControlData& parentControlData = getControlData(controlData.getParentHashKey());
			if ((controlData.hasChildWindow() == false) && 
				(controlData._dockingControlType == DockingControlType::Docker || controlData._dockingControlType == DockingControlType::DockerDock) &&
				(parentControlData._dockingControlType == DockingControlType::Dock || parentControlData._dockingControlType == DockingControlType::DockerDock) &&
				isInControlInteractionArea(_mousePosition, controlData) == true)
			{
				const fs::Float4& parentControlCenterPosition = getControlCenterPosition(parentControlData);
				const float previewShortLengthMax = 160.0f;
				const float previewShortLength = fs::min(parentControlData._displaySize._x * 0.5f, previewShortLengthMax);

				fs::Rect interactionBoxRect;
				fs::Rect previewRect;
				
				// 초기화
				if (_mouseDownUp == false)
				{
					controlData._lastDockingMethodCandidate = DockingMethod::COUNT;
				}

				// Top
				{
					interactionBoxRect.left(parentControlCenterPosition._x - kDockingInteractionLong * 0.5f);
					interactionBoxRect.right(interactionBoxRect.left() + kDockingInteractionLong);
					interactionBoxRect.top(parentControlData._position._y + parentControlData.getDockOffsetSize()._y + kDockingInteractionOffset);
					interactionBoxRect.bottom(interactionBoxRect.top() + kDockingInteractionShort);

					const fs::Float2& dockPosition = parentControlData.getDockPosition(DockingMethod::TopSide);
					previewRect.position(dockPosition);
					previewRect.right(previewRect.left() + parentControlData._displaySize._x);
					previewRect.bottom(previewRect.top() + previewShortLength);

					if (isDragging == true)
					{
						fnRenderDockingBox(interactionBoxRect, parentControlData);

						DockDatum& parentControlDockDatum = parentControlData.getDockDatum(DockingMethod::TopSide);
						if (controlData._lastDockingMethodCandidate == DockingMethod::COUNT && interactionBoxRect.contains(_mousePosition) == true)
						{
							controlData._lastDockingMethodCandidate = DockingMethod::TopSide;

							if (parentControlDockDatum.isRawDockSizeSet() == true)
							{
								previewRect.right(previewRect.left() + parentControlData.getDockSize(DockingMethod::TopSide)._x);
								previewRect.bottom(previewRect.top() + parentControlData.getDockSize(DockingMethod::TopSide)._y);

								fnRenderPreview(previewRect);
							}
							else
							{
								parentControlDockDatum.setRawDockSize(previewRect.size());

								fnRenderPreview(previewRect);
							}
						}
					}
				}

				// Bottom
				{
					interactionBoxRect.left(parentControlCenterPosition._x - kDockingInteractionLong * 0.5f);
					interactionBoxRect.right(interactionBoxRect.left() + kDockingInteractionLong);
					interactionBoxRect.bottom(parentControlData._position._y + parentControlData._displaySize._y - kDockingInteractionOffset);
					interactionBoxRect.top(interactionBoxRect.bottom() - kDockingInteractionShort);

					const fs::Float2& dockPosition = parentControlData.getDockPosition(DockingMethod::BottomSide);
					previewRect.position(dockPosition);
					previewRect.right(previewRect.left() + parentControlData._displaySize._x);
					previewRect.bottom(previewRect.top() + previewShortLength);

					if (isDragging == true)
					{
						fnRenderDockingBox(interactionBoxRect, parentControlData);

						DockDatum& parentControlDockDatum = parentControlData.getDockDatum(DockingMethod::BottomSide);
						if (controlData._lastDockingMethodCandidate == DockingMethod::COUNT && interactionBoxRect.contains(_mousePosition) == true)
						{
							controlData._lastDockingMethodCandidate = DockingMethod::BottomSide;

							if (parentControlDockDatum.isRawDockSizeSet() == true)
							{
								previewRect.right(previewRect.left() + parentControlData.getDockSize(DockingMethod::BottomSide)._x);
								previewRect.bottom(previewRect.top() + parentControlData.getDockSize(DockingMethod::BottomSide)._y);

								fnRenderPreview(previewRect);
							}
							else
							{
								parentControlDockDatum.setRawDockSize(previewRect.size());

								fnRenderPreview(previewRect);
							}
						}
					}
				}

				// Left
				{
					interactionBoxRect.left(parentControlData._position._x + kDockingInteractionOffset);
					interactionBoxRect.right(interactionBoxRect.left() + kDockingInteractionShort);
					interactionBoxRect.top(parentControlCenterPosition._y - kDockingInteractionLong * 0.5f);
					interactionBoxRect.bottom(interactionBoxRect.top() + kDockingInteractionLong);

					const fs::Float2& dockPosition = parentControlData.getDockPosition(DockingMethod::LeftSide);
					previewRect.position(dockPosition);
					previewRect.right(previewRect.left() + previewShortLength);
					previewRect.bottom(previewRect.top() + parentControlData._displaySize._y - parentControlData.getDockOffsetSize()._y);

					if (isDragging == true)
					{
						fnRenderDockingBox(interactionBoxRect, parentControlData);

						DockDatum& parentControlDockDatum = parentControlData.getDockDatum(DockingMethod::LeftSide);
						if (controlData._lastDockingMethodCandidate == DockingMethod::COUNT && interactionBoxRect.contains(_mousePosition) == true)
						{
							controlData._lastDockingMethodCandidate = DockingMethod::LeftSide;
							
							if (parentControlDockDatum.isRawDockSizeSet() == true)
							{
								previewRect.right(previewRect.left() + parentControlData.getDockSize(DockingMethod::LeftSide)._x);
								previewRect.bottom(previewRect.top() + parentControlData.getDockSize(DockingMethod::LeftSide)._y);

								fnRenderPreview(previewRect);
							}
							else
							{
								parentControlDockDatum.setRawDockSize(previewRect.size());

								fnRenderPreview(previewRect);
							}
						}
					}
				}

				// Right
				{
					interactionBoxRect.right(parentControlData._position._x + parentControlData._displaySize._x - kDockingInteractionOffset);
					interactionBoxRect.left(interactionBoxRect.right() - kDockingInteractionShort);
					interactionBoxRect.top(parentControlCenterPosition._y - kDockingInteractionLong * 0.5f);
					interactionBoxRect.bottom(interactionBoxRect.top() + kDockingInteractionLong);

					const fs::Float2& dockPosition = parentControlData.getDockPosition(DockingMethod::RightSide);
					previewRect.position(dockPosition);
					previewRect.right(previewRect.left() + previewShortLength);
					previewRect.bottom(previewRect.top() + parentControlData._displaySize._y - parentControlData.getDockOffsetSize()._y);

					if (isDragging == true)
					{
						fnRenderDockingBox(interactionBoxRect, parentControlData);

						DockDatum& parentControlDockDatum = parentControlData.getDockDatum(DockingMethod::RightSide);
						if (controlData._lastDockingMethodCandidate == DockingMethod::COUNT && interactionBoxRect.contains(_mousePosition) == true)
						{
							controlData._lastDockingMethodCandidate = DockingMethod::RightSide;

							if (parentControlDockDatum.isRawDockSizeSet() == true)
							{
								previewRect.right(previewRect.left() + parentControlData.getDockSize(DockingMethod::RightSide)._x);
								previewRect.bottom(previewRect.top() + parentControlData.getDockSize(DockingMethod::RightSide)._y);

								fnRenderPreview(previewRect);
							}
							else
							{
								parentControlDockDatum.setRawDockSize(previewRect.size());

								fnRenderPreview(previewRect);
							}
						}
					}
				}

				if (_mouseDownUp == true && controlData._lastDockingMethodCandidate != DockingMethod::COUNT)
				{
					if (controlData.isDocking() == false)
					{
						// Docking 시작.

						dock(controlData.getHashKey(), parentControlData.getHashKey());

						_draggedControlHashKey = 0;
					}
				}
			}
		}

		void GuiContext::dock(const uint64 dockedControlHashKey, const uint64 dockControlHashKey) noexcept
		{
			ControlData& dockedControlData = getControlData(dockedControlHashKey);
			dockedControlData.swapDockingStateContext();

			if (dockedControlData._lastDockingMethod != dockedControlData._lastDockingMethodCandidate)
			{
				dockedControlData._lastDockingMethod = dockedControlData._lastDockingMethodCandidate;

				dockedControlData._lastDockingMethodCandidate = DockingMethod::COUNT;
			}

			ControlData& dockControlData = getControlData(dockControlHashKey);
			DockDatum& parentControlDockDatum = dockControlData.getDockDatum(dockedControlData._lastDockingMethod);
			if (dockedControlData._lastDockingMethod != dockedControlData._lastDockingMethodCandidate)
			{
				dockedControlData._displaySize = dockControlData.getDockSize(dockedControlData._lastDockingMethod);
			}
			parentControlDockDatum._dockedControlHashArray.emplace_back(dockedControlData.getHashKey());

			dockedControlData._resizingMask = ResizingMask::fromDockingMethod(dockedControlData._lastDockingMethod);
			dockedControlData._position = dockControlData.getDockPosition(dockedControlData._lastDockingMethod);
			dockedControlData.connectToDock(dockControlHashKey);

			parentControlDockDatum._dockedControlIndexShown = parentControlDockDatum.getDockedControlIndex(dockedControlData.getHashKey());

			updateDockDatum(dockControlHashKey);

			// 내가 Focus 였다면 Dock 을 가진 컨트롤로 옮기자!
			if (isControlFocused(dockedControlData) == true)
			{
				_focusedControlHashKey = dockControlHashKey;
			}
		}

		void GuiContext::undock(const uint64 dockedControlHashKey) noexcept
		{
			ControlData& dockedControlData = getControlData(dockedControlHashKey);
			ControlData& dockControlData = getControlData(dockedControlData.getDockControlHashKey());
			DockDatum& dockDatum = dockControlData.getDockDatum(dockedControlData._lastDockingMethod);
			const uint32 changeTargetParentDockedControlCount = static_cast<uint32>(dockDatum._dockedControlHashArray.size());
			int32 indexToErase = -1;
			for (uint32 iter = 0; iter < changeTargetParentDockedControlCount; ++iter)
			{
				if (dockDatum._dockedControlHashArray[iter] == dockedControlData.getHashKey())
				{
					indexToErase = static_cast<int32>(iter);
				}
			}
			if (0 <= indexToErase)
			{
				dockDatum._dockedControlHashArray.erase(dockDatum._dockedControlHashArray.begin() + indexToErase);
			}
			else
			{
				FS_LOG_ERROR("김장원", "Docked Control 이 Parent 의 Child Array 에 없는 상황입니다!!!");
			}

			dockedControlData.swapDockingStateContext();

			_draggedControlInitialPosition = dockedControlData._position;
			_focusedControlHashKey = dockedControlData.getHashKey();

			const uint64 dockControlHashKeyCopy = dockedControlData.getDockControlHashKey();

			dockedControlData.disconnectFromDock();
			dockDatum._dockedControlIndexShown = fs::min(dockDatum._dockedControlIndexShown, static_cast<int32>(dockDatum._dockedControlHashArray.size() - 1));
			dockedControlData._lastDockingMethodCandidate = DockingMethod::COUNT;

			updateDockDatum(dockControlHashKeyCopy);
		}

		void GuiContext::updateDockDatum(const uint64 dockControlHashKey, const bool dontUpdateWidthArray) noexcept
		{
			ControlData& dockControlData = getControlData(dockControlHashKey);
			for (DockingMethod dockingMethodIter = static_cast<DockingMethod>(0); dockingMethodIter != DockingMethod::COUNT; dockingMethodIter = static_cast<DockingMethod>(static_cast<uint32>(dockingMethodIter) + 1))
			{
				DockDatum& dockDatum = dockControlData.getDockDatum(dockingMethodIter);
				const uint32 dockedControlCount = static_cast<uint32>(dockDatum._dockedControlHashArray.size());
				dockDatum._dockedControlTitleBarOffsetArray.resize(dockedControlCount);
				dockDatum._dockedControlTitleBarWidthArray.resize(dockedControlCount);

				float titleBarWidthSum = 0.0f;
				for (uint32 dockedControlIndex = 0; dockedControlIndex < dockedControlCount; ++dockedControlIndex)
				{
					ControlData& dockedControlData = getControlData(dockDatum._dockedControlHashArray[dockedControlIndex]);
					dockedControlData._displaySize = dockControlData.getDockSize(dockingMethodIter);
					dockedControlData._position = dockControlData.getDockPosition(dockingMethodIter);
					
					const wchar_t* const title = dockedControlData.getText();
					const float titleBarWidth = _shapeFontRendererContextTopMost.calculateTextWidth(title, fs::StringUtil::wcslen(title)) + 16.0f;
					dockDatum._dockedControlTitleBarOffsetArray[dockedControlIndex] = titleBarWidthSum;
					if (dontUpdateWidthArray == false)
					{
						dockDatum._dockedControlTitleBarWidthArray[dockedControlIndex] = titleBarWidth;
					}
					titleBarWidthSum += titleBarWidth;
				}
			}
		}

		const bool GuiContext::isInteractingInternal(const ControlData& controlData) const noexcept
		{
			if (_focusedControlHashKey != 0 && isAncestorFocusedInclusiveXXX(controlData) == false)
			{
				// Focus 가 있는 Control 이 존재하지만 내가 Focus 는 아닌 경우

				fs::Window::CursorType dummyCursorType;
				ResizingMethod dummyResizingMethod;
				ResizingMask dummyResizingMask;
				const ControlData& focusedControlData = getControlData(_focusedControlHashKey);
				if (isInControlInteractionArea(_mousePosition, focusedControlData) == true || isInControlBorderArea(_mousePosition, focusedControlData, dummyCursorType, dummyResizingMask, dummyResizingMethod) == true)
				{
					// 마우스가 Focus Control 과 상호작용할 경우 나와는 상호작용하지 않는것으로 판단!!
					return false;
				}
			}
			return true;
		}

		const bool GuiContext::isDraggingControl(const ControlData& controlData) const noexcept
		{
			if (_mouseButtonDown == false)
			{
				_draggedControlHashKey = 0;
				return false;
			}

			if (_draggedControlHashKey == 0)
			{
				if (_resizedControlHashKey != 0 || controlData._isDraggable == false || isInteractingInternal(controlData) == false)
				{
					return false;
				}

				if (_mouseButtonDown == true && isInControlInteractionArea(_mousePosition, controlData) == true && isInControlInteractionArea(_mouseDownPosition, controlData) == true)
				{
					// Drag 시작
					_isDragBegun = true;
					_draggedControlHashKey = controlData.getHashKey();
					return true;
				}
			}
			else
			{
				if (controlData.getHashKey() == _draggedControlHashKey)
				{
					return true;
				}
			}
			return false;
		}

		const bool GuiContext::isResizingControl(const ControlData& controlData) const noexcept
		{
			if (_mouseButtonDown == false)
			{
				_resizedControlHashKey = 0;
				return false;
			}

			if (_resizedControlHashKey == 0)
			{
				if (_draggedControlHashKey != 0 || controlData.isResizable() == false || isInteractingInternal(controlData) == false)
				{
					return false;
				}

				fs::Window::CursorType newCursorType;
				ResizingMask resizingMask;
				if (_mouseButtonDown == true && isInControlBorderArea(_mousePosition, controlData, newCursorType, resizingMask, _resizingMethod) == true && isInControlBorderArea(_mouseDownPosition, controlData, newCursorType, resizingMask, _resizingMethod) == true)
				{
					if (controlData._resizingMask.overlaps(resizingMask) == true)
					{
						_resizedControlHashKey = controlData.getHashKey();
						_isResizeBegun = true;
						_cursorType = newCursorType;
						return true;
					}
				}
			}
			else
			{
				if (controlData.getHashKey() == _resizedControlHashKey)
				{
					return true;
				}
			}
			return false;
		}
		
		const bool GuiContext::isAncestorFocusedInclusiveXXX(const ControlData& controlData) const noexcept
		{
			if (_focusedControlHashKey == controlData.getHashKey())
			{
				return true;
			}
			return isAncestorFocused(controlData);
		}

		const bool GuiContext::isAncestorInclusive(const ControlData& controlData, const uint64 ancestorCandidateHashKey) const noexcept
		{
			return isAncestorRecursiveXXX(controlData.getHashKey(), ancestorCandidateHashKey);
		}

		const bool GuiContext::isAncestorRecursiveXXX(const uint64 currentControlHashKey, const uint64 ancestorCandidateHashKey) const noexcept
		{
			if (currentControlHashKey == 0)
			{
				return false;
			}

			if (currentControlHashKey == ancestorCandidateHashKey)
			{
				return true;
			}

			const uint64 parentControlHashKey = getControlData(currentControlHashKey).getParentHashKey();
			return isAncestorRecursiveXXX(parentControlHashKey, ancestorCandidateHashKey);
		}

		const bool GuiContext::isDescendantInclusive(const ControlData& controlData, const uint64 descendantCandidateHashKey) const noexcept
		{
			return isDescendantRecursiveXXX(controlData.getHashKey(), descendantCandidateHashKey);
		}

		const bool GuiContext::isDescendantRecursiveXXX(const uint64 currentControlHashKey, const uint64 descendantCandidateHashKey) const noexcept
		{
			if (currentControlHashKey == descendantCandidateHashKey)
			{
				return true;
			}

			const ControlData& controlData = getControlData(currentControlHashKey);
			const auto& previousChildControlDataHashKeyArray = controlData.getPreviousChildControlDataHashKeyArray();
			for (const auto& previousChildControlDataHashKey : previousChildControlDataHashKeyArray)
			{
				if (isDescendantRecursiveXXX(previousChildControlDataHashKey, descendantCandidateHashKey) == true)
				{
					return true;
				}
			}

			return false;
		}

		const bool GuiContext::isAncestorFocused(const ControlData& controlData) const noexcept
		{
			return isAncestorFocusedRecursiveXXX(controlData.getParentHashKey());
		}

		const bool GuiContext::isAncestorFocusedRecursiveXXX(const uint64 hashKey) const noexcept
		{
			if (hashKey == 0)
			{
				return false;
			}

			if (hashKey == _focusedControlHashKey)
			{
				return true;
			}

			const uint64 parentHashKey = getControlData(hashKey).getParentHashKey();
			return isAncestorFocusedRecursiveXXX(parentHashKey);
		}

		const bool GuiContext::needToColorFocused(const ControlData& controlData) const noexcept
		{
			const ControlData& closestFocusableAncestorInclusive = getClosestFocusableAncestorInclusiveInternal(controlData);

			// #0. Focused
			const bool isFocused = isControlFocused(closestFocusableAncestorInclusive);
			if (isFocused == true)
			{
				return true;
			}

			// #1. Child Control Focused
			const bool isDescendantFocused_ = isDescendantFocused(closestFocusableAncestorInclusive);
			if (isDescendantFocused_ == true)
			{
				return true;
			}

			/*
			// #1. DockHosting
			const bool isDockHosting = closestFocusableAncestorInclusive.isDockHosting();
			if (isDockHosting == true && (isFocused == true || isDescendantFocused(closestFocusableAncestorInclusive)))
			{
				return true;
			}
			*/

			// #2. Docking
			const bool isDocking = closestFocusableAncestorInclusive.isDocking();
			const ControlData& dockControlData = getControlData(closestFocusableAncestorInclusive.getDockControlHashKey());
			return (isDocking == true && (dockControlData.isRootControl() == true || isControlFocused(dockControlData) == true || isDescendantFocused(dockControlData) == true));
		}

		const bool GuiContext::isDescendantFocused(const ControlData& controlData) const noexcept
		{
			return isDescendantInclusive(controlData, _pressedControlHashKey);
		}

		const bool GuiContext::isDescendantHovered(const ControlData& controlData) const noexcept
		{
			return isDescendantInclusive(controlData, _hoveredControlHashKey);
		}

		const GuiContext::ControlData& GuiContext::getClosestFocusableAncestorInclusiveInternal(const ControlData& controlData) const noexcept
		{
			if (controlData.getHashKey() <= 1)
			{
				// ROOT
				return controlData;
			}

			if (controlData._isFocusable == true)
			{
				return controlData;
			}

			return getClosestFocusableAncestorInclusiveInternal(getControlData(controlData.getParentHashKey()));
		}

		void GuiContext::render()
		{
			FS_ASSERT("김장원", _controlStackPerFrame.empty() == true, "begin 과 end 호출 횟수가 맞지 않습니다!!!");

			_graphicDevice->getWindow()->setCursorType(_cursorType);

			if (_tooltipParentWindowHashKey != 0)
			{
				pushTooltipWindow(_tooltipTextFinal, _tooltipPosition - getControlData(_tooltipParentWindowHashKey)._position + fs::Float2(12.0f, -16.0f));
			}

			// Viewport setting
			_graphicDevice->useScissorRectanglesWithMultipleViewports();
			_graphicDevice->getDxDeviceContext()->RSSetViewports(static_cast<UINT>(_viewportArrayPerFrame.size()), &_viewportArrayPerFrame[0]);
			_graphicDevice->getDxDeviceContext()->RSSetScissorRects(static_cast<UINT>(_scissorRectangleArrayPerFrame.size()), &_scissorRectangleArrayPerFrame[0]);

			// Background
			if (_shapeFontRendererContextBackground.hasData() == true)
			{
				_shapeFontRendererContextBackground.render();
			}
		
			// Foreground
			if (_shapeFontRendererContextForeground.hasData() == true)
			{
				_shapeFontRendererContextForeground.render();
			}

			// TopMost
			if (_shapeFontRendererContextTopMost.hasData() == true)
			{
				_shapeFontRendererContextTopMost.render();
			}
			
			// Viewport setting
			_graphicDevice->useFullScreenViewport();

			_shapeFontRendererContextBackground.flushData();
			_shapeFontRendererContextForeground.flushData();
			_shapeFontRendererContextTopMost.flushData();

			resetStatesPerFrame();
		}

		void GuiContext::resetStatesPerFrame()
		{
			_isMouseInteractionDonePerFrame = false;
			_clickedControlHashKeyPerFrame = 0;

			_controlStackPerFrame.clear();

			_rootControlData.clearPerFrameData();

			_viewportArrayPerFrame.clear();
			_scissorRectangleArrayPerFrame.clear();

			// FullScreen Viewport & ScissorRectangle is at index 0!
			_viewportArrayPerFrame.emplace_back(_viewportFullScreen);
			_scissorRectangleArrayPerFrame.emplace_back(_scissorRectangleFullScreen);

			if (_resizedControlHashKey == 0)
			{
				_cursorType = fs::Window::CursorType::Arrow;
			}

			_keyCode = fs::Window::EventData::KeyCode::NONE;

			// 다음 프레임에 가장 먼저 렌더링 되는 것!!
			renderDock(_rootControlData, _shapeFontRendererContextBackground);
		}
	}
}

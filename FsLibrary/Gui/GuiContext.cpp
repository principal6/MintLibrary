#include <stdafx.h>
#include <FsLibrary/Gui/GuiContext.h>

#include <FsLibrary/Container/StringUtil.hpp>

#include <FsLibrary/SimpleRendering/GraphicDevice.h>

#include <FsLibrary/Platform/WindowsWindow.h>

#include <FsLibrary/Profiler/ScopedCpuProfiler.h>

#include <functional>


namespace fs
{
	namespace Gui
	{
		GuiContext::GuiContext(fs::SimpleRendering::GraphicDevice* const graphicDevice)
			: _graphicDevice{ graphicDevice }
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
			, _tooltipTextFinal{ nullptr }
		{
			getNamedColor(NamedColor::NormalState) = fs::SimpleRendering::Color(45, 47, 49);
			getNamedColor(NamedColor::HoverState) = fs::SimpleRendering::Color(126, 128, 130);
			getNamedColor(NamedColor::PressedState) = fs::SimpleRendering::Color(46, 72, 88);

			getNamedColor(NamedColor::WindowFocused) = fs::SimpleRendering::Color(3, 5, 7);
			getNamedColor(NamedColor::WindowOutOfFocus) = fs::SimpleRendering::Color(3, 5, 7);

			getNamedColor(NamedColor::TitleBarFocused) = getNamedColor(NamedColor::WindowFocused);
			getNamedColor(NamedColor::TitleBarOutOfFocus) = getNamedColor(NamedColor::WindowOutOfFocus);
			//getNamedColor(NamedColor::TitleBarFocused) = fs::SimpleRendering::Color(3, 5, 57);
			//getNamedColor(NamedColor::TitleBarOutOfFocus) = fs::SimpleRendering::Color(3, 5, 107);

			getNamedColor(NamedColor::TooltipBackground) = fs::SimpleRendering::Color(200, 255, 220);

			getNamedColor(NamedColor::ScrollBarTrack) = fs::SimpleRendering::Color(150, 152, 154, 96);
			getNamedColor(NamedColor::ScrollBarThumb) = fs::SimpleRendering::Color(160, 162, 164);

			getNamedColor(NamedColor::LightFont) = fs::SimpleRendering::Color(233, 235, 237);
			getNamedColor(NamedColor::DarkFont) = fs::SimpleRendering::Color(53, 55, 57);
		}

		GuiContext::~GuiContext()
		{
			__noop;
		}

		void GuiContext::initialize(const char* const font)
		{
			const fs::SimpleRendering::FontRendererContext::FontData& fontData = _graphicDevice->getFontRendererContext().getFontData();
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


			fs::Window::WindowsWindow* const windowsWindow = static_cast<fs::Window::WindowsWindow*>(window);
			if (windowsWindow->hasEvent() == true)
			{
				const fs::Window::EventData& eventData = windowsWindow->peekEvent();
				if (eventData._type == fs::Window::EventType::MouseMove)
				{
					_mousePosition = fs::Float2(eventData._data._mousePosition);
				}
				else if (eventData._type == fs::Window::EventType::MouseDown)
				{
					_mouseDownPosition = fs::Float2(eventData._data._mousePosition);
					_mouseButtonDown = true;
				}
				else if (eventData._type == fs::Window::EventType::MouseUp)
				{
					_mouseUpPosition = fs::Float2(eventData._data._mousePosition);
					if (_mouseButtonDown == true)
					{
						_mouseDownUp = true;
					}
					_mouseButtonDown = false;
				}
			}
		}

		const bool GuiContext::isInControlInternal(const fs::Float2& screenPosition, const fs::Float2& controlPosition, const fs::Float2& interactionSize) const noexcept
		{
			const fs::Float2 max = controlPosition + interactionSize;
			if (controlPosition._x <= screenPosition._x && screenPosition._x <= max._x &&
				controlPosition._y <= screenPosition._y && screenPosition._y <= max._y)
			{
				return true;
			}
			return false;
		}

		const bool GuiContext::isInControlInteractionArea(const fs::Float2& screenPosition, const ControlData& controlData) const noexcept
		{
			return isInControlInternal(screenPosition, controlData._position, controlData._interactionSize);
		}

		const bool GuiContext::isInControlBorderArea(const fs::Float2& screenPosition, const ControlData& controlData, fs::Window::CursorType& outCursorType, ResizingMask& outResizingMask, ResizingMethod& outResizingMethod) const noexcept
		{
			const fs::Float2 extendedPosition = controlData._position - fs::Float2(kHalfBorderThickness);
			const fs::Float2 extendedInteractionSize = controlData._interactionSize + fs::Float2(kHalfBorderThickness * 2.0f);
			const fs::Float2 originalMax = controlData._position + controlData._interactionSize;
			if (isInControlInternal(screenPosition, extendedPosition, extendedInteractionSize) == true)
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

		const bool GuiContext::isPositionInRect(const fs::Float2& position, const Rect& rect) const noexcept
		{
			return (rect.left() <= position._x && position._x <= rect.right() && rect.top() <= position._y && position._y <= rect.bottom());
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

			ParamPrepareControlData paramPrepareControlData;
			{
				paramPrepareControlData._initialDisplaySize = windowParam._size;
				paramPrepareControlData._initialResizingMask.setAllTrue();
				paramPrepareControlData._desiredPositionInParent = windowParam._position;
				paramPrepareControlData._innerPadding = Rect(windowInnerPadding);
				paramPrepareControlData._displaySizeMin._x = titleWidth + kTitleBarInnerPadding.left() + kTitleBarInnerPadding.right() + kDefaultRoundButtonRadius * 2.0f;
				paramPrepareControlData._displaySizeMin._y = kTitleBarBaseSize._y + 16.0f;
				paramPrepareControlData._alwaysResetPosition = false;
				paramPrepareControlData._viewportUsage = ViewportUsage::Parent; // ROOT
			}
			nextNoAutoPositioned();
			prepareControlData(windowControlData, paramPrepareControlData);

			const ControlData& parentControlData = getControlData(windowControlData.getParentHashKey());
			const bool isParentAlsoWindow = parentControlData.getControlType() == ControlType::Window;
			if (isParentAlsoWindow == true)
			{
				windowControlData._position += parentControlData._deltaPosition;
				windowControlData._deltaPosition = parentControlData._deltaPosition; // 계층 가장 아래 Window 까지 전파되도록
			}

			fs::SimpleRendering::Color color;
			bool isFocused = processFocusControl(windowControlData, getNamedColor(NamedColor::WindowFocused), getNamedColor(NamedColor::WindowOutOfFocus), color);
			const bool isAncestorFocused_ = isAncestorFocused(windowControlData);
			
			if (isParentAlsoWindow == true)
			{
				// 아래에서 isAncestorFocused_ 만으로 RendererContext 를 판단하게 하기 위해 isFocused = false 로!!!
				isFocused = false;
			}
			fs::SimpleRendering::ShapeFontRendererContext& shapeFontRendererContext = (isFocused || isAncestorFocused_) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;

			// Viewport & Scissor rectangle
			{
				if (isParentAlsoWindow == true)
				{
					windowControlData.setViewportIndexXXX(parentControlData.getChildViewportIndex());
				}

				// ScrollBar state
				const ScrollBarType scrollBarState = static_cast<ScrollBarType>(windowControlData._internalValue._i);
				const bool hasScrollBarVert = (scrollBarState == ScrollBarType::Both || scrollBarState == ScrollBarType::Vert);
				const bool hasScrollBarHorz = (scrollBarState == ScrollBarType::Both || scrollBarState == ScrollBarType::Horz);

				// Viewport & ScissorRectangle for child controls !!!
				windowControlData.setChildViewportIndexXXX(static_cast<uint32>(_viewportArrayPerFrame.size()));

				D3D11_RECT scissorRectangleForChild;
				scissorRectangleForChild.left = static_cast<LONG>(windowControlData._position._x + paramPrepareControlData._innerPadding.left());
				scissorRectangleForChild.top = static_cast<LONG>(windowControlData._position._y + paramPrepareControlData._innerPadding.top() + kTitleBarBaseSize._y);
				scissorRectangleForChild.right = static_cast<LONG>(windowControlData._position._x + windowControlData._displaySize._x - paramPrepareControlData._innerPadding.left() - paramPrepareControlData._innerPadding.right() - ((hasScrollBarVert == true) ? kScrollBarThickness : 0.0f));
				scissorRectangleForChild.bottom = static_cast<LONG>(windowControlData._position._y + windowControlData._displaySize._y - paramPrepareControlData._innerPadding.top() - paramPrepareControlData._innerPadding.bottom() - ((hasScrollBarHorz == true) ? kScrollBarThickness : 0.0f));
				if (isParentAlsoWindow == true)
				{
					const D3D11_RECT& parentScissorRectangle = _scissorRectangleArrayPerFrame[parentControlData.getChildViewportIndex()];
					scissorRectangleForChild.left = fs::max(scissorRectangleForChild.left, parentScissorRectangle.left);
					scissorRectangleForChild.right = fs::min(scissorRectangleForChild.right, parentScissorRectangle.right);
					scissorRectangleForChild.top = fs::max(scissorRectangleForChild.top, parentScissorRectangle.top);
					scissorRectangleForChild.bottom = fs::min(scissorRectangleForChild.bottom, parentScissorRectangle.bottom);

					// ScissorRectangle 에 음수가 들어가는 것 방지!! (중요)
					scissorRectangleForChild.right = fs::max(scissorRectangleForChild.left, scissorRectangleForChild.right);
					scissorRectangleForChild.bottom = fs::max(scissorRectangleForChild.top, scissorRectangleForChild.bottom);
				}
				_scissorRectangleArrayPerFrame.emplace_back(scissorRectangleForChild);
				_viewportArrayPerFrame.emplace_back(_viewportFullScreen);
			}

			const bool isOpen = windowControlData.isControlState(ControlState::VisibleOpen);
			if (isOpen == true)
			{
				shapeFontRendererContext.setViewportIndex(windowControlData.getViewportIndex());
				shapeFontRendererContext.setColor(color);

				const fs::Float4& windowCenterPosition = getControlCenterPosition(windowControlData);
				shapeFontRendererContext.setPosition(windowCenterPosition + fs::Float4(0, titleBarSize._y * 0.5f, 0, 0));
				if (windowControlData.isDocking() == false)
				{
					shapeFontRendererContext.drawHalfRoundedRectangle(windowControlData._displaySize - fs::Float2(0, titleBarSize._y), (kDefaultRoundnessInPixel * 2.0f / windowControlData._displaySize.minElement()), 0.0f);
				}
				else
				{
					shapeFontRendererContext.setColor(getNamedColor(NamedColor::WindowFocused));
					shapeFontRendererContext.drawRectangle(windowControlData._displaySize - fs::Float2(0, titleBarSize._y), 0.0f, 0.0f);
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
			
			ParamPrepareControlData paramPrepareControlData;
			{
				const float textWidth = _shapeFontRendererContextBackground.calculateTextWidth(text, fs::StringUtil::wcslen(text));
				paramPrepareControlData._initialDisplaySize = fs::Float2(textWidth + 24, fs::SimpleRendering::kDefaultFontSize + 12);
			}
			prepareControlData(controlData, paramPrepareControlData);
		
			fs::SimpleRendering::Color color;
			const bool isClicked = processClickControl(controlData, getNamedColor(NamedColor::NormalState), getNamedColor(NamedColor::HoverState), getNamedColor(NamedColor::PressedState), color);
		
			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::SimpleRendering::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused_ == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;
			const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setColor(color);
			shapeFontRendererContext.setPosition(controlCenterPosition);
			const fs::Float2& displaySize = controlData._displaySize;
			shapeFontRendererContext.drawRoundedRectangle(displaySize, (kDefaultRoundnessInPixel * 2.0f / displaySize.minElement()), 0.0f, 0.0f);

			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setTextColor(getNamedColor(NamedColor::LightFont) * fs::SimpleRendering::Color(1.0f, 1.0f, 1.0f, color.a()));
			shapeFontRendererContext.drawDynamicText(text, controlCenterPosition, fs::SimpleRendering::TextRenderDirectionHorz::Centered, fs::SimpleRendering::TextRenderDirectionVert::Centered, kFontScaleB);

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

			ParamPrepareControlData paramPrepareControlData;
			{
				paramPrepareControlData._initialDisplaySize = (labelParam._size == fs::Float2::kZero) ? fs::Float2(textWidth + 24, fs::SimpleRendering::kDefaultFontSize + 12) : labelParam._size;
			}
			prepareControlData(controlData, paramPrepareControlData);
			
			fs::SimpleRendering::Color colorWithAlpha = fs::SimpleRendering::Color(255, 255, 255);
			processShowOnlyControl(controlData, colorWithAlpha);

			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::SimpleRendering::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused_ == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;
			const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setColor(labelParam._backgroundColor);
			shapeFontRendererContext.setPosition(controlCenterPosition);
			const fs::Float2& displaySize = controlData._displaySize;
			shapeFontRendererContext.drawRectangle(displaySize, 0.0f, 0.0f);

			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setTextColor((labelParam._fontColor.isTransparent() == true) ? getNamedColor(NamedColor::LightFont) * colorWithAlpha : labelParam._fontColor);

			fs::Float4 textPosition = controlCenterPosition;
			fs::SimpleRendering::TextRenderDirectionHorz textRenderDirectionHorz = fs::SimpleRendering::TextRenderDirectionHorz::Centered;
			fs::SimpleRendering::TextRenderDirectionVert textRenderDirectionVert = fs::SimpleRendering::TextRenderDirectionVert::Centered;
			if (labelParam._alignmentHorz != TextAlignmentHorz::Middle)
			{
				if (labelParam._alignmentHorz == TextAlignmentHorz::Left)
				{
					textPosition._x = controlData._position._x;
					textRenderDirectionHorz = fs::SimpleRendering::TextRenderDirectionHorz::Rightward;
				}
				else
				{
					textPosition._x = controlData._position._x + controlData._displaySize._x;
					textRenderDirectionHorz = fs::SimpleRendering::TextRenderDirectionHorz::Leftward;
				}
			}
			if (labelParam._alignmentVert != TextAlignmentVert::Center)
			{
				if (labelParam._alignmentVert == TextAlignmentVert::Top)
				{
					textPosition._y = controlData._position._y;
					textRenderDirectionVert = fs::SimpleRendering::TextRenderDirectionVert::Downward;
				}
				else
				{
					textPosition._y = controlData._position._y + controlData._displaySize._y;
					textRenderDirectionVert = fs::SimpleRendering::TextRenderDirectionVert::Upward;
				}
			}
			shapeFontRendererContext.drawDynamicText(text, textPosition, textRenderDirectionHorz, textRenderDirectionVert, kFontScaleB);
		}

		const bool GuiContext::beginSlider(const wchar_t* const name, const SliderParam& SliderParam, float& outValue)
		{
			static constexpr ControlType trackControlType = ControlType::Slider;

			bool isChanged = false;
			const float sliderValidLength = SliderParam._size._x - kSliderThumbRadius * 2.0f;
			ControlData& trackControlData = getControlData(name, trackControlType);
			
			ParamPrepareControlData paramPrepareControlDataTrack;
			{
				paramPrepareControlDataTrack._initialDisplaySize._x = SliderParam._size._x;
				paramPrepareControlDataTrack._initialDisplaySize._y = (0.0f == SliderParam._size._y) ? kSliderThumbRadius * 2.0f : SliderParam._size._y;
			}
			prepareControlData(trackControlData, paramPrepareControlDataTrack);
			
			fs::SimpleRendering::Color trackbColor = getNamedColor(NamedColor::HoverState);
			processShowOnlyControl(trackControlData, trackbColor, true);

			{
				static constexpr ControlType thumbControlType = ControlType::SliderThumb;

				const ControlData& parentWindowControlData = getParentWindowControlData(trackControlData);

				ControlData& thumbControlData = getControlData(name, thumbControlType);
				thumbControlData._position._x = trackControlData._position._x + trackControlData._internalValue._f * sliderValidLength;
				thumbControlData._position._y = trackControlData._position._y + trackControlData._displaySize._y * 0.5f - thumbControlData._displaySize._y * 0.5f;
				thumbControlData._isDraggable = true;
				thumbControlData._draggingConstraints.top(thumbControlData._position._y);
				thumbControlData._draggingConstraints.bottom(thumbControlData._draggingConstraints.top());
				thumbControlData._draggingConstraints.left(trackControlData._position._x);
				thumbControlData._draggingConstraints.right(thumbControlData._draggingConstraints.left() + sliderValidLength);

				ParamPrepareControlData paramPrepareControlDataThumb;
				{
					paramPrepareControlDataThumb._initialDisplaySize._x = kSliderThumbRadius * 2.0f;
					paramPrepareControlDataThumb._initialDisplaySize._y = kSliderThumbRadius * 2.0f;
					paramPrepareControlDataThumb._desiredPositionInParent = trackControlData._position - parentWindowControlData._position;
				}
				nextNoAutoPositioned();
				prepareControlData(thumbControlData, paramPrepareControlDataThumb);
				
				static constexpr fs::SimpleRendering::Color kThumbBaseColor = fs::SimpleRendering::Color(120, 130, 200);
				fs::SimpleRendering::Color thumbColor;
				processScrollableControl(thumbControlData, kThumbBaseColor, kThumbBaseColor.scaleRgb(1.5f), thumbColor);

				const float thumbAt = (thumbControlData._position._x - trackControlData._position._x) / sliderValidLength;
				if (trackControlData._internalValue._f != thumbAt)
				{
					_controlStackPerFrame.emplace_back(ControlStackData(trackControlData));

					isChanged = true;
				}
				trackControlData._internalValue._f = thumbAt;
				outValue = thumbAt;

				const bool isAncestorFocused_ = isAncestorFocused(trackControlData);
				fs::SimpleRendering::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused_ == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;

				// Draw track
				{
					const float trackRadius = kSliderTrackThicknes * 0.5f;
					const float trackRectLength = SliderParam._size._x - trackRadius * 2.0f;
					const float trackRectLeftLength = thumbAt * sliderValidLength;
					const float trackRectRightLength = trackRectLength - trackRectLeftLength;

					const fs::Float4& trackCenterPosition = getControlCenterPosition(trackControlData);
					fs::Float4 trackRenderPosition = trackCenterPosition - fs::Float4(trackRectLength * 0.5f, 0.0f, 0.0f, 0.0f);

					// Left(or Upper) half circle
					shapeFontRendererContext.setViewportIndex(thumbControlData.getViewportIndex());
					shapeFontRendererContext.setColor(kThumbBaseColor);
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
					shapeFontRendererContext.setColor(fs::SimpleRendering::Color::kWhite.scaleA(thumbColor.a()));
					shapeFontRendererContext.drawCircle(kSliderThumbRadius);
					shapeFontRendererContext.setColor(thumbColor);
					shapeFontRendererContext.drawCircle(kSliderThumbRadius - 2.0f);
				}
			}
			
			return isChanged;
		}

		void GuiContext::pushScrollBar(const ScrollBarType scrollBarType)
		{
			static constexpr ControlType trackControlType = ControlType::ScrollBar;
			static std::function fnCalculatePureWindowWidth = [this](const ControlData& windowControlData, const ScrollBarType& scrollBarState)
			{
				return windowControlData._displaySize._x - windowControlData.getInnerPadding().left() - windowControlData.getInnerPadding().right() - ((scrollBarState == ScrollBarType::Both || scrollBarState == ScrollBarType::Vert) ? kScrollBarThickness * 2.0f : 0.0f);
			};
			static std::function fnCalculatePureWindowHeight = [this](const ControlData& windowControlData, const ScrollBarType& scrollBarState)
			{
				return windowControlData._displaySize._y - kTitleBarBaseSize._y - windowControlData.getInnerPadding().top() - windowControlData.getInnerPadding().bottom() - ((scrollBarState == ScrollBarType::Both || scrollBarState == ScrollBarType::Horz) ? kScrollBarThickness * 2.0f : 0.0f);
			};

			ControlData& parentWindowControlData = getControlDataStackTopXXX();
			if (parentWindowControlData.getControlType() != ControlType::Window)
			{
				FS_ASSERT("김장원", false, "ScrollBar 는 현재 Window 에만 장착 가능합니다...");
				return;
			}

			ScrollBarType& parentWindowControlScrollBarState = reinterpret_cast<ScrollBarType&>(parentWindowControlData._internalValue._i);
			const bool isAncestorFocused = isAncestorFocusedInclusiveXXX(parentWindowControlData);
			const fs::Float2& parentWindowPreviousClientSize = parentWindowControlData.getPreviousContentAreaSize();

			// Vertical Track
			if (scrollBarType == ScrollBarType::Vert || scrollBarType == ScrollBarType::Both)
			{
				ControlData& trackControlData = getControlData(generateControlKeyString(parentWindowControlData, L"ScrollBarVertTrack", trackControlType), trackControlType);

				ParamPrepareControlData paramPrepareControlDataTrack;
				{
					paramPrepareControlDataTrack._initialDisplaySize._x = kScrollBarThickness;
					paramPrepareControlDataTrack._initialDisplaySize._y = fnCalculatePureWindowHeight(parentWindowControlData, parentWindowControlScrollBarState);
					paramPrepareControlDataTrack._desiredPositionInParent._x = parentWindowControlData._displaySize._x - kHalfBorderThickness * 2.0f;
					paramPrepareControlDataTrack._desiredPositionInParent._y = kTitleBarBaseSize._y + parentWindowControlData.getInnerPadding().top();
					paramPrepareControlDataTrack._parentHashKeyOverride = parentWindowControlData.getHashKey();
					paramPrepareControlDataTrack._alwaysResetDisplaySize = true;
					paramPrepareControlDataTrack._alwaysResetPosition = true;
					paramPrepareControlDataTrack._ignoreForClientSize = true;
					paramPrepareControlDataTrack._viewportUsage = ViewportUsage::Parent;
				}
				nextNoAutoPositioned();
				prepareControlData(trackControlData, paramPrepareControlDataTrack);

				fs::SimpleRendering::Color trackColor = getNamedColor(NamedColor::ScrollBarTrack);
				processShowOnlyControl(trackControlData, trackColor, true);

				const float parentWindowPureDisplayHeight = fnCalculatePureWindowHeight(parentWindowControlData, parentWindowControlScrollBarState);
				const float extraSize = parentWindowPreviousClientSize._y - parentWindowPureDisplayHeight;
				if (0.0f <= extraSize)
				{
					// Update parent window scrollbar state
					if (parentWindowControlScrollBarState != ScrollBarType::Both)
					{
						parentWindowControlScrollBarState = (parentWindowControlScrollBarState == ScrollBarType::Horz) ? ScrollBarType::Both : ScrollBarType::Vert;
					}

					// Rendering track
					fs::SimpleRendering::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;
					shapeFontRendererContext.setViewportIndex(trackControlData.getViewportIndex());
					shapeFontRendererContext.setColor(trackColor);
					shapeFontRendererContext.drawLine(trackControlData._position, trackControlData._position + fs::Float2(0.0f, trackControlData._displaySize._y), kScrollBarThickness);

					// Thumb
					const float radius = kScrollBarThickness * 0.5f;
					const float thumbSizeRatio = (parentWindowPureDisplayHeight / parentWindowPreviousClientSize._y);
					const float thumbSize = parentWindowPureDisplayHeight * thumbSizeRatio - radius * 2.0f;
					{
						static constexpr ControlType thumbControlType = ControlType::ScrollBarThumb;

						ControlData& thumbControlData = getControlData(generateControlKeyString(parentWindowControlData, L"ScrollBarVertThumb", thumbControlType), thumbControlType);
						const float trackRemnantSize = std::abs(trackControlData._displaySize._y - thumbSize);

						ParamPrepareControlData paramPrepareControlDataThumb;
						{
							paramPrepareControlDataThumb._initialDisplaySize._x = kScrollBarThickness;
							paramPrepareControlDataThumb._initialDisplaySize._y = thumbSize;
							paramPrepareControlDataThumb._desiredPositionInParent._x = paramPrepareControlDataTrack._desiredPositionInParent._x - kScrollBarThickness * 0.5f;
							paramPrepareControlDataThumb._desiredPositionInParent._y = paramPrepareControlDataTrack._desiredPositionInParent._y;
							paramPrepareControlDataThumb._parentHashKeyOverride = parentWindowControlData.getHashKey();
							paramPrepareControlDataThumb._alwaysResetDisplaySize = true;
							paramPrepareControlDataThumb._ignoreForClientSize = true;
							paramPrepareControlDataThumb._viewportUsage = ViewportUsage::Parent;

							thumbControlData._isDraggable = true;
							thumbControlData._draggingConstraints.left(parentWindowControlData._position._x + paramPrepareControlDataThumb._desiredPositionInParent._x);
							thumbControlData._draggingConstraints.right(thumbControlData._draggingConstraints.left());
							thumbControlData._draggingConstraints.top(trackControlData._position._y);
							thumbControlData._draggingConstraints.bottom(trackControlData._position._y + trackRemnantSize);
						}
						nextNoAutoPositioned();
						prepareControlData(thumbControlData, paramPrepareControlDataThumb);

						// @중요
						// Calculate position from internal value
						thumbControlData._position._y = parentWindowControlData._position._y + paramPrepareControlDataTrack._desiredPositionInParent._y + (thumbControlData._internalValue._f * trackRemnantSize);

						fs::SimpleRendering::Color thumbColor;
						processScrollableControl(thumbControlData, getNamedColor(NamedColor::ScrollBarThumb), getNamedColor(NamedColor::ScrollBarThumb).scaleRgb(1.25f), thumbColor);

						const float thumbAtRatio = (trackRemnantSize < 1.0f) ? 0.0f : fs::Math::saturate((thumbControlData._position._y - thumbControlData._draggingConstraints.top()) / trackRemnantSize);
						thumbControlData._internalValue._f = thumbAtRatio;
						parentWindowControlData._displayOffset._y = -thumbAtRatio * (parentWindowPreviousClientSize._y - trackControlData._displaySize._y); // Scrolling!

						// Rendering thumb
						{
							fs::Float4 thumbRenderPosition = fs::Float4(thumbControlData._position._x + kScrollBarThickness * 0.5f, thumbControlData._position._y, 0.0f, 1.0f);
							const float rectHeight = thumbSize - radius * 2.0f;
							thumbRenderPosition._y += radius;
							shapeFontRendererContext.setViewportIndex(thumbControlData.getViewportIndex());
							shapeFontRendererContext.setColor(thumbColor);

							// Upper half circle
							shapeFontRendererContext.setPosition(thumbRenderPosition);
							shapeFontRendererContext.drawHalfCircle(radius, 0.0f);

							// Rect
							if (0.0f < rectHeight)
							{
								thumbRenderPosition._y += rectHeight * 0.5f;
								shapeFontRendererContext.setPosition(thumbRenderPosition);
								shapeFontRendererContext.drawRectangle(thumbControlData._displaySize - fs::Float2(0.0f, radius * 2.0f), 0.0f, 0.0f);
							}

							// Lower half circle
							if (0.0f < rectHeight)
							{
								thumbRenderPosition._y += rectHeight * 0.5f;
							}
							shapeFontRendererContext.setPosition(thumbRenderPosition);
							shapeFontRendererContext.drawHalfCircle(radius, fs::Math::kPi);
						}
					}
				}
				else
				{
					parentWindowControlScrollBarState = (parentWindowControlScrollBarState == ScrollBarType::Vert || parentWindowControlScrollBarState == ScrollBarType::None) ? ScrollBarType::None : ScrollBarType::Horz;
				}
			}

			// Horizontal Track
			if (scrollBarType == ScrollBarType::Horz || scrollBarType == ScrollBarType::Both)
			{
				ControlData& trackControlData = getControlData(generateControlKeyString(parentWindowControlData, L"ScrollBarHorzTrack", trackControlType), trackControlType);
				
				ParamPrepareControlData paramPrepareControlDataTrack;
				{
					paramPrepareControlDataTrack._initialDisplaySize._x = fnCalculatePureWindowWidth(parentWindowControlData, parentWindowControlScrollBarState);
					paramPrepareControlDataTrack._initialDisplaySize._y = kScrollBarThickness;
					paramPrepareControlDataTrack._desiredPositionInParent._x = parentWindowControlData.getInnerPadding().left();
					paramPrepareControlDataTrack._desiredPositionInParent._y = parentWindowControlData._displaySize._y - kHalfBorderThickness * 2.0f;
					paramPrepareControlDataTrack._parentHashKeyOverride = parentWindowControlData.getHashKey();
					paramPrepareControlDataTrack._alwaysResetDisplaySize = true;
					paramPrepareControlDataTrack._alwaysResetPosition = true;
					paramPrepareControlDataTrack._ignoreForClientSize = true;
					paramPrepareControlDataTrack._viewportUsage = ViewportUsage::Parent;
				}
				nextNoAutoPositioned();
				prepareControlData(trackControlData, paramPrepareControlDataTrack);

				fs::SimpleRendering::Color trackColor = getNamedColor(NamedColor::ScrollBarTrack);
				processShowOnlyControl(trackControlData, trackColor, true);

				const float parentWindowPureDisplayWidth = fnCalculatePureWindowWidth(parentWindowControlData, parentWindowControlScrollBarState);
				const float extraSize = parentWindowPreviousClientSize._x - parentWindowPureDisplayWidth;
				if (0.0f <= extraSize)
				{
					// Update parent window scrollbar state
					if (parentWindowControlScrollBarState != ScrollBarType::Both)
					{
						parentWindowControlScrollBarState = (parentWindowControlScrollBarState == ScrollBarType::Vert) ? ScrollBarType::Both : ScrollBarType::Horz;
					}

					// Rendering track
					fs::SimpleRendering::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;
					shapeFontRendererContext.setViewportIndex(trackControlData.getViewportIndex());
					shapeFontRendererContext.setColor(trackColor);
					shapeFontRendererContext.drawLine(trackControlData._position, trackControlData._position + fs::Float2(trackControlData._displaySize._x, 0.0f), kScrollBarThickness);

					// Thumb
					const float radius = kScrollBarThickness * 0.5f;
					const float thumbSizeRatio = (parentWindowPureDisplayWidth / parentWindowPreviousClientSize._x);
					const float thumbSize = parentWindowPureDisplayWidth * thumbSizeRatio - radius * 2.0f;
					{
						static constexpr ControlType thumbControlType = ControlType::ScrollBarThumb;

						ControlData& thumbControlData = getControlData(generateControlKeyString(parentWindowControlData, L"ScrollBarHorzThumb", thumbControlType), thumbControlType);
						const float trackRemnantSize = std::abs(trackControlData._displaySize._x - thumbSize);

						ParamPrepareControlData paramPrepareControlDataThumb;
						{
							paramPrepareControlDataThumb._initialDisplaySize._x = thumbSize;
							paramPrepareControlDataThumb._initialDisplaySize._y = kScrollBarThickness;
							paramPrepareControlDataThumb._desiredPositionInParent._x = paramPrepareControlDataTrack._desiredPositionInParent._x;
							paramPrepareControlDataThumb._desiredPositionInParent._y = paramPrepareControlDataTrack._desiredPositionInParent._y - kScrollBarThickness * 0.5f;
							paramPrepareControlDataThumb._parentHashKeyOverride = parentWindowControlData.getHashKey();
							paramPrepareControlDataThumb._alwaysResetDisplaySize = true;
							paramPrepareControlDataThumb._alwaysResetPosition = false; // 중요!
							paramPrepareControlDataThumb._ignoreForClientSize = true;
							paramPrepareControlDataThumb._viewportUsage = ViewportUsage::Parent;

							thumbControlData._isDraggable = true;
							thumbControlData._draggingConstraints.left(trackControlData._position._x);
							thumbControlData._draggingConstraints.right(trackControlData._position._x + trackRemnantSize);
							thumbControlData._draggingConstraints.top(parentWindowControlData._position._y + paramPrepareControlDataThumb._desiredPositionInParent._y);
							thumbControlData._draggingConstraints.bottom(thumbControlData._draggingConstraints.top());
						}
						nextNoAutoPositioned();
						prepareControlData(thumbControlData, paramPrepareControlDataThumb);

						// @중요
						// Calculate position from internal value
						thumbControlData._position._x = parentWindowControlData._position._x + paramPrepareControlDataTrack._desiredPositionInParent._x + (thumbControlData._internalValue._f * trackRemnantSize);

						fs::SimpleRendering::Color thumbColor;
						processScrollableControl(thumbControlData, getNamedColor(NamedColor::ScrollBarThumb), getNamedColor(NamedColor::ScrollBarThumb).scaleRgb(1.25f), thumbColor);

						const float thumbAtRatio = (trackRemnantSize < 1.0f) ? 0.0f : fs::Math::saturate((thumbControlData._position._x - thumbControlData._draggingConstraints.left()) / trackRemnantSize);
						thumbControlData._internalValue._f = thumbAtRatio;
						parentWindowControlData._displayOffset._x = -thumbAtRatio * (parentWindowPreviousClientSize._x - trackControlData._displaySize._x + ((scrollBarType == ScrollBarType::Both) ? kScrollBarThickness : 0.0f)); // Scrolling!

						// Rendering thumb
						{
							fs::Float4 thumbRenderPosition = fs::Float4(thumbControlData._position._x, thumbControlData._position._y + kScrollBarThickness * 0.5f, 0.0f, 1.0f);
							const float rectHeight = thumbSize - radius * 2.0f;
							thumbRenderPosition._x += radius;
							shapeFontRendererContext.setViewportIndex(thumbControlData.getViewportIndex());
							shapeFontRendererContext.setColor(thumbColor);

							// Upper half circle
							shapeFontRendererContext.setPosition(thumbRenderPosition);
							shapeFontRendererContext.drawHalfCircle(radius, +fs::Math::kPiOverTwo);

							// Rect
							if (0.0f < rectHeight)
							{
								thumbRenderPosition._x += rectHeight * 0.5f;
								shapeFontRendererContext.setPosition(thumbRenderPosition);
								shapeFontRendererContext.drawRectangle(thumbControlData._displaySize - fs::Float2(radius * 2.0f, 0.0f), 0.0f, 0.0f);
							}

							// Lower half circle
							if (0.0f < rectHeight)
							{
								thumbRenderPosition._x += rectHeight * 0.5f;
							}
							shapeFontRendererContext.setPosition(thumbRenderPosition);
							shapeFontRendererContext.drawHalfCircle(radius, -fs::Math::kPiOverTwo);
						}
					}
				}
				else
				{
					parentWindowControlScrollBarState = (parentWindowControlScrollBarState == ScrollBarType::Horz || parentWindowControlScrollBarState == ScrollBarType::None) ? ScrollBarType::None : ScrollBarType::Vert;
				}
			}
		}

		void GuiContext::renderDock(const ControlData& controlData, fs::SimpleRendering::ShapeFontRendererContext& shapeFontRendererContext)
		{
			if (controlData._dockingControlType == DockingControlType::Dock || controlData._dockingControlType == DockingControlType::DockerDock)
			{
				for (DockingMethod dockingMethodIter = static_cast<DockingMethod>(0); dockingMethodIter != DockingMethod::COUNT; dockingMethodIter = static_cast<DockingMethod>(static_cast<uint32>(dockingMethodIter) + 1))
				{
					const DockDatum& dockDatum = controlData.getDockDatum(dockingMethodIter);
					if (dockDatum._dockedControlHashArray.empty() == false)
					{
						const fs::Float2& dockPosition = controlData.getDockPosition(dockingMethodIter);

						//shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
						shapeFontRendererContext.setViewportIndex(controlData.getChildViewportIndex());
						
						//shapeFontRendererContext.setColor(getNamedColor(NamedColor::WindowFocused));
						shapeFontRendererContext.setColor(getNamedColor(NamedColor::PressedState));
						shapeFontRendererContext.setPosition(fs::Float4(dockPosition._x + dockDatum._dockSize._x * 0.5f, dockPosition._y + dockDatum._dockSize._y * 0.5f, 0, 0));

						shapeFontRendererContext.drawRectangle(dockDatum._dockSize, 0.0f, 0.0f);
					}
				}
			}
		}

		void GuiContext::endControlInternal(const ControlType controlType)
		{
			FS_ASSERT("김장원", _controlStackPerFrame.back()._controlType == controlType, "begin 과 end 의 ControlType 이 다릅니다!!!");
			_controlStackPerFrame.pop_back();
		}

		fs::Float2 GuiContext::beginTitleBar(const wchar_t* const windowTitle, const fs::Float2& titleBarSize, const Rect& innerPadding)
		{
			static constexpr ControlType controlType = ControlType::TitleBar;

			ControlData& controlData = getControlData(windowTitle, controlType);
			controlData._isDraggable = true;
			controlData._isFocusable = true;
			controlData._delegateHashKey = controlData.getParentHashKey();
			
			ParamPrepareControlData paramPrepareControlData;
			{
				paramPrepareControlData._initialDisplaySize = titleBarSize;
				paramPrepareControlData._deltaInteractionSize = fs::Float2(-innerPadding.right() - kDefaultRoundButtonRadius * 2.0f, 0.0f);
				paramPrepareControlData._alwaysResetDisplaySize = true;
				paramPrepareControlData._viewportUsage = ViewportUsage::Parent;
			}
			prepareControlData(controlData, paramPrepareControlData);
			
			fs::SimpleRendering::Color titleBarColor;
			const bool isFocused = processFocusControl(controlData, getNamedColor(NamedColor::TitleBarFocused), getNamedColor(NamedColor::TitleBarOutOfFocus), titleBarColor);

			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::SimpleRendering::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused_ == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());

			const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);
			ControlData& parentControlData = getControlData(controlData.getParentHashKey());
			shapeFontRendererContext.setColor(titleBarColor);
			if (parentControlData.isDocking() == true)
			{
				const float textWidth = shapeFontRendererContext.calculateTextWidth(windowTitle, fs::StringUtil::wcslen(windowTitle));
				const fs::Float2& displaySizeOverride = fs::Float2(textWidth + 16.0f, controlData._displaySize._y);

				shapeFontRendererContext.setPosition(controlCenterPosition - fs::Float4((controlData._displaySize._x - displaySizeOverride._x) * 0.5f, 0, 0, 0));
				shapeFontRendererContext.drawRectangle(displaySizeOverride, 0.0f, 0.0f);
			}
			else
			{
				shapeFontRendererContext.setPosition(controlCenterPosition);
				shapeFontRendererContext.drawHalfRoundedRectangle(controlData._displaySize, (kDefaultRoundnessInPixel * 2.0f / controlData._displaySize.minElement()), fs::Math::kPi);

				shapeFontRendererContext.setColor(fs::SimpleRendering::Color(127, 127, 127));
				shapeFontRendererContext.drawLine(controlData._position + fs::Float2(0.0f, titleBarSize._y), controlData._position + fs::Float2(controlData._displaySize._x, titleBarSize._y), 1.0f);
			}

			const fs::Float4& titleBarTextPosition = fs::Float4(controlData._position._x, controlData._position._y, 0.0f, 1.0f) + fs::Float4(innerPadding.left(), titleBarSize._y * 0.5f, 0.0f, 0.0f);
			const bool needToColorOutOfFocus_ = needToColorOutOfFocus(parentControlData);
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setTextColor((needToColorOutOfFocus_ == true) ? getNamedColor(NamedColor::DarkFont) : getNamedColor(NamedColor::LightFont));
			shapeFontRendererContext.drawDynamicText(windowTitle, titleBarTextPosition, fs::SimpleRendering::TextRenderDirectionHorz::Rightward, fs::SimpleRendering::TextRenderDirectionVert::Centered, 0.9375f);

			_controlStackPerFrame.emplace_back(ControlStackData(controlData));

			// Close button
			if (parentControlData.isDocking() == false)
			{
				// 중요
				nextNoAutoPositioned();
				nextControlPosition(fs::Float2(titleBarSize._x - kDefaultRoundButtonRadius * 2.0f - innerPadding.right(), (titleBarSize._y - kDefaultRoundButtonRadius * 2.0f) * 0.5f));

				beginRoundButton(windowTitle, fs::SimpleRendering::Color(1.0f, 0.375f, 0.375f));
				endRoundButton();
			}

			// Window Offset 재조정!!
			parentControlData.setOffsetY_XXX(titleBarSize._y + parentControlData.getInnerPadding().top());

			return titleBarSize;
		}

		const bool GuiContext::beginRoundButton(const wchar_t* const windowTitle, const fs::SimpleRendering::Color& color)
		{
			static constexpr ControlType controlType = ControlType::RoundButton;

			const ControlData& parentWindowData = getParentWindowControlData();

			const float radius = kDefaultRoundButtonRadius;
			ControlData& controlData = getControlData(windowTitle, controlType);

			ParamPrepareControlData paramPrepareControlData;
			{
				paramPrepareControlData._parentHashKeyOverride = parentWindowData.getHashKey();
				paramPrepareControlData._initialDisplaySize = fs::Float2(radius * 2.0f);
				paramPrepareControlData._viewportUsage = ViewportUsage::Parent;
			}
			prepareControlData(controlData, paramPrepareControlData);
			
			fs::SimpleRendering::Color controlColor;
			const bool isClicked = processClickControl(controlData, color, color.scaleRgb(1.5f), color.scaleRgb(0.75f), controlColor);

			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::SimpleRendering::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused_ == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;

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

			ParamPrepareControlData paramPrepareControlData;
			{
				const float tooltipTextWidth = _shapeFontRendererContextForeground.calculateTextWidth(tooltipText, fs::StringUtil::wcslen(tooltipText)) * kTooltipFontScale;
				paramPrepareControlData._initialDisplaySize = fs::Float2(tooltipTextWidth + tooltipWindowPadding * 2.0f, fs::SimpleRendering::kDefaultFontSize * kTooltipFontScale + tooltipWindowPadding);
				paramPrepareControlData._desiredPositionInParent = position;
				paramPrepareControlData._alwaysResetParent = true;
				paramPrepareControlData._alwaysResetDisplaySize = true;
				paramPrepareControlData._alwaysResetPosition = true;
				paramPrepareControlData._parentHashKeyOverride = _tooltipParentWindowHashKey; // ROOT
				paramPrepareControlData._viewportUsage = ViewportUsage::Parent;
			}
			nextNoAutoPositioned();
			prepareControlData(controlData, paramPrepareControlData);
			
			fs::SimpleRendering::Color dummyColor;
			processShowOnlyControl(controlData, dummyColor);

			fs::SimpleRendering::ShapeFontRendererContext& shapeFontRendererContext = _shapeFontRendererContextForeground;

			// Viewport & Scissor rectangle
			{
				controlData.setViewportIndexXXX(static_cast<uint32>(_viewportArrayPerFrame.size()));

				D3D11_RECT scissorRectangleForMe;
				scissorRectangleForMe.left = static_cast<LONG>(controlData._position._x);
				scissorRectangleForMe.top = static_cast<LONG>(controlData._position._y);
				scissorRectangleForMe.right = static_cast<LONG>(scissorRectangleForMe.left + controlData._displaySize._x);
				scissorRectangleForMe.bottom = static_cast<LONG>(scissorRectangleForMe.top + controlData._displaySize._y);
				_scissorRectangleArrayPerFrame.emplace_back(scissorRectangleForMe);
				_viewportArrayPerFrame.emplace_back(_viewportFullScreen);
			}

			{
				const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);
				shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
				shapeFontRendererContext.setColor(getNamedColor(NamedColor::TooltipBackground));
				shapeFontRendererContext.setPosition(controlCenterPosition);
				shapeFontRendererContext.drawRoundedRectangle(controlData._displaySize, (kDefaultRoundnessInPixel / controlData._displaySize.minElement()) * 0.75f, 0.0f, 0.0f);

				const fs::Float4& textPosition = fs::Float4(controlData._position._x, controlData._position._y, 0.0f, 1.0f) + fs::Float4(tooltipWindowPadding, paramPrepareControlData._initialDisplaySize._y * 0.5f, 0.0f, 0.0f);
				shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
				shapeFontRendererContext.setTextColor(getNamedColor(NamedColor::DarkFont));
				shapeFontRendererContext.drawDynamicText(tooltipText, textPosition, fs::SimpleRendering::TextRenderDirectionHorz::Rightward, fs::SimpleRendering::TextRenderDirectionVert::Centered, kTooltipFontScale);
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
			return _controlIdMap[hashKey];
		}

		void GuiContext::calculateControlChildAt(ControlData& controlData) noexcept
		{
			fs::Float2& controlChildAt = const_cast<fs::Float2&>(controlData.getChildAt());
			controlChildAt = controlData._position + ((_nextNoAutoPositioned == false) ? fs::Float2(controlData.getInnerPadding().left(), controlData.getInnerPadding().top()) : fs::Float2::kZero) + controlData._displayOffset;
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

		void GuiContext::prepareControlData(ControlData& controlData, const ParamPrepareControlData& controlDataParam) noexcept
		{
			const bool isNewData = controlData._displaySize.isNan();

			controlData.clearPerFrameData();
			if ((isNewData == true) || (controlDataParam._alwaysResetParent == true))
			{
				const ControlData& stackTopControlData = getControlDataStackTopXXX();
				const uint64 parentHashKey = (controlDataParam._parentHashKeyOverride == 0) ? stackTopControlData.getHashKey() : controlDataParam._parentHashKeyOverride;
				controlData.setParentHashKeyXXX(parentHashKey);

				if (isNewData == true)
				{
					controlData._resizingMask = controlDataParam._initialResizingMask;
				}
			}

			ControlData& parentControlData = getControlData(controlData.getParentHashKey());
			{
				std::vector<ControlData>& parentChildControlArray = const_cast<std::vector<ControlData>&>(parentControlData.getChildControlDataArray());
				parentChildControlArray.emplace_back(controlData);
			}

			// Has child window
			const ControlType controlType = controlData.getControlType();
			if (controlType == ControlType::Window)
			{
				const_cast<bool&>(parentControlData.hasChildWindowInternalXXX()) = true;
			}

			// Inner padding
			{
				Rect& controlInnerPadding = const_cast<Rect&>(controlData.getInnerPadding());
				controlInnerPadding = controlDataParam._innerPadding;
			}

			// Display size, Display size min
			if (isNewData == true || controlDataParam._alwaysResetDisplaySize == true)
			{
				const float maxDisplaySizeX = parentControlData._displaySize._x - ((_nextNoAutoPositioned == false) ? (parentControlData.getInnerPadding().left() * 2.0f) : 0.0f);

				controlData._displaySize._x = (_nextControlSize._x <= 0.0f) ? fs::min(maxDisplaySizeX, controlDataParam._initialDisplaySize._x) :
					((_nextSizingForced == true) ? _nextControlSize._x : fs::min(maxDisplaySizeX, _nextControlSize._x));
				controlData._displaySize._y = (_nextControlSize._y <= 0.0f) ? controlDataParam._initialDisplaySize._y :
					((_nextSizingForced == true) ? _nextControlSize._y : fs::max(controlDataParam._initialDisplaySize._y, _nextControlSize._y));

				fs::Float2& controlDisplaySizeMin = const_cast<fs::Float2&>(controlData.getDisplaySizeMin());
				controlDisplaySizeMin = controlDataParam._displaySizeMin;
			}

			// Interaction size!!!
			controlData._interactionSize = controlData._displaySize + controlDataParam._deltaInteractionSize;

			// Position, Parent offset, Parent child at, Parent client size
			if (_nextNoAutoPositioned == false)
			{
				// Auto-positioned

				float deltaX = 0.0f;
				float deltaY = 0.0f;
				fs::Float2& parentControlChildAt = const_cast<fs::Float2&>(parentControlData.getChildAt());
				fs::Float2& parentControlNextChildOffset = const_cast<fs::Float2&>(parentControlData.getNextChildOffset());
				if (_nextSameLine == true)
				{
					deltaX = (parentControlNextChildOffset._x + kDefaultIntervalX);
					parentControlChildAt._x += deltaX;

					parentControlNextChildOffset = controlData._displaySize;
				}
				else
				{
					if (parentControlData.getContentAreaSize()._x == 0.0f)
					{
						// 맨 처음 추가되는 컨트롤은 SameLine 일 수 없지만 ClientSize 에는 추가되어야 하므로...
						deltaX = controlData._displaySize._x + kDefaultIntervalX;
					}

					parentControlChildAt._x = parentControlData._position._x + parentControlData.getInnerPadding().left() + parentControlData._displayOffset._x; // @중요
					if (0.0f < parentControlNextChildOffset._y)
					{
						deltaY = parentControlNextChildOffset._y;
						parentControlChildAt._y += deltaY;
					}

					parentControlNextChildOffset = controlData._displaySize;
				}

				if (_nextNoAutoPositioned == false)
				{
					parentControlNextChildOffset._y += kDefaultIntervalY;
				}

				// Parent client size
				fs::Float2& parentControlClientSize = const_cast<fs::Float2&>(parentControlData.getContentAreaSize());
				if (controlDataParam._ignoreForClientSize == false)
				{
					parentControlClientSize += fs::Float2(deltaX, deltaY);
				}

				controlData._position = parentControlChildAt;
			}
			else
			{
				// NO Auto-positioned

				if (controlDataParam._alwaysResetPosition == true || isNewData == true)
				{
					controlData._position = parentControlData._position;

					if (controlDataParam._desiredPositionInParent.isNan() == true)
					{
						controlData._position += _nextControlPosition;
					}
					else
					{
						controlData._position += controlDataParam._desiredPositionInParent;
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
				controlData.setViewportIndexXXX((controlDataParam._viewportUsage == ViewportUsage::Parent) ? parentControlData.getViewportIndex() : parentControlData.getChildViewportIndex());
			}
		}

		const bool GuiContext::processClickControl(ControlData& controlData, const fs::SimpleRendering::Color& normalColor, const fs::SimpleRendering::Color& hoverColor, const fs::SimpleRendering::Color& pressedColor, fs::SimpleRendering::Color& outBackgroundColor) noexcept
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

			if (needToColorOutOfFocus(controlData) == true)
			{
				// Out-of-focus alpha
				outBackgroundColor.scaleA(kDefaultOutOfFocusAlpha);
			}

			processControlCommonInternal(controlData);

			return isClicked;
		}

		const bool GuiContext::processFocusControl(ControlData& controlData, const fs::SimpleRendering::Color& focusedColor, const fs::SimpleRendering::Color& nonFocusedColor, fs::SimpleRendering::Color& outBackgroundColor) noexcept
		{
			processControlInteractionInternal(controlData, true);

			const uint64 controlHashKey = (0 != controlData._delegateHashKey) ? controlData._delegateHashKey : controlData.getHashKey();

			// Check new focus
			if (_draggedControlHashKey == 0 && _resizedControlHashKey == 0 && controlData._isFocusable == true && (isControlPressed(controlData) == true || isControlClicked(controlData) == true))
			{
				// Focus entered
				_focusedControlHashKey = controlHashKey;
			}

			bool isFocused = false;
			if (controlHashKey == _focusedControlHashKey)
			{
				// Focused

				outBackgroundColor = focusedColor;
				outBackgroundColor.a(kDefaultFocusedAlpha);

				isFocused = true;
			}
			else
			{
				// Out of focus

				outBackgroundColor = nonFocusedColor;
				outBackgroundColor.a(kDefaultOutOfFocusAlpha);
			}

			processControlCommonInternal(controlData);

			return isFocused;
		}

		void GuiContext::processShowOnlyControl(ControlData& controlData, fs::SimpleRendering::Color& outBackgroundColor, const bool doNotSetMouseInteractionDone) noexcept
		{
			processControlInteractionInternal(controlData, doNotSetMouseInteractionDone);

			if (needToColorOutOfFocus(controlData) == false)
			{
				outBackgroundColor.scaleA(kDefaultFocusedAlpha);
			}
			else if (_focusedControlHashKey != controlData.getHashKey())
			{
				outBackgroundColor.scaleA(kDefaultOutOfFocusAlpha);
			}

			processControlCommonInternal(controlData);
		}

		const bool GuiContext::processScrollableControl(ControlData& controlData, const fs::SimpleRendering::Color& normalColor, const fs::SimpleRendering::Color& dragColor, fs::SimpleRendering::Color& outBackgroundColor) noexcept
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

			if (needToColorOutOfFocus(controlData) == true)
			{
				outBackgroundColor.scaleA(kDefaultOutOfFocusAlpha);
			}

			processControlCommonInternal(controlData);

			return isPressed;
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

			if (_isMouseInteractionDonePerFrame == true)
			{
				return;
			}

			const ControlData& parentControlData = getControlData(controlData.getParentHashKey());
			const bool isMouseInParentInteractionArea = isInControlInteractionArea(_mousePosition, parentControlData);
			const bool isMouseInInteractionArea = isInControlInteractionArea(_mousePosition, controlData);
			if (isMouseInParentInteractionArea == true && isMouseInInteractionArea == true)
			{
				// Hovered

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

				if (_mouseButtonDown == false)
				{
					fnResetPressDataIfMe(controlHashKey);
				}

				if (_mouseButtonDown == true && isMouseDownInInteractionArea == true)
				{
					// Pressed (Mouse down)
					fnResetHoverDataIfMe(controlHashKey);

					if (_pressedControlHashKey != controlHashKey)
					{
						_pressedControlHashKey = controlHashKey;

						_pressedControlInitialPosition = controlData._position;
					}
				}

				if (_mouseDownUp == true && isMouseDownInInteractionArea == true)
				{
					// Clicked (only in interaction area)
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

			if (_nextTooltipText != nullptr && isControlHovered(controlData) == true && _hoverStartTimeMs + 1000 < fs::Profiler::getCurrentTimeMs())
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

					if (changeTargetControlData.getDisplaySizeMin()._x < newDisplaySizeX)
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

					if (changeTargetControlData.getDisplaySizeMin()._y < newDisplaySizeY)
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
					ControlData& changeTargetParentControlData = getControlData(changeTargetControlData.getParentHashKey());
					DockDatum& changeTargetParentControlDockDatum = changeTargetParentControlData.getDockDatum(changeTargetControlData._lastDockingMethod);
					changeTargetParentControlDockDatum._dockSize = changeTargetControlDisplaySize;
				}
				else if (changeTargetControlData._dockingControlType == DockingControlType::Dock || changeTargetControlData._dockingControlType == DockingControlType::DockerDock)
				{
					for (DockingMethod dockingMethodIter = static_cast<DockingMethod>(0); dockingMethodIter != DockingMethod::COUNT; dockingMethodIter = static_cast<DockingMethod>(static_cast<uint32>(dockingMethodIter) + 1))
					{
						const DockDatum& dockDatum = changeTargetControlData.getDockDatum(dockingMethodIter);
						const uint32 dockedControlCount = static_cast<uint32>(dockDatum._dockedControlHashArray.size());
						if (0 < dockedControlCount)
						{
							// Docked Control 의 위치를 갱신해줘야 한다!

							for (uint32 dockedControlIndex = 0; dockedControlIndex < dockedControlCount; ++dockedControlIndex)
							{
								const uint64 dockedControlHashKey = dockDatum._dockedControlHashArray[dockedControlIndex];
								ControlData& dockedControlData = getControlData(dockedControlHashKey);
								dockedControlData._position = changeTargetControlData.getDockPosition(dockingMethodIter);
							}
						}
					}
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

					static constexpr float kDockingBreakDistance = 16.0f;
					const fs::Float2 mouseDeltaPosition = _mousePosition - _mouseDownPosition;
					if (kDockingBreakDistance < std::abs(mouseDeltaPosition._x) || kDockingBreakDistance < std::abs(mouseDeltaPosition._y))
					{
						ControlData& changeTargetParentControlData = getControlData(changeTargetControlData.getParentHashKey());
						DockDatum& changeTargetParentControlDockDatum = changeTargetParentControlData.getDockDatum(changeTargetControlData._lastDockingMethod);
						const uint32 changeTargetParentDockedControlCount = static_cast<uint32>(changeTargetParentControlDockDatum._dockedControlHashArray.size());
						int32 indexToErase = -1;
						for (uint32 iter = 0; iter < changeTargetParentDockedControlCount; ++iter)
						{
							if (changeTargetParentControlDockDatum._dockedControlHashArray[iter] == changeTargetControlData.getHashKey())
							{
								indexToErase = static_cast<int32>(iter);
							}
						}
						if (0 <= indexToErase)
						{
							changeTargetParentControlDockDatum._dockedControlHashArray.erase(changeTargetParentControlDockDatum._dockedControlHashArray.begin() + indexToErase);
						}
						else
						{
							FS_LOG_ERROR("김장원", "Docked Control 이 Parent 의 Child Array 에 없는 상황입니다!!!");
						}

						changeTargetControlData.swapDockingStateContext();

						_draggedControlInitialPosition = changeTargetControlData._position;

						changeTargetControlData.disconnectFromDock();
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
			static constexpr fs::SimpleRendering::Color color = fs::SimpleRendering::Color(100, 110, 160);
			static std::function fnRenderDockingBox = [&](const Rect& boxRect, const ControlData& parentControlData)
			{
				const float horzRenderingCenterOffset = kDockingInteractionShort * 0.5f + kDockingInteractionOffset;
				const float horzBoxWidth = kDockingInteractionShort - kDockingInteractionDisplayBorderThickness * 2.0f;
				const float horzBoxHeight = kDockingInteractionLong - kDockingInteractionDisplayBorderThickness * 2.0f;

				const fs::Float4& parentControlCenterPosition = getControlCenterPosition(parentControlData);
				fs::Float4 renderPosition = parentControlCenterPosition;
				renderPosition._x = boxRect.center()._x;
				renderPosition._y = boxRect.center()._y;
				_shapeFontRendererContextTopMost.setViewportIndex(parentControlData.getViewportIndex());

				const bool isMouseInBoxRect = isPositionInRect(_mousePosition, boxRect);
				_shapeFontRendererContextTopMost.setColor(((isMouseInBoxRect == true) ? color.scaleRgb(1.5f) : color));
				_shapeFontRendererContextTopMost.setPosition(renderPosition);
				_shapeFontRendererContextTopMost.drawRectangle(fs::Float2(horzBoxWidth, horzBoxHeight), kDockingInteractionDisplayBorderThickness, 0.0f);
			};
			static std::function fnRenderPreview = [&](const Rect& previewRect)
			{
				_shapeFontRendererContextBackground.setViewportIndex(0);
				_shapeFontRendererContextBackground.setColor(color.scaleA(0.5f));
				_shapeFontRendererContextBackground.setPosition(fs::Float4(previewRect.center()._x, previewRect.center()._y, 0.0f, 1.0f));
				_shapeFontRendererContextBackground.drawRectangle(previewRect.size(), 0.0f, 0.0f);
			};

			ControlData& parentControlData = getControlData(controlData.getParentHashKey());
			if ((controlData.hasChildWindow() == false) && 
				(controlData._dockingControlType == DockingControlType::Docker || controlData._dockingControlType == DockingControlType::DockerDock) &&
				(parentControlData._dockingControlType == DockingControlType::Dock || parentControlData._dockingControlType == DockingControlType::DockerDock))
			{
				const fs::Float4& parentControlCenterPosition = getControlCenterPosition(parentControlData);
				const float horzInteractionLength = kDockingInteractionShort * 2.0f + kDockingInteractionOffset;
				const float previewShortLengthMax = 160.0f;
				const float previewShortLength = fs::min(parentControlData._displaySize._x * 0.5f, previewShortLengthMax);

				Rect interactionBoxRect;
				Rect previewRect;

				// Left
				if (_mousePosition._x <= parentControlData._position._x + horzInteractionLength)
				{
					interactionBoxRect.left(parentControlData._position._x + kDockingInteractionOffset);
					interactionBoxRect.right(interactionBoxRect.left() + kDockingInteractionShort);
					interactionBoxRect.top(parentControlCenterPosition._y - kDockingInteractionLong * 0.5f);
					interactionBoxRect.bottom(interactionBoxRect.top() + kDockingInteractionLong);

					previewRect.position(parentControlData._position);
					previewRect.right(previewRect.left() + previewShortLength);
					previewRect.bottom(previewRect.top() + parentControlData._displaySize._y);

					if (isDragging == true)
					{
						fnRenderDockingBox(interactionBoxRect, parentControlData);

						DockDatum& parentControlDockDatum = parentControlData.getDockDatum(DockingMethod::LeftSide);
						if (parentControlDockDatum._dockSize != fs::Float2::kZero)
						{
							previewRect.right(previewRect.left() + parentControlDockDatum._dockSize._x);
							previewRect.bottom(previewRect.top() + parentControlDockDatum._dockSize._y);

							fnRenderPreview(previewRect);
						}
						else
						{
							parentControlDockDatum._dockSize = previewRect.size();

							fnRenderPreview(previewRect);
						}

						controlData._lastDockingMethodCandidate = DockingMethod::LeftSide;
					}
				}

				// Right
				if (parentControlData._position._x + parentControlData._displaySize._x - horzInteractionLength <= _mousePosition._x)
				{
					interactionBoxRect.right(parentControlData._position._x + parentControlData._displaySize._x - kDockingInteractionOffset);
					interactionBoxRect.left(interactionBoxRect.right() - kDockingInteractionShort);
					interactionBoxRect.top(parentControlCenterPosition._y - kDockingInteractionLong * 0.5f);
					interactionBoxRect.bottom(interactionBoxRect.top() + kDockingInteractionLong);

					previewRect.right(parentControlData._position._x + parentControlData._displaySize._x);
					previewRect.left(previewRect.right() - previewShortLength);
					previewRect.top(parentControlData._position._y);
					previewRect.bottom(previewRect.top() + parentControlData._displaySize._y);

					if (isDragging == true)
					{
						fnRenderDockingBox(interactionBoxRect, parentControlData);

						DockDatum& parentControlDockDatum = parentControlData.getDockDatum(DockingMethod::RightSide);
						if (parentControlDockDatum._dockSize != fs::Float2::kZero)
						{
							previewRect.left(previewRect.right() - parentControlDockDatum._dockSize._x);
							previewRect.bottom(previewRect.top() + parentControlDockDatum._dockSize._y);

							fnRenderPreview(previewRect);
						}
						else
						{
							parentControlDockDatum._dockSize = previewRect.size();

							fnRenderPreview(previewRect);
						}

						controlData._lastDockingMethodCandidate = DockingMethod::RightSide;
					}
				}

				if (_mouseDownUp == true && isPositionInRect(_mouseUpPosition, interactionBoxRect) == true && controlData._lastDockingMethodCandidate != DockingMethod::COUNT)
				{
					if (controlData.isDocking() == false)
					{
						controlData.swapDockingStateContext();

						if (controlData._lastDockingMethod != controlData._lastDockingMethodCandidate)
						{
							controlData._lastDockingMethod = controlData._lastDockingMethodCandidate;

							controlData._lastDockingMethodCandidate = DockingMethod::COUNT;
						}

						DockDatum& parentControlDockDatum = parentControlData.getDockDatum(controlData._lastDockingMethod);
						if (controlData._lastDockingMethod != controlData._lastDockingMethodCandidate)
						{
							controlData._displaySize = parentControlDockDatum._dockSize;
						}
						parentControlDockDatum._dockedControlHashArray.emplace_back(controlData.getHashKey());

						controlData._resizingMask = ResizingMask::fromDockingMethod(controlData._lastDockingMethod);
						controlData._position = parentControlData.getDockPosition(controlData._lastDockingMethod);
						controlData.connectToDock(parentControlData.getHashKey());

						_draggedControlHashKey = 0;
					}
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
					// 마우스가 Focus Control 과 상호작용할 경우
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

		const bool GuiContext::isAncestor(const uint64 myHashKey, const uint64 ancestorCandidateHashKey) const noexcept
		{
			if (myHashKey == 0)
			{
				return false;
			}

			const uint64 parentHashKey = getControlData(myHashKey).getParentHashKey();
			if (parentHashKey == ancestorCandidateHashKey)
			{
				return true;
			}
			return isAncestor(parentHashKey, ancestorCandidateHashKey);
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

		const bool GuiContext::needToColorOutOfFocus(const ControlData& controlData) const noexcept
		{
			const ControlData& closestFocusableAncestor = getClosestFocusableAncestorInternal(controlData);
			return (closestFocusableAncestor.isDocking() == false && isControlFocused(closestFocusableAncestor) == false);
		}

		const GuiContext::ControlData& GuiContext::getClosestFocusableAncestorInternal(const ControlData& controlData) const noexcept
		{
			if (controlData.getHashKey() == 0)
			{
				// ROOT
				return controlData;
			}

			if (controlData._isFocusable == true)
			{
				return controlData;
			}

			return getClosestFocusableAncestorInternal(getControlData(controlData.getParentHashKey()));
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

			// 다음 프레임에 가장 먼저 렌더링 되는 것!!
			renderDock(_rootControlData, _shapeFontRendererContextBackground);
		}
	}
}

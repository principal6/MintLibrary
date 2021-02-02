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
			, _resizeMethod{ ResizeMethod::ResizeOnly }
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

			getNamedColor(NamedColor::TooltipBackground) = fs::SimpleRendering::Color(255, 255, 160);

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
			_viewportTopMost = _graphicDevice->getFullScreenViewport();
			_scissorRectangleTopMost.left = static_cast<LONG>(0);
			_scissorRectangleTopMost.top = static_cast<LONG>(0);
			_scissorRectangleTopMost.right = static_cast<LONG>(_rootControlData.getDisplaySize()._x);
			_scissorRectangleTopMost.bottom = static_cast<LONG>(_rootControlData.getDisplaySize()._y);

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

		const bool GuiContext::isInControlBorderArea(const fs::Float2& screenPosition, const ControlData& controlData, fs::Window::CursorType& outCursorType, ResizeMethod& outResizeMethod) const noexcept
		{
			const fs::Float2 extendedPosition = controlData._position - fs::Float2(kHalfBorderThickness);
			const fs::Float2 extendedInteractionSize = controlData._interactionSize + fs::Float2(kHalfBorderThickness * 2.0f);
			const fs::Float2 originalMax = controlData._position + controlData._interactionSize;
			if (isInControlInternal(screenPosition, extendedPosition, extendedInteractionSize) == true)
			{
				const bool left = screenPosition._x <= controlData._position._x + kHalfBorderThickness;
				const bool right = originalMax._x - kHalfBorderThickness <= screenPosition._x;
				const bool top = screenPosition._y <= controlData._position._y + kHalfBorderThickness;
				const bool bottom = originalMax._y - kHalfBorderThickness <= screenPosition._y;
				
				const bool leftRight = left || right;
				const bool topBottom = top || bottom;
				const bool leftTopOrRightBottom = (left && top) || (right && bottom);
				const bool leftBottomOrRightTop = (left && bottom) || (right && top);
				
				outResizeMethod = ResizeMethod::ResizeOnly;
				if (left)
				{
					outResizeMethod = ResizeMethod::RepositionHorz;
				}
				if (top)
				{
					outResizeMethod = ResizeMethod::RepositionVert;

					if (left)
					{
						outResizeMethod = ResizeMethod::RepositionBoth;
					}
				}

				if (leftTopOrRightBottom == true)
				{
					outCursorType = fs::Window::CursorType::SizeLeftTilted;
				}
				else if (leftBottomOrRightTop == true)
				{
					outCursorType = fs::Window::CursorType::SizeRightTilted;
				}
				else if (leftRight == true)
				{
					outCursorType = fs::Window::CursorType::SizeHorz;
				}
				else if (topBottom == true)
				{
					outCursorType = fs::Window::CursorType::SizeVert;
				}
				return (leftRight || topBottom);
			}
			return false;
		}

		const bool GuiContext::beginWindow(const wchar_t* const title, const WindowParam& windowParam)
		{
			static constexpr ControlType controlType = ControlType::Window;
			const float windowInnerPadding = 4.0f;

			fs::Float2 titleBarSize = kTitleBarBaseSize;

			// 중요
			nextNoAutoPositioned();

			const float titleWidth = _shapeFontRendererContextForeground.calculateTextWidth(title, fs::StringUtil::wcslen(title));
			ControlDataParam controlDataParam;
			controlDataParam._initialDisplaySize = windowParam._size;
			controlDataParam._desiredPositionInParent = windowParam._position;
			controlDataParam._innerPadding = Rect(windowInnerPadding);
			controlDataParam._displaySizeMin._x = titleWidth + kTitleBarInnerPadding.left() + kTitleBarInnerPadding.right() + kDefaultRoundButtonRadius * 2.0f;
			controlDataParam._displaySizeMin._y = titleBarSize._y + 12.0f;
			controlDataParam._alwaysResetPosition = false;
			controlDataParam._viewportUsage = ViewportUsage::Parent; // ROOT
			ControlData& windowControlData = getControlData(title, controlType, controlDataParam);
			windowControlData._dockingType = DockingType::DockerDock;
			windowControlData._isFocusable = true;
			windowControlData._isResizable = true;

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
				const ScrollBarType scrollBarState = static_cast<ScrollBarType>(windowControlData._value._i);
				const bool hasScrollBarVert = (scrollBarState == ScrollBarType::Both || scrollBarState == ScrollBarType::Vert);
				const bool hasScrollBarHorz = (scrollBarState == ScrollBarType::Both || scrollBarState == ScrollBarType::Horz);

				// Viewport & ScissorRectangle for child controls !!!
				windowControlData.setChildViewportIndexXXX(static_cast<uint32>(_viewportArrayPerFrame.size()));

				D3D11_RECT scissorRectangleForChild;
				scissorRectangleForChild.left = static_cast<LONG>(windowControlData._position._x + controlDataParam._innerPadding.left());
				scissorRectangleForChild.top = static_cast<LONG>(windowControlData._position._y + controlDataParam._innerPadding.top() + kTitleBarBaseSize._y);
				scissorRectangleForChild.right = static_cast<LONG>(windowControlData._position._x + windowControlData.getDisplaySize()._x - controlDataParam._innerPadding.left() - controlDataParam._innerPadding.right() - ((hasScrollBarVert == true) ? kScrollBarThickness : 0.0f));
				scissorRectangleForChild.bottom = static_cast<LONG>(windowControlData._position._y + windowControlData.getDisplaySize()._y - controlDataParam._innerPadding.top() - controlDataParam._innerPadding.bottom() - ((hasScrollBarHorz == true) ? kScrollBarThickness : 0.0f));
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
				_viewportArrayPerFrame.emplace_back(_viewportTopMost);
			}

			const bool isOpen = windowControlData.isControlState(ControlState::VisibleOpen);
			if (isOpen == true)
			{
				const fs::Float4& windowCenterPosition = getControlCenterPosition(windowControlData);
				shapeFontRendererContext.setViewportIndex(windowControlData.getViewportIndex());
				shapeFontRendererContext.setColor(color);
				shapeFontRendererContext.setPosition(windowCenterPosition);
				shapeFontRendererContext.drawRoundedRectangle(windowControlData.getDisplaySize(), (kDefaultRoundnessInPixel * 2.0f / windowControlData.getDisplaySize().minElement()), 0.0f, 0.0f);

				_controlStackPerFrame.emplace_back(ControlStackData(windowControlData));
			}

			// 중요
			nextNoAutoPositioned();

			titleBarSize._x = windowControlData.getDisplaySize()._x;
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
			
			const float textWidth = _shapeFontRendererContextBackground.calculateTextWidth(text, fs::StringUtil::wcslen(text));
			ControlDataParam controlDataParam;
			controlDataParam._initialDisplaySize = fs::Float2(textWidth + 24, fs::SimpleRendering::kDefaultFontSize + 12);
			ControlData& controlData = getControlData(text, controlType, controlDataParam);
			
			fs::SimpleRendering::Color color;
			const bool isClicked = processClickControl(controlData, getNamedColor(NamedColor::NormalState), getNamedColor(NamedColor::HoverState), getNamedColor(NamedColor::PressedState), color);
		
			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::SimpleRendering::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused_ == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;
			const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setColor(color);
			shapeFontRendererContext.setPosition(controlCenterPosition);
			const fs::Float2& displaySize = controlData.getDisplaySize();
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
			ControlDataParam controlDataParam;
			controlDataParam._initialDisplaySize = (labelParam._size == fs::Float2::kZero) ? fs::Float2(textWidth + 24, fs::SimpleRendering::kDefaultFontSize + 12) : labelParam._size;
			ControlData& controlData = getControlData(text, controlType, controlDataParam);

			fs::SimpleRendering::Color colorWithAlpha = fs::SimpleRendering::Color(255, 255, 255);
			processShowOnlyControl(controlData, colorWithAlpha);

			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::SimpleRendering::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused_ == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;
			const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setColor(labelParam._backgroundColor);
			shapeFontRendererContext.setPosition(controlCenterPosition);
			const fs::Float2& displaySize = controlData.getDisplaySize();
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
					textPosition._x = controlData._position._x + controlData.getDisplaySize()._x;
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
					textPosition._y = controlData._position._y + controlData.getDisplaySize()._y;
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
			ControlDataParam trackControlDataParam;
			trackControlDataParam._initialDisplaySize._x = SliderParam._size._x;
			trackControlDataParam._initialDisplaySize._y = (0.0f == SliderParam._size._y) ? kSliderThumbRadius * 2.0f : SliderParam._size._y;
			ControlData& trackControlData = getControlData(name, trackControlType, trackControlDataParam);
			
			fs::SimpleRendering::Color trackbColor = getNamedColor(NamedColor::HoverState);
			processShowOnlyControl(trackControlData, trackbColor, true);

			{
				static constexpr ControlType thumbControlType = ControlType::SliderThumb;

				const ControlData& parentWindowControlData = getParentWindowControlData(trackControlData);

				nextNoAutoPositioned();

				ControlDataParam thumbControlDataParam;
				thumbControlDataParam._initialDisplaySize._x = kSliderThumbRadius * 2.0f;
				thumbControlDataParam._initialDisplaySize._y = kSliderThumbRadius * 2.0f;
				thumbControlDataParam._desiredPositionInParent = trackControlData._position - parentWindowControlData._position;
				ControlData& thumbControlData = getControlData(name, thumbControlType, thumbControlDataParam);
				thumbControlData._isDraggable = true;
				thumbControlData._position._x = trackControlData._position._x + trackControlData._value._f * sliderValidLength;
				thumbControlData._position._y = trackControlData._position._y + trackControlData.getDisplaySize()._y * 0.5f - thumbControlData.getDisplaySize()._y * 0.5f;
				thumbControlData._draggingConstraints.top(thumbControlData._position._y);
				thumbControlData._draggingConstraints.bottom(thumbControlData._draggingConstraints.top());
				thumbControlData._draggingConstraints.left(trackControlData._position._x);
				thumbControlData._draggingConstraints.right(thumbControlData._draggingConstraints.left() + sliderValidLength);

				static constexpr fs::SimpleRendering::Color kThumbBaseColor = fs::SimpleRendering::Color(120, 130, 200);
				fs::SimpleRendering::Color thumbColor;
				processScrollableControl(thumbControlData, kThumbBaseColor, kThumbBaseColor.scaleRgb(1.5f), thumbColor);

				const float thumbAt = (thumbControlData._position._x - trackControlData._position._x) / sliderValidLength;
				if (trackControlData._value._f != thumbAt)
				{
					_controlStackPerFrame.emplace_back(ControlStackData(trackControlData));

					isChanged = true;
				}
				trackControlData._value._f = thumbAt;
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
				return windowControlData.getDisplaySize()._x - windowControlData.getInnerPadding().left() - windowControlData.getInnerPadding().right() - ((scrollBarState == ScrollBarType::Both || scrollBarState == ScrollBarType::Vert) ? kScrollBarThickness * 2.0f : 0.0f);
			};
			static std::function fnCalculatePureWindowHeight = [this](const ControlData& windowControlData, const ScrollBarType& scrollBarState)
			{
				return windowControlData.getDisplaySize()._y - kTitleBarBaseSize._y - windowControlData.getInnerPadding().top() - windowControlData.getInnerPadding().bottom() - ((scrollBarState == ScrollBarType::Both || scrollBarState == ScrollBarType::Horz) ? kScrollBarThickness * 2.0f : 0.0f);
			};

			ControlData& parentWindowControlData = getControlDataStackTopXXX();
			if (parentWindowControlData.getControlType() != ControlType::Window)
			{
				FS_ASSERT("김장원", false, "ScrollBar 는 현재 Window 에만 장착 가능합니다...");
				return;
			}

			ScrollBarType& parentWindowControlScrollBarState = reinterpret_cast<ScrollBarType&>(parentWindowControlData._value._i);
			const bool isAncestorFocused = isAncestorFocusedInclusiveXXX(parentWindowControlData);
			const fs::Float2& parentWindowPreviousClientSize = parentWindowControlData.getPreviousClientSize();

			// Vertical Track
			if (scrollBarType == ScrollBarType::Vert || scrollBarType == ScrollBarType::Both)
			{
				// 중요!
				nextNoAutoPositioned();

				ControlDataParam trackControlDataParam;
				trackControlDataParam._initialDisplaySize._x = kScrollBarThickness;
				trackControlDataParam._initialDisplaySize._y = fnCalculatePureWindowHeight(parentWindowControlData, parentWindowControlScrollBarState);
				trackControlDataParam._desiredPositionInParent._x = parentWindowControlData.getDisplaySize()._x - kHalfBorderThickness * 2.0f;
				trackControlDataParam._desiredPositionInParent._y = kTitleBarBaseSize._y + parentWindowControlData.getInnerPadding().top();
				trackControlDataParam._parentHashKeyOverride = parentWindowControlData.getHashKey();
				trackControlDataParam._alwaysResetDisplaySize = true;
				trackControlDataParam._alwaysResetPosition = true;
				trackControlDataParam._ignoreForClientSize = true;
				trackControlDataParam._viewportUsage = ViewportUsage::Parent;
				ControlData& trackControlData = getControlData(generateControlKeyString(parentWindowControlData, L"ScrollBarVertTrack", trackControlType), trackControlType, trackControlDataParam);

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
					shapeFontRendererContext.drawLine(trackControlData._position, trackControlData._position + fs::Float2(0.0f, trackControlData.getDisplaySize()._y), kScrollBarThickness);

					// Thumb
					const float thumbSizeRatio = (parentWindowPureDisplayHeight / parentWindowPreviousClientSize._y);
					const float thumbSize = parentWindowPureDisplayHeight * thumbSizeRatio;
					{
						static constexpr ControlType thumbControlType = ControlType::ScrollBarThumb;

						// 중요!
						nextNoAutoPositioned();

						ControlDataParam thumbControlDataParam;
						thumbControlDataParam._initialDisplaySize._x = kScrollBarThickness;
						thumbControlDataParam._initialDisplaySize._y = thumbSize;
						thumbControlDataParam._desiredPositionInParent._x = trackControlDataParam._desiredPositionInParent._x - kScrollBarThickness * 0.5f;
						thumbControlDataParam._desiredPositionInParent._y = trackControlDataParam._desiredPositionInParent._y;
						thumbControlDataParam._parentHashKeyOverride = parentWindowControlData.getHashKey();
						thumbControlDataParam._alwaysResetDisplaySize = true;
						thumbControlDataParam._ignoreForClientSize = true;
						thumbControlDataParam._viewportUsage = ViewportUsage::Parent;
						ControlData& thumbControlData = getControlData(generateControlKeyString(parentWindowControlData, L"ScrollBarVertThumb", thumbControlType), thumbControlType, thumbControlDataParam);
						const float radius = kScrollBarThickness * 0.5f;
						const float trackRemnantSize = std::abs(trackControlData.getDisplaySize()._y - thumbSize - radius);
						thumbControlData._isDraggable = true;
						thumbControlData._draggingConstraints.left(parentWindowControlData._position._x + thumbControlDataParam._desiredPositionInParent._x);
						thumbControlData._draggingConstraints.right(thumbControlData._draggingConstraints.left());
						thumbControlData._draggingConstraints.top(trackControlData._position._y);
						thumbControlData._draggingConstraints.bottom(trackControlData._position._y + trackRemnantSize);

						// @중요
						// Calculate position from internal value
						thumbControlData._position._y = parentWindowControlData._position._y + trackControlDataParam._desiredPositionInParent._y + (thumbControlData._value._f * trackRemnantSize);

						fs::SimpleRendering::Color thumbColor;
						processScrollableControl(thumbControlData, getNamedColor(NamedColor::ScrollBarThumb), getNamedColor(NamedColor::ScrollBarThumb).scaleRgb(1.25f), thumbColor);

						const float thumbAtRatio = (trackRemnantSize < 1.0f) ? 0.0f : fs::Math::saturate((thumbControlData._position._y - thumbControlData._draggingConstraints.top()) / trackRemnantSize);
						thumbControlData._value._f = thumbAtRatio;
						parentWindowControlData._displayOffset._y = -thumbAtRatio * (parentWindowPreviousClientSize._y - trackControlData.getDisplaySize()._y); // Scrolling!

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
								shapeFontRendererContext.drawRectangle(thumbControlData.getDisplaySize() - fs::Float2(0.0f, radius * 2.0f), 0.0f, 0.0f);
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
				// 중요!
				nextNoAutoPositioned();

				ControlDataParam trackControlDataParam;
				trackControlDataParam._initialDisplaySize._x = fnCalculatePureWindowWidth(parentWindowControlData, parentWindowControlScrollBarState);
				trackControlDataParam._initialDisplaySize._y = kScrollBarThickness;
				trackControlDataParam._desiredPositionInParent._x = parentWindowControlData.getInnerPadding().left();
				trackControlDataParam._desiredPositionInParent._y = parentWindowControlData.getDisplaySize()._y - kHalfBorderThickness * 2.0f;
				trackControlDataParam._parentHashKeyOverride = parentWindowControlData.getHashKey();
				trackControlDataParam._alwaysResetDisplaySize = true;
				trackControlDataParam._alwaysResetPosition = true;
				trackControlDataParam._ignoreForClientSize = true;
				trackControlDataParam._viewportUsage = ViewportUsage::Parent;
				ControlData& trackControlData = getControlData(generateControlKeyString(parentWindowControlData, L"ScrollBarHorzTrack", trackControlType), trackControlType, trackControlDataParam);

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
					shapeFontRendererContext.drawLine(trackControlData._position, trackControlData._position + fs::Float2(trackControlData.getDisplaySize()._x, 0.0f), kScrollBarThickness);

					// Thumb
					const float thumbSizeRatio = (parentWindowPureDisplayWidth / parentWindowPreviousClientSize._x);
					const float thumbSize = parentWindowPureDisplayWidth * thumbSizeRatio;
					{
						static constexpr ControlType thumbControlType = ControlType::ScrollBarThumb;

						// 중요!
						nextNoAutoPositioned();

						ControlDataParam thumbControlDataParam;
						thumbControlDataParam._initialDisplaySize._x = thumbSize;
						thumbControlDataParam._initialDisplaySize._y = kScrollBarThickness;
						thumbControlDataParam._desiredPositionInParent._x = trackControlDataParam._desiredPositionInParent._x;
						thumbControlDataParam._desiredPositionInParent._y = trackControlDataParam._desiredPositionInParent._y - kScrollBarThickness * 0.5f;
						thumbControlDataParam._parentHashKeyOverride = parentWindowControlData.getHashKey();
						thumbControlDataParam._alwaysResetDisplaySize = true;
						thumbControlDataParam._alwaysResetPosition = false; // 중요!
						thumbControlDataParam._ignoreForClientSize = true;
						thumbControlDataParam._viewportUsage = ViewportUsage::Parent;
						ControlData& thumbControlData = getControlData(generateControlKeyString(parentWindowControlData, L"ScrollBarHorzThumb", thumbControlType), thumbControlType, thumbControlDataParam);
						const float radius = kScrollBarThickness * 0.5f;
						const float trackRemnantSize = std::abs(trackControlData.getDisplaySize()._x - thumbSize - radius);
						thumbControlData._isDraggable = true;
						thumbControlData._draggingConstraints.left(trackControlData._position._x);
						thumbControlData._draggingConstraints.right(trackControlData._position._x + trackRemnantSize);
						thumbControlData._draggingConstraints.top(parentWindowControlData._position._y + thumbControlDataParam._desiredPositionInParent._y);
						thumbControlData._draggingConstraints.bottom(thumbControlData._draggingConstraints.top());

						// @중요
						// Calculate position from internal value
						thumbControlData._position._x = parentWindowControlData._position._x + trackControlDataParam._desiredPositionInParent._x + (thumbControlData._value._f * trackRemnantSize);

						fs::SimpleRendering::Color thumbColor;
						processScrollableControl(thumbControlData, getNamedColor(NamedColor::ScrollBarThumb), getNamedColor(NamedColor::ScrollBarThumb).scaleRgb(1.25f), thumbColor);

						const float thumbAtRatio = (trackRemnantSize < 1.0f) ? 0.0f : fs::Math::saturate((thumbControlData._position._x - thumbControlData._draggingConstraints.left()) / trackRemnantSize);
						thumbControlData._value._f = thumbAtRatio;
						parentWindowControlData._displayOffset._x = -thumbAtRatio * (parentWindowPreviousClientSize._x - trackControlData.getDisplaySize()._x + ((scrollBarType == ScrollBarType::Both) ? kScrollBarThickness : 0.0f)); // Scrolling!

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
								shapeFontRendererContext.drawRectangle(thumbControlData.getDisplaySize() - fs::Float2(radius * 2.0f, 0.0f), 0.0f, 0.0f);
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

		void GuiContext::endControlInternal(const ControlType controlType)
		{
			FS_ASSERT("김장원", _controlStackPerFrame.back()._controlType == controlType, "begin 과 end 의 ControlType 이 다릅니다!!!");
			_controlStackPerFrame.pop_back();
		}

		fs::Float2 GuiContext::beginTitleBar(const wchar_t* const windowTitle, const fs::Float2& titleBarSize, const Rect& innerPadding)
		{
			static constexpr ControlType controlType = ControlType::TitleBar;

			ControlDataParam controlDataParam;
			controlDataParam._initialDisplaySize = titleBarSize;
			controlDataParam._deltaInteractionSize = fs::Float2(-innerPadding.right() - kDefaultRoundButtonRadius * 2.0f, 0.0f);
			controlDataParam._alwaysResetDisplaySize = true;
			controlDataParam._viewportUsage = ViewportUsage::Parent;
			ControlData& controlData = getControlData(windowTitle, controlType, controlDataParam);
			controlData._isDraggable = true;
			controlData._isFocusable = true;
			controlData._delegateHashKey = controlData.getParentHashKey();

			fs::SimpleRendering::Color titleBarColor;
			const bool isFocused = processFocusControl(controlData, getNamedColor(NamedColor::TitleBarFocused), getNamedColor(NamedColor::TitleBarOutOfFocus), titleBarColor);

			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::SimpleRendering::ShapeFontRendererContext& shapeFontRendererContext = (isAncestorFocused_ == true) ? _shapeFontRendererContextForeground : _shapeFontRendererContextBackground;
			
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setColor(fs::SimpleRendering::Color(127, 127, 127));
			shapeFontRendererContext.drawLine(controlData._position + fs::Float2(0.0f, titleBarSize._y), controlData._position + fs::Float2(controlData.getDisplaySize()._x, titleBarSize._y), 1.0f);

			const fs::Float4& titleBarTextPosition = fs::Float4(controlData._position._x, controlData._position._y, 0.0f, 1.0f) + fs::Float4(innerPadding.left(), titleBarSize._y * 0.5f, 0.0f, 0.0f);
			const bool isClosestFocusableAncestorFocused_ = isClosestFocusableAncestorFocused(controlData);
			shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
			shapeFontRendererContext.setTextColor((isClosestFocusableAncestorFocused_ == true) ? getNamedColor(NamedColor::LightFont) : getNamedColor(NamedColor::DarkFont));
			shapeFontRendererContext.drawDynamicText(windowTitle, titleBarTextPosition, fs::SimpleRendering::TextRenderDirectionHorz::Rightward, fs::SimpleRendering::TextRenderDirectionVert::Centered, 0.9375f);

			_controlStackPerFrame.emplace_back(ControlStackData(controlData));

			// Close button
			{
				// 중요
				nextNoAutoPositioned();
				nextControlPosition(fs::Float2(titleBarSize._x - kDefaultRoundButtonRadius * 2.0f - innerPadding.right(), (titleBarSize._y - kDefaultRoundButtonRadius * 2.0f) * 0.5f));

				beginRoundButton(windowTitle, fs::SimpleRendering::Color(1.0f, 0.375f, 0.375f));
				endRoundButton();
			}

			// Window Offset 재조정!!
			ControlData& parentControlData = getControlData(controlData.getParentHashKey());
			parentControlData.setOffsetY_XXX(titleBarSize._y + parentControlData.getInnerPadding().top());

			return titleBarSize;
		}

		const bool GuiContext::beginRoundButton(const wchar_t* const windowTitle, const fs::SimpleRendering::Color& color)
		{
			static constexpr ControlType controlType = ControlType::RoundButton;

			const ControlData& parentWindowData = getParentWindowControlData();

			const float radius = kDefaultRoundButtonRadius;
			ControlDataParam controlDataParam;
			controlDataParam._initialDisplaySize = fs::Float2(radius * 2.0f);
			controlDataParam._viewportUsage = ViewportUsage::Parent;
			controlDataParam._parentHashKeyOverride = parentWindowData.getHashKey();
			ControlData& controlData = getControlData(windowTitle, controlType, controlDataParam);

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

			// 중요
			nextNoAutoPositioned();

			const float tooltipTextWidth = _shapeFontRendererContextForeground.calculateTextWidth(tooltipText, fs::StringUtil::wcslen(tooltipText)) * kTooltipFontScale;
			ControlDataParam controlDataParam;
			controlDataParam._initialDisplaySize = fs::Float2(tooltipTextWidth + tooltipWindowPadding * 2.0f, fs::SimpleRendering::kDefaultFontSize * kTooltipFontScale + tooltipWindowPadding);
			controlDataParam._desiredPositionInParent = position;
			controlDataParam._parentHashKeyOverride = _tooltipParentWindowHashKey; // ROOT
			controlDataParam._alwaysResetParent = true;
			controlDataParam._alwaysResetDisplaySize = true;
			controlDataParam._alwaysResetPosition = true;
			controlDataParam._hashGenerationKeyOverride = L"TooltipWindow";
			controlDataParam._viewportUsage = ViewportUsage::Parent;
			ControlData& controlData = getControlData(tooltipText, controlType, controlDataParam);

			fs::SimpleRendering::Color dummyColor;
			processShowOnlyControl(controlData, dummyColor);

			fs::SimpleRendering::ShapeFontRendererContext& shapeFontRendererContext = _shapeFontRendererContextForeground;

			// Viewport & Scissor rectangle
			{
				controlData.setViewportIndexXXX(static_cast<uint32>(_viewportArrayPerFrame.size()));

				D3D11_RECT scissorRectangleForMe;
				scissorRectangleForMe.left = static_cast<LONG>(controlData._position._x);
				scissorRectangleForMe.top = static_cast<LONG>(controlData._position._y);
				scissorRectangleForMe.right = static_cast<LONG>(scissorRectangleForMe.left + controlData.getDisplaySize()._x);
				scissorRectangleForMe.bottom = static_cast<LONG>(scissorRectangleForMe.top + controlData.getDisplaySize()._y);
				_scissorRectangleArrayPerFrame.emplace_back(scissorRectangleForMe);
				_viewportArrayPerFrame.emplace_back(_viewportTopMost);
			}

			{
				const fs::Float4& controlCenterPosition = getControlCenterPosition(controlData);
				shapeFontRendererContext.setViewportIndex(controlData.getViewportIndex());
				shapeFontRendererContext.setColor(getNamedColor(NamedColor::TooltipBackground));
				shapeFontRendererContext.setPosition(controlCenterPosition);
				shapeFontRendererContext.drawRoundedRectangle(controlData.getDisplaySize(), (kDefaultRoundnessInPixel / controlData.getDisplaySize().minElement()) * 0.75f, 0.0f, 0.0f);

				const fs::Float4& textPosition = fs::Float4(controlData._position._x, controlData._position._y, 0.0f, 1.0f) + fs::Float4(tooltipWindowPadding, controlDataParam._initialDisplaySize._y * 0.5f, 0.0f, 0.0f);
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

		GuiContext::ControlData& GuiContext::getControlData(const wchar_t* const text, const ControlType controlType, const ControlDataParam& controlDataParam) noexcept
		{
			bool isNewData = false;
			const uint64 hashKey = generateControlHashKeyXXX((controlDataParam._hashGenerationKeyOverride == nullptr) ? text : controlDataParam._hashGenerationKeyOverride, controlType);
			auto found = _controlIdMap.find(hashKey);
			if (found == _controlIdMap.end())
			{
				const ControlData& stackTopControlData = getControlDataStackTopXXX();
				const uint64 parentHashKey = (controlDataParam._parentHashKeyOverride == 0) ? stackTopControlData.getHashKey() : controlDataParam._parentHashKeyOverride;
				ControlData newControlData{ hashKey, parentHashKey, controlType };
				_controlIdMap[hashKey] = newControlData;
				
				isNewData = true;
			}
			
			ControlData& controlData = _controlIdMap[hashKey];
			controlData.clearPerFrameData();
			if (controlDataParam._alwaysResetParent == true)
			{
				const ControlData& stackTopControlData = getControlDataStackTopXXX();
				const uint64 parentHashKey = (controlDataParam._parentHashKeyOverride == 0) ? stackTopControlData.getHashKey() : controlDataParam._parentHashKeyOverride;
				controlData.setParentHashKeyXXX(parentHashKey);
			}

			ControlData& parentControlData = getControlData(controlData.getParentHashKey());
			{
				std::vector<ControlData>& parentChildControlArray = const_cast<std::vector<ControlData>&>(parentControlData.getChildControlDataArray());
				parentChildControlArray.emplace_back(controlData);
			}

			if (controlType == ControlType::Window)
			{
				const_cast<bool&>(parentControlData.hasChildWindowInternalXXX()) = true;
			}

			Rect& controlInnerPadding = const_cast<Rect&>(controlData.getInnerPadding());
			fs::Float2& controlDisplaySize = const_cast<fs::Float2&>(controlData.getDisplaySize());
			std::vector<ControlData>& myChildControlArray = const_cast<std::vector<ControlData>&>(controlData.getChildControlDataArray());
			controlInnerPadding = controlDataParam._innerPadding;

			// Display size, Display size min
			if (isNewData == true || controlDataParam._alwaysResetDisplaySize == true)
			{
				const float maxDisplaySizeX = parentControlData.getDisplaySize()._x - ((_nextNoAutoPositioned == false) ? (parentControlData.getInnerPadding().left() * 2.0f) : 0.0f);
				controlDisplaySize._x = (_nextControlSize._x <= 0.0f) ? fs::min(maxDisplaySizeX, controlDataParam._initialDisplaySize._x) :
					((_nextSizingForced == true) ? _nextControlSize._x : fs::min(maxDisplaySizeX, _nextControlSize._x));
				controlDisplaySize._y = (_nextControlSize._y <= 0.0f) ? controlDataParam._initialDisplaySize._y :
					((_nextSizingForced == true) ? _nextControlSize._y : fs::max(controlDataParam._initialDisplaySize._y, _nextControlSize._y));

				fs::Float2& controlDisplaySizeMin = const_cast<fs::Float2&>(controlData.getDisplaySizeMin());
				controlDisplaySizeMin = controlDataParam._displaySizeMin;
			}

			// Interaction size!!!
			controlData._interactionSize = controlDisplaySize + controlDataParam._deltaInteractionSize;

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

					parentControlNextChildOffset = controlData.getDisplaySize();
				}
				else
				{
					if (parentControlData.getClientSize()._x == 0.0f)
					{
						// 맨 처음 추가되는 컨트롤은 SameLine 일 수 없지만 ClientSize 에는 추가되어야 하므로...
						deltaX = controlData.getDisplaySize()._x + kDefaultIntervalX;
					}

					parentControlChildAt._x = parentControlData._position._x + parentControlData.getInnerPadding().left() + parentControlData._displayOffset._x; // @중요
					if (0.0f < parentControlNextChildOffset._y)
					{
						deltaY = parentControlNextChildOffset._y;
						parentControlChildAt._y += deltaY;
					}

					parentControlNextChildOffset = controlData.getDisplaySize();
				}

				if (_nextNoAutoPositioned == false)
				{
					parentControlNextChildOffset._y += kDefaultIntervalY;
				}

				// Parent client size
				fs::Float2& parentControlClientSize = const_cast<fs::Float2&>(parentControlData.getClientSize());
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

			return controlData;
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

			if (isClosestFocusableAncestorFocused(controlData) == false)
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

			if (isClosestFocusableAncestorFocused(controlData) == true)
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

			if (isClosestFocusableAncestorFocused(controlData) == false)
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
				if (controlData._isResizable == true && isInControlBorderArea(_mousePosition, controlData, newCursorType, _resizeMethod) == true)
				{
					_cursorType = newCursorType;

					_isMouseInteractionDonePerFrame = true;
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
					_resizedControlInitialDisplaySize = changeTargetControlData.getDisplaySize();
					
					_isResizeBegun = false;
				}

				fs::Float2& changeTargetControlDisplaySize = const_cast<fs::Float2&>(changeTargetControlData.getDisplaySize());

				const float flipHorz = (_resizeMethod == ResizeMethod::RepositionHorz || _resizeMethod == ResizeMethod::RepositionBoth) ? -1.0f : +1.0f;
				const float flipVert = (_resizeMethod == ResizeMethod::RepositionVert || _resizeMethod == ResizeMethod::RepositionBoth) ? -1.0f : +1.0f;
				if (_cursorType != fs::Window::CursorType::SizeVert)
				{
					if (flipHorz < 0.0f)
					{
						changeTargetControlData._position._x = _resizedControlInitialPosition._x - mouseDeltaPosition._x * flipHorz;
					}

					changeTargetControlDisplaySize._x = fs::max(changeTargetControlData.getDisplaySizeMin()._x, _resizedControlInitialDisplaySize._x + mouseDeltaPosition._x * flipHorz);
				}
				if (_cursorType != fs::Window::CursorType::SizeHorz)
				{
					if (flipVert < 0.0f)
					{
						changeTargetControlData._position._y = _resizedControlInitialPosition._y - mouseDeltaPosition._y * flipVert;
					}

					changeTargetControlDisplaySize._y = fs::max(changeTargetControlData.getDisplaySizeMin()._y, _resizedControlInitialDisplaySize._y + mouseDeltaPosition._y * flipVert);
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
				
				// Set delta position
				changeTargetControlData._deltaPosition = changeTargetControlData._position - originalPosition;

				processControlDocking(changeTargetControlData);

				_isMouseInteractionDonePerFrame = true;
			}

			resetNextStates();
		}

		void GuiContext::processControlDocking(ControlData& controlData) noexcept
		{
			static constexpr fs::SimpleRendering::Color color = fs::SimpleRendering::Color(100, 110, 200);
			//return;

			const ControlData& parentControlData = getControlData(controlData.getParentHashKey());
			if ((controlData.hasChildWindow() == false) && 
				(controlData._dockingType == DockingType::Docker || controlData._dockingType == DockingType::DockerDock) &&
				(parentControlData._dockingType == DockingType::Dock || parentControlData._dockingType == DockingType::DockerDock))
			{
				const fs::Float4& changeTargetParentControlCenterPosition = getControlCenterPosition(parentControlData);

				const float horzInteractionLength = kDockingInteractionShort * 2.0f + kDockingInteractionOffset;
				const float horzRenderingCenterOffset = kDockingInteractionShort * 0.5f + kDockingInteractionOffset;
				const float horzBoxWidth = kDockingInteractionShort - kDockingInteractionDisplayBorderThickness * 2.0f;
				const float horzBoxHeight = kDockingInteractionLong - kDockingInteractionDisplayBorderThickness * 2.0f;

				fs::SimpleRendering::ShapeFontRendererContext& shapeFontRendererContext = _shapeFontRendererContextTopMost;
				shapeFontRendererContext.setViewportIndex(0); // TopMost
				shapeFontRendererContext.setColor(color);

				// Left
				if (_mousePosition._x <= parentControlData._position._x + horzInteractionLength)
				{
					fs::Float4 renderPosition = changeTargetParentControlCenterPosition;
					renderPosition._x += -parentControlData.getDisplaySize()._x * 0.5f + horzRenderingCenterOffset;
					renderPosition._z = 0.0f; // TopMost
					shapeFontRendererContext.setPosition(renderPosition);
					shapeFontRendererContext.drawRectangle(fs::Float2(horzBoxWidth, horzBoxHeight), kDockingInteractionDisplayBorderThickness, 0.0f);
				}

				// Right
				if (parentControlData._position._x + parentControlData.getDisplaySize()._x - horzInteractionLength <= _mousePosition._x)
				{
					fs::Float4 renderPosition = changeTargetParentControlCenterPosition;
					renderPosition._x += +parentControlData.getDisplaySize()._x * 0.5f - horzRenderingCenterOffset;
					renderPosition._z = 0.0f; // TopMost
					shapeFontRendererContext.setPosition(renderPosition);
					shapeFontRendererContext.drawRectangle(fs::Float2(horzBoxWidth, horzBoxHeight), kDockingInteractionDisplayBorderThickness, 0.0f);
				}
			}
		}

		const bool GuiContext::isInteractingInternal(const ControlData& controlData) const noexcept
		{
			if (_focusedControlHashKey != 0 && isAncestorFocusedInclusiveXXX(controlData) == false)
			{
				// Focus 가 있는 Control 이 존재하지만 내가 Focus 는 아닌 경우

				fs::Window::CursorType dummyCursorType;
				ResizeMethod dummyResizeMethod;
				const ControlData& focusedControlData = getControlData(_focusedControlHashKey);
				if (isInControlInteractionArea(_mousePosition, focusedControlData) == true || isInControlBorderArea(_mousePosition, focusedControlData, dummyCursorType, dummyResizeMethod) == true)
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
				if (_draggedControlHashKey != 0 || controlData._isResizable == false || isInteractingInternal(controlData) == false)
				{
					return false;
				}

				fs::Window::CursorType newCursorType;
				if (_mouseButtonDown == true && isInControlBorderArea(_mousePosition, controlData, newCursorType, _resizeMethod) == true && isInControlBorderArea(_mouseDownPosition, controlData, newCursorType, _resizeMethod) == true)
				{
					_resizedControlHashKey = controlData.getHashKey();
					_isResizeBegun = true;
					_cursorType = newCursorType;
					return true;
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

		const bool GuiContext::isClosestFocusableAncestorFocused(const ControlData& controlData) const noexcept
		{
			return isClosestFocusableAncestorFocusedRecursiveXXX(controlData.getParentHashKey());
		}

		const bool GuiContext::isClosestFocusableAncestorFocusedRecursiveXXX(const uint64 hashKey) const noexcept
		{
			if (hashKey == 0)
			{
				return false;
			}

			const bool isFocusable = getControlData(hashKey)._isFocusable;
			if (isFocusable == true)
			{
				return (hashKey == _focusedControlHashKey);
			}

			const uint64 parentHashKey = getControlData(hashKey).getParentHashKey();
			return isClosestFocusableAncestorFocusedRecursiveXXX(parentHashKey);
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
				_graphicDevice->useScissorRectanglesWithMultipleViewports();

				_graphicDevice->getDxDeviceContext()->RSSetViewports(static_cast<UINT>(1), &_viewportTopMost);
				_graphicDevice->getDxDeviceContext()->RSSetScissorRects(static_cast<UINT>(1), &_scissorRectangleTopMost);

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
			_viewportArrayPerFrame.emplace_back(_viewportTopMost);
			_scissorRectangleArrayPerFrame.emplace_back(_scissorRectangleTopMost);

			if (_resizedControlHashKey == 0)
			{
				_cursorType = fs::Window::CursorType::Arrow;
			}
		}
	}
}

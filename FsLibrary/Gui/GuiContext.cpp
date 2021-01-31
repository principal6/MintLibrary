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
			, _shapeRendererBackground{ _graphicDevice }
			, _fontRendererBackground{ _graphicDevice }
			, _shapeRendererForeground{ _graphicDevice }
			, _fontRendererForeground{ _graphicDevice }
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
			if (_fontRendererBackground.loadFont(font) == false)
			{
				_fontRendererBackground.pushGlyphRange(fs::SimpleRendering::GlyphRange(0, 0x33DD));
				_fontRendererBackground.pushGlyphRange(fs::SimpleRendering::GlyphRange(L'°¡', L'ÆR'));
				_fontRendererBackground.bakeFont(font, fs::SimpleRendering::kDefaultFontSize, font, 2048, 1, 1);
				_fontRendererBackground.loadFont(font);
			}

			if (_fontRendererForeground.loadFont(font) == false)
			{
				_fontRendererForeground.pushGlyphRange(fs::SimpleRendering::GlyphRange(0, 0x33DD));
				_fontRendererForeground.pushGlyphRange(fs::SimpleRendering::GlyphRange(L'°¡', L'ÆR'));
				_fontRendererForeground.bakeFont(font, fs::SimpleRendering::kDefaultFontSize, font, 2048, 1, 1);
				_fontRendererForeground.loadFont(font);
			}

			_shapeRendererBackground.initializeShaders();
			_shapeRendererBackground.setUseMultipleViewports();
			_fontRendererBackground.initializeShaders();
			_fontRendererBackground.setUseMultipleViewports();

			_shapeRendererForeground.initializeShaders();
			_shapeRendererForeground.setUseMultipleViewports();
			_fontRendererForeground.initializeShaders();
			_fontRendererForeground.setUseMultipleViewports();

			resetNextStates();
			resetStatesPerFrame();
		}

		void GuiContext::handleEvents(fs::Window::IWindow* const window)
		{
			// ÃÊ±âÈ­
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

			// Áß¿ä
			nextNoAutoPositioned();

			const float titleWidth = _fontRendererForeground.calculateTextWidth(title, fs::StringUtil::wcslen(title));
			ControlDataParam controlDataParam;
			controlDataParam._initialDisplaySize = windowParam._size;
			controlDataParam._desiredPositionInParent = windowParam._position;
			controlDataParam._innerPadding = Rect(windowInnerPadding);
			controlDataParam._displaySizeMin._x = titleWidth + kTitleBarInnerPadding.left() + kTitleBarInnerPadding.right() + kDefaultRoundButtonRadius * 2.0f;
			controlDataParam._displaySizeMin._y = titleBarSize._y + 12.0f;
			controlDataParam._alwaysResetPosition = false;
			ControlData& windowControlData = getControlData(title, controlType, controlDataParam);
			windowControlData._isFocusable = true;
			windowControlData._isResizable = true;
			//windowControlData._value._i = static_cast<uint32>(windowParam._scrollBarType); // Áß¿ä!

			fs::SimpleRendering::Color color;
			const bool isFocused = processFocusControl(windowControlData, getNamedColor(NamedColor::WindowFocused), getNamedColor(NamedColor::WindowOutOfFocus), color);
			fs::SimpleRendering::ShapeRenderer& shapeRenderer = (isFocused == true) ? _shapeRendererForeground : _shapeRendererBackground;

			// Viewport & Scissor rectangle
			{
				std::vector<D3D11_VIEWPORT>& viewportArray = (isFocused == true) ? _viewportArrayForegroundPerFrame : _viewportArrayBackgroundPerFrame;
				std::vector<D3D11_RECT>& scissorRectangleArray = (isFocused == true) ? _scissorRectangleArrayForegroundPerFrame : _scissorRectangleArrayBackgroundPerFrame;

				windowControlData.setViewportIndexXXX(static_cast<uint32>(viewportArray.size()));

				// Window
				D3D11_RECT scissorRectangleForMe;
				scissorRectangleForMe.left = static_cast<LONG>(windowControlData._position._x);
				scissorRectangleForMe.top = static_cast<LONG>(windowControlData._position._y);
				scissorRectangleForMe.right = static_cast<LONG>(scissorRectangleForMe.left + windowControlData.getDisplaySize()._x);
				scissorRectangleForMe.bottom = static_cast<LONG>(scissorRectangleForMe.top + windowControlData.getDisplaySize()._y);
				scissorRectangleArray.emplace_back(scissorRectangleForMe);
				viewportArray.emplace_back(_graphicDevice->getFullScreenViewport());

				// ScrollBar state
				const ScrollBarType scrollBarState = static_cast<ScrollBarType>(windowControlData._value._i);
				const bool hasScrollBarVert = (scrollBarState == ScrollBarType::Both || scrollBarState == ScrollBarType::Vert);
				const bool hasScrollBarHorz = (scrollBarState == ScrollBarType::Both || scrollBarState == ScrollBarType::Horz);

				// Child !!!
				D3D11_RECT scissorRectangleForChild;
				scissorRectangleForChild.left = static_cast<LONG>(windowControlData._position._x + controlDataParam._innerPadding.left());
				scissorRectangleForChild.top = static_cast<LONG>(windowControlData._position._y + controlDataParam._innerPadding.top() + kTitleBarBaseSize._y);
				scissorRectangleForChild.right = static_cast<LONG>(windowControlData._position._x + windowControlData.getDisplaySize()._x - controlDataParam._innerPadding.left() - controlDataParam._innerPadding.right() - ((hasScrollBarVert == true) ? kScrollBarThickness : 0.0f));
				scissorRectangleForChild.bottom = static_cast<LONG>(windowControlData._position._y + windowControlData.getDisplaySize()._y - controlDataParam._innerPadding.top() - controlDataParam._innerPadding.bottom() - ((hasScrollBarHorz == true) ? kScrollBarThickness : 0.0f));
				scissorRectangleArray.emplace_back(scissorRectangleForChild);
				viewportArray.emplace_back(_graphicDevice->getFullScreenViewport());
			}

			const bool isOpen = windowControlData.isControlState(ControlState::VisibleOpen);
			if (isOpen == true)
			{
				const fs::Float3& windowCenterPosition = getControlCenterPosition(windowControlData);
				shapeRenderer.setColor(color);
				shapeRenderer.setPosition(windowCenterPosition);
				shapeRenderer.drawRoundedRectangle(windowControlData.getDisplaySize(), (kDefaultRoundnessInPixel * 2.0f / windowControlData.getDisplaySize().minElement()), 0.0f, 0.0f);

				_controlStackPerFrame.emplace_back(ControlStackData(windowControlData));
			}

			// Áß¿ä
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

		void GuiContext::endWindow()
		{
			FS_ASSERT("±èÀå¿ø", _controlStackPerFrame.back()._controlType == ControlType::Window, "begin °ú end ÀÇ ControlType ÀÌ ´Ù¸¨´Ï´Ù!!!");

			_controlStackPerFrame.pop_back();
		}
		
		const bool GuiContext::beginButton(const wchar_t* const text)
		{
			static constexpr ControlType controlType = ControlType::Button;
			
			const float textWidth = _fontRendererBackground.calculateTextWidth(text, fs::StringUtil::wcslen(text));
			ControlDataParam controlDataParam;
			controlDataParam._initialDisplaySize = fs::Float2(textWidth + 24, fs::SimpleRendering::kDefaultFontSize + 12);
			ControlData& controlData = getControlData(text, controlType, controlDataParam);
			
			fs::SimpleRendering::Color color;
			const bool isClicked = processClickControl(controlData, getNamedColor(NamedColor::NormalState), getNamedColor(NamedColor::HoverState), getNamedColor(NamedColor::PressedState), color);
		
			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::SimpleRendering::ShapeRenderer& shapeRenderer = (isAncestorFocused_ == true) ? _shapeRendererForeground : _shapeRendererBackground;
			fs::SimpleRendering::FontRenderer& fontRenderer = (isAncestorFocused_ == true) ? _fontRendererForeground : _fontRendererBackground;
			const fs::Float3& controlCenterPosition = getControlCenterPosition(controlData);
			shapeRenderer.setColor(color);
			shapeRenderer.setPosition(controlCenterPosition);
			const fs::Float2& displaySize = controlData.getDisplaySize();
			shapeRenderer.drawRoundedRectangle(displaySize, (kDefaultRoundnessInPixel * 2.0f / displaySize.minElement()), 0.0f, 0.0f);

			fontRenderer.setColor(getNamedColor(NamedColor::LightFont) * fs::SimpleRendering::Color(1.0f, 1.0f, 1.0f, color.a()));
			fontRenderer.drawDynamicText(text, controlCenterPosition, fs::SimpleRendering::TextRenderDirectionHorz::Centered, fs::SimpleRendering::TextRenderDirectionVert::Centered, kFontScaleB);

			if (isClicked == true)
			{
				_controlStackPerFrame.emplace_back(ControlStackData(controlData));
			}

			return isClicked;
		}

		void GuiContext::endButton()
		{
			FS_ASSERT("±èÀå¿ø", _controlStackPerFrame.back()._controlType == ControlType::Button, "begin °ú end ÀÇ ControlType ÀÌ ´Ù¸¨´Ï´Ù!!!");
			_controlStackPerFrame.pop_back();
		}

		void GuiContext::pushLabel(const wchar_t* const text, const LabelParam& labelParam)
		{
			static constexpr ControlType controlType = ControlType::Label;

			const float textWidth = _fontRendererBackground.calculateTextWidth(text, fs::StringUtil::wcslen(text));
			ControlDataParam controlDataParam;
			controlDataParam._initialDisplaySize = (labelParam._size == fs::Float2::kZero) ? fs::Float2(textWidth + 24, fs::SimpleRendering::kDefaultFontSize + 12) : labelParam._size;
			ControlData& controlData = getControlData(text, controlType, controlDataParam);

			fs::SimpleRendering::Color colorWithAlpha = fs::SimpleRendering::Color(255, 255, 255);
			processShowOnlyControl(controlData, colorWithAlpha);

			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::SimpleRendering::ShapeRenderer& shapeRenderer = (isAncestorFocused_ == true) ? _shapeRendererForeground : _shapeRendererBackground;
			fs::SimpleRendering::FontRenderer& fontRenderer = (isAncestorFocused_ == true) ? _fontRendererForeground : _fontRendererBackground;
			const fs::Float3& controlCenterPosition = getControlCenterPosition(controlData);
			shapeRenderer.setColor(labelParam._backgroundColor);
			shapeRenderer.setPosition(controlCenterPosition);
			const fs::Float2& displaySize = controlData.getDisplaySize();
			shapeRenderer.drawRectangle(displaySize, 0.0f, 0.0f);

			fontRenderer.setColor((labelParam._fontColor.isTransparent() == true) ? getNamedColor(NamedColor::LightFont) * colorWithAlpha : labelParam._fontColor);

			fs::Float3 textPosition = controlCenterPosition;
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
			fontRenderer.drawDynamicText(text, textPosition, textRenderDirectionHorz, textRenderDirectionVert, kFontScaleB);
		}

		void GuiContext::pushScrollBar(const ScrollBarType scrollBarType)
		{
			static constexpr ControlType trackControlType = ControlType::ScrollBar;
			static std::function fnCalculatePureWindowWidth = [this](const ControlData& windowControlData, const ScrollBarType& scrollBarState)
			{
				return windowControlData.getDisplaySize()._x - windowControlData.getInnerPadding().left() - windowControlData.getInnerPadding().right() - ((scrollBarState == ScrollBarType::Both) ? kScrollBarThickness * 2.0f : 0.0f);
			};
			static std::function fnCalculatePureWindowHeight = [this](const ControlData& windowControlData, const ScrollBarType& scrollBarState)
			{
				return windowControlData.getDisplaySize()._y - kTitleBarBaseSize._y - windowControlData.getInnerPadding().top() - windowControlData.getInnerPadding().bottom() - ((scrollBarState == ScrollBarType::Both) ? kScrollBarThickness * 2.0f : 0.0f);
			};

			ControlData& parentWindowControlData = getControlDataStackTop();
			if (parentWindowControlData.getControlType() != ControlType::Window)
			{
				FS_ASSERT("±èÀå¿ø", false, "ScrollBar ´Â ÇöÀç Window ¿¡¸¸ ÀåÂø °¡´ÉÇÕ´Ï´Ù...");
				return;
			}

			ScrollBarType& parentWindowControlScrollBarState = reinterpret_cast<ScrollBarType&>(parentWindowControlData._value._i);
			const bool isParentWindowFocused = isControlFocused(parentWindowControlData);
			const fs::Float2& parentWindowPreviousClientSize = parentWindowControlData.getPreviousClientSize();

			// Vertical Track
			if (scrollBarType == ScrollBarType::Vert || scrollBarType == ScrollBarType::Both)
			{
				// Áß¿ä!
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
				trackControlDataParam._useParentViewport = true;
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
					fs::SimpleRendering::ShapeRenderer& shapeRenderer = (isParentWindowFocused == true) ? _shapeRendererForeground : _shapeRendererBackground;
					shapeRenderer.setPositionZ(trackControlData.getViewportIndexAsFloat());
					shapeRenderer.setColor(trackColor);
					shapeRenderer.drawLine(trackControlData._position, trackControlData._position + fs::Float2(0.0f, trackControlData.getDisplaySize()._y), kScrollBarThickness);

					// Thumb
					const float thumbSizeRatio = (parentWindowPureDisplayHeight / parentWindowPreviousClientSize._y);
					const float thumbSize = parentWindowPureDisplayHeight * thumbSizeRatio;
					{
						static constexpr ControlType thumbControlType = ControlType::ScrollBarThumb;

						// Áß¿ä!
						nextNoAutoPositioned();

						ControlDataParam thumbControlDataParam;
						thumbControlDataParam._initialDisplaySize._x = kScrollBarThickness;
						thumbControlDataParam._initialDisplaySize._y = thumbSize;
						thumbControlDataParam._desiredPositionInParent._x = trackControlDataParam._desiredPositionInParent._x - kScrollBarThickness * 0.5f;
						thumbControlDataParam._desiredPositionInParent._y = trackControlDataParam._desiredPositionInParent._y;
						thumbControlDataParam._parentHashKeyOverride = parentWindowControlData.getHashKey();
						thumbControlDataParam._alwaysResetDisplaySize = true;
						thumbControlDataParam._ignoreForClientSize = true;
						thumbControlDataParam._useParentViewport = true;
						ControlData& thumbControlData = getControlData(generateControlKeyString(parentWindowControlData, L"ScrollBarVertThumb", thumbControlType), thumbControlType, thumbControlDataParam);
						const float radius = kScrollBarThickness * 0.5f;
						const float trackRemnantSize = std::abs(trackControlData.getDisplaySize()._y - thumbSize - radius);
						thumbControlData._isDraggable = true;
						thumbControlData._draggingConstraints.left(parentWindowControlData._position._x + thumbControlDataParam._desiredPositionInParent._x);
						thumbControlData._draggingConstraints.right(thumbControlData._draggingConstraints.left());
						thumbControlData._draggingConstraints.top(trackControlData._position._y);
						thumbControlData._draggingConstraints.bottom(trackControlData._position._y + trackRemnantSize);

						// @Áß¿ä
						// Calculate position from internal value
						thumbControlData._position._y = parentWindowControlData._position._y + trackControlDataParam._desiredPositionInParent._y + (thumbControlData._value._f * trackRemnantSize);

						fs::SimpleRendering::Color thumbColor;
						processScrollableControl(thumbControlData, getNamedColor(NamedColor::ScrollBarThumb), getNamedColor(NamedColor::ScrollBarThumb).scaleRgb(1.25f), thumbColor);

						const float thumbAtRatio = (trackRemnantSize < 1.0f) ? 0.0f : fs::Math::saturate((thumbControlData._position._y - thumbControlData._draggingConstraints.top()) / trackRemnantSize);
						thumbControlData._value._f = thumbAtRatio;
						parentWindowControlData._displayOffset._y = -thumbAtRatio * (parentWindowPreviousClientSize._y - trackControlData.getDisplaySize()._y); // Scrolling!

						// Rendering thumb
						{
							fs::Float3 thumbRenderPosition = fs::Float3(thumbControlData._position._x + kScrollBarThickness * 0.5f, thumbControlData._position._y, thumbControlData.getViewportIndexAsFloat());
							const float rectHeight = thumbSize - radius * 2.0f;
							thumbRenderPosition._y += radius;
							shapeRenderer.setColor(thumbColor);

							// Upper half circle
							shapeRenderer.setPosition(thumbRenderPosition);
							shapeRenderer.drawHalfCircle(radius, 0.0f);

							// Rect
							if (0.0f < rectHeight)
							{
								thumbRenderPosition._y += rectHeight * 0.5f;
								shapeRenderer.setPosition(thumbRenderPosition);
								shapeRenderer.drawRectangle(thumbControlData.getDisplaySize() - fs::Float2(0.0f, radius * 2.0f), 0.0f, 0.0f);
							}

							// Lower half circle
							if (0.0f < rectHeight)
							{
								thumbRenderPosition._y += rectHeight * 0.5f;
							}
							shapeRenderer.setPosition(thumbRenderPosition);
							shapeRenderer.drawHalfCircle(radius, fs::Math::kPi);
						}
					}
				}
				else
				{
					parentWindowControlScrollBarState = (parentWindowControlScrollBarState == ScrollBarType::Both) ? ScrollBarType::Horz : ScrollBarType::None;
				}
			}

			// Horizontal Track
			if (scrollBarType == ScrollBarType::Horz || scrollBarType == ScrollBarType::Both)
			{
				// Áß¿ä!
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
				trackControlDataParam._useParentViewport = true;
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
					fs::SimpleRendering::ShapeRenderer& shapeRenderer = (isParentWindowFocused == true) ? _shapeRendererForeground : _shapeRendererBackground;
					shapeRenderer.setPositionZ(trackControlData.getViewportIndexAsFloat());
					shapeRenderer.setColor(trackColor);
					shapeRenderer.drawLine(trackControlData._position, trackControlData._position + fs::Float2(trackControlData.getDisplaySize()._x, 0.0f), kScrollBarThickness);

					// Thumb
					const float thumbSizeRatio = (parentWindowPureDisplayWidth / parentWindowPreviousClientSize._x);
					const float thumbSize = parentWindowPureDisplayWidth * thumbSizeRatio;
					{
						static constexpr ControlType thumbControlType = ControlType::ScrollBarThumb;

						// Áß¿ä!
						nextNoAutoPositioned();

						ControlDataParam thumbControlDataParam;
						thumbControlDataParam._initialDisplaySize._x = thumbSize;
						thumbControlDataParam._initialDisplaySize._y = kScrollBarThickness;
						thumbControlDataParam._desiredPositionInParent._x = trackControlDataParam._desiredPositionInParent._x;
						thumbControlDataParam._desiredPositionInParent._y = trackControlDataParam._desiredPositionInParent._y - kScrollBarThickness * 0.5f;
						thumbControlDataParam._parentHashKeyOverride = parentWindowControlData.getHashKey();
						thumbControlDataParam._alwaysResetDisplaySize = true;
						thumbControlDataParam._alwaysResetPosition = false; // Áß¿ä!
						thumbControlDataParam._ignoreForClientSize = true;
						thumbControlDataParam._useParentViewport = true;
						ControlData& thumbControlData = getControlData(generateControlKeyString(parentWindowControlData, L"ScrollBarHorzThumb", thumbControlType), thumbControlType, thumbControlDataParam);
						const float radius = kScrollBarThickness * 0.5f;
						const float trackRemnantSize = std::abs(trackControlData.getDisplaySize()._x - thumbSize - radius);
						thumbControlData._isDraggable = true;
						thumbControlData._draggingConstraints.left(trackControlData._position._x);
						thumbControlData._draggingConstraints.right(trackControlData._position._x + trackRemnantSize);
						thumbControlData._draggingConstraints.top(parentWindowControlData._position._y + thumbControlDataParam._desiredPositionInParent._y);
						thumbControlData._draggingConstraints.bottom(thumbControlData._draggingConstraints.top());

						// @Áß¿ä
						// Calculate position from internal value
						thumbControlData._position._x = parentWindowControlData._position._x + trackControlDataParam._desiredPositionInParent._x + (thumbControlData._value._f * trackRemnantSize);

						fs::SimpleRendering::Color thumbColor;
						processScrollableControl(thumbControlData, getNamedColor(NamedColor::ScrollBarThumb), getNamedColor(NamedColor::ScrollBarThumb).scaleRgb(1.25f), thumbColor);

						const float thumbAtRatio = (trackRemnantSize < 1.0f) ? 0.0f : fs::Math::saturate((thumbControlData._position._x - thumbControlData._draggingConstraints.left()) / trackRemnantSize);
						thumbControlData._value._f = thumbAtRatio;
						parentWindowControlData._displayOffset._x = -thumbAtRatio * (parentWindowPreviousClientSize._x - trackControlData.getDisplaySize()._x + ((scrollBarType == ScrollBarType::Both) ? kScrollBarThickness : 0.0f)); // Scrolling!

						// Rendering thumb
						{
							fs::Float3 thumbRenderPosition = fs::Float3(thumbControlData._position._x, thumbControlData._position._y + kScrollBarThickness * 0.5f, thumbControlData.getViewportIndexAsFloat());
							const float rectHeight = thumbSize - radius * 2.0f;
							thumbRenderPosition._x += radius;
							shapeRenderer.setColor(thumbColor);

							// Upper half circle
							shapeRenderer.setPosition(thumbRenderPosition);
							shapeRenderer.drawHalfCircle(radius, +fs::Math::kPiOverTwo);

							// Rect
							if (0.0f < rectHeight)
							{
								thumbRenderPosition._x += rectHeight * 0.5f;
								shapeRenderer.setPosition(thumbRenderPosition);
								shapeRenderer.drawRectangle(thumbControlData.getDisplaySize() - fs::Float2(radius * 2.0f, 0.0f), 0.0f, 0.0f);
							}

							// Lower half circle
							if (0.0f < rectHeight)
							{
								thumbRenderPosition._x += rectHeight * 0.5f;
							}
							shapeRenderer.setPosition(thumbRenderPosition);
							shapeRenderer.drawHalfCircle(radius, -fs::Math::kPiOverTwo);
						}
					}
				}
				else
				{
					parentWindowControlScrollBarState = (parentWindowControlScrollBarState == ScrollBarType::Both) ? ScrollBarType::Vert : ScrollBarType::None;
				}
			}
		}

		fs::Float2 GuiContext::beginTitleBar(const wchar_t* const windowTitle, const fs::Float2& titleBarSize, const Rect& innerPadding)
		{
			static constexpr ControlType controlType = ControlType::TitleBar;

			ControlDataParam controlDataParam;
			controlDataParam._initialDisplaySize = titleBarSize;
			controlDataParam._deltaInteractionSize = fs::Float2(-innerPadding.right() - kDefaultRoundButtonRadius * 2.0f, 0.0f);
			controlDataParam._alwaysResetDisplaySize = true;
			controlDataParam._useParentViewport = true;
			ControlData& controlData = getControlData(windowTitle, controlType, controlDataParam);
			controlData._isDraggable = true;
			controlData._isFocusable = true;
			controlData._delegateHashKey = controlData.getParentHashKey();

			fs::SimpleRendering::Color titleBarColor;
			const bool isFocused = processFocusControl(controlData, getNamedColor(NamedColor::TitleBarFocused), getNamedColor(NamedColor::TitleBarOutOfFocus), titleBarColor);

			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::SimpleRendering::ShapeRenderer& shapeRenderer = (isAncestorFocused_ == true) ? _shapeRendererForeground : _shapeRendererBackground;
			fs::SimpleRendering::FontRenderer& fontRenderer = (isAncestorFocused_ == true) ? _fontRendererForeground : _fontRendererBackground;
			
			shapeRenderer.setColor(fs::SimpleRendering::Color(127, 127, 127));
			shapeRenderer.drawLine(controlData._position + fs::Float2(0.0f, titleBarSize._y), controlData._position + fs::Float2(controlData.getDisplaySize()._x, titleBarSize._y), 1.0f);

			const fs::Float3& titleBarTextPosition = fs::Float3(controlData._position._x, controlData._position._y, 0.0f) + fs::Float3(innerPadding.left(), titleBarSize._y * 0.5f, controlData.getViewportIndexAsFloat());
			fontRenderer.setColor((isAncestorFocused_ == true) ? getNamedColor(NamedColor::LightFont) : getNamedColor(NamedColor::DarkFont));
			fontRenderer.drawDynamicText(windowTitle, titleBarTextPosition, fs::SimpleRendering::TextRenderDirectionHorz::Rightward, fs::SimpleRendering::TextRenderDirectionVert::Centered, 0.9375f);

			_controlStackPerFrame.emplace_back(ControlStackData(controlData));

			// Close button
			{
				// Áß¿ä
				nextNoAutoPositioned();
				nextControlPosition(fs::Float2(titleBarSize._x - kDefaultRoundButtonRadius * 2.0f - innerPadding.right(), (titleBarSize._y - kDefaultRoundButtonRadius * 2.0f) * 0.5f));

				beginRoundButton(windowTitle, fs::SimpleRendering::Color(1.0f, 0.375f, 0.375f));
				endRoundButton();
			}

			// Window Offset ÀçÁ¶Á¤!!
			ControlData& parentControlData = getControlData(controlData.getParentHashKey());
			parentControlData.setOffsetY_XXX(titleBarSize._y + parentControlData.getInnerPadding().top());

			return titleBarSize;
		}

		void GuiContext::endTitleBar()
		{
			FS_ASSERT("±èÀå¿ø", _controlStackPerFrame.back()._controlType == ControlType::TitleBar, "begin °ú end ÀÇ ControlType ÀÌ ´Ù¸¨´Ï´Ù!!!");
			_controlStackPerFrame.pop_back();
		}

		const bool GuiContext::beginRoundButton(const wchar_t* const windowTitle, const fs::SimpleRendering::Color& color)
		{
			static constexpr ControlType controlType = ControlType::RoundButton;

			const ControlData& parentWindowData = getParentWindowControlData();

			const float radius = kDefaultRoundButtonRadius;
			ControlDataParam controlDataParam;
			controlDataParam._initialDisplaySize = fs::Float2(radius * 2.0f);
			controlDataParam._useParentViewport = true;
			controlDataParam._parentHashKeyOverride = parentWindowData.getHashKey();
			ControlData& controlData = getControlData(windowTitle, controlType, controlDataParam);

			fs::SimpleRendering::Color controlColor;
			const bool isClicked = processClickControl(controlData, color, color.scaleRgb(1.5f), color.scaleRgb(0.75f), controlColor);

			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::SimpleRendering::ShapeRenderer& shapeRenderer = (isAncestorFocused_ == true) ? _shapeRendererForeground : _shapeRendererBackground;

			const fs::Float3& controlCenterPosition = getControlCenterPosition(controlData);
			shapeRenderer.setColor(controlColor);
			shapeRenderer.setPosition(controlCenterPosition);
			shapeRenderer.drawCircle(radius);

			_controlStackPerFrame.emplace_back(ControlStackData(controlData));

			return isClicked;
		}

		void GuiContext::endRoundButton()
		{
			FS_ASSERT("±èÀå¿ø", _controlStackPerFrame.back()._controlType == ControlType::RoundButton, "begin °ú end ÀÇ ControlType ÀÌ ´Ù¸¨´Ï´Ù!!!");
			_controlStackPerFrame.pop_back();
		}

		void GuiContext::pushTooltipWindow(const wchar_t* const tooltipText, const fs::Float2& position)
		{
			static constexpr ControlType controlType = ControlType::TooltipWindow;
			static constexpr float kTooltipFontScale = kFontScaleC;
			const float tooltipWindowPadding = 8.0f;

			// Áß¿ä
			nextNoAutoPositioned();

			const float tooltipTextWidth = _fontRendererForeground.calculateTextWidth(tooltipText, fs::StringUtil::wcslen(tooltipText)) * kTooltipFontScale;
			ControlDataParam controlDataParam;
			controlDataParam._initialDisplaySize = fs::Float2(tooltipTextWidth + tooltipWindowPadding * 2.0f, fs::SimpleRendering::kDefaultFontSize * kTooltipFontScale + tooltipWindowPadding);
			controlDataParam._desiredPositionInParent = position;
			controlDataParam._parentHashKeyOverride = _tooltipParentWindowHashKey; // ROOT
			controlDataParam._alwaysResetParent = true;
			controlDataParam._alwaysResetDisplaySize = true;
			controlDataParam._alwaysResetPosition = true;
			controlDataParam._hashGenerationKeyOverride = L"TooltipWindow";
			ControlData& controlData = getControlData(tooltipText, controlType, controlDataParam);

			fs::SimpleRendering::Color dummyColor;
			processShowOnlyControl(controlData, dummyColor);
			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::SimpleRendering::ShapeRenderer& shapeRenderer = _shapeRendererForeground;
			fs::SimpleRendering::FontRenderer& fontRenderer = _fontRendererForeground;

			// Viewport & Scissor rectangle
			{
				std::vector<D3D11_VIEWPORT>& viewportArray = _viewportArrayForegroundPerFrame;
				std::vector<D3D11_RECT>& scissorRectangleArray = _scissorRectangleArrayForegroundPerFrame;

				controlData.setViewportIndexXXX(static_cast<uint32>(viewportArray.size()));

				D3D11_RECT scissorRectangleForMe;
				scissorRectangleForMe.left = static_cast<LONG>(controlData._position._x);
				scissorRectangleForMe.top = static_cast<LONG>(controlData._position._y);
				scissorRectangleForMe.right = static_cast<LONG>(scissorRectangleForMe.left + controlData.getDisplaySize()._x);
				scissorRectangleForMe.bottom = static_cast<LONG>(scissorRectangleForMe.top + controlData.getDisplaySize()._y);
				scissorRectangleArray.emplace_back(scissorRectangleForMe);
				viewportArray.emplace_back(_graphicDevice->getFullScreenViewport());
			}

			{
				const fs::Float3& controlCenterPosition = getControlCenterPosition(controlData);
				shapeRenderer.setColor(getNamedColor(NamedColor::TooltipBackground));
				shapeRenderer.setPosition(controlCenterPosition);
				shapeRenderer.drawRoundedRectangle(controlData.getDisplaySize(), (kDefaultRoundnessInPixel / controlData.getDisplaySize().minElement()) * 0.75f, 0.0f, 0.0f);

				const fs::Float3& textPosition = fs::Float3(controlData._position._x, controlData._position._y, 0.0f) + fs::Float3(tooltipWindowPadding, controlDataParam._initialDisplaySize._y * 0.5f, controlData.getViewportIndexAsFloat());
				fontRenderer.setColor(getNamedColor(NamedColor::DarkFont));
				fontRenderer.drawDynamicText(tooltipText, textPosition, fs::SimpleRendering::TextRenderDirectionHorz::Rightward, fs::SimpleRendering::TextRenderDirectionVert::Centered, kTooltipFontScale);
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
				const ControlData& stackTopControlData = getControlDataStackTop();
				const uint64 parentHashKey = (controlDataParam._parentHashKeyOverride == 0) ? stackTopControlData.getHashKey() : controlDataParam._parentHashKeyOverride;
				ControlData newControlData{ hashKey, parentHashKey, controlType };
				_controlIdMap[hashKey] = newControlData;
				
				isNewData = true;
			}
			
			ControlData& controlData = _controlIdMap[hashKey];
			if (controlDataParam._alwaysResetParent == true)
			{
				const ControlData& stackTopControlData = getControlDataStackTop();
				const uint64 parentHashKey = (controlDataParam._parentHashKeyOverride == 0) ? stackTopControlData.getHashKey() : controlDataParam._parentHashKeyOverride;
				controlData.setParentHashKeyXXX(parentHashKey);
			}
			ControlData& parentControlData = getControlData(controlData.getParentHashKey());

			Rect& controlInnerPadding = const_cast<Rect&>(controlData.getInnerPadding());
			fs::Float2& controlDisplaySize = const_cast<fs::Float2&>(controlData.getDisplaySize());
			fs::Float2& controlNextChildOffset = const_cast<fs::Float2&>(controlData.getNextChildOffset());
			fs::Float2& controlClientSize = const_cast<fs::Float2&>(controlData.getClientSize());
			fs::Float2& controlPreviousClientSize = const_cast<fs::Float2&>(controlData.getPreviousClientSize());
			controlInnerPadding = controlDataParam._innerPadding;
			controlNextChildOffset.setZero();
			controlPreviousClientSize = controlClientSize;
			controlClientSize.setZero();

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

					parentControlNextChildOffset._x = fs::max(parentControlNextChildOffset._x, controlData.getDisplaySize()._x);
					parentControlNextChildOffset._y = fs::max(parentControlNextChildOffset._y, controlData.getDisplaySize()._y);
				}
				else
				{
					if (parentControlData.getClientSize()._x == 0.0f)
					{
						// ¸Ç Ã³À½ Ãß°¡µÇ´Â ÄÁÆ®·ÑÀº SameLine ÀÏ ¼ö ¾øÁö¸¸ ClientSize ¿¡´Â Ãß°¡µÇ¾î¾ß ÇÏ¹Ç·Î...
						deltaX = controlData.getDisplaySize()._x + kDefaultIntervalX;
					}

					parentControlChildAt._x = parentControlData._position._x + parentControlData.getInnerPadding().left() + parentControlData._displayOffset._x; // @Áß¿ä
					if (0.0f < parentControlNextChildOffset._y)
					{
						deltaY = parentControlNextChildOffset._y;
						parentControlChildAt._y += deltaY;
					}

					parentControlNextChildOffset = controlData.getDisplaySize();

					if (_nextNoAutoPositioned == false)
					{
						parentControlNextChildOffset._y += kDefaultIntervalY;
					}
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
						
					if (controlDataParam._desiredPositionInParent == fs::Float2::kZero)
					{
						controlData._position += _nextControlPosition;
					}
					else
					{
						controlData._position += controlDataParam._desiredPositionInParent;
					}
				}
			}

			// Drag constraints Àû¿ë! (Dragging ÀÌ ¾Æ´Ò ¶§µµ Constraint °¡ Àû¿ëµÇ¾î¾ß ÇÔ.. ¿¹¸¦ µé¸é resizing Áß¿¡!)
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
				if (controlData.getControlType() != ControlType::TooltipWindow)
				{
					const bool isParentControlWindow = (parentControlData.getControlType() == ControlType::Window);
					controlData.setViewportIndexXXX((isParentControlWindow == true && controlDataParam._useParentViewport == false) ? parentControlData.getViewportIndex() + 1 : parentControlData.getViewportIndex());
				}
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
			FS_ASSERT("±èÀå¿ø", _controlStackPerFrame.empty() == false, "Control ½ºÅÃÀÌ ºñ¾îÀÖÀ» ¶§ È£ÃâµÇ¸é ¾È µË´Ï´Ù!!!");

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

			if (isAncestorFocused(controlData) == false)
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
			if (_draggedControlHashKey == 0 && _resizedControlHashKey == 0 && controlData._isFocusable == true && isControlClicked(controlData) == true)
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

			if (isAncestorFocused(controlData) == true)
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

			if (isAncestorFocused(controlData) == false)
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
					_hoverStartTimeMs = 0;
					_hoverStarted = false;
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
				
				if (_hoveredControlHashKey != controlHashKey)
				{
					_hoveredControlHashKey = controlHashKey;
					_hoverStartTimeMs = fs::Profiler::getCurrentTimeMs();
					_hoverStarted = true;
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
					_clickedControlHashKeyPerFrame = controlHashKey;
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

				_isMouseInteractionDonePerFrame = true;
			}

			resetNextStates();
		}

		const bool GuiContext::isInteractingInternal(const ControlData& controlData) const noexcept
		{
			if (_focusedControlHashKey != 0 && isMeOrAncestorFocusedXXX(controlData) == false)
			{
				// Focus °¡ ÀÖ´Â Control ÀÌ Á¸ÀçÇÏÁö¸¸ ³»°¡ Focus ´Â ¾Æ´Ñ °æ¿ì

				fs::Window::CursorType dummyCursorType;
				ResizeMethod dummyResizeMethod;
				const ControlData& focusedControlData = getControlData(_focusedControlHashKey);
				if (isInControlInteractionArea(_mousePosition, focusedControlData) == true || isInControlBorderArea(_mousePosition, focusedControlData, dummyCursorType, dummyResizeMethod) == true)
				{
					// ¸¶¿ì½º°¡ Focus Control °ú »óÈ£ÀÛ¿ëÇÒ °æ¿ì
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
					// Drag ½ÃÀÛ
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
		
		const bool GuiContext::isControlHovered(const ControlData& controlData) const noexcept
		{
			return (_hoveredControlHashKey == controlData.getHashKey());
		}

		const bool GuiContext::isControlPressed(const ControlData& controlData) const noexcept
		{
			return (_pressedControlHashKey == controlData.getHashKey());
		}

		const bool GuiContext::isControlClicked(const ControlData& controlData) const noexcept
		{
			return (_clickedControlHashKeyPerFrame == controlData.getHashKey());
		}

		const bool GuiContext::isControlFocused(const ControlData& controlData) const noexcept
		{
			return (_focusedControlHashKey == controlData.getHashKey());
		}

		const bool GuiContext::isMeOrAncestorFocusedXXX(const ControlData& controlData) const noexcept
		{
			if (_focusedControlHashKey == controlData.getHashKey())
			{
				return true;
			}
			return isAncestorFocused(controlData);
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

			return isAncestorFocusedRecursiveXXX(getControlData(hashKey).getParentHashKey());
		}

		const fs::SimpleRendering::Color& GuiContext::getNamedColor(const NamedColor namedColor) const noexcept
		{
			return _namedColors[static_cast<uint32>(namedColor)];
		}

		fs::SimpleRendering::Color& GuiContext::getNamedColor(const NamedColor namedColor) noexcept
		{
			return _namedColors[static_cast<uint32>(namedColor)];
		}

		void GuiContext::render()
		{
			FS_ASSERT("±èÀå¿ø", _controlStackPerFrame.empty() == true, "begin °ú end È£Ãâ È½¼ö°¡ ¸ÂÁö ¾Ê½À´Ï´Ù!!!");

			_graphicDevice->getWindow()->setCursorType(_cursorType);

			if (_tooltipParentWindowHashKey != 0)
			{
				pushTooltipWindow(_tooltipTextFinal, _tooltipPosition - getControlData(_tooltipParentWindowHashKey)._position + fs::Float2(12.0f, -16.0f));
			}

			const bool useBackgroundViewports = (0 < _viewportArrayBackgroundPerFrame.size());
			const bool useForegroundViewports = (0 < _viewportArrayForegroundPerFrame.size());
			const bool shouldSetViewports = (useBackgroundViewports || useForegroundViewports);
			if (shouldSetViewports == true)
			{
				_graphicDevice->useScissorRectanglesWithMultipleViewports();
			}

			// Background
			{
				if (useBackgroundViewports == true)
				{
					_graphicDevice->getDxDeviceContext()->RSSetViewports(static_cast<UINT>(_viewportArrayBackgroundPerFrame.size()), &_viewportArrayBackgroundPerFrame[0]);
					_graphicDevice->getDxDeviceContext()->RSSetScissorRects(static_cast<UINT>(_scissorRectangleArrayBackgroundPerFrame.size()), &_scissorRectangleArrayBackgroundPerFrame[0]);
				}

				_shapeRendererBackground.render();
				_fontRendererBackground.render();
			}
		
			// Foreground
			{
				if (useForegroundViewports == true)
				{
					_graphicDevice->useScissorRectanglesWithMultipleViewports();

					_graphicDevice->getDxDeviceContext()->RSSetViewports(static_cast<UINT>(_viewportArrayForegroundPerFrame.size()), &_viewportArrayForegroundPerFrame[0]);
					_graphicDevice->getDxDeviceContext()->RSSetScissorRects(static_cast<UINT>(_scissorRectangleArrayForegroundPerFrame.size()), &_scissorRectangleArrayForegroundPerFrame[0]);
				}

				_shapeRendererForeground.render();
				_fontRendererForeground.render();
			}
			
			if (shouldSetViewports == true)
			{
				_graphicDevice->useFullScreenViewport();
			}

			_shapeRendererBackground.flushData();
			_fontRendererBackground.flushData();
			
			_shapeRendererForeground.flushData();
			_fontRendererForeground.flushData();

			resetStatesPerFrame();
		}

		void GuiContext::resetStatesPerFrame()
		{
			_isMouseInteractionDonePerFrame = false;
			_clickedControlHashKeyPerFrame = 0;

			_controlStackPerFrame.clear();

			_viewportArrayBackgroundPerFrame.clear();
			_scissorRectangleArrayBackgroundPerFrame.clear();

			_viewportArrayForegroundPerFrame.clear();
			_scissorRectangleArrayForegroundPerFrame.clear();

			const fs::Float2& windowSize = fs::Float2(_graphicDevice->getWindowSize());
			_rootControlData = ControlData(1, 0, fs::Gui::ControlType::ROOT, windowSize);

			if (_resizedControlHashKey == 0)
			{
				_cursorType = fs::Window::CursorType::Arrow;
			}
		}
	}
}

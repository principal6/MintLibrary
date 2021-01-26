#include <stdafx.h>
#include <FsLibrary/Gui/GuiContext.h>

#include <FsLibrary/Container/StringUtil.hpp>

#include <FsLibrary/SimpleRendering/GraphicDevice.h>

#include <FsLibrary/Platform/WindowsWindow.h>


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
			, _isDragBegun{ false }
			, _draggedControlHashKey{ 0 }
			, _isResizeBegun{ false }
			, _resizedControlHashKey{ 0 }
			, _resizeMethod{ ResizeMethod::ResizeOnly }
			, _mouseButtonDown{ false }
			, _mouseDownUp{ false }
			, _cursorType{ fs::Window::CursorType::Arrow }
		{
			getNamedColor(NamedColor::NormalState) = fs::SimpleRendering::Color(45, 47, 49);
			getNamedColor(NamedColor::HoverState) = fs::SimpleRendering::Color(126, 128, 130);
			getNamedColor(NamedColor::PressedState) = fs::SimpleRendering::Color(46, 72, 88);

			getNamedColor(NamedColor::WindowFocused) = fs::SimpleRendering::Color(3, 5, 7);
			getNamedColor(NamedColor::WindowOutOfFocus) = fs::SimpleRendering::Color(3, 5, 7);

			getNamedColor(NamedColor::TitleBarFocused) = getNamedColor(NamedColor::WindowFocused);
			getNamedColor(NamedColor::TitleBarOutOfFocus) = getNamedColor(NamedColor::WindowOutOfFocus);

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
			if (controlPosition._x < screenPosition._x && screenPosition._x < max._x &&
				controlPosition._y < screenPosition._y && screenPosition._y < max._y)
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
			static constexpr float kHalfBorderThickness = 5.0f;
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
			shapeRenderer.drawRoundedRectangle(controlData.getDisplaySize(), (kDefaultRoundnessInPixel * 2.0f / controlData.getDisplaySize().minElement()), 0.0f, 0.0f);

			fontRenderer.setColor(getNamedColor(NamedColor::LightFont) * fs::SimpleRendering::Color(1.0f, 1.0f, 1.0f, color.a()));
			fontRenderer.drawDynamicText(text, controlCenterPosition, fs::SimpleRendering::TextRenderDirectionHorz::Centered, fs::SimpleRendering::TextRenderDirectionVert::Centered, 0.8888f);


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

		const bool GuiContext::beginWindow(const wchar_t* const title, const fs::Float2& size, const fs::Float2& position)
		{
			static constexpr ControlType controlType = ControlType::Window;
			const float windowInnerPadding = 4.0f;

			static const InnerPadding& titleBarInnerPadding = InnerPadding(12.0f, 6.0f, 6.0f, 6.0f);
			fs::Float2 titleBarSize = fs::Float2(0.0f, fs::SimpleRendering::kDefaultFontSize + titleBarInnerPadding.top() + titleBarInnerPadding.bottom());

			// Áß¿ä
			nextNoAutoPositioned();

			const float titleWidth = _fontRendererForeground.calculateTextWidth(title, fs::StringUtil::wcslen(title));
			ControlDataParam controlDataParam;
			controlDataParam._initialDisplaySize = size;
			controlDataParam._desiredPosition = position;
			controlDataParam._innerPadding = InnerPadding(windowInnerPadding);
			controlDataParam._displaySizeMin._x = titleWidth + titleBarInnerPadding.left() + titleBarInnerPadding.right() + kDefaultRoundButtonRadius * 2.0f;
			controlDataParam._displaySizeMin._y = titleBarSize._y;
			ControlData& windowControlData = getControlData(title, controlType, controlDataParam);
			
			fs::SimpleRendering::Color color;
			const bool isFocused = processFocusControl(windowControlData, getNamedColor(NamedColor::WindowFocused), getNamedColor(NamedColor::WindowOutOfFocus), color);
			fs::SimpleRendering::ShapeRenderer& shapeRenderer = (isFocused == true) ? _shapeRendererForeground : _shapeRendererBackground;

			// Viewport & Scissor rectangle
			{
				std::vector<D3D11_VIEWPORT>& viewportArray = (isFocused == true) ? _viewportArrayForegroundPerFrame : _viewportArrayBackgroundPerFrame;
				std::vector<D3D11_RECT>& scissorRectangleArray = (isFocused == true) ? _scissorRectangleArrayForegroundPerFrame : _scissorRectangleArrayBackgroundPerFrame;

				windowControlData.setViewportIndexXXX(static_cast<uint32>(viewportArray.size()));

				D3D11_RECT scissorRectangle;
				scissorRectangle.left = static_cast<LONG>(windowControlData._position._x);
				scissorRectangle.top = static_cast<LONG>(windowControlData._position._y);
				scissorRectangle.right = static_cast<LONG>(scissorRectangle.left + windowControlData.getDisplaySize()._x);
				scissorRectangle.bottom = static_cast<LONG>(scissorRectangle.top + windowControlData.getDisplaySize()._y);
				scissorRectangleArray.emplace_back(scissorRectangle);
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
			beginTitleBar(title, titleBarSize, titleBarInnerPadding);
			endTitleBar();

			return isOpen;
		}

		void GuiContext::endWindow()
		{
			FS_ASSERT("±èÀå¿ø", _controlStackPerFrame.back()._controlType == ControlType::Window, "begin °ú end ÀÇ ControlType ÀÌ ´Ù¸¨´Ï´Ù!!!");
			_controlStackPerFrame.pop_back();
		}

		fs::Float2 GuiContext::beginTitleBar(const wchar_t* const windowTitle, const fs::Float2& titleBarSize, const InnerPadding& innerPadding)
		{
			static constexpr ControlType controlType = ControlType::TitleBar;

			ControlDataParam controlDataParam;
			controlDataParam._initialDisplaySize = titleBarSize;
			controlDataParam._deltaInteractionSize = fs::Float2(-innerPadding.right() - kDefaultRoundButtonRadius * 2.0f, 0.0f);
			controlDataParam._resetDisplaySize = true;
			ControlData& controlData = getControlData(windowTitle, controlType, controlDataParam);
			controlData._dragTargetHashKey = controlData.getParentHashKey();

			fs::SimpleRendering::Color titleBarColor;
			const bool isFocused = processFocusControl(controlData, getNamedColor(NamedColor::TitleBarFocused), getNamedColor(NamedColor::TitleBarOutOfFocus), titleBarColor);

			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::SimpleRendering::ShapeRenderer& shapeRenderer = (isAncestorFocused_ == true) ? _shapeRendererForeground : _shapeRendererBackground;
			fs::SimpleRendering::FontRenderer& fontRenderer = (isAncestorFocused_ == true) ? _fontRendererForeground : _fontRendererBackground;
			
			shapeRenderer.setColor(fs::SimpleRendering::Color(127, 127, 127));
			shapeRenderer.drawLine(controlData._position + fs::Float2(0.0f, titleBarSize._y), controlData._position + fs::Float2(controlData.getDisplaySize()._x, titleBarSize._y), 1.0f);

			const fs::Float3& titleBarTextPosition = fs::Float3(controlData._position._x, controlData._position._y, 0.0f) + fs::Float3(innerPadding.left(), titleBarSize._y * 0.5f, static_cast<float>(controlData.getViewportIndex()));
			fontRenderer.setColor((isAncestorFocused_ == true) ? getNamedColor(NamedColor::LightFont) : getNamedColor(NamedColor::DarkFont));
			fontRenderer.drawDynamicText(windowTitle, titleBarTextPosition, fs::SimpleRendering::TextRenderDirectionHorz::Rightward, fs::SimpleRendering::TextRenderDirectionVert::Centered, 0.9375f);

			_controlStackPerFrame.emplace_back(ControlStackData(controlData));

			// Close button
			{
				// Áß¿ä
				nextNoAutoPositioned();
				nextControlPosition(fs::Float2(titleBarSize._x - kDefaultRoundButtonRadius * 2.0f - innerPadding.right(), (titleBarSize._y - kDefaultRoundButtonRadius * 2.0f) * 0.5f));

				beginRoundButton(windowTitle, fs::SimpleRendering::Color(1.0f, 0.25f, 0.25f));
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

			const float radius = kDefaultRoundButtonRadius;
			ControlDataParam controlDataParam;
			controlDataParam._initialDisplaySize = fs::Float2(radius * 2.0f);
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

		const uint64 GuiContext::generateControlHashKey(const wchar_t* const text, const ControlType controlType) const noexcept
		{
			static std::wstring hashKeyWstring;
			hashKeyWstring.clear();
			hashKeyWstring.append(text);
			hashKeyWstring.append(std::to_wstring(static_cast<uint16>(controlType)));
			return fs::StringUtil::hashRawString64(hashKeyWstring.c_str());
		}

		GuiContext::ControlData& GuiContext::getControlData(const wchar_t* const text, const ControlType controlType, const ControlDataParam& getControlDataParam) noexcept
		{
			bool isNewData = false;
			const uint64 hashKey = generateControlHashKey(text, controlType);
			auto found = _controlIdMap.find(hashKey);
			if (found == _controlIdMap.end())
			{
				const ControlData& stackTopControlData = getStackTopControlData();

				ControlData newControlData{ hashKey, stackTopControlData.getHashKey(), controlType };
				_controlIdMap[hashKey] = newControlData;
				
				isNewData = true;
			}
			
			ControlData& controlData = _controlIdMap[hashKey];
			ControlData& parentControlData = getControlData(controlData.getParentHashKey());

			InnerPadding& controlInnerPadding = const_cast<InnerPadding&>(controlData.getInnerPadding());
			fs::Float2& controlDisplaySize = const_cast<fs::Float2&>(controlData.getDisplaySize());
			fs::Float2& controlOffset = const_cast<fs::Float2&>(controlData.getOffset());
			controlInnerPadding = getControlDataParam._innerPadding;
			controlOffset = fs::Float2::kZero;

			// Display size, Display size min
			if (isNewData == true || getControlDataParam._resetDisplaySize == true)
			{
				const float maxDisplaySizeX = parentControlData.getDisplaySize()._x - ((_nextNoAutoPositioned == false) ? (parentControlData.getInnerPadding().left() * 2.0f) : 0.0f);
				controlDisplaySize._x = (_nextControlSize._x <= 0.0f) ? fs::min(maxDisplaySizeX, getControlDataParam._initialDisplaySize._x) :
					((_nextSizingForced == true) ? _nextControlSize._x : fs::min(maxDisplaySizeX, _nextControlSize._x));
				controlDisplaySize._y = (_nextControlSize._y <= 0.0f) ? getControlDataParam._initialDisplaySize._y :
					((_nextSizingForced == true) ? _nextControlSize._y : fs::max(getControlDataParam._initialDisplaySize._y, _nextControlSize._y));

				fs::Float2& controlDisplaySizeMin = const_cast<fs::Float2&>(controlData.getDisplaySizeMin());
				controlDisplaySizeMin = getControlDataParam._displaySizeMin;
			}

			// Interaction size!!!
			controlData._interactionSize = controlDisplaySize + getControlDataParam._deltaInteractionSize;

			// Position, Parent offset, Parent cChild at
			if (_nextNoAutoPositioned == false)
			{
				// Auto-positioned

				fs::Float2& parentControlChildAt = const_cast<fs::Float2&>(parentControlData.getChildAt());
				fs::Float2& parentControlOffset = const_cast<fs::Float2&>(parentControlData.getOffset());
				if (_nextSameLine == true)
				{
					parentControlChildAt._x += (parentControlOffset._x + kDefaultIntervalX);

					parentControlOffset._x = fs::max(parentControlOffset._x, controlData.getDisplaySize()._x);
					parentControlOffset._y = fs::max(parentControlOffset._y, controlData.getDisplaySize()._y);
				}
				else
				{
					parentControlChildAt._x = parentControlData._position._x + parentControlData.getInnerPadding().left();
					if (0.0f < parentControlOffset._y)
					{
						parentControlChildAt._y += parentControlOffset._y;
					}

					parentControlOffset = controlData.getDisplaySize();

					if (_nextNoAutoPositioned == false)
					{
						parentControlOffset._y += kDefaultIntervalY;
					}
				}

				controlData._position = parentControlChildAt;
			}
			else
			{
				// NO Auto-positioned

				if (controlType != ControlType::Window || isNewData == true)
				{
					controlData._position = parentControlData._position; // +fs::Float2(parentControlData._innerPadding._x, parentControlData._innerPadding._z);
						
					if (getControlDataParam._desiredPosition == fs::Float2::kZero)
					{
						controlData._position += _nextControlPosition;
					}
					else
					{
						controlData._position += getControlDataParam._desiredPosition;
					}
				}
			}

			// Focus, State
			{
				if (controlType == ControlType::Window)
				{
					controlData.setControlState(ControlState::VisibleOpen);

					controlData._isFocusable = true;
					controlData._isResizable = true;
				}
				else
				{
					controlData.setControlState(ControlState::Visible);
				}

				if (controlType == ControlType::TitleBar)
				{
					controlData._isDraggable = true;
				}
			}

			calculateControlChildAt(controlData);

			controlData.setViewportIndexXXX(parentControlData.getViewportIndex());

			resetNextStates();

			return controlData;
		}

		void GuiContext::calculateControlChildAt(ControlData& controlData) noexcept
		{
			fs::Float2& controlChildAt = const_cast<fs::Float2&>(controlData.getChildAt());
			controlChildAt = controlData._position + ((_nextNoAutoPositioned == false) ? fs::Float2(controlData.getInnerPadding().left(), controlData.getInnerPadding().top()) : fs::Float2::kZero);
		}

		const bool GuiContext::processClickControl(ControlData& controlData, const fs::SimpleRendering::Color& normalColor, const fs::SimpleRendering::Color& hoverColor, const fs::SimpleRendering::Color& pressedColor, fs::SimpleRendering::Color& outBackgroundColor) noexcept
		{
			bool result = false;
			outBackgroundColor = normalColor;

			if (shouldApplyChange(controlData) == true && isInControlInteractionArea(_mousePosition, controlData) == true)
			{
				outBackgroundColor = hoverColor;

				const bool isMouseDownIn = isInControlInteractionArea(_mouseDownPosition, controlData);
				if (_mouseButtonDown == true && isMouseDownIn == true)
				{
					outBackgroundColor = pressedColor;
				}

				if (_mouseDownUp == true && isMouseDownIn == true)
				{
					// Clicked
					result = true;
				}
			}

			if (isAncestorFocused(controlData) == false)
			{
				outBackgroundColor.a(kDefaultOutOfFocusAlpha);
			}

			// Áß¿ä
			processControlCommonInternal(controlData);

			return result;
		}

		const bool GuiContext::processFocusControl(ControlData& controlData, const fs::SimpleRendering::Color& focusedColor, const fs::SimpleRendering::Color& nonFocusedColor, fs::SimpleRendering::Color& outBackgroundColor) noexcept
		{
			bool result = false;
			if (controlData.getHashKey() == _focusedControlHashKey)
			{
				// Focused

				outBackgroundColor = focusedColor;
				outBackgroundColor.a(kDefaultFocusedAlpha);

				result = true;
			}
			else
			{
				// Out of focus

				outBackgroundColor = nonFocusedColor;
				outBackgroundColor.a(kDefaultOutOfFocusAlpha);
			}
			
			// »õ focus È®ÀÎ
			if (_draggedControlHashKey == 0 && _resizedControlHashKey == 0 && controlData._isFocusable == true && isInControlInteractionArea(_mousePosition, controlData) == true)
			{
				const bool isMouseDownIn = isInControlInteractionArea(_mouseDownPosition, controlData);
				if (_mouseButtonDown == true && isMouseDownIn == true)
				{
					// Focus entered

					if (shouldApplyChange(controlData) == true)
					{
						outBackgroundColor = focusedColor;
						outBackgroundColor.a(kDefaultFocusedAlpha);

						_focusedControlHashKey = controlData.getHashKey();

						result = true;
					}
				}
			}

			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			if (isAncestorFocused_ == true)
			{
				outBackgroundColor = focusedColor;
				outBackgroundColor.a(kDefaultFocusedAlpha);
			}
			else if (_focusedControlHashKey != controlData.getHashKey())
			{
				outBackgroundColor.a(kDefaultOutOfFocusAlpha);
			}

			// Áß¿ä
			processControlCommonInternal(controlData);

			return result;
		}

		void GuiContext::processControlCommonInternal(ControlData& controlData) noexcept
		{
			if (_resizedControlHashKey == 0 && shouldApplyChange(controlData) == true)
			{
				fs::Window::CursorType newCursorType;
				if (controlData._isResizable == true && isInControlBorderArea(_mousePosition, controlData, newCursorType, _resizeMethod) == true)
				{
					_cursorType = newCursorType;
				}
			}

			ControlData& changeTargetControlData = (controlData._dragTargetHashKey == 0) ? controlData : getControlData(controlData._dragTargetHashKey);
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
			}
			else if (isDragging == true)
			{
				if (_isDragBegun == true)
				{
					_draggedControlInitialPosition = changeTargetControlData._position;
					_isDragBegun = false;
				}
				changeTargetControlData._position = _draggedControlInitialPosition + mouseDeltaPosition;
			}
		}

		const bool GuiContext::shouldApplyChange(const ControlData& controlData) const noexcept
		{
			if (_focusedControlHashKey != 0 && isMeOrAncestorFocusedXXX(controlData) == false)
			{
				// Focus °¡ ÀÖ´Â Control ÀÌ Á¸ÀçÇÏÁö¸¸ ³»°¡ Focus ´Â ¾Æ´Ñ °æ¿ì

				fs::Window::CursorType dummyCursorType;
				ResizeMethod dummyResizeMethod;
				const ControlData& focusedControlData = getControlData(_focusedControlHashKey);
				if (isInControlInteractionArea(_mousePosition, focusedControlData) == true || isInControlBorderArea(_mousePosition, focusedControlData, dummyCursorType, dummyResizeMethod) == true)
				{
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
				if (_resizedControlHashKey != 0 || controlData._isDraggable == false || shouldApplyChange(controlData) == false)
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
				if (_draggedControlHashKey != 0 || controlData._isResizable == false || shouldApplyChange(controlData) == false)
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
			_controlStackPerFrame.clear();

			_viewportArrayBackgroundPerFrame.clear();
			_scissorRectangleArrayBackgroundPerFrame.clear();

			_viewportArrayForegroundPerFrame.clear();
			_scissorRectangleArrayForegroundPerFrame.clear();

			const fs::Float2& windowSize = fs::Float2(_graphicDevice->getWindowSize());
			_rootControlData = ControlData(0, 0, fs::Gui::ControlType::ROOT, windowSize);

			if (_resizedControlHashKey == 0)
			{
				_cursorType = fs::Window::CursorType::Arrow;
			}
		}

	}
}

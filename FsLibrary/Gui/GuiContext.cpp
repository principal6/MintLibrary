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
			, _draggedControlHashKey{ 0 }
			, _mouseButtonDown{ false }
			, _mouseDownUp{ false }
		{
			getNamedColor(NamedColor::NormalState) = fs::SimpleRendering::Color(35, 37, 39);
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
			_fontRendererBackground.initializeShaders();
			_shapeRendererForeground.initializeShaders();
			_fontRendererForeground.initializeShaders();

			resetNextStates();
			resetControlStatesPerFrame();
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

		const bool GuiContext::isInControl(const fs::Float2& screenPosition, const ControlData& controlData) const noexcept
		{
			const fs::Float2 max = controlData._position + controlData._interactionSize;
			if (controlData._position._x < screenPosition._x && screenPosition._x < max._x &&
				controlData._position._y < screenPosition._y && screenPosition._y < max._y)
			{
				return true;
			}
			return false;
		}

		const bool GuiContext::beginButton(const wchar_t* const text)
		{
			static constexpr ControlType controlType = ControlType::Button;
			
			const float textWidth = _fontRendererBackground.calculateTextWidth(text, fs::StringUtil::wcslen(text));
			const ControlData& controlData = getControlData(text, fs::Float2(textWidth + 24, fs::SimpleRendering::kDefaultFontSize + 12), controlType);
			
			fs::SimpleRendering::Color color;
			const bool isClicked = processClickControl(controlData, getNamedColor(NamedColor::NormalState), getNamedColor(NamedColor::HoverState), getNamedColor(NamedColor::PressedState), color);
		
			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::SimpleRendering::ShapeRenderer& shapeRenderer = (isAncestorFocused_ == true) ? _shapeRendererForeground : _shapeRendererBackground;
			fs::SimpleRendering::FontRenderer& fontRenderer = (isAncestorFocused_ == true) ? _fontRendererForeground : _fontRendererBackground;
			const fs::Float3& controlCenterPosition = getControlCenterPosition(controlData);
			shapeRenderer.setColor(color);
			shapeRenderer.setPosition(controlCenterPosition);
			shapeRenderer.drawRoundedRectangle(controlData._displaySize, (kDefaultRoundnessInPixel * 2.0f / controlData._displaySize.minElement()), 0.0f, 0.0f);

			fontRenderer.setColor(getNamedColor(NamedColor::LightFont) * fs::SimpleRendering::Color(1.0f, 1.0f, 1.0f, color.a()));
			fontRenderer.drawDynamicText(text, controlCenterPosition, fs::SimpleRendering::TextRenderDirectionHorz::Centered, fs::SimpleRendering::TextRenderDirectionVert::Centered, 0.8888f);


			if (isClicked == true)
			{
				_controlStackPerFrame.emplace_back(ControlStackData(controlType, controlData._hashKey));
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
			
			// Áß¿ä
			nextNoAutoPositioned();

			ControlData& windowControlData = getControlData(title, size, controlType, position, fs::Float4(4.0f));

			fs::SimpleRendering::Color color;
			const bool isFocused = processFocusControl(windowControlData, getNamedColor(NamedColor::WindowFocused), getNamedColor(NamedColor::WindowOutOfFocus), color);
			fs::SimpleRendering::ShapeRenderer& shapeRenderer = (isFocused == true) ? _shapeRendererForeground : _shapeRendererBackground;
			//fs::SimpleRendering::FontRenderer& fontRenderer = (isFocused == true) ? _fontRendererForeground : _fontRenderer;

			const bool isOpen = windowControlData._controlState == ControlState::VisibleOpen;
			if (isOpen == true)
			{
				const fs::Float3& windowCenterPosition = getControlCenterPosition(windowControlData);
				shapeRenderer.setColor(color);
				shapeRenderer.setPosition(windowCenterPosition);
				shapeRenderer.drawRoundedRectangle(windowControlData._displaySize, (kDefaultRoundnessInPixel * 2.0f / windowControlData._displaySize.minElement()), 0.0f, 0.0f);

				_controlStackPerFrame.emplace_back(ControlStackData(controlType, windowControlData._hashKey));
			}

			// Áß¿ä
			nextNoAutoPositioned();

			beginTitleBar(title, size._x);
			{
				const ControlData& titleBarControlData = getControlData(_controlStackPerFrame.back()._hashKey);
				if (isDraggingControl(titleBarControlData) == true)
				{
					fs::Float2 draggingDeltaPosition = _mousePosition - _mouseDownPosition;
					if (_dragStarted == true)
					{
						_draggedControlInitialPosition = windowControlData._position;
						_dragStarted = false;
					}
					windowControlData._position = _draggedControlInitialPosition + draggingDeltaPosition;
				}
			}
			endTitleBar();

			return isOpen;
		}

		void GuiContext::endWindow()
		{
			FS_ASSERT("±èÀå¿ø", _controlStackPerFrame.back()._controlType == ControlType::Window, "begin °ú end ÀÇ ControlType ÀÌ ´Ù¸¨´Ï´Ù!!!");
			_controlStackPerFrame.pop_back();
		}

		void GuiContext::beginTitleBar(const wchar_t* const windowTitle, const float width)
		{
			const float paddingLeft = 12.0f;
			const float paddingRight = 6.0f;
			const float paddingTopBottom = 12.0f;
			static constexpr ControlType controlType = ControlType::TitleBar;

			const fs::Float2& titleBarSize = fs::Float2(width, fs::SimpleRendering::kDefaultFontSize + paddingTopBottom);
			const ControlData& controlData = getControlData(windowTitle, titleBarSize, controlType);

			fs::SimpleRendering::Color titleBarColor;
			const bool isFocused = processFocusControl(controlData, getNamedColor(NamedColor::TitleBarFocused), getNamedColor(NamedColor::TitleBarOutOfFocus), titleBarColor);

			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::SimpleRendering::ShapeRenderer& shapeRenderer = (isAncestorFocused_ == true) ? _shapeRendererForeground : _shapeRendererBackground;
			fs::SimpleRendering::FontRenderer& fontRenderer = (isAncestorFocused_ == true) ? _fontRendererForeground : _fontRendererBackground;
			
			shapeRenderer.setColor(fs::SimpleRendering::Color(127, 127, 127));
			shapeRenderer.drawLine(controlData._position + fs::Float2(0.0f, titleBarSize._y), controlData._position + fs::Float2(controlData._displaySize._x, titleBarSize._y), 1.0f);

			const fs::Float3& titleBarTextPosition = fs::Float3(controlData._position._x, controlData._position._y, 0.0f) + fs::Float3(paddingLeft, titleBarSize._y * 0.5f, 0.0f);
			fontRenderer.setColor((isAncestorFocused_ == true) ? getNamedColor(NamedColor::LightFont) : getNamedColor(NamedColor::DarkFont));
			fontRenderer.drawDynamicText(windowTitle, titleBarTextPosition, fs::SimpleRendering::TextRenderDirectionHorz::Rightward, fs::SimpleRendering::TextRenderDirectionVert::Centered, 0.9375f);

			_controlStackPerFrame.emplace_back(ControlStackData(controlType, controlData._hashKey));

			// Close button
			{
				// Áß¿ä
				nextNoAutoPositioned();
				nextControlPosition(fs::Float2(titleBarSize._x - kDefaultRoundButtonRadius * 2.0f - paddingRight, (titleBarSize._y - kDefaultRoundButtonRadius * 2.0f) * 0.5f));

				beginRoundButton(windowTitle, fs::SimpleRendering::Color(1.0f, 0.25f, 0.25f));
				endRoundButton();
			}
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
			const fs::Float2& controlSize = fs::Float2(radius * 2.0f);
			const ControlData& controlData = getControlData(windowTitle, controlSize, controlType);

			fs::SimpleRendering::Color controlColor;
			const bool isClicked = processClickControl(controlData, color, color.scaleRgb(1.5f), color.scaleRgb(0.75f), controlColor);

			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			fs::SimpleRendering::ShapeRenderer& shapeRenderer = (isAncestorFocused_ == true) ? _shapeRendererForeground : _shapeRendererBackground;

			const fs::Float3& controlCenterPosition = getControlCenterPosition(controlData);
			shapeRenderer.setColor(controlColor);
			shapeRenderer.setPosition(controlCenterPosition);
			shapeRenderer.drawCircle(radius);

			_controlStackPerFrame.emplace_back(ControlStackData(controlType, controlData._hashKey));

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

		GuiContext::ControlData& GuiContext::getControlData(const wchar_t* const text, const fs::Float2& defaultSize, const ControlType controlType, const fs::Float2& desiredPosition, const fs::Float4& innerPadding) noexcept
		{
			bool isNewData = false;
			const uint64 hashKey = generateControlHashKey(text, controlType);
			auto found = _controlIdMap.find(hashKey);
			if (found == _controlIdMap.end())
			{
				const ControlData& stackTopControlData = getStackTopControlData();

				ControlData newControlData;
				newControlData._hashKey = hashKey;
				newControlData._parentHashKey = stackTopControlData._hashKey;

				_controlIdMap[hashKey] = newControlData;
				
				isNewData = true;
			}

			ControlData& controlData = _controlIdMap[hashKey];
			ControlData& parentControlData = getControlData(controlData._parentHashKey);

			controlData._innerPadding = innerPadding;

			const float maxDisplaySizeX = parentControlData._displaySize._x - ((_nextNoAutoPositioned == false) ? (parentControlData._innerPadding._x * 2.0f) : 0.0f);
			controlData._displaySize._x = (_nextControlSize._x <= 0.0f) ? fs::min(maxDisplaySizeX, defaultSize._x) :
				((_nextSizingForced == true) ? _nextControlSize._x : fs::min(maxDisplaySizeX, _nextControlSize._x));
			controlData._displaySize._y = (_nextControlSize._y <= 0.0f) ? defaultSize._y :
				((_nextSizingForced == true) ? _nextControlSize._y : fs::max(defaultSize._y, _nextControlSize._y));

			controlData._interactionSize = controlData._displaySize;

			if (_nextSameLine == true)
			{
				parentControlData._childAt._x += (parentControlData._offset._x + kDefaultIntervalX);

				parentControlData._offset._x = fs::max(parentControlData._offset._x, controlData._displaySize._x);
				parentControlData._offset._y = fs::max(parentControlData._offset._y, controlData._displaySize._y);
			}
			else
			{
				parentControlData._childAt._x = parentControlData._position._x + parentControlData._innerPadding._x;
				if (0.0f < parentControlData._offset._y)
				{
					parentControlData._childAt._y += parentControlData._offset._y;
				}

				parentControlData._offset = controlData._displaySize;

				if (_nextNoAutoPositioned == false)
				{
					parentControlData._offset._y += kDefaultIntervalY;
				}
			}

			// Position
			{
				if (_nextNoAutoPositioned == true)
				{
					if (controlType != ControlType::Window || isNewData == true)
					{
						controlData._position = parentControlData._position; // +fs::Float2(parentControlData._innerPadding._x, parentControlData._innerPadding._z);
						
						if (desiredPosition == fs::Float2::kZero)
						{
							controlData._position += _nextControlPosition;
						}
						else
						{
							controlData._position += desiredPosition;
						}
					}
				}
				else
				{
					controlData._position = parentControlData._childAt;
				}
			}

			// Focus, State
			{
				if (controlType == ControlType::Window)
				{
					controlData._controlState = ControlState::VisibleOpen;

					controlData._isFocusable = true;
				}
				else
				{
					controlData._controlState = ControlState::Visible;
				}
			}

			controlData._controlTypeForDebug = controlType;

			calculateControlChildAt(controlData);

			resetNextStates();

			return controlData;
		}

		void GuiContext::calculateControlChildAt(ControlData& controlData) noexcept
		{
			controlData._childAt = controlData._position + fs::Float2(controlData._innerPadding._x, controlData._innerPadding._z);
		}

		const bool GuiContext::processClickControl(const ControlData& controlData, const fs::SimpleRendering::Color& normalColor, const fs::SimpleRendering::Color& hoverColor, const fs::SimpleRendering::Color& pressedColor, fs::SimpleRendering::Color& outBackgroundColor) const noexcept
		{
			bool result = false;
			outBackgroundColor = normalColor;

			bool changeMyState = true;
			if (_focusedControlHashKey != 0 && isMeOrAncestorFocusedXXX(controlData) == false)
			{
				// Focus ´Â ÀÖÁö¸¸ ³»°¡ Focus °¡ ¾Æ´Ñ °æ¿ì
				if (isInControl(_mousePosition, getControlData(_focusedControlHashKey)) == true)
				{
					changeMyState = false;
				}
			}

			if (changeMyState == true && isInControl(_mousePosition, controlData) == true)
			{
				outBackgroundColor = hoverColor;

				const bool isMouseDownIn = isInControl(_mouseDownPosition, controlData);
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
			return result;
		}

		const bool GuiContext::processFocusControl(const ControlData& controlData, const fs::SimpleRendering::Color& focusedColor, const fs::SimpleRendering::Color& nonFocusedColor, fs::SimpleRendering::Color& outBackgroundColor) const noexcept
		{
			bool result = false;
			if (controlData._hashKey == _focusedControlHashKey)
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
			if (isInControl(_mousePosition, controlData) == true)
			{
				if (controlData._isFocusable == true)
				{
					const bool isMouseDownIn = isInControl(_mouseDownPosition, controlData);
					if (_mouseButtonDown == true && isMouseDownIn == true)
					{
						// Focus entered

						bool shouldBeFocused = false;
						if (_focusedControlHashKey != 0)
						{
							if (isInControl(_mouseDownPosition, getControlData(_focusedControlHashKey)) == false)
							{
								shouldBeFocused = true;
							}
						}
						else
						{
							shouldBeFocused = true;
						}

						if (shouldBeFocused == true)
						{
							outBackgroundColor = focusedColor;
							outBackgroundColor.a(kDefaultFocusedAlpha);

							_focusedControlHashKey = controlData._hashKey;

							result = true;
						}
					}
				}
			}

			const bool isAncestorFocused_ = isAncestorFocused(controlData);
			if (isAncestorFocused_ == true)
			{
				outBackgroundColor = focusedColor;
				outBackgroundColor.a(kDefaultFocusedAlpha);
			}
			else if (_focusedControlHashKey != controlData._hashKey)
			{
				outBackgroundColor.a(kDefaultOutOfFocusAlpha);
			}
			return result;
		}

		const bool GuiContext::isDraggingControl(const ControlData& controlData) const noexcept
		{
			if (_draggedControlHashKey == 0)
			{
				if (isInControl(_mousePosition, controlData) == true)
				{
					if (isInControl(_mouseDownPosition, controlData) == true && _mouseButtonDown == true)
					{
						_dragStarted = true;
						_draggedControlHashKey = controlData._hashKey;
						return true;
					}
				}
			}
			else
			{
				if (_mouseButtonDown == false)
				{
					_draggedControlHashKey = 0;
					return false;
				}
				else
				{
					if (_draggedControlHashKey == controlData._hashKey)
					{
						return true;
					}
				}
			}

			return false;
		}

		const bool GuiContext::isMeOrAncestorFocusedXXX(const ControlData& controlData) const noexcept
		{
			if (_focusedControlHashKey == controlData._hashKey)
			{
				return true;
			}
			return isAncestorFocused(controlData);
		}

		const bool GuiContext::isAncestorFocused(const ControlData& controlData) const noexcept
		{
			return isAncestorFocusedRecursiveXXX(controlData._parentHashKey);
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

			return isAncestorFocusedRecursiveXXX(getControlData(hashKey)._parentHashKey);
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

			_shapeRendererBackground.render();
			_fontRendererBackground.render();

			_shapeRendererForeground.render();
			_fontRendererForeground.render();

			_shapeRendererBackground.flushData();
			_fontRendererBackground.flushData();
			
			_shapeRendererForeground.flushData();
			_fontRendererForeground.flushData();

			resetControlStatesPerFrame();
		}

		void GuiContext::resetControlStatesPerFrame()
		{
			_controlStackPerFrame.clear();

			_rootControlData = ControlData();
			
			const fs::Float2& windowSize = fs::Float2(_graphicDevice->getWindowSize());
			_rootControlData._displaySize = windowSize;
		}

	}
}

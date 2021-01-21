#include <stdafx.h>
#include <FsLibrary/Gui/GuiContext.h>

#include <FsLibrary/Container/StringUtil.hpp>

#include <FsLibrary/SimpleRendering/GraphicDevice.h>

#include <FsLibrary/Platform/WindowsWindow.h>


namespace fs
{
	namespace Gui
	{
		GuiContext::ControlData::ControlData()
		{
			_innerPadding._x = kDefaultPaddingX;
			_innerPadding._y = kDefaultPaddingY;

			_displaySize._x = kDefaultControlWidth;

			_childAt = _innerPadding;
		}


		GuiContext::GuiContext(fs::SimpleRendering::GraphicDevice* const graphicDevice)
			: _shapeRenderer{ graphicDevice }
			, _fontRenderer{ graphicDevice }
			, _mouseButtonDown{ false }
			, _mouseDownUp{ false }
		{
			_normalStateColor = fs::SimpleRendering::Color(80, 126, 154);
			_hoverStateColor = fs::SimpleRendering::Color(129, 167, 190);
			_pressedStateColor = fs::SimpleRendering::Color(58, 91, 111);

			_windowBackgroundColor = fs::SimpleRendering::Color(101, 147, 175);

			_lightFontColor = fs::SimpleRendering::Color(233, 239, 243);

			resetNextStates();
		}

		GuiContext::~GuiContext()
		{
			__noop;
		}

		void GuiContext::initialize(const char* const font)
		{
			if (_fontRenderer.loadFont(font) == false)
			{
				_fontRenderer.pushGlyphRange(fs::SimpleRendering::GlyphRange(0, 0x33DD));
				_fontRenderer.pushGlyphRange(fs::SimpleRendering::GlyphRange(L'∞°', L'∆R'));
				_fontRenderer.bakeFont(font, kDefaultFontSize, font, 2048, 1, 1);
				_fontRenderer.loadFont(font);
			}

			_shapeRenderer.initializeShaders();
			_fontRenderer.initializeShaders();
		}

		void GuiContext::handleEvents(fs::Window::IWindow* const window)
		{
			// √ ±‚»≠
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

		const bool GuiContext::isMouseIn(const fs::Float2& mousePosition, const ControlData& controlData) const noexcept
		{
			const fs::Float2 max = controlData._position + controlData._interactionSize;
			if (controlData._position._x < mousePosition._x && mousePosition._x < max._x &&
				controlData._position._y < mousePosition._y && mousePosition._y < max._y)
			{
				return true;
			}
			return false;
		}

		void GuiContext::nextSameLine()
		{
			_isSameLine = true;
		}

		void GuiContext::nextControlSize(const fs::Float2& size, const bool force)
		{
			_nextControlSize = size;
			_isSizingForced = force;
		}

		void GuiContext::resetNextStates()
		{
			_isSameLine = false;
			_nextControlSize = fs::Float2(0.0f);
			_isSizingForced = false;
		}

		const bool GuiContext::beginButton(const char* const id, const wchar_t* const text)
		{
			const uint32 controlIndex = static_cast<uint32>(_controlDataArray.size());
			//const uint64 idHash = fs::StringUtil::hashRawString64(id);
			//_idToIndexMap[idHash] = controlIndex;

			const float textWidth = _fontRenderer.calculateTextWidth(text, fs::StringUtil::wcslen(text));
			const ControlData& controlData = createControlData(fs::Float2(textWidth + 16, kDefaultFontSize + 8));

			fs::SimpleRendering::Color color;
			const bool result = processClickControl(controlData, color);
		
			const fs::Float3& controlCenterPosition = getControlCenterPosition(controlData);
			_shapeRenderer.setColor(color);
			_shapeRenderer.setPosition(controlCenterPosition);
			_shapeRenderer.drawRoundedRectangle(controlData._displaySize, kDefaultRoundness, 0.0f, 0.0f);

			_fontRenderer.setColor(_lightFontColor);
			_fontRenderer.drawDynamicText(text, controlCenterPosition.xy(), fs::SimpleRendering::TextRenderDirectionHorz::Centered, fs::SimpleRendering::TextRenderDirectionVert::Centered);

			_controlDataArray.emplace_back(controlData);

			if (result == true)
			{
				_controlIndexStack.emplace_back(controlIndex);
			}

			return result;
		}

		void GuiContext::endButton()
		{
			_controlIndexStack.pop_back();
		}

		GuiContext::ControlData& GuiContext::getParentControlDataXXX() noexcept
		{
			if (_controlIndexStack.empty() == true)
			{
				return _rootControlData;
			}
			return _controlDataArray[_controlIndexStack.back()];
		}

		GuiContext::ControlData GuiContext::createControlData(const fs::Float2& defaultSize) noexcept
		{
			ControlData& parentControlData = getParentControlDataXXX();

			ControlData controlData;
			const float maxDisplaySizeX = parentControlData._displaySize._x - (parentControlData._innerPadding._x * 2.0f);
			controlData._displaySize._x = (_nextControlSize._x <= 0.0f) ? fs::min(maxDisplaySizeX, defaultSize._x) : 
				((_isSizingForced == true) ? _nextControlSize._x : fs::min(maxDisplaySizeX, _nextControlSize._x));
			controlData._displaySize._y = (_nextControlSize._y <= 0.0f) ? defaultSize._y :
				((_isSizingForced == true) ? _nextControlSize._y : fs::max(defaultSize._y, _nextControlSize._y));

			controlData._interactionSize = controlData._displaySize;

			if (_isSameLine == true)
			{
				parentControlData._childAt._x += (parentControlData._offset._x + kDefaultIntervalX);

				parentControlData._offset._x = fs::max(parentControlData._offset._x, controlData._displaySize._x);
				parentControlData._offset._y = fs::max(parentControlData._offset._y, controlData._displaySize._y);
			}
			else
			{
				parentControlData._childAt._x = parentControlData._innerPadding._x;
				parentControlData._childAt._y += (parentControlData._offset._y + kDefaultIntervalY);

				parentControlData._offset = controlData._displaySize;
			}

			controlData._position = parentControlData._childAt;

			resetNextStates();

			return controlData;
		}

		fs::Float3 GuiContext::getControlCenterPosition(const ControlData& controlData) const noexcept
		{
			return fs::Float3(controlData._position._x + controlData._displaySize._x * 0.5f, controlData._position._y + controlData._displaySize._y * 0.5f, 0);
		}

		const bool GuiContext::processClickControl(const ControlData& controlData, fs::SimpleRendering::Color& outBackgroundColor) const noexcept
		{
			outBackgroundColor = _normalStateColor;
			if (isMouseIn(_mousePosition, controlData) == true)
			{
				outBackgroundColor = _hoverStateColor;

				const bool isMouseDownIn = isMouseIn(_mouseDownPosition, controlData);
				if (_mouseButtonDown == true && isMouseDownIn == true)
				{
					outBackgroundColor = _pressedStateColor;
				}

				if (_mouseDownUp == true && isMouseDownIn == true)
				{
					return true;
				}
			}
			return false;
		}

		void GuiContext::render()
		{
			FS_ASSERT("±Ë¿Âø¯", _controlIndexStack.empty() == true, "begin ∞˙ end »£√‚ »Ωºˆ∞° ∏¬¡ˆ æ Ω¿¥œ¥Ÿ!!!");

			_shapeRenderer.render();
			_fontRenderer.render();


			_idToIndexMap.clear();
			_controlDataArray.clear();


			_shapeRenderer.flushData();
			_fontRenderer.flushData();

			_rootControlData = ControlData();
		}
	}
}

#include <stdafx.h>
#include <MintRenderingBase/Include/Gui/GuiContext.h>

#include <MintContainer/Include/Hash.hpp>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/HashMap.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/Gui/ControlData.hpp>

#include <MintPlatform/Include/WindowsWindow.h>

#include <MintLibrary/Include/ScopedCpuProfiler.h>


#pragma optimize("", off)


namespace mint
{
    namespace Gui
    {
        namespace ControlCommonHelpers
        {
            MINT_INLINE const bool isInControl(const mint::Float2& screenPosition, const mint::Float2& controlPosition, const mint::Float2& controlPositionOffset, const mint::Float2& interactionSize) noexcept
            {
                const mint::Float2 max = controlPosition + controlPositionOffset + interactionSize;
                if (controlPosition._x + controlPositionOffset._x <= screenPosition._x && screenPosition._x <= max._x &&
                    controlPosition._y + controlPositionOffset._y <= screenPosition._y && screenPosition._y <= max._y)
                {
                    return true;
                }
                return false;
            }

            MINT_INLINE const bool isInControlInteractionArea(const mint::Float2& screenPosition, const ControlData& controlData) noexcept
            {
                if (controlData.isDockHosting() == true)
                {
                    const mint::Float2 positionOffset{ controlData.getDockSizeIfHosting(DockingMethod::LeftSide)._x, controlData.getDockSizeIfHosting(DockingMethod::TopSide)._y };
                    return ControlCommonHelpers::isInControl(screenPosition, controlData._position, positionOffset, controlData.getNonDockInteractionSize());
                }
                return ControlCommonHelpers::isInControl(screenPosition, controlData._position, mint::Float2::kZero, controlData.getInteractionSize());
            }

            MINT_INLINE const bool isInControlBorderArea(const mint::Float2& screenPosition, const ControlData& controlData, mint::Window::CursorType& outCursorType, ResizingMask& outResizingMask, ResizingMethod& outResizingMethod) noexcept
            {
                const mint::Float2 extendedPosition = controlData._position - mint::Float2(kHalfBorderThickness);
                const mint::Float2 extendedInteractionSize = controlData.getInteractionSize() + mint::Float2(kHalfBorderThickness * 2.0f);
                const mint::Float2 originalMax = controlData._position + controlData.getInteractionSize();
                if (ControlCommonHelpers::isInControl(screenPosition, extendedPosition, mint::Float2::kZero, extendedInteractionSize) == true)
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
                        outCursorType = mint::Window::CursorType::SizeLeftTilted;
                    }
                    else if (bottomLeftOrTopRight == true)
                    {
                        outCursorType = mint::Window::CursorType::SizeRightTilted;
                    }
                    else if (leftOrRight == true)
                    {
                        outCursorType = mint::Window::CursorType::SizeHorz;
                    }
                    else if (topOrBottom == true)
                    {
                        outCursorType = mint::Window::CursorType::SizeVert;
                    }

                    const bool result = (leftOrRight || topOrBottom);
                    return result;
                }
                return false;
            }

            MINT_INLINE mint::Float2 getControlLeftCenterPosition(const ControlData& controlData)
            {
                return mint::Float2(controlData._position._x, controlData._position._y + controlData._displaySize._y * 0.5f);
            };

            MINT_INLINE mint::Float2 getControlRightCenterPosition(const ControlData& controlData)
            {
                return mint::Float2(controlData._position._x + controlData._displaySize._x, controlData._position._y + controlData._displaySize._y * 0.5f);
            };
        }


        namespace PrepareControlDataUtils
        {
            PrepareControlDataParam prepareInputBox(const CommonControlParam& common, const float fontSize) noexcept
            {
                PrepareControlDataParam prepareControlDataParam;
                {
                    prepareControlDataParam._initialDisplaySize._x = common._size._x;
                    prepareControlDataParam._initialDisplaySize._y = mint::max(fontSize, common._size._y);
                    prepareControlDataParam._offset = common._offset;
                }
                return prepareControlDataParam;
            }
        }
        

        void GuiContext::ControlInteractionStates::setControlHovered(const ControlData& controlData) noexcept
        {
            resetHover();

            _hoveredControlHashKey = controlData.getHashKey();

            if (_hoverStarted == false)
            {
                _hoverStarted = true;
            }
        }

        const bool GuiContext::ControlInteractionStates::setControlPressed(const ControlData& controlData) noexcept
        {
            if (isControlHovered(controlData) == true)
            {
                resetHover();
            }

            if (_pressedControlHashKey != controlData.getHashKey())
            {
                _pressedControlHashKey = controlData.getHashKey();
                _pressedControlInitialPosition = controlData._position;

                return true;
            }

            return false;
        }

        const bool GuiContext::ControlInteractionStates::setControlClicked(const ControlData& controlData) noexcept
        {
            if (_pressedControlHashKey == controlData.getHashKey())
            {
                _clickedControlHashKeyPerFrame = controlData.getHashKey();

                return true;
            }
            return false;
        }

        void GuiContext::ControlInteractionStates::setControlFocused(const ControlData& controlData) noexcept
        {
            _focusedControlHashKey = controlData.getHashKey();
        }

        const bool GuiContext::ControlInteractionStates::isControlHovered(const ControlData& controlData) const noexcept
        {
            return (controlData.getHashKey() == _hoveredControlHashKey);
        }

        const bool GuiContext::ControlInteractionStates::isControlPressed(const ControlData& controlData) const noexcept
        {
            return (controlData.getHashKey() == _pressedControlHashKey);
        }

        const bool GuiContext::ControlInteractionStates::isControlClicked(const ControlData& controlData) const noexcept
        {
            return (controlData.getHashKey() == _clickedControlHashKeyPerFrame);
        }

        const bool GuiContext::ControlInteractionStates::isControlFocused(const ControlData& controlData) const noexcept
        {
            return (controlData.getHashKey() == _focusedControlHashKey);
        }

        const bool GuiContext::ControlInteractionStates::isHoveringMoreThan(const uint64 durationMs) const noexcept
        {
            return (_hoverStarted == true && _hoverStartTimeMs + durationMs < mint::Profiler::getCurrentTimeMs());
        }

        void GuiContext::ControlInteractionStates::resetPerFrameStates(const MouseStates& mouseStates) noexcept
        {
            if (mouseStates.isButtonDownUp() == true)
            {
                if (_pressedControlHashKey == 1)
                {
                    _focusedControlHashKey = 0;
                }

                _pressedControlHashKey = 0;
            }

            _isMouseInteractionDoneThisFrame = false;
            _clickedControlHashKeyPerFrame = 0;
        }

        void GuiContext::ControlInteractionStates::resetHover() noexcept
        {
            _hoveredControlHashKey = 0;
            _hoverStartTimeMs = mint::Profiler::getCurrentTimeMs();
            _tooltipPosition.setZero();
            _tooltipParentWindowHashKey = 0;
        }

        void GuiContext::ControlInteractionStates::resetHoverIf(const ControlData& controlData) noexcept
        {
            if (controlData.getHashKey() == _hoveredControlHashKey)
            {
                resetHover();
            }
        }

        void GuiContext::ControlInteractionStates::resetPressIf(const ControlData& controlData) noexcept
        {
            if (controlData.getHashKey() == _pressedControlHashKey)
            {
                _pressedControlHashKey = 0;
                _pressedControlInitialPosition.setZero();
            }
        }

        const mint::Float2 GuiContext::ControlInteractionStates::getTooltipWindowPosition(const ControlData& tooltipParentWindow) const noexcept
        {
            return _tooltipPosition - tooltipParentWindow._position + mint::Float2(12.0f, -16.0f);
        }

        void GuiContext::ControlInteractionStates::setTooltipData(const MouseStates& mouseStates, const wchar_t* const tooltipText, const uint64 tooltipParentWindowHashKey) noexcept
        {
            _tooltipTextFinal = tooltipText;
            _tooltipPosition = mouseStates.getPosition();
            _tooltipParentWindowHashKey = tooltipParentWindowHashKey;

            _hoverStarted = false;
        }


        GuiContext::GuiContext(mint::RenderingBase::GraphicDevice* const graphicDevice)
            : _graphicDevice{ graphicDevice }
            , _fontSize{ 0.0f }
            , _rendererContexts{ _graphicDevice, _graphicDevice, _graphicDevice, _graphicDevice, _graphicDevice }
            , _updateScreenSizeCounter{ 0 }
            , _isDragBegun{ false }
            , _draggedControlHashKey{ 0 }
            , _isResizeBegun{ false }
            , _resizedControlHashKey{ 0 }
            , _resizingMethod{ ResizingMethod::ResizeOnly }
            , _caretBlinkIntervalMs{ 0 }
            , _wcharInput{ L'\0'}
            , _wcharInputCandiate{ L'\0'}
            , _keyCode{ mint::Window::EventData::KeyCode::NONE }
        {
            setNamedColor(NamedColor::NormalState, mint::RenderingBase::Color(45, 47, 49));
            setNamedColor(NamedColor::HoverState, getNamedColor(NamedColor::NormalState).addedRgb(0.25f));
            setNamedColor(NamedColor::PressedState, mint::RenderingBase::Color(80, 100, 120));

            setNamedColor(NamedColor::WindowFocused, mint::RenderingBase::Color(3, 5, 7));
            setNamedColor(NamedColor::WindowOutOfFocus, getNamedColor(NamedColor::WindowFocused));
            setNamedColor(NamedColor::Dock, getNamedColor(NamedColor::NormalState));
            setNamedColor(NamedColor::ShownInDock, mint::RenderingBase::Color(23, 25, 27));
            setNamedColor(NamedColor::HighlightColor, mint::RenderingBase::Color(100, 180, 255));

            setNamedColor(NamedColor::TitleBarFocused, getNamedColor(NamedColor::WindowFocused));
            setNamedColor(NamedColor::TitleBarOutOfFocus, getNamedColor(NamedColor::WindowOutOfFocus));

            setNamedColor(NamedColor::TooltipBackground, mint::RenderingBase::Color(200, 255, 220));

            setNamedColor(NamedColor::ScrollBarTrack, mint::RenderingBase::Color(80, 82, 84));
            setNamedColor(NamedColor::ScrollBarThumb, getNamedColor(NamedColor::ScrollBarTrack).addedRgb(0.25f));

            setNamedColor(NamedColor::LightFont, mint::RenderingBase::Color(233, 235, 237));
            setNamedColor(NamedColor::DarkFont, getNamedColor(NamedColor::LightFont).addedRgb(-0.75f));
            setNamedColor(NamedColor::ShownInDockFont, getNamedColor(NamedColor::HighlightColor));
        }

        GuiContext::~GuiContext()
        {
            __noop;
        }

        void GuiContext::initialize(const char* const font)
        {
            _fontSize = static_cast<float>(_graphicDevice->getFontRendererContext().getFontSize());
            
            _caretBlinkIntervalMs = _graphicDevice->getWindow()->getCaretBlinkIntervalMs();

            const mint::RenderingBase::FontRendererContext::FontData& fontData = _graphicDevice->getFontRendererContext().getFontData();
            for (int32 rendererContextIndex = 0; rendererContextIndex < getRendererContextLayerCount(); rendererContextIndex++)
            {
                if (_rendererContexts[rendererContextIndex].initializeFontData(fontData) == false)
                {
                    MINT_ASSERT("김장원", false, "ShapeFontRendererContext::initializeFont() 에 실패했습니다!");
                }

                _rendererContexts[rendererContextIndex].initializeShaders();
                _rendererContexts[rendererContextIndex].setUseMultipleViewports();
            }

            const mint::Float2& windowSize = mint::Float2(_graphicDevice->getWindowSize());
            _rootControlData = ControlData(1, 0, mint::Gui::ControlType::ROOT, windowSize);
            _rootControlData._isFocusable = false;

            updateScreenSize(windowSize);

            _nextControlStates.reset();
            resetPerFrameStates();
        }

        void GuiContext::updateScreenSize(const mint::Float2& newScreenSize)
        {
            _clipRectFullScreen = _graphicDevice->getFullScreenClipRect();

            _rootControlData._displaySize = newScreenSize;
            _rootControlData.setClipRectXXX(_clipRectFullScreen);
            _rootControlData.setClipRectForChildrenXXX(_clipRectFullScreen);
            _rootControlData.setClipRectForDocksXXX(_clipRectFullScreen);

            _updateScreenSizeCounter = 2;
        }

        void GuiContext::processEvent(mint::Window::IWindow* const window) noexcept
        {
            MINT_ASSERT("김장원", window != nullptr, "window 가 nullptr 이면 안 됩니다!");
            
            // 초기화
            _mouseStates.resetPerFrame();
            
            if (window->hasEvent() == true)
            {
                const mint::Window::EventData& eventData = window->peekEvent();
                if (eventData._type == mint::Window::EventType::MouseMove)
                {
                    _mouseStates.setPosition(eventData._value.getMousePosition());
                }
                else if (eventData._type == mint::Window::EventType::MouseDown)
                {
                    MINT_LOG("김장원", "Event: MouseDown");

                    _mouseStates.setButtonDown();
                    _mouseStates.setButtonDownPosition(eventData._value.getMousePosition());
                }
                else if (eventData._type == mint::Window::EventType::MouseUp)
                {
                    MINT_LOG("김장원", "Event: MouseUp");

                    if (_taskWhenMouseUp.isSet())
                    {
                        updateDockDatum(_taskWhenMouseUp.getUpdateDockDatum());
                    }

                    _mouseStates.setButtonUp();
                    _mouseStates.setButtonUpPosition(eventData._value.getMousePosition());
                }
                else if (eventData._type == mint::Window::EventType::MouseWheel)
                {
                    _mouseStates._mouseWheel = eventData._value.getMouseWheel();
                }
                else if (eventData._type == mint::Window::EventType::MouseDoubleClicked)
                {
                    _mouseStates.setDoubleClicked();
                    _mouseStates.setButtonDownPosition(eventData._value.getMousePosition());
                }
                else if (eventData._type == mint::Window::EventType::KeyStroke)
                {
                    _wcharInputCandiate = L'\0';

                    _wcharInput = eventData._value.getInputWchar();
                }
                else if (eventData._type == mint::Window::EventType::KeyStrokeCandidate)
                {
                    _wcharInputCandiate = eventData._value.getInputWchar();
                }
                else if (eventData._type == mint::Window::EventType::KeyDown)
                {
                    _keyCode = eventData._value.getKeyCode();
                    
                    if (isFocusedControlInputBox() == true && mint::Window::EventData::isKeyCodeAlnum(_keyCode) == true)
                    {
                        _keyCode = mint::Window::EventData::KeyCode::NONE;
                        window->popEvent();
                    }
                }
            }

            // Root 컨트롤의 Interaction 이 가장 먼저 처리되어야 한다!
            processControlInteractionInternal(_rootControlData, false);
        }

        const bool GuiContext::shouldInteract(const mint::Float2& screenPosition, const ControlData& controlData) const noexcept
        {
            const ControlType controlType = controlData.getControlType();
            const ControlData& parentControlData = getControlData(controlData.getParentHashKey());
            if (controlType == ControlType::Window || parentControlData.hasChildWindow() == false)
            {
                return true;
            }

            // ParentControlData 가 Root 거나 Window 일 때만 여기에 온다.
            const auto& childWindowHashKeyMap = parentControlData.getChildWindowHashKeyMap();
            mint::BucketViewer bucketViewer = childWindowHashKeyMap.getBucketViewer();
            for (; bucketViewer.isValid(); bucketViewer.next())
            {
                const ControlData& childWindowControlData = getControlData(*bucketViewer.view()._key);
                if (ControlCommonHelpers::isInControlInteractionArea(screenPosition, childWindowControlData) == true)
                {
                    return false;
                }
            }
            return true;
        }

        void GuiContext::testWindow(VisibleState& inoutVisibleState)
        {
            mint::Gui::WindowParam windowParam;
            windowParam._common._size = mint::Float2(500.0f, 500.0f);
            windowParam._position = mint::Float2(200.0f, 50.0f);
            windowParam._scrollBarType = mint::Gui::ScrollBarType::Both;
            if (beginWindow(L"TestWindow", windowParam, inoutVisibleState) == true)
            {
                static VisibleState childWindowVisibleState0;
                static VisibleState childWindowVisibleState1;


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

                if (beginCheckBox(L"체크박스") == true)
                {
                    endCheckBox();
                }

                {
                    mint::Gui::SliderParam sliderParam;
                    sliderParam._common._size._y = 32.0f;
                    float value = 0.0f;
                    if (beginSlider(L"Slider0", sliderParam, value) == true)
                    {
                        endSlider();
                    }
                }


                nextSameLine();
                if (beginButton(L"ChildWindow0") == true)
                {
                    childWindowVisibleState0 = VisibleState::VisibleOpen;

                    endButton();
                }

                nextSameLine();
                if (beginButton(L"ChildWindow1") == true)
                {
                    childWindowVisibleState1 = VisibleState::VisibleOpen;

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
                    mint::Gui::TextBoxParam textBoxParam;
                    textBoxParam._common._size._x = 240.0f;
                    textBoxParam._common._size._y = 24.0f;
                    textBoxParam._alignmentHorz = mint::Gui::TextAlignmentHorz::Center;
                    if (beginTextBox(L"TextBox", textBoxParam, textBoxContent) == true)
                    {
                        endTextBox();
                    }
                }

                mint::Gui::ListViewParam listViewParam;
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
                    mint::Gui::WindowParam testWindowParam;
                    testWindowParam._common._size = mint::Float2(200.0f, 240.0f);
                    testWindowParam._scrollBarType = mint::Gui::ScrollBarType::Both;
                    testWindowParam._initialDockingMethod = mint::Gui::DockingMethod::BottomSide;
                    if (beginWindow(L"1ST", testWindowParam, childWindowVisibleState0))
                    {
                        if (beginButton(L"테스트!!") == true)
                        {
                            endButton();
                        }

                        endWindow();
                    }
                }

                {
                    mint::Gui::WindowParam testWindowParam;
                    testWindowParam._common._size = mint::Float2(100.0f, 100.0f);
                    testWindowParam._position._x = 10.0f;
                    testWindowParam._position._y = 60.0f;
                    testWindowParam._initialDockingMethod = mint::Gui::DockingMethod::BottomSide;
                    if (beginWindow(L"2NDDD", testWindowParam, childWindowVisibleState1))
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

        void GuiContext::testDockedWindow(VisibleState& inoutVisibleState)
        {
            mint::Gui::WindowParam windowParam;
            windowParam._common._size = mint::Float2(300.0f, 400.0f);
            windowParam._position = mint::Float2(20.0f, 50.0f);
            windowParam._initialDockingMethod = mint::Gui::DockingMethod::RightSide;
            windowParam._initialDockingSize._x = 240.0f;
            if (beginWindow(L"TestDockedWindow", windowParam, inoutVisibleState) == true)
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

                pushLabel(L"TestLable00000", L"A label!");

                nextSameLine();

                if (beginButton(L"Fourth") == true)
                {
                    endButton();
                }

                endWindow();
            }
        }

        const bool GuiContext::beginWindow(const wchar_t* const title, const WindowParam& windowParam, VisibleState& inoutVisibleState)
        {
            static constexpr ControlType controlType = ControlType::Window;
            
            nextNoAutoPositioned();

            ControlData& windowControlData = createOrGetControlData(title, controlType);
            windowControlData._dockingControlType = DockingControlType::DockerDock;
            windowControlData._isFocusable = true;
            windowControlData._controlValue._windowData._titleBarSize = kTitleBarBaseSize;
            if (windowControlData.visibleStateEquals(inoutVisibleState) == false)
            {
                windowControlData.setVisibleState(inoutVisibleState);

                if (windowControlData.isControlVisible() == true)
                {
                    setControlFocused(windowControlData);
                }
            }
            dockWindowOnceInitially(windowControlData, windowParam._initialDockingMethod, windowParam._initialDockingSize);

            PrepareControlDataParam prepareControlDataParam;
            {
                const float titleWidth = calculateTextWidth(title, mint::StringUtil::wcslen(title));
                prepareControlDataParam._initialDisplaySize = windowParam._common._size;
                prepareControlDataParam._initialResizingMask.setAllTrue();
                prepareControlDataParam._desiredPositionInParent = windowParam._position;
                prepareControlDataParam._innerPadding = kWindowInnerPadding;
                prepareControlDataParam._displaySizeMin._x = titleWidth + kTitleBarInnerPadding.horz() + kDefaultRoundButtonRadius * 2.0f;
                prepareControlDataParam._displaySizeMin._y = windowControlData.getTopOffsetToClientArea() + 16.0f;
                prepareControlDataParam._alwaysResetPosition = false;
                prepareControlDataParam._clipRectUsage = ClipRectUsage::Own;
                prepareControlDataParam._deltaInteractionSizeByDock._x = -windowControlData.getHorzDockSizeSum();
                prepareControlDataParam._deltaInteractionSizeByDock._y = -windowControlData.getVertDockSizeSum();
            }
            prepareControlData(windowControlData, prepareControlDataParam);

            updateWindowPositionByParentWindow(windowControlData);
            
            updateDockingWindowDisplay(windowControlData);

            const bool needToProcessControl = needToProcessWindowControl(windowControlData);
            {
                mint::RenderingBase::Color finalBackgroundColor;
                const bool isFocused = (needToProcessControl == true) 
                    ? processFocusControl(windowControlData, getNamedColor(NamedColor::WindowFocused), getNamedColor(NamedColor::WindowOutOfFocus), finalBackgroundColor)
                    : false;
                
                // Viewport & Scissor rectangle
                {
                    const ControlData& parentControlData = getControlData(windowControlData.getParentHashKey());
                    const bool isParentAlsoWindow = parentControlData.isTypeOf(ControlType::Window);
                    {
                        mint::Rect clipRectForMe = windowControlData.getControlRect();
                        if (isParentAlsoWindow == true)
                        {
                            clipRectForMe.clipBy(parentControlData.getClipRectForDocks());
                        }
                        setClipRectForMe(windowControlData, clipRectForMe);
                    }
                    {
                        mint::Rect clipRectForDocks = windowControlData.getControlPaddedRect();
                        clipRectForDocks.top() += static_cast<LONG>(kTitleBarBaseSize._y);
                        if (isParentAlsoWindow == true)
                        {
                            clipRectForDocks.clipBy(parentControlData.getClipRectForDocks());
                        }
                        setClipRectForDocks(windowControlData, clipRectForDocks);
                    }
                    {
                        const bool hasScrollBarVert = windowControlData._controlValue._commonData.isScrollBarEnabled(ScrollBarType::Vert);
                        const bool hasScrollBarHorz = windowControlData._controlValue._commonData.isScrollBarEnabled(ScrollBarType::Horz);

                        mint::Rect clipRectForChildren = windowControlData.getControlPaddedRect();
                        clipRectForChildren.top() += static_cast<LONG>(windowControlData.getTopOffsetToClientArea() + windowControlData.getDockSizeIfHosting(DockingMethod::TopSide)._y);
                        clipRectForChildren.left() += static_cast<LONG>(windowControlData.getDockSizeIfHosting(DockingMethod::LeftSide)._x);
                        clipRectForChildren.right() -= static_cast<LONG>(((hasScrollBarVert == true) ? kScrollBarThickness : 0.0f) + windowControlData.getDockSizeIfHosting(DockingMethod::RightSide)._x);
                        clipRectForChildren.bottom() -= static_cast<LONG>(((hasScrollBarHorz == true) ? kScrollBarThickness : 0.0f) + windowControlData.getDockSizeIfHosting(DockingMethod::BottomSide)._y);
                        if (isParentAlsoWindow == true)
                        {
                            clipRectForChildren.clipBy(parentControlData.getClipRect());
                        }
                        setClipRectForChildren(windowControlData, clipRectForChildren);
                    }
                }

                if (needToProcessControl == true)
                {
                    const bool isAncestorFocused = isAncestorControlFocused(windowControlData);
                    windowControlData._rendererContextLayer = (isFocused || isAncestorFocused) ? RendererContextLayer::Foreground : RendererContextLayer::Background;
                    
                    mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(windowControlData);
                    rendererContext.setClipRect(windowControlData.getClipRect());

                    const mint::Float4& windowCenterPosition = getControlCenterPosition(windowControlData);
                    rendererContext.setColor(finalBackgroundColor);
                    rendererContext.setPosition(windowCenterPosition + mint::Float4(0, windowControlData._controlValue._windowData._titleBarSize._y * 0.5f, 0, 0));
                    if (windowControlData.isDocking() == true)
                    {
                        mint::RenderingBase::Color inDockColor = getNamedColor(NamedColor::ShownInDock);
                        inDockColor.a(finalBackgroundColor.a());
                        rendererContext.setColor(inDockColor);
                        rendererContext.drawRectangle(windowControlData._displaySize - mint::Float2(0, windowControlData._controlValue._windowData._titleBarSize._y), 0.0f, 0.0f);
                    }
                    else
                    {
                        rendererContext.drawHalfRoundedRectangle(windowControlData._displaySize - mint::Float2(0, windowControlData._controlValue._windowData._titleBarSize._y), (kDefaultRoundnessInPixel * 2.0f / windowControlData._displaySize.minElement()), 0.0f);
                    }

                    processDock(windowControlData, rendererContext);
                    _controlStackPerFrame.push_back(ControlStackData(windowControlData));
                }
            }
            
            if (windowControlData.isControlVisible() == true)
            {
                windowControlData._controlValue._windowData._titleBarSize._x = windowControlData._displaySize._x;
                {
                    nextNoAutoPositioned(); // 중요

                    beginTitleBar(title, windowControlData._controlValue._windowData._titleBarSize, kTitleBarInnerPadding, inoutVisibleState);
                    endTitleBar();
                }

                if (windowParam._scrollBarType != ScrollBarType::None)
                {
                    pushScrollBar(windowParam._scrollBarType);
                }
            }
            
            return needToProcessControl;
        }

        void GuiContext::dockWindowOnceInitially(ControlData& windowControlData, const DockingMethod dockingMethod, const mint::Float2& initialDockingSize)
        {
            MINT_ASSERT("김장원", windowControlData.isTypeOf(ControlType::Window) == true, "Window 가 아니면 사용하면 안 됩니다!");

            // Initial docking
            if (windowControlData._updateCount == 2 && dockingMethod != DockingMethod::COUNT)
            {
                windowControlData._lastDockingMethodCandidate = dockingMethod;

                ControlData& parentControlData = getControlData(windowControlData.getParentHashKey());
                if (dockingMethod == DockingMethod::LeftSide || dockingMethod == DockingMethod::RightSide)
                {
                    parentControlData.setDockSize(dockingMethod, mint::Float2(initialDockingSize._x, parentControlData._displaySize._y));
                }
                else
                {
                    parentControlData.setDockSize(dockingMethod, mint::Float2(parentControlData._displaySize._x, initialDockingSize._y));
                }

                dock(windowControlData.getHashKey(), parentControlData.getHashKey());
            }
        }

        void GuiContext::updateWindowPositionByParentWindow(ControlData& windowControlData) noexcept
        {
            MINT_ASSERT("김장원", windowControlData.isTypeOf(ControlType::Window) == true, "Window 가 아니면 사용하면 안 됩니다!");

            const ControlData& parentControlData = getControlData(windowControlData.getParentHashKey());
            const bool isParentAlsoWindow = parentControlData.isTypeOf(ControlType::Window);
            if (isParentAlsoWindow == true)
            {
                // 부모 윈도우가 이동한 만큼 내 위치도 이동!
                windowControlData._position += parentControlData._currentFrameDeltaPosition;

                // 계층 가장 아래 Window 까지 전파되도록
                windowControlData._currentFrameDeltaPosition = parentControlData._currentFrameDeltaPosition;
            }
        }

        void GuiContext::updateDockingWindowDisplay(ControlData& windowControlData) noexcept
        {
            MINT_ASSERT("김장원", windowControlData.isTypeOf(ControlType::Window) == true, "Window 가 아니면 사용하면 안 됩니다!");

            if (windowControlData.isDocking() == true)
            {
                const ControlData& dockControlData = getControlData(windowControlData.getDockControlHashKey());
                if (0 < _updateScreenSizeCounter)
                {
                    windowControlData._position = dockControlData.getDockPosition(windowControlData._lastDockingMethod);
                    windowControlData._displaySize = dockControlData.getDockSize(windowControlData._lastDockingMethod);
                }
            }
        }

        const bool GuiContext::needToProcessWindowControl(const ControlData& windowControlData) const noexcept
        {
            MINT_ASSERT("김장원", windowControlData.isTypeOf(ControlType::Window) == true, "Window 가 아니면 사용하면 안 됩니다!");

            const bool isDocking = windowControlData.isDocking();
            bool needToProcessControl = windowControlData.isControlVisible();
            if (isDocking == true)
            {
                const ControlData& dockControlData = getControlData(windowControlData.getDockControlHashKey());
                const bool isShownInDock = dockControlData.isShowingInDock(windowControlData);
                needToProcessControl &= (isDocking && isShownInDock);
            }
            return needToProcessControl;
        }

        const bool GuiContext::beginButton(const wchar_t* const text)
        {
            static constexpr ControlType controlType = ControlType::Button;
            
            ControlData& controlData = createOrGetControlData(text, controlType);
            PrepareControlDataParam prepareControlDataParam;
            {
                const float textWidth = calculateTextWidth(text, mint::StringUtil::wcslen(text));
                prepareControlDataParam._initialDisplaySize = mint::Float2(textWidth + 24, _fontSize + 12);
            }
            prepareControlData(controlData, prepareControlDataParam);
        
            mint::RenderingBase::Color finalBackgroundColor;
            const bool isClicked = processClickControl(controlData, getNamedColor(NamedColor::NormalState), getNamedColor(NamedColor::HoverState), getNamedColor(NamedColor::PressedState), finalBackgroundColor);
            
            const mint::Float4& controlCenterPosition = getControlCenterPosition(controlData);
            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            rendererContext.setClipRect(controlData.getClipRect());
            rendererContext.setColor(finalBackgroundColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(controlData._displaySize, (kDefaultRoundnessInPixel * 2.0f / controlData._displaySize.minElement()), 0.0f, 0.0f);

            rendererContext.setTextColor(getNamedColor(NamedColor::LightFont) * mint::RenderingBase::Color(1.0f, 1.0f, 1.0f, finalBackgroundColor.a()));
            rendererContext.drawDynamicText(text, controlCenterPosition, 
                mint::RenderingBase::FontRenderingOption(mint::RenderingBase::TextRenderDirectionHorz::Centered, mint::RenderingBase::TextRenderDirectionVert::Centered, kFontScaleB));

            if (isClicked == true)
            {
                _controlStackPerFrame.push_back(ControlStackData(controlData));
            }
            return isClicked;
        }

        const bool GuiContext::beginCheckBox(const wchar_t* const text, bool* const outIsChecked)
        {
            static constexpr ControlType controlType = ControlType::CheckBox;

            ControlData& controlData = createOrGetControlData(text, controlType);
            PrepareControlDataParam prepareControlDataParam;
            {
                prepareControlDataParam._initialDisplaySize = kCheckBoxSize;
            }
            prepareControlData(controlData, prepareControlDataParam);

            mint::RenderingBase::Color finalBackgroundColor;
            const bool isClicked = processToggleControl(controlData, getNamedColor(NamedColor::NormalState), getNamedColor(NamedColor::NormalState), getNamedColor(NamedColor::HighlightColor), finalBackgroundColor);
            const bool isChecked = controlData._controlValue._booleanData.get();
            if (nullptr != outIsChecked)
            {
                *outIsChecked = isChecked;
            }

            const mint::Float4& controlCenterPosition = getControlCenterPosition(controlData);
            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            rendererContext.setClipRect(controlData.getClipRect());
            rendererContext.setColor(finalBackgroundColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(controlData._displaySize, (kDefaultRoundnessInPixel / controlData._displaySize.minElement()), 0.0f, 0.0f);

            if (isChecked == true)
            {
                mint::Float2 p0 = mint::Float2(controlCenterPosition._x - 1.0f, controlCenterPosition._y + 4.0f);
                rendererContext.setColor(getNamedColor(NamedColor::LightFont));
                rendererContext.drawLine(p0, p0 + mint::Float2(-4.0f, -5.0f), 2.0f);
                rendererContext.drawLine(p0, p0 + mint::Float2(+7.0f, -8.0f), 2.0f);
            }

            rendererContext.setTextColor(getNamedColor(NamedColor::LightFont) * mint::RenderingBase::Color(1.0f, 1.0f, 1.0f, finalBackgroundColor.a()));
            rendererContext.drawDynamicText(text, controlCenterPosition + mint::Float4(kCheckBoxSize._x * 0.75f, 0.0f, 0.0f, 0.0f), 
                mint::RenderingBase::FontRenderingOption(mint::RenderingBase::TextRenderDirectionHorz::Rightward, mint::RenderingBase::TextRenderDirectionVert::Centered, kFontScaleB));

            if (isClicked == true)
            {
                _controlStackPerFrame.push_back(ControlStackData(controlData));
            }
            return isClicked;
        }

        void GuiContext::pushLabel(const wchar_t* const name, const wchar_t* const text, const LabelParam& labelParam)
        {
            static constexpr ControlType controlType = ControlType::Label;

            ControlData& controlData = createOrGetControlData(text, controlType, generateControlKeyString(name, controlType));
            PrepareControlDataParam prepareControlDataParam;
            {
                const float textWidth = calculateTextWidth(text, mint::StringUtil::wcslen(text));
                prepareControlDataParam._initialDisplaySize = ((labelParam._common._size == mint::Float2::kZero)
                    ? mint::Float2(textWidth + labelParam._paddingForAutoSize._x, _fontSize + labelParam._paddingForAutoSize._y)
                    : labelParam._common._size);
                prepareControlDataParam._offset = labelParam._common._offset;
            }
            prepareControlData(controlData, prepareControlDataParam);
            
            mint::RenderingBase::Color colorWithAlpha = mint::RenderingBase::Color(255, 255, 255);
            processShowOnlyControl(controlData, colorWithAlpha);

            const mint::Float4& controlCenterPosition = getControlCenterPosition(controlData);
            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            rendererContext.setClipRect(controlData.getClipRect());
            rendererContext.setColor(labelParam._common._backgroundColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRectangle(controlData._displaySize, 0.0f, 0.0f);

            rendererContext.setTextColor((labelParam._common._fontColor.isTransparent() == true) ? getNamedColor(NamedColor::LightFont) * colorWithAlpha : labelParam._common._fontColor);
            const mint::Float4 textPosition = labelCalculateTextPosition(labelParam, controlData);
            const mint::RenderingBase::FontRenderingOption fontRenderingOption = labelGetFontRenderingOption(labelParam, controlData);
            rendererContext.drawDynamicText(text, textPosition, fontRenderingOption);
        }

        mint::Float4 GuiContext::labelCalculateTextPosition(const LabelParam& labelParam, const ControlData& labelControlData) const noexcept
        {
            MINT_ASSERT("김장원", labelControlData.isTypeOf(ControlType::Label) == true, "Label 이 아니면 사용하면 안 됩니다!");

            mint::Float4 textPosition = getControlCenterPosition(labelControlData);
            if (labelParam._alignmentHorz != TextAlignmentHorz::Center)
            {
                if (labelParam._alignmentHorz == TextAlignmentHorz::Left)
                {
                    textPosition._x = labelControlData._position._x;
                }
                else
                {
                    textPosition._x = labelControlData._position._x + labelControlData._displaySize._x;
                }
            }
            if (labelParam._alignmentVert != TextAlignmentVert::Middle)
            {
                if (labelParam._alignmentVert == TextAlignmentVert::Top)
                {
                    textPosition._y = labelControlData._position._y;
                }
                else
                {
                    textPosition._y = labelControlData._position._y + labelControlData._displaySize._y;
                }
            }
            return textPosition;
        }

        mint::RenderingBase::FontRenderingOption GuiContext::labelGetFontRenderingOption(const LabelParam& labelParam, const ControlData& labelControlData) const noexcept
        {
            MINT_ASSERT("김장원", labelControlData.isTypeOf(ControlType::Label) == true, "Label 이 아니면 사용하면 안 됩니다!");

            mint::RenderingBase::TextRenderDirectionHorz textRenderDirectionHorz = mint::RenderingBase::TextRenderDirectionHorz::Centered;
            mint::RenderingBase::TextRenderDirectionVert textRenderDirectionVert = mint::RenderingBase::TextRenderDirectionVert::Centered;
            if (labelParam._alignmentHorz != TextAlignmentHorz::Center)
            {
                if (labelParam._alignmentHorz == TextAlignmentHorz::Left)
                {
                    textRenderDirectionHorz = mint::RenderingBase::TextRenderDirectionHorz::Rightward;
                }
                else
                {
                    textRenderDirectionHorz = mint::RenderingBase::TextRenderDirectionHorz::Leftward;
                }
            }
            if (labelParam._alignmentVert != TextAlignmentVert::Middle)
            {
                if (labelParam._alignmentVert == TextAlignmentVert::Top)
                {
                    textRenderDirectionVert = mint::RenderingBase::TextRenderDirectionVert::Downward;
                }
                else
                {
                    textRenderDirectionVert = mint::RenderingBase::TextRenderDirectionVert::Upward;
                }
            }
            return mint::RenderingBase::FontRenderingOption(textRenderDirectionHorz, textRenderDirectionVert, kFontScaleB);
        }

        const bool GuiContext::beginSlider(const wchar_t* const name, const SliderParam& sliderParam, float& outValue)
        {
            static constexpr ControlType trackControlType = ControlType::Slider;

            ControlData& trackControlData = createOrGetControlData(name, trackControlType);
            PrepareControlDataParam prepareControlDataParamForTrack;
            {
                prepareControlDataParamForTrack._initialDisplaySize._x = sliderParam._common._size._x;
                prepareControlDataParamForTrack._initialDisplaySize._y = (0.0f == sliderParam._common._size._y) ? kSliderThumbRadius * 2.0f : sliderParam._common._size._y;
            }
            prepareControlData(trackControlData, prepareControlDataParamForTrack);
            
            mint::RenderingBase::Color trackColor = getNamedColor(NamedColor::HoverState);
            processShowOnlyControl(trackControlData, trackColor, false);

            bool isChanged = false;
            {
                static constexpr ControlType thumbControlType = ControlType::SliderThumb;

                nextNoAutoPositioned();

                const float sliderValidLength = sliderParam._common._size._x - kSliderThumbRadius * 2.0f;
                ControlData& thumbControlData = createOrGetControlData(name, thumbControlType);
                thumbControlData._position._x = trackControlData._position._x + trackControlData._controlValue._thumbData._thumbAt * sliderValidLength;
                thumbControlData._position._y = trackControlData._position._y + trackControlData._displaySize._y * 0.5f - thumbControlData._displaySize._y * 0.5f;
                thumbControlData._isDraggable = true;
                thumbControlData._draggingConstraints.top(thumbControlData._position._y);
                thumbControlData._draggingConstraints.bottom(thumbControlData._draggingConstraints.top());
                thumbControlData._draggingConstraints.left(trackControlData._position._x);
                thumbControlData._draggingConstraints.right(thumbControlData._draggingConstraints.left() + sliderValidLength);
                PrepareControlDataParam prepareControlDataParamForThumb;
                {
                    const ControlData& parentWindowControlData = getParentWindowControlData(trackControlData);

                    prepareControlDataParamForThumb._initialDisplaySize._x = kSliderThumbRadius * 2.0f;
                    prepareControlDataParamForThumb._initialDisplaySize._y = kSliderThumbRadius * 2.0f;
                    prepareControlDataParamForThumb._alwaysResetPosition = false;
                    prepareControlDataParamForThumb._desiredPositionInParent = trackControlData._position - parentWindowControlData._position;
                }
                prepareControlData(thumbControlData, prepareControlDataParamForThumb);
                
                mint::RenderingBase::Color thumbColor;
                processScrollableControl(thumbControlData, getNamedColor(NamedColor::HighlightColor), getNamedColor(NamedColor::HighlightColor).addedRgb(0.125f), thumbColor);

                const float thumbAt = (thumbControlData._position._x - trackControlData._position._x) / sliderValidLength;
                if (trackControlData._controlValue._thumbData._thumbAt != thumbAt)
                {
                    _controlStackPerFrame.push_back(ControlStackData(trackControlData));

                    isChanged = true;
                }
                trackControlData._controlValue._thumbData._thumbAt = thumbAt;
                outValue = thumbAt;
                
                // 반드시 thumbAt 이 갱신된 이후에 draw 를 한다.
                sliderDrawTrack(sliderParam, trackControlData, trackColor);
                sliderDrawThumb(sliderParam, thumbControlData, thumbColor);
            }
            
            return isChanged;
        }

        void GuiContext::sliderDrawTrack(const SliderParam& sliderParam, const ControlData& trackControlData, const mint::RenderingBase::Color& trackColor) noexcept
        {
            MINT_ASSERT("김장원", trackControlData.isTypeOf(ControlType::Slider) == true, "Slider (Track) 이 아니면 사용하면 안 됩니다!");

            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(trackControlData);
            const float trackRadius = kSliderTrackThicknes * 0.5f;
            const float trackRectLength = sliderParam._common._size._x - trackRadius * 2.0f;

            const float thumbAt = trackControlData._controlValue._thumbData._thumbAt;
            const float sliderValidLength = sliderParam._common._size._x - kSliderThumbRadius * 2.0f;
            const float trackRectLeftLength = thumbAt * sliderValidLength;
            const float trackRectRightLength = trackRectLength - trackRectLeftLength;

            const mint::Float4& trackCenterPosition = getControlCenterPosition(trackControlData);
            mint::Float4 trackRenderPosition = trackCenterPosition - mint::Float4(trackRectLength * 0.5f, 0.0f, 0.0f, 0.0f);

            // Left(or Upper) half circle
            rendererContext.setClipRect(trackControlData.getClipRect());
            rendererContext.setColor(getNamedColor(NamedColor::HighlightColor));
            rendererContext.setPosition(trackRenderPosition);
            rendererContext.drawHalfCircle(trackRadius, +mint::Math::kPiOverTwo);

            // Left rect
            trackRenderPosition._x += trackRectLeftLength * 0.5f;
            rendererContext.setPosition(trackRenderPosition);
            rendererContext.drawRectangle(mint::Float2(trackRectLeftLength, kSliderTrackThicknes), 0.0f, 0.0f);
            trackRenderPosition._x += trackRectLeftLength * 0.5f;

            // Right rect
            rendererContext.setColor(trackColor);
            trackRenderPosition._x += trackRectRightLength * 0.5f;
            rendererContext.setPosition(trackRenderPosition);
            rendererContext.drawRectangle(mint::Float2(trackRectRightLength, kSliderTrackThicknes), 0.0f, 0.0f);
            trackRenderPosition._x += trackRectRightLength * 0.5f;

            // Right(or Lower) half circle
            rendererContext.setPosition(trackRenderPosition);
            rendererContext.drawHalfCircle(trackRadius, -mint::Math::kPiOverTwo);
        }

        void GuiContext::sliderDrawThumb(const SliderParam& sliderParam, const ControlData& thumbControlData, const mint::RenderingBase::Color& thumbColor) noexcept
        {
            MINT_ASSERT("김장원", thumbControlData.isTypeOf(ControlType::SliderThumb) == true, "Slider Thumb 이 아니면 사용하면 안 됩니다!");

            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(thumbControlData);
            const mint::Float4& thumbCenterPosition = getControlCenterPosition(thumbControlData);
            rendererContext.setPosition(thumbCenterPosition);
            rendererContext.setColor(mint::RenderingBase::Color::kWhite.scaledA(thumbColor.a()));
            rendererContext.drawCircle(kSliderThumbRadius);

            rendererContext.setColor(thumbColor);
            rendererContext.drawCircle(kSliderThumbRadius - 2.0f);
        }

        const bool GuiContext::beginTextBox(const wchar_t* const name, const TextBoxParam& textBoxParam, std::wstring& outText)
        {
            static constexpr ControlType controlType = ControlType::TextBox;
            
            ControlData& controlData = createOrGetControlData(name, controlType);
            controlData._isFocusable = true;
            prepareControlData(controlData, PrepareControlDataUtils::prepareInputBox(textBoxParam._common, _fontSize));
            
            mint::RenderingBase::Color finalBackgroundColor;
            const bool wasFocused = _controlInteractionStates.isControlFocused(controlData);
            const bool isFocused = processFocusControl(controlData, textBoxParam._common._backgroundColor, textBoxParam._common._backgroundColor.addedRgb(-0.125f), finalBackgroundColor);
            {
                const ControlData& parentControlData = getControlData(controlData.getParentHashKey());
                mint::Rect clipRectForMe = controlData.getControlRect();
                clipRectForMe.clipBy(parentControlData.getClipRectForChildren());
                setClipRectForMe(controlData, clipRectForMe);
            }

            const wchar_t inputCandidate[2]{ _wcharInputCandiate, L'\0' };
            const float inputCandidateWidth = ((isFocused == true) && (32 <= _wcharInputCandiate)) ? calculateTextWidth(inputCandidate, 1) : 0.0f;
            const uint16 textLength = static_cast<uint16>(outText.length());
            mint::Float4 textRenderOffset;
            if (controlData._controlValue._textBoxData._textDisplayOffset == 0)
            {
                const float fullTextWidth = calculateTextWidth(outText.c_str(), textLength);
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
                textBoxProcessInput(wasFocused, textBoxParam._textInputMode, controlData, textRenderOffset, outText);
            }

            // Caret 의 렌더링 위치가 TextBox 를 벗어나는 경우 처리!!
            const float textWidthTillCaret = calculateTextWidth(outText.c_str(), mint::min(controlData._controlValue._textBoxData._caretAt, textLength));
            inputBoxUpdateTextDisplayOffset(textLength, textWidthTillCaret, inputCandidateWidth, controlData);

            // Box 렌더링
            const mint::Float4& controlCenterPosition = getControlCenterPosition(controlData);
            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            rendererContext.setClipRect(controlData.getClipRect());
            rendererContext.setColor(finalBackgroundColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(controlData._displaySize, (textBoxParam._roundnessInPixel / controlData._displaySize.minElement()), 0.0f, 0.0f);

            // Text 및 Caret 렌더링
            const bool needToRenderInputCandidate = (isFocused == true && 32 <= _wcharInputCandiate);
            if (needToRenderInputCandidate == true)
            {
                textBoxDrawTextWithInputCandidate(textBoxParam._common, textRenderOffset, controlData, outText);
            }
            else
            {
                inputBoxDrawTextWithoutInputCandidate(textBoxParam._common, textRenderOffset, true, controlData, outText);
            }

            // Selection 렌더링
            inputBoxDrawSelection(textRenderOffset, controlData, outText);

            return false;
        }
        
        void GuiContext::textBoxProcessInput(const bool wasControlFocused, const TextInputMode textInputMode, ControlData& controlData, mint::Float4& textRenderOffset, std::wstring& outText) noexcept
        {
            static std::wstring errorMessage;
            if (true == errorMessage.empty())
            {
                errorMessage.resize(1024);
                mint::formatString(errorMessage.data(), 1024, L"텍스트 길이가 %d 자를 넘을 수 없습니다!", kTextBoxMaxTextLength);
            }

            TextBoxProcessInputResult result;
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            uint16& caretState = controlData._controlValue._textBoxData._caretState;
            uint64& lastCaretBlinkTimeMs = controlData._controlValue._textBoxData._lastCaretBlinkTimeMs;
            const uint64 currentTimeMs = mint::Profiler::getCurrentTimeMs();
            if (lastCaretBlinkTimeMs + _caretBlinkIntervalMs < currentTimeMs)
            {
                lastCaretBlinkTimeMs = currentTimeMs;

                caretState ^= 1;
            }

            const mint::Window::IWindow* const window = _graphicDevice->getWindow();
            const bool isMouseLeftDown = window->isMouseDown(mint::Window::EventData::MouseButton::Left);
            const bool isMouseLeftDownFirst = window->isMouseDownFirst(mint::Window::EventData::MouseButton::Left);
            if (isMouseLeftDown == true || isMouseLeftDownFirst == true)
            {
                // 마우스 입력 처리
                textBoxProcessInputMouse(controlData, textRenderOffset, outText, result);
            }
            else
            {
                const bool isShiftKeyDown = window->isKeyDown(mint::Window::EventData::KeyCode::Shift);
                const bool isControlKeyDown = window->isKeyDown(mint::Window::EventData::KeyCode::Control);
                const uint16 oldCaretAt = controlData._controlValue._textBoxData._caretAt;
                if (32 <= _wcharInputCandiate)
                {
                    textBoxEraseSelection(controlData, outText);
                }
                else if (_wcharInput != L'\0')
                {
                    // 글자 입력 or 키 입력

                    const bool isInputCharacter = (32 <= _wcharInput);
                    if (isInputCharacter == true)
                    {
                        if (textBoxIsValidInput(_wcharInput, caretAt, textInputMode, outText) == true)
                        {
                            textBoxEraseSelection(controlData, outText);

                            if (textBoxInsertWchar(_wcharInput, caretAt, outText) == false)
                            {
                                window->showMessageBox(L"오류", errorMessage.c_str(), mint::Window::MessageBoxType::Error);
                            }
                        }
                    }
                    else
                    {
                        const uint16 selectionLength = controlData._controlValue._textBoxData._selectionLength;

                        if (_wcharInput == VK_BACK) // BackSpace
                        {
                            textBoxProcessInputKeyDeleteBefore(controlData, outText);
                        }
                        else if (isControlKeyDown == true && _wcharInput == 0x01) // Ctrl + A
                        {
                            inputBoxProcessInputKeySelectAll(controlData, outText);
                        }
                        else if (isControlKeyDown == true && _wcharInput == 0x03) // Ctrl + C
                        {
                            textBoxProcessInputKeyCopy(controlData, outText);
                        }
                        else if (isControlKeyDown == true && _wcharInput == 0x18) // Ctrl + X
                        {
                            textBoxProcessInputKeyCut(controlData, outText);
                        }
                        else if (isControlKeyDown == true && _wcharInput == 0x16) // Ctrl + V
                        {
                            textBoxProcessInputKeyPaste(controlData, outText, errorMessage.c_str());
                        }
                    }

                    result._clearWcharInput = true;
                }
                else
                {
                    // 키 눌림 처리

                    if (_keyCode == mint::Window::EventData::KeyCode::Left)
                    {
                        textBoxProcessInputCaretToPrev(controlData);
                    }
                    else if (_keyCode == mint::Window::EventData::KeyCode::Right)
                    {
                        textBoxProcessInputCaretToNext(controlData, outText);
                    }
                    else if (_keyCode == mint::Window::EventData::KeyCode::Home)
                    {
                        textBoxProcessInputCaretToHead(controlData);
                    }
                    else if (_keyCode == mint::Window::EventData::KeyCode::End)
                    {
                        textBoxProcessInputCaretToTail(controlData, outText);
                    }
                    else if (_keyCode == mint::Window::EventData::KeyCode::Delete)
                    {
                        textBoxProcessInputKeyDeleteAfter(controlData, outText);
                    }
                }

                // Caret 위치가 바뀐 경우 refresh
                if (oldCaretAt != caretAt)
                {
                    textBoxRefreshCaret(currentTimeMs, caretState, lastCaretBlinkTimeMs);

                    // Selection
                    if (isShiftKeyDown == true && _keyCode != mint::Window::EventData::KeyCode::NONE)
                    {
                        textBoxUpdateSelection(oldCaretAt, caretAt, controlData);
                    }
                }

                if (isShiftKeyDown == false && isControlKeyDown == false &&
                    _keyCode != mint::Window::EventData::KeyCode::NONE &&
                    _keyCode != mint::Window::EventData::KeyCode::Control &&
                    _keyCode != mint::Window::EventData::KeyCode::Alt)
                {
                    // Selection 해제
                    uint16& selectionLength = controlData._controlValue._textBoxData._selectionLength;
                    selectionLength = 0;
                }
            }

            if (wasControlFocused == false)
            {
                textBoxRefreshCaret(currentTimeMs, caretState, lastCaretBlinkTimeMs);
            }

            if (result._clearKeyCode == true)
            {
                _keyCode = mint::Window::EventData::KeyCode::NONE;
            }
            if (result._clearWcharInput == true)
            {
                _wcharInput = L'\0';
            }
        }

        void GuiContext::textBoxProcessInputMouse(ControlData& controlData, mint::Float4& textRenderOffset, std::wstring& outText, TextBoxProcessInputResult& result)
        {
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            const float textDisplayOffset = controlData._controlValue._textBoxData._textDisplayOffset;
            const float positionInText = _mouseStates.getPosition()._x - controlData._position._x + textDisplayOffset - textRenderOffset._x;
            const uint16 textLength = static_cast<uint16>(outText.length());
            const uint32 newCaretAt = calculateIndexFromPositionInText(outText.c_str(), textLength, positionInText);
            const bool isMouseLeftDownFirst = _graphicDevice->getWindow()->isMouseDownFirst(mint::Window::EventData::MouseButton::Left);
            if (isMouseLeftDownFirst == true)
            {
                caretAt = newCaretAt;

                controlData._controlValue._textBoxData._selectionLength = 0;
                controlData._controlValue._textBoxData._selectionStart = caretAt;
            }
            else
            {
                if (newCaretAt != caretAt)
                {
                    textBoxUpdateSelection(caretAt, newCaretAt, controlData);

                    caretAt = newCaretAt;
                }
            }

            result._clearWcharInput = true;
            result._clearKeyCode = true;
        }

        void GuiContext::textBoxProcessInputKeyDeleteBefore(ControlData& controlData, std::wstring& outText)
        {
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            const uint16 selectionLength = controlData._controlValue._textBoxData._selectionLength;
            if (0 < selectionLength)
            {
                textBoxEraseSelection(controlData, outText);
            }
            else
            {
                caretAt = mint::min(caretAt, static_cast<uint16>(outText.length()));

                if (outText.empty() == false && 0 < caretAt)
                {
                    outText.erase(outText.begin() + caretAt - 1);

                    caretAt = mint::max(caretAt - 1, 0);
                }
            }
        }
        
        void GuiContext::textBoxProcessInputKeyDeleteAfter(ControlData& controlData, std::wstring& outText)
        {
            const uint16 selectionLength = controlData._controlValue._textBoxData._selectionLength;
            if (0 < selectionLength)
            {
                textBoxEraseSelection(controlData, outText);
            }
            else
            {
                const uint16 textLength = static_cast<uint16>(outText.length());
                uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
                if (0 < textLength && caretAt < textLength)
                {
                    outText.erase(outText.begin() + caretAt);

                    caretAt = mint::min(caretAt, textLength);
                }
            }
        }
        
        void GuiContext::inputBoxProcessInputKeySelectAll(ControlData& controlData, std::wstring& outText)
        {
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            controlData._controlValue._textBoxData._selectionStart = 0;
            caretAt = controlData._controlValue._textBoxData._selectionLength = static_cast<uint16>(outText.length());
        }

        void GuiContext::textBoxProcessInputKeyCopy(ControlData& controlData, std::wstring& outText)
        {
            const uint16 selectionLength = controlData._controlValue._textBoxData._selectionLength;
            if (selectionLength == 0)
            {
                return;
            }

            const uint16 selectionStart = controlData._controlValue._textBoxData._selectionStart;
            _graphicDevice->getWindow()->textToClipboard(&outText[selectionStart], selectionLength);
        }

        void GuiContext::textBoxProcessInputKeyCut(ControlData& controlData, std::wstring& outText)
        {
            textBoxProcessInputKeyCopy(controlData, outText);

            textBoxEraseSelection(controlData, outText);
        }

        void GuiContext::textBoxProcessInputKeyPaste(ControlData& controlData, std::wstring& outText, const wchar_t* const errorMessage)
        {
            std::wstring fromClipboard;
            _graphicDevice->getWindow()->textFromClipboard(fromClipboard);

            if (fromClipboard.empty() == true)
            {
                return;
            }

            textBoxEraseSelection(controlData, outText);

            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            if (false == textBoxInsertWstring(fromClipboard, caretAt, outText))
            {
                if (errorMessage != nullptr)
                {
                    _graphicDevice->getWindow()->showMessageBox(L"오류", errorMessage, mint::Window::MessageBoxType::Error);
                }
            }
        }

        void GuiContext::textBoxProcessInputCaretToPrev(ControlData& controlData)
        {
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            caretAt = mint::max(caretAt - 1, 0);
        }

        void GuiContext::textBoxProcessInputCaretToNext(ControlData& controlData, const std::wstring& text)
        {
            const uint16 textLength = static_cast<uint16>(text.length());
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            caretAt = mint::min(caretAt + 1, static_cast<int32>(textLength));
        }

        void GuiContext::textBoxProcessInputCaretToHead(ControlData& controlData)
        {
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            caretAt = 0;

            float& textDisplayOffset = controlData._controlValue._textBoxData._textDisplayOffset;
            textDisplayOffset = 0.0f;
        }

        void GuiContext::textBoxProcessInputCaretToTail(ControlData& controlData, const std::wstring& text)
        {
            const uint16 textLength = static_cast<uint16>(text.length());
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            caretAt = textLength;

            float& textDisplayOffset = controlData._controlValue._textBoxData._textDisplayOffset;
            const float textWidth = calculateTextWidth(text.c_str(), textLength);
            textDisplayOffset = mint::max(0.0f, textWidth - controlData._displaySize._x);
        }

        void GuiContext::textBoxRefreshCaret(const uint64 currentTimeMs, uint16& caretState, uint64& lastCaretBlinkTimeMs) noexcept
        {
            lastCaretBlinkTimeMs = currentTimeMs;
            caretState = 0;
        }

        void GuiContext::textBoxEraseSelection(ControlData& controlData, std::wstring& outText) noexcept
        {
            const uint16 selectionLength = controlData._controlValue._textBoxData._selectionLength;
            if (selectionLength == 0)
            {
                return;
            }

            const uint16 selectionStart = controlData._controlValue._textBoxData._selectionStart;
            outText.erase(selectionStart, selectionLength);

            const uint16 textLength = static_cast<uint16>(outText.length());
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            caretAt = mint::min(static_cast<uint16>(caretAt - selectionLength), textLength);

            controlData._controlValue._textBoxData._selectionLength = 0;
        }

        const bool GuiContext::textBoxInsertWchar(const wchar_t input, uint16& caretAt, std::wstring& outText)
        {
            if (outText.length() < kTextBoxMaxTextLength)
            {
                caretAt = mint::min(caretAt, static_cast<uint16>(outText.length()));

                outText.insert(outText.begin() + caretAt, input);

                ++caretAt;

                return true;
            }
            return false;
        }

        const bool GuiContext::textBoxInsertWstring(const std::wstring& input, uint16& caretAt, std::wstring& outText)
        {
            bool result = false;
            const uint32 oldLength = static_cast<uint32>(outText.length());
            if (oldLength < kTextBoxMaxTextLength)
            {
                uint32 deltaLength = static_cast<uint32>(input.length());
                if (kTextBoxMaxTextLength < oldLength + input.length())
                {
                    deltaLength = kTextBoxMaxTextLength - oldLength;
                }
                else
                {
                    result = true;
                }

                outText.insert(caretAt, input.substr(0, deltaLength));

                caretAt += static_cast<uint16>(deltaLength);
            }
            return result;
        }

        void GuiContext::textBoxUpdateSelection(const uint16 oldCaretAt, const uint16 caretAt, ControlData& controlData)
        {
            uint16& selectionStart = controlData._controlValue._textBoxData._selectionStart;
            uint16& selectionLength = controlData._controlValue._textBoxData._selectionLength;

            if (selectionLength == 0)
            {
                // 새 Selection
                selectionStart = mint::min(caretAt, oldCaretAt);
                selectionLength = mint::max(caretAt, oldCaretAt) - selectionStart;
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
                        selectionStart = (isLeftWard == true) ? mint::min(selectionStart, caretAt) : mint::max(selectionStart, caretAt);
                        selectionLength = selectionEnd - selectionStart;
                    }
                    else
                    {
                        // from Tail
                        const uint16 selectionEnd = (isLeftWard == true) ? mint::min(oldSelectionEnd, caretAt) : mint::max(oldSelectionEnd, caretAt);
                        selectionStart = mint::min(selectionStart, caretAt);
                        selectionLength = selectionEnd - selectionStart;
                    }
                }
            }
        }

        const bool GuiContext::textBoxIsValidInput(const wchar_t input, const uint16 caretAt, const TextInputMode textInputMode, const std::wstring& text) noexcept
        {
            bool result = false;
            if (textInputMode == TextInputMode::General)
            {
                result = true;
            }
            else if (textInputMode == TextInputMode::NumberOnly)
            {
                const wchar_t kPointSign = L'.';
                const wchar_t kMinusSign = L'-';
                const wchar_t kZero = L'0';
                const wchar_t kNine = L'9';
                result = (kZero <= input && input <= kNine);
                if (kPointSign == input)
                {
                    if (text.find(input) == std::wstring::npos)
                    {
                        result = true;
                    }
                }
                else if (kMinusSign == input)
                {
                    if (text.find(input) == std::wstring::npos && caretAt == 0)
                    {
                        result = true;
                    }
                }
            }

            return result;
        }

        void GuiContext::inputBoxUpdateTextDisplayOffset(const uint16 textLength, const float textWidthTillCaret, const float inputCandidateWidth, ControlData& controlData) noexcept
        {
            const uint16 caretAt = controlData._controlValue._textBoxData._caretAt;
            float& textDisplayOffset = controlData._controlValue._textBoxData._textDisplayOffset;
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

                    textDisplayOffset = mint::max(textDisplayOffset, 0.0f);
                }

                if (textWidthTillCaret + inputCandidateWidth < controlData._displaySize._x)
                {
                    textDisplayOffset = 0.0f;
                }
            }
        }

        void GuiContext::textBoxDrawTextWithInputCandidate(const CommonControlParam& commonControlParam, const mint::Float4& textRenderOffset, ControlData& controlData, std::wstring& outText) noexcept
        {
            MINT_ASSERT("김장원", controlData.isTypeOf(ControlType::TextBox) == true, "TextBox 가 아니면 사용하면 안 됩니다!");

            const mint::Float2& controlLeftCenterPosition = ControlCommonHelpers::getControlLeftCenterPosition(controlData);
            const float textDisplayOffset = controlData._controlValue._textBoxData._textDisplayOffset;
            const mint::Float4 textRenderPosition = mint::Float4(controlLeftCenterPosition._x - textDisplayOffset, controlLeftCenterPosition._y, 0, 0);
            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);

            // Text 렌더링 (Caret 이전)
            const uint16 caretAt = controlData._controlValue._textBoxData._caretAt;
            if (outText.empty() == false)
            {
                rendererContext.setTextColor(commonControlParam._fontColor);
                rendererContext.drawDynamicText(outText.c_str(), caretAt, textRenderPosition + textRenderOffset,
                    mint::RenderingBase::FontRenderingOption(mint::RenderingBase::TextRenderDirectionHorz::Rightward, mint::RenderingBase::TextRenderDirectionVert::Centered));
            }

            // Input Candidate 렌더링
            const wchar_t inputCandidate[2]{ _wcharInputCandiate, L'\0' };
            const uint16 textLength = static_cast<uint16>(outText.length());
            const float textWidthTillCaret = calculateTextWidth(outText.c_str(), mint::min(caretAt, textLength));
            rendererContext.setTextColor(commonControlParam._fontColor);
            rendererContext.drawDynamicText(inputCandidate, mint::Float4(controlLeftCenterPosition._x + textWidthTillCaret - textDisplayOffset, controlLeftCenterPosition._y, 0, 0) + textRenderOffset,
                mint::RenderingBase::FontRenderingOption(mint::RenderingBase::TextRenderDirectionHorz::Rightward, mint::RenderingBase::TextRenderDirectionVert::Centered));

            // Text 렌더링 (Caret 이후)
            const bool isFocused = _controlInteractionStates.isControlFocused(controlData);
            const float inputCandidateWidth = ((isFocused == true) && (32 <= _wcharInputCandiate)) ? calculateTextWidth(inputCandidate, 1) : 0.0f;
            if (outText.empty() == false)
            {
                rendererContext.setTextColor(commonControlParam._fontColor);
                rendererContext.drawDynamicText(outText.c_str() + caretAt, textLength - caretAt, textRenderPosition + mint::Float4(textWidthTillCaret + inputCandidateWidth, 0, 0, 0) + textRenderOffset,
                    mint::RenderingBase::FontRenderingOption(mint::RenderingBase::TextRenderDirectionHorz::Rightward, mint::RenderingBase::TextRenderDirectionVert::Centered));
            }

            // Caret 렌더링 (Input Candidate 의 바로 뒤에!)
            const bool needToRenderCaret = (isFocused == true && controlData._controlValue._textBoxData._caretState == 0);
            if (needToRenderCaret == true)
            {
                const float caretHeight = _fontSize;
                const mint::Float2& p0 = mint::Float2(controlLeftCenterPosition._x + textWidthTillCaret + inputCandidateWidth - textDisplayOffset + textRenderOffset._x, controlLeftCenterPosition._y - caretHeight * 0.5f);
                rendererContext.setColor(mint::RenderingBase::Color::kBlack);
                rendererContext.drawLine(p0, p0 + mint::Float2(0.0f, caretHeight), 2.0f);
            }
        }

        void GuiContext::inputBoxDrawTextWithoutInputCandidate(const CommonControlParam& commonControlParam, const mint::Float4& textRenderOffset, const bool renderCaret, ControlData& controlData, std::wstring& outText) noexcept
        {
            MINT_ASSERT("김장원", controlData.isInputBoxType() == true, "호환되지 않는 컨트롤 타입입니다!");

            const mint::Float2& controlLeftCenterPosition = ControlCommonHelpers::getControlLeftCenterPosition(controlData);
            const float textDisplayOffset = controlData._controlValue._textBoxData._textDisplayOffset;
            const mint::Float4 textRenderPosition = mint::Float4(controlLeftCenterPosition._x - textDisplayOffset, controlLeftCenterPosition._y, 0, 0);
            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);

            // Text 전체 렌더링
            if (outText.empty() == false)
            {
                rendererContext.setTextColor(commonControlParam._fontColor);
                rendererContext.drawDynamicText(outText.c_str(), textRenderPosition + textRenderOffset,
                    mint::RenderingBase::FontRenderingOption(mint::RenderingBase::TextRenderDirectionHorz::Rightward, mint::RenderingBase::TextRenderDirectionVert::Centered));
            }

            // Caret 렌더링
            const bool isFocused = _controlInteractionStates.isControlFocused(controlData);
            const bool needToRenderCaret = (isFocused == true && controlData._controlValue._textBoxData._caretState == 0);
            if (renderCaret == true && needToRenderCaret == true)
            {
                const uint16 caretAt = controlData._controlValue._textBoxData._caretAt;
                const uint16 textLength = static_cast<uint16>(outText.length());
                const float textWidthTillCaret = calculateTextWidth(outText.c_str(), mint::min(caretAt, textLength));
                const float caretHeight = _fontSize;
                const mint::Float2& p0 = mint::Float2(controlLeftCenterPosition._x + textWidthTillCaret - textDisplayOffset + textRenderOffset._x, controlLeftCenterPosition._y - caretHeight * 0.5f);
                rendererContext.setColor(mint::RenderingBase::Color::kBlack);
                rendererContext.drawLine(p0, p0 + mint::Float2(0.0f, caretHeight), 2.0f);
            }
        }

        void GuiContext::inputBoxDrawSelection(const mint::Float4& textRenderOffset, ControlData& textBoxControlData, std::wstring& outText) noexcept
        {
            MINT_ASSERT("김장원", textBoxControlData.isInputBoxType() == true, "호환되지 않는 컨트롤 타입입니다!");

            const bool isFocused = _controlInteractionStates.isControlFocused(textBoxControlData);
            if (isFocused == false)
            {
                return;
            }

            const uint16 selectionStart = textBoxControlData._controlValue._textBoxData._selectionStart;
            const uint16 selectionLength = textBoxControlData._controlValue._textBoxData._selectionLength;
            const uint16 selectionEnd = selectionStart + selectionLength;
            if (0 < selectionLength)
            {
                const mint::Float2& controlLeftCenterPosition = ControlCommonHelpers::getControlLeftCenterPosition(textBoxControlData);
                const float textDisplayOffset = textBoxControlData._controlValue._textBoxData._textDisplayOffset;
                const float textWidthTillSelectionStart = calculateTextWidth(outText.c_str(), selectionStart);
                const float textWidthTillSelectionEnd = calculateTextWidth(outText.c_str(), selectionEnd);
                const float textWidthSelection = textWidthTillSelectionEnd - textWidthTillSelectionStart;
                
                mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(textBoxControlData);
                const mint::Float4 selectionRenderPosition = mint::Float4(controlLeftCenterPosition._x - textDisplayOffset + textWidthTillSelectionStart + textWidthSelection * 0.5f, controlLeftCenterPosition._y, 0, 0);
                rendererContext.setPosition(selectionRenderPosition + textRenderOffset);
                rendererContext.setColor(getNamedColor(NamedColor::HighlightColor).addedRgb(-0.375f).scaledA(0.25f));
                rendererContext.drawRectangle(mint::Float2(textWidthSelection, _fontSize * 1.25f), 0.0f, 0.0f);
            }
        }

        const bool GuiContext::beginValueSliderFloat(const wchar_t* const name, const CommonControlParam& commonControlParam, const float roundnessInPixel, const int32 decimalDigits, float& value)
        {
            static constexpr ControlType controlType = ControlType::ValueSliderFloat;

            ControlData& controlData = createOrGetControlData(name, controlType);
            controlData._isFocusable = true;
            controlData._needDoubleClickToFocus = true;
            prepareControlData(controlData, PrepareControlDataUtils::prepareInputBox(commonControlParam, _fontSize));

            mint::RenderingBase::Color finalBackgroundColor;
            const bool wasFocused = _controlInteractionStates.isControlFocused(controlData);
            const bool isFocused = processFocusControl(controlData, commonControlParam._backgroundColor, commonControlParam._backgroundColor.addedRgb(-0.125f), finalBackgroundColor);
            {
                const ControlData& parentControlData = getControlData(controlData.getParentHashKey());
                mint::Rect clipRectForMe = controlData.getControlRect();
                clipRectForMe.clipBy(parentControlData.getClipRectForChildren());
                setClipRectForMe(controlData, clipRectForMe);
            }

            constexpr uint32 kTextBufferSize = 255;
            wchar_t format[kTextBufferSize];
            wchar_t buffer[kTextBufferSize];
            mint::formatString(format, L"%%.%df", decimalDigits);
            mint::formatString(buffer, format, value);
            std::wstring text(buffer);
            const uint16 textLength = static_cast<uint16>(text.length());
            mint::Float4 textRenderOffset;
            if (controlData._controlValue._textBoxData._textDisplayOffset == 0)
            {
                const float fullTextWidth = calculateTextWidth(text.c_str(), textLength);
                
                // 가운데 정렬!
                textRenderOffset._x = (controlData._displaySize._x - fullTextWidth) * 0.5f;
            }

            // Input 처리
            valueSliderFloatProcessInput(wasFocused, controlData, textRenderOffset, value, text);

            if (wasFocused == false && isFocused == true)
            {
                inputBoxProcessInputKeySelectAll(controlData, text);
            }

            // Caret 의 렌더링 위치가 TextBox 를 벗어나는 경우 처리!!
            const float textWidthTillCaret = calculateTextWidth(text.c_str(), mint::min(controlData._controlValue._textBoxData._caretAt, textLength));
            inputBoxUpdateTextDisplayOffset(textLength, textWidthTillCaret, 0.0f, controlData);

            // Box 렌더링
            const mint::Float4& controlCenterPosition = getControlCenterPosition(controlData);
            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            rendererContext.setClipRect(controlData.getClipRect());
            rendererContext.setColor(finalBackgroundColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(controlData._displaySize, (roundnessInPixel / controlData._displaySize.minElement()), 0.0f, 0.0f);

            // Text 렌더링
            inputBoxDrawTextWithoutInputCandidate(commonControlParam, textRenderOffset, isFocused, controlData, text);

            // Selection 렌더링
            inputBoxDrawSelection(textRenderOffset, controlData, text);

            return false;
        }

        void GuiContext::valueSliderFloatProcessInput(const bool wasControlFocused, ControlData& controlData, mint::Float4& textRenderOffset, float& value, std::wstring& outText) noexcept
        {
            const TextInputMode textInputMode = TextInputMode::NumberOnly;

            TextBoxProcessInputResult result;
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            uint16& caretState = controlData._controlValue._textBoxData._caretState;
            uint64& lastCaretBlinkTimeMs = controlData._controlValue._textBoxData._lastCaretBlinkTimeMs;
            const uint64 currentTimeMs = mint::Profiler::getCurrentTimeMs();
            if (lastCaretBlinkTimeMs + _caretBlinkIntervalMs < currentTimeMs)
            {
                lastCaretBlinkTimeMs = currentTimeMs;

                caretState ^= 1;
            }

            if (_controlInteractionStates.isControlFocused(controlData) == true)
            {
                if (_mouseStates.isButtonDownThisFrame() == true)
                {
                    // 마우스 입력 처리
                    textBoxProcessInputMouse(controlData, textRenderOffset, outText, result);
                }
                else
                {
                    const mint::Window::IWindow* const window = _graphicDevice->getWindow();
                    const bool isShiftKeyDown = window->isKeyDown(mint::Window::EventData::KeyCode::Shift);
                    const bool isControlKeyDown = window->isKeyDown(mint::Window::EventData::KeyCode::Control);
                    const uint16 oldCaretAt = controlData._controlValue._textBoxData._caretAt;
                    if (32 <= _wcharInputCandiate)
                    {
                        textBoxEraseSelection(controlData, outText);
                    }
                    else if (_wcharInput != L'\0')
                    {
                        // 글자 입력 or 키 입력

                        const bool isInputCharacter = (32 <= _wcharInput);
                        if (isInputCharacter == true)
                        {
                            if (textBoxIsValidInput(_wcharInput, caretAt, textInputMode, outText) == true)
                            {
                                textBoxEraseSelection(controlData, outText);

                                textBoxInsertWchar(_wcharInput, caretAt, outText);
                            }
                        }
                        else
                        {
                            const uint16 selectionLength = controlData._controlValue._textBoxData._selectionLength;

                            if (_wcharInput == VK_BACK) // BackSpace
                            {
                                textBoxProcessInputKeyDeleteBefore(controlData, outText);
                            }
                            else if (isControlKeyDown == true && _wcharInput == 0x01) // Ctrl + A
                            {
                                inputBoxProcessInputKeySelectAll(controlData, outText);
                            }
                            else if (isControlKeyDown == true && _wcharInput == 0x03) // Ctrl + C
                            {
                                textBoxProcessInputKeyCopy(controlData, outText);
                            }
                            else if (isControlKeyDown == true && _wcharInput == 0x18) // Ctrl + X
                            {
                                textBoxProcessInputKeyCut(controlData, outText);
                            }
                            else if (isControlKeyDown == true && _wcharInput == 0x16) // Ctrl + V
                            {
                                textBoxProcessInputKeyPaste(controlData, outText);
                            }
                        }

                        result._clearWcharInput = true;
                    }
                    else
                    {
                        // 키 눌림 처리

                        if (_keyCode == mint::Window::EventData::KeyCode::Left)
                        {
                            textBoxProcessInputCaretToPrev(controlData);
                        }
                        else if (_keyCode == mint::Window::EventData::KeyCode::Right)
                        {
                            textBoxProcessInputCaretToNext(controlData, outText);
                        }
                        else if (_keyCode == mint::Window::EventData::KeyCode::Home)
                        {
                            textBoxProcessInputCaretToHead(controlData);
                        }
                        else if (_keyCode == mint::Window::EventData::KeyCode::End)
                        {
                            textBoxProcessInputCaretToTail(controlData, outText);
                        }
                        else if (_keyCode == mint::Window::EventData::KeyCode::Delete)
                        {
                            textBoxProcessInputKeyDeleteAfter(controlData, outText);
                        }
                    }

                    // Caret 위치가 바뀐 경우 refresh
                    if (oldCaretAt != caretAt)
                    {
                        textBoxRefreshCaret(currentTimeMs, caretState, lastCaretBlinkTimeMs);

                        // Selection
                        if (isShiftKeyDown == true && _keyCode != mint::Window::EventData::KeyCode::NONE)
                        {
                            textBoxUpdateSelection(oldCaretAt, caretAt, controlData);
                        }
                    }

                    if (isShiftKeyDown == false && isControlKeyDown == false &&
                        _keyCode != mint::Window::EventData::KeyCode::NONE &&
                        _keyCode != mint::Window::EventData::KeyCode::Control &&
                        _keyCode != mint::Window::EventData::KeyCode::Alt)
                    {
                        // Selection 해제
                        uint16& selectionLength = controlData._controlValue._textBoxData._selectionLength;
                        selectionLength = 0;
                    }
                }

                if (wasControlFocused == false)
                {
                    textBoxRefreshCaret(currentTimeMs, caretState, lastCaretBlinkTimeMs);
                }

                if (result._clearKeyCode == true)
                {
                    _keyCode = mint::Window::EventData::KeyCode::NONE;
                }
                if (result._clearWcharInput == true)
                {
                    _wcharInput = L'\0';
                }

                if (outText.empty() == true)
                {
                    value = 0.0f;
                }
                else
                {
                    value = std::stof(outText);
                }
            }
            else
            {
                if (_mouseStates.isButtonDown() == true && ControlCommonHelpers::isInControlInteractionArea(_mouseStates.getButtonDownPosition(), controlData) == true)
                {
                    const mint::Float2 dragDelta = _mouseStates.getMouseDragDelta();
                    value += (dragDelta._x - dragDelta._y) * 0.1f;

                    _mouseStates.setButtonDownPositionCopy(_mouseStates.getPosition());
                }
            }
        }
        
        const bool GuiContext::beginListView(const wchar_t* const name, int16& outSelectedListItemIndex, const ListViewParam& listViewParam)
        {
            static constexpr ControlType controlType = ControlType::ListView;
            
            ControlData& controlData = createOrGetControlData(name, controlType);
            controlData._isFocusable = false;

            PrepareControlDataParam prepareControlDataParam;
            {
                prepareControlDataParam._initialDisplaySize._x = 160.0f;
                prepareControlDataParam._initialDisplaySize._y = 100.0f;
            }
            prepareControlData(controlData, prepareControlDataParam);

            if (controlData.getPreviousChildControlCount() == 0)
            {
                controlData._controlValue._itemData.deselect();
            }
            outSelectedListItemIndex = controlData._controlValue._itemData.getSelectedItemIndex();

            mint::RenderingBase::Color finalBackgroundColor = getNamedColor(NamedColor::LightFont);
            processShowOnlyControl(controlData, finalBackgroundColor, false);

            {
                const ControlData& parentControlData = getControlData(controlData.getParentHashKey());
                mint::Rect clipRectForMe = controlData.getControlRect();
                clipRectForMe.clipBy(parentControlData.getClipRectForChildren());
                setClipRectForMe(controlData, clipRectForMe);
            }
            {
                mint::Rect clipRectForChildren = controlData.getControlRect();
                const float halfRoundnessInPixel = kDefaultRoundnessInPixel * 0.5f;
                const float quarterRoundnessInPixel = halfRoundnessInPixel * 0.5f;
                clipRectForChildren.left(clipRectForChildren.left() + static_cast<LONG>(quarterRoundnessInPixel));
                clipRectForChildren.right(clipRectForChildren.right() - static_cast<LONG>(halfRoundnessInPixel));

                const ControlData& parentControlData = getControlData(controlData.getParentHashKey());
                clipRectForChildren.clipBy(parentControlData.getClipRectForChildren());
                setClipRectForChildren(controlData, clipRectForChildren);
            }

            const mint::Float4& controlCenterPosition = getControlCenterPosition(controlData);
            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            rendererContext.setClipRect(controlData.getClipRect());
            rendererContext.setColor(finalBackgroundColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(controlData._displaySize, (kDefaultRoundnessInPixel / controlData._displaySize.minElement()), 0.0f, 0.0f);
            
            if (listViewParam._useScrollBar == true)
            {
                controlData._controlValue._commonData.enableScrollBar(mint::Gui::ScrollBarType::Vert);
            }
            else
            {
                controlData._controlValue._commonData.disableScrollBar(mint::Gui::ScrollBarType::Vert);
            }
            _controlStackPerFrame.push_back(ControlStackData(controlData));
            return true;
        }

        void GuiContext::endListView()
        {
            ControlData& controlData = getControlStackTopXXX();
            const bool hasScrollBarVert = controlData._controlValue._commonData.isScrollBarEnabled(ScrollBarType::Vert);
            if (hasScrollBarVert == true)
            {
                pushScrollBar(mint::Gui::ScrollBarType::Vert);
            }

            endControlInternal(ControlType::ListView);
        }

        void GuiContext::pushListItem(const wchar_t* const text)
        {
            static constexpr ControlType controlType = ControlType::ListItem;
            
            ControlData& controlData = createOrGetControlData(text, controlType);
            controlData._isFocusable = false;

            ControlData& parentControlData = getControlData(controlData.getParentHashKey());
            PrepareControlDataParam prepareControlDataParam;
            {
                prepareControlDataParam._initialDisplaySize._x = parentControlData._displaySize._x;
                prepareControlDataParam._initialDisplaySize._y = _fontSize + 12.0f;
                prepareControlDataParam._innerPadding.left(prepareControlDataParam._initialDisplaySize._y * 0.25f);
                prepareControlDataParam._noIntervalForNextSibling = true;
                prepareControlDataParam._clipRectUsage = mint::Gui::ClipRectUsage::ParentsChild;
            }
            prepareControlData(controlData, prepareControlDataParam);

            const int16 parentSelectedItemIndex = parentControlData._controlValue._itemData.getSelectedItemIndex();
            const int16 myIndex = static_cast<int16>(parentControlData.getChildControlDataHashKeyArray().size() - 1);
            mint::RenderingBase::Color finalColor;
            const mint::RenderingBase::Color inputColor = (parentSelectedItemIndex == myIndex) ? getNamedColor(NamedColor::HighlightColor) : getNamedColor(NamedColor::LightFont);
            const bool isClicked = processClickControl(controlData, inputColor, inputColor, inputColor, finalColor);
            if (isClicked == true)
            {
                parentControlData._controlValue._itemData.select(myIndex);
            }

            const mint::Float4& controlCenterPosition = getControlCenterPosition(controlData);
            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            rendererContext.setClipRect(controlData.getClipRect());
            rendererContext.setColor(finalColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(controlData._displaySize, (kDefaultRoundnessInPixel / controlData._displaySize.minElement()), 0.0f, 0.0f);

            const mint::Float2& controlLeftCenterPosition = ControlCommonHelpers::getControlLeftCenterPosition(controlData);
            rendererContext.setTextColor(getNamedColor(NamedColor::DarkFont));
            rendererContext.drawDynamicText(text, mint::Float4(controlLeftCenterPosition._x + controlData.getInnerPadding().left(), controlLeftCenterPosition._y, 0, 0),
                mint::RenderingBase::FontRenderingOption(mint::RenderingBase::TextRenderDirectionHorz::Rightward, mint::RenderingBase::TextRenderDirectionVert::Centered));
        }

        const bool GuiContext::beginMenuBar(const wchar_t* const name)
        {
            static constexpr ControlType controlType = ControlType::MenuBar;

            nextNoAutoPositioned();

            ControlData& menuBar = createOrGetControlData(name, controlType);
            ControlData& menuBarParent = getControlData(menuBar.getParentHashKey());
            const bool isMenuBarParentRoot = menuBarParent.isTypeOf(ControlType::ROOT);
            const bool isMenuBarParentWindow = menuBarParent.isTypeOf(ControlType::Window);
            if (isMenuBarParentRoot == false && isMenuBarParentWindow == false)
            {
                MINT_LOG_ERROR("김장원", "MenuBar 는 Window 나 Root 컨트롤의 자식으로만 사용할 수 있습니다!");
                return false;
            }
            menuBarParent._controlValue._commonData._menuBarType = MenuBarType::Top; // TODO...

            PrepareControlDataParam prepareControlDataParam;
            {
                prepareControlDataParam._alwaysResetDisplaySize = true;
                prepareControlDataParam._initialDisplaySize._x = menuBarParent._displaySize._x;
                prepareControlDataParam._initialDisplaySize._y = kMenuBarBaseSize._y;
                prepareControlDataParam._desiredPositionInParent._x = 0.0f;
                prepareControlDataParam._desiredPositionInParent._y = (isMenuBarParentWindow == true) ? kTitleBarBaseSize._y : 0.0f;
                prepareControlDataParam._clipRectUsage = ClipRectUsage::ParentsOwn;
            }
            prepareControlData(menuBar, prepareControlDataParam);

            const bool wasToggled = menuBar._controlValue._booleanData.get();
            const mint::Float2 interactionSize = mint::Float2(menuBar._controlValue._itemData._itemSize._x, menuBar.getInteractionSize()._y);
            if (true == _controlInteractionStates.hasPressedControl() 
                && ControlCommonHelpers::isInControl(_mouseStates.getPosition(), menuBar._position, mint::Float2::kZero, interactionSize) == false)
            {
                menuBar._controlValue._booleanData.set(false);
            }
            menuBar._controlValue._itemData._itemSize._x = 0.0f;

            const bool isToggled = menuBar._controlValue._booleanData.get();
            const uint32 previousChildCount = static_cast<uint32>(menuBar.getPreviousChildControlDataHashKeyArray().size());
            if ((previousChildCount == 0 || isToggled == false) && wasToggled == false)
            {
                // wasToggled 덕분에 다음 프레임에 -1 로 세팅된다. 한 번은 자식 함수들이 쭉 호출된다는 뜻!

                menuBar._controlValue._itemData.deselect();
            }

            mint::RenderingBase::Color color = getNamedColor(NamedColor::NormalState);
            processShowOnlyControl(menuBar, color, false);
            if (isMenuBarParentRoot == true)
            {
                color.a(1.0f);
            }

            const mint::Float4& controlCenterPosition = getControlCenterPosition(menuBar);
            menuBar._rendererContextLayer = getUpperRendererContextLayer(menuBarParent);
            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(menuBar);
            rendererContext.setClipRect(menuBar.getClipRect());
            rendererContext.setColor(color);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(menuBar._displaySize, 0.0f, 0.0f, 0.0f);

            _controlStackPerFrame.push_back(ControlStackData(menuBar));
            return true;
        }

        const bool GuiContext::beginMenuBarItem(const wchar_t* const text)
        {
            static constexpr ControlType controlType = ControlType::MenuBarItem;

            nextNoAutoPositioned();

            ControlData& menuBar = getControlStackTopXXX();
            ControlData& menuBarItem = createOrGetControlData(text, controlType, generateControlKeyString(menuBar, text, controlType));
            if (menuBar.isTypeOf(ControlType::MenuBar) == false)
            {
                MINT_LOG_ERROR("김장원", "MenuBarItem 은 MenuBar 컨트롤의 자식으로만 사용할 수 있습니다!");
                return false;
            }

            PrepareControlDataParam prepareControlDataParam;
            {
                const uint32 textLength = mint::StringUtil::wcslen(text);
                const float textWidth = calculateTextWidth(text, textLength);
                prepareControlDataParam._initialDisplaySize._x = textWidth + kMenuBarItemTextSpace;
                prepareControlDataParam._initialDisplaySize._y = kMenuBarBaseSize._y;
                prepareControlDataParam._desiredPositionInParent._x = menuBar._controlValue._itemData._itemSize._x;
                prepareControlDataParam._desiredPositionInParent._y = 0.0f;
                prepareControlDataParam._clipRectUsage = ClipRectUsage::ParentsOwn;
            }
            prepareControlData(menuBarItem, prepareControlDataParam);
            menuBar._controlValue._itemData._itemSize._x += menuBarItem._displaySize._x;
            menuBarItem._controlValue._itemData._itemSize._y = 0.0f;

            const int16 menuBarSelectedItemIndex = menuBar._controlValue._itemData.getSelectedItemIndex();
            const int16 myIndex = static_cast<int16>(menuBar.getChildControlDataHashKeyArray().size() - 1);
            const bool wasMeSelected = (menuBarSelectedItemIndex == myIndex);
            mint::RenderingBase::Color finalBackgroundColor;
            const mint::RenderingBase::Color& normalColor = (wasMeSelected == true) ? getNamedColor(NamedColor::PressedState) : getNamedColor(NamedColor::NormalState);
            const mint::RenderingBase::Color& hoverColor = (wasMeSelected == true) ? getNamedColor(NamedColor::PressedState) : getNamedColor(NamedColor::HoverState);
            const mint::RenderingBase::Color& pressedColor = (wasMeSelected == true) ? getNamedColor(NamedColor::PressedState) : getNamedColor(NamedColor::PressedState);
            const bool isClicked = processClickControl(menuBarItem, normalColor, hoverColor, pressedColor, finalBackgroundColor);
            const bool isParentAncestorPressed = isAncestorControlPressed(menuBar);
            const bool isDescendantHovered = isDescendantControlHovered(menuBarItem);
            const bool& isParentControlToggled = menuBar._controlValue._booleanData.get();
            const bool wasParentControlToggled = isParentControlToggled;
            if (isClicked == true)
            {    
                menuBar._controlValue._booleanData.set(!isParentControlToggled);
                menuBar._controlValue._itemData.select(myIndex);
            }
            else if (wasMeSelected == true && isParentAncestorPressed == true)
            {
                menuBar._controlValue._booleanData.set(false);
                menuBar._controlValue._itemData.deselect();
            }
            if (_controlInteractionStates.isControlHovered(menuBarItem) == true && isParentControlToggled == true)
            {
                menuBar._controlValue._itemData.select(myIndex);
            }
            
            const mint::Float4& controlCenterPosition = getControlCenterPosition(menuBarItem);
            
            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(menuBarItem);
            rendererContext.setClipRect(menuBarItem.getClipRect());
            rendererContext.setColor(finalBackgroundColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(menuBarItem._displaySize, 0.0f, 0.0f, 0.0f);

            const mint::Float2& controlLeftCenterPosition = ControlCommonHelpers::getControlLeftCenterPosition(menuBarItem);
            rendererContext.setTextColor(getNamedColor(NamedColor::LightFont));
            rendererContext.drawDynamicText(text, mint::Float4(controlLeftCenterPosition._x + menuBarItem.getInnerPadding().left() + menuBarItem._displaySize._x * 0.5f, controlLeftCenterPosition._y, 0, 0),
                mint::RenderingBase::FontRenderingOption(mint::RenderingBase::TextRenderDirectionHorz::Centered, mint::RenderingBase::TextRenderDirectionVert::Centered));

            const bool isMeSelected = (menuBarSelectedItemIndex == myIndex);
            const bool result = (isClicked || isMeSelected || (isParentAncestorPressed && wasMeSelected));
            if (result == true)
            {
                _controlStackPerFrame.push_back(ControlStackData(menuBarItem));
            }
            return result;
        }

        const bool GuiContext::beginMenuItem(const wchar_t* const text)
        {
            static constexpr ControlType controlType = ControlType::MenuItem;

            nextNoAutoPositioned();
            nextControlSizeNonContrainedToParent();

            ControlData& menuItem = createOrGetControlData(text, controlType);
            menuItem._isInteractableOutsideParent = true;

            ControlData& menuItemParent = getControlData(menuItem.getParentHashKey());
            const ControlType parentControlType = menuItemParent.getControlType();
            const bool isParentControlMenuItem = (parentControlType == ControlType::MenuItem);
            if (parentControlType != ControlType::MenuBarItem && isParentControlMenuItem == false)
            {
                MINT_LOG_ERROR("김장원", "MenuItem 은 MenuBarItem 이나 MenuItem 컨트롤의 자식으로만 사용할 수 있습니다!");
                return false;
            }

            PrepareControlDataParam prepareControlDataParam;
            {
                prepareControlDataParam._alwaysResetDisplaySize = true;
                prepareControlDataParam._initialDisplaySize._x = menuItemParent._controlValue._itemData._itemSize._x;
                prepareControlDataParam._initialDisplaySize._y = kMenuBarBaseSize._y;
                prepareControlDataParam._innerPadding.left(kMenuItemSpaceLeft);
                prepareControlDataParam._desiredPositionInParent._x = (isParentControlMenuItem == true) ? menuItemParent._displaySize._x : 0.0f;
                prepareControlDataParam._desiredPositionInParent._y = menuItemParent._controlValue._itemData._itemSize._y + ((isParentControlMenuItem == true) ? 0.0f : prepareControlDataParam._initialDisplaySize._y);
            }
            prepareControlData(menuItem, prepareControlDataParam);

            const uint32 textLength = mint::StringUtil::wcslen(text);
            const float textWidth = calculateTextWidth(text, textLength);
            menuItemParent._controlValue._itemData._itemSize._x = mint::max(menuItemParent._controlValue._itemData._itemSize._x, textWidth + kMenuItemSpaceRight);
            menuItemParent._controlValue._itemData._itemSize._y += menuItem._displaySize._y;
            menuItem._controlValue._itemData._itemSize._y = 0.0f;

            const bool isDescendantHovered = isDescendantControlHoveredInclusive(menuItem);
            mint::RenderingBase::Color finalBackgroundColor;
            {
                const mint::RenderingBase::Color& normalColor = getNamedColor((isDescendantHovered == true) ? NamedColor::HoverState : NamedColor::NormalState);
                const mint::RenderingBase::Color& hoverColor = getNamedColor(NamedColor::HoverState);
                const mint::RenderingBase::Color& pressedColor = getNamedColor(NamedColor::PressedState);
                processClickControl(menuItem, normalColor, hoverColor, pressedColor, finalBackgroundColor);
                finalBackgroundColor.a(1.0f);
            }
            const bool isHovered = _controlInteractionStates.isControlHovered(menuItem);
            const bool isPresssed = _controlInteractionStates.isControlPressed(menuItem);
            const bool& isToggled = menuItem._controlValue._booleanData.get();
            const int16 myIndex = static_cast<int16>(menuItemParent.getChildControlDataHashKeyArray().size() - 1);
            const bool isMeSelected = (menuItemParent._controlValue._itemData.isSelected(myIndex));
            if (isHovered == true)
            {
                menuItemParent._controlValue._itemData.select(myIndex);
            }
            else if (isHovered == false && isDescendantHovered  == false && isToggled == true)
            {
                menuItemParent._controlValue._itemData.deselect();
            }
            menuItem._controlValue._booleanData.set(isMeSelected);

            const mint::Float4& controlCenterPosition = getControlCenterPosition(menuItem);
            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(menuItem);
            rendererContext.setClipRect(menuItem.getClipRect());
            rendererContext.setColor(finalBackgroundColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(menuItem._displaySize, 0.0f, 0.0f, 0.0f);

            const uint16 previousMaxChildCount = menuItem.getPreviousMaxChildControlCount();
            if (0 < previousMaxChildCount)
            {
                const mint::Float2& controlRightCenterPosition = ControlCommonHelpers::getControlRightCenterPosition(menuItem);
                mint::Float2 a = controlRightCenterPosition + mint::Float2(-14, -5);
                mint::Float2 b = controlRightCenterPosition + mint::Float2( -4,  0);
                mint::Float2 c = controlRightCenterPosition + mint::Float2(-14, +5);
                rendererContext.setColor(getNamedColor((isToggled == true) ? NamedColor::HighlightColor : NamedColor::LightFont));
                rendererContext.drawSolidTriangle(a, b, c);
            }

            const mint::Float2& controlLeftCenterPosition = ControlCommonHelpers::getControlLeftCenterPosition(menuItem);
            rendererContext.setTextColor(getNamedColor(NamedColor::LightFont));
            rendererContext.drawDynamicText(text, mint::Float4(controlLeftCenterPosition._x + menuItem.getInnerPadding().left(), controlLeftCenterPosition._y, 0, 0), 
                mint::RenderingBase::FontRenderingOption(mint::RenderingBase::TextRenderDirectionHorz::Rightward, mint::RenderingBase::TextRenderDirectionVert::Centered));

            // (previousMaxChildCount) 최초 업데이트 시 Child 가 다 등록되어야 하므로 controlData._updateCount 를 이용한다.
            const bool result = (isToggled || isPresssed || menuItem._updateCount <= 1);
            if (result == true)
            {
                _controlStackPerFrame.push_back(ControlStackData(menuItem));
            }
            return result;
        }

        void GuiContext::pushScrollBar(const ScrollBarType scrollBarType)
        {
            const bool useVertical = (scrollBarType == ScrollBarType::Vert || scrollBarType == ScrollBarType::Both);
            if (useVertical == true)
            {
                pushScrollBarVert();
            }

            const bool useHorizontal = (scrollBarType == ScrollBarType::Horz || scrollBarType == ScrollBarType::Both);
            if (useHorizontal == true)
            {
                pushScrollBarHorz();
            }
        }

        void GuiContext::pushScrollBarVert() noexcept
        {
            ControlData& parent = getControlStackTopXXX();
            const float parentWindowPureDisplayHeight = parent.getPureDisplayHeight();
            const float titleBarOffsetX = (parent.isTypeOf(mint::Gui::ControlType::Window) == true) ? kHalfBorderThickness * 2.0f : kScrollBarThickness * 0.5f;

            ScrollBarTrackParam scrollBarTrackParam;
            scrollBarTrackParam._common._size._x = kScrollBarThickness;
            scrollBarTrackParam._common._size._y = parentWindowPureDisplayHeight;
            scrollBarTrackParam._positionInParent._x = parent._displaySize._x - titleBarOffsetX;
            scrollBarTrackParam._positionInParent._y = parent.getTopOffsetToClientArea() + parent.getInnerPadding().top();
            
            bool hasExtraSize = false;
            const bool isParentAncestorFocusedInclusive = isAncestorControlFocusedInclusiveXXX(parent);
            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(parent);
            ControlData& scrollBarTrack = pushScrollBarTrack(ScrollBarType::Vert, scrollBarTrackParam, rendererContext, hasExtraSize);
            if (hasExtraSize == true)
            {
                parent._controlValue._commonData.enableScrollBar(ScrollBarType::Vert);

                pushScrollBarThumb(ScrollBarType::Vert, parentWindowPureDisplayHeight, parent.getPreviousContentAreaSize()._y, scrollBarTrack, rendererContext);
            }
            else
            {
                parent._controlValue._commonData.disableScrollBar(ScrollBarType::Vert);

                parent._childDisplayOffset._y = 0.0f; // Scrolling!
            }
        }

        void GuiContext::pushScrollBarHorz() noexcept
        {
            ControlData& parent = getControlStackTopXXX();
            const float parentWindowPureDisplayWidth = parent.getPureDisplayWidth();
            const mint::Float2& menuBarThicknes = parent.getMenuBarThickness();

            ScrollBarTrackParam scrollBarTrackParam;
            scrollBarTrackParam._common._size._x = parentWindowPureDisplayWidth;
            scrollBarTrackParam._common._size._y = kScrollBarThickness;
            scrollBarTrackParam._positionInParent._x = parent.getInnerPadding().left() + menuBarThicknes._x;
            scrollBarTrackParam._positionInParent._y = parent._displaySize._y - kHalfBorderThickness * 2.0f;

            bool hasExtraSize = false;
            const bool isParentAncestorFocusedInclusive = isAncestorControlFocusedInclusiveXXX(parent);
            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(parent);
            ControlData& scrollBarTrack = pushScrollBarTrack(ScrollBarType::Horz, scrollBarTrackParam, rendererContext, hasExtraSize);
            if (hasExtraSize == true)
            {
                parent._controlValue._commonData.enableScrollBar(ScrollBarType::Horz);

                pushScrollBarThumb(ScrollBarType::Horz, parentWindowPureDisplayWidth, parent.getPreviousContentAreaSize()._x, scrollBarTrack, rendererContext);
            }
            else
            {
                parent._controlValue._commonData.disableScrollBar(ScrollBarType::Horz);

                parent._childDisplayOffset._x = 0.0f; // Scrolling!
            }
        }

        ControlData& GuiContext::pushScrollBarTrack(const ScrollBarType scrollBarType, const ScrollBarTrackParam& scrollBarTrackParam, mint::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext, bool& outHasExtraSize)
        {
            static constexpr ControlType trackControlType = ControlType::ScrollBar;
            MINT_ASSERT("김장원", (scrollBarType != ScrollBarType::Both) && (scrollBarType != ScrollBarType::None), "잘못된 scrollBarType 입력값입니다.");

            outHasExtraSize = false;
            nextNoAutoPositioned();

            const bool isVert = (scrollBarType == ScrollBarType::Vert);
            ControlData& parentControlData = getControlStackTopXXX();
            ControlData& trackControlData = createOrGetControlData(generateControlKeyString((isVert == true) ? L"ScrollBarVertTrack" : L"ScrollBarHorzTrack", trackControlType), trackControlType);

            PrepareControlDataParam prepareControlDataParamForTrack;
            {
                prepareControlDataParamForTrack._initialDisplaySize = scrollBarTrackParam._common._size;
                prepareControlDataParamForTrack._desiredPositionInParent = scrollBarTrackParam._positionInParent;
                if (isVert == true)
                {
                    prepareControlDataParamForTrack._desiredPositionInParent._x -= parentControlData.getDockSizeIfHosting(DockingMethod::RightSide)._x;
                    prepareControlDataParamForTrack._desiredPositionInParent._y += parentControlData.getDockSizeIfHosting(DockingMethod::TopSide)._y;
                }
                else
                {
                    prepareControlDataParamForTrack._desiredPositionInParent._x += parentControlData.getDockSizeIfHosting(DockingMethod::LeftSide)._x;
                    prepareControlDataParamForTrack._desiredPositionInParent._y -= parentControlData.getDockSizeIfHosting(DockingMethod::BottomSide)._y;
                }
                prepareControlDataParamForTrack._parentHashKeyOverride = parentControlData.getHashKey();
                prepareControlDataParamForTrack._alwaysResetDisplaySize = true;
                prepareControlDataParamForTrack._alwaysResetPosition = true;
                prepareControlDataParamForTrack._ignoreMeForContentAreaSize = true;
                prepareControlDataParamForTrack._clipRectUsage = ClipRectUsage::ParentsOwn;
            }
            prepareControlData(trackControlData, prepareControlDataParamForTrack);

            mint::RenderingBase::Color trackColor = getNamedColor(NamedColor::ScrollBarTrack);
            processShowOnlyControl(trackControlData, trackColor, false);

            // Vertical Track
            if (isVert == true)
            {
                const float parentWindowPureDisplayHeight = parentControlData.getPureDisplayHeight();
                const float extraSize = parentControlData.getPreviousContentAreaSize()._y - parentWindowPureDisplayHeight;
                if (0.0f <= extraSize)
                {
                    // Rendering track
                    const float radius = kScrollBarThickness * 0.5f;
                    {
                        const float rectLength = trackControlData._displaySize._y - radius * 2.0f;
                        shapeFontRendererContext.setClipRect(trackControlData.getClipRect());
                        shapeFontRendererContext.setColor(trackColor);

                        mint::Float4 trackRenderPosition = mint::Float4(trackControlData._position._x, trackControlData._position._y + radius, 0.0f, 1.0f);

                        // Upper half circle
                        shapeFontRendererContext.setPosition(trackRenderPosition);
                        shapeFontRendererContext.drawHalfCircle(radius, 0.0f);

                        // Rect
                        if (0.0f < rectLength)
                        {
                            trackRenderPosition._y += rectLength * 0.5f;
                            shapeFontRendererContext.setPosition(trackRenderPosition);
                            shapeFontRendererContext.drawRectangle(trackControlData._displaySize - mint::Float2(0.0f, radius * 2.0f), 0.0f, 0.0f);
                        }

                        // Lower half circle
                        if (0.0f < rectLength)
                        {
                            trackRenderPosition._y += rectLength * 0.5f;
                        }
                        shapeFontRendererContext.setPosition(trackRenderPosition);
                        shapeFontRendererContext.drawHalfCircle(radius, mint::Math::kPi);
                    }

                    outHasExtraSize = true;
                }
            }
            else
            {
                const float parentWindowPureDisplayWidth = parentControlData.getPureDisplayWidth();
                const float extraSize = parentControlData.getPreviousContentAreaSize()._x - parentWindowPureDisplayWidth;
                if (0.0f <= extraSize)
                {
                    // Rendering track
                    const float radius = kScrollBarThickness * 0.5f;
                    {
                        const float rectLength = trackControlData._displaySize._x - radius * 2.0f;
                        shapeFontRendererContext.setClipRect(trackControlData.getClipRect());
                        shapeFontRendererContext.setColor(trackColor);

                        mint::Float4 trackRenderPosition = mint::Float4(trackControlData._position._x + radius, trackControlData._position._y, 0.0f, 1.0f);

                        // Left half circle
                        shapeFontRendererContext.setPosition(trackRenderPosition);
                        shapeFontRendererContext.drawHalfCircle(radius, +mint::Math::kPiOverTwo);

                        // Rect
                        if (0.0f < rectLength)
                        {
                            trackRenderPosition._x += rectLength * 0.5f;
                            shapeFontRendererContext.setPosition(trackRenderPosition);
                            shapeFontRendererContext.drawRectangle(trackControlData._displaySize - mint::Float2(radius * 2.0f, 0.0f), 0.0f, 0.0f);
                        }

                        // Right half circle
                        if (0.0f < rectLength)
                        {
                            trackRenderPosition._x += rectLength * 0.5f;
                        }
                        shapeFontRendererContext.setPosition(trackRenderPosition);
                        shapeFontRendererContext.drawHalfCircle(radius, -mint::Math::kPiOverTwo);
                    }

                    outHasExtraSize = true;
                }
            }

            return trackControlData;
        }

        void GuiContext::pushScrollBarThumb(const ScrollBarType scrollBarType, const float visibleLength, const float totalLength, const ControlData& scrollBarTrack, mint::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext)
        {
            static constexpr ControlType thumbControlType = ControlType::ScrollBarThumb;
            
            nextNoAutoPositioned();

            const float radius = kScrollBarThickness * 0.5f;
            const float thumbSizeRatio = (visibleLength / totalLength);
            const float thumbSize = visibleLength * thumbSizeRatio - radius * 2.0f;
            const float trackRemnantSize = std::abs(visibleLength - thumbSize);
            ControlData& scrollBarParent = getControlData(scrollBarTrack.getParentHashKey());

            if (scrollBarType == ScrollBarType::Vert)
            {
                ControlData& thumbControlData = createOrGetControlData(generateControlKeyString(scrollBarParent, L"ScrollBarVertThumb", thumbControlType), thumbControlType);
                PrepareControlDataParam prepareControlDataParamForThumb;
                {
                    prepareControlDataParamForThumb._alwaysResetDisplaySize = true;
                    prepareControlDataParamForThumb._initialDisplaySize._x = kScrollBarThickness;
                    prepareControlDataParamForThumb._initialDisplaySize._y = thumbSize;

                    prepareControlDataParamForThumb._desiredPositionInParent = getControlPositionInParentSpace(scrollBarTrack);
                    prepareControlDataParamForThumb._desiredPositionInParent._x -= kScrollBarThickness * 0.5f;

                    prepareControlDataParamForThumb._parentHashKeyOverride = scrollBarParent.getHashKey();
                    prepareControlDataParamForThumb._ignoreMeForContentAreaSize = true;
                    prepareControlDataParamForThumb._clipRectUsage = ClipRectUsage::ParentsOwn;

                    thumbControlData._isDraggable = true;
                    thumbControlData._draggingConstraints.left(scrollBarTrack._position._x - kScrollBarThickness * 0.5f);
                    thumbControlData._draggingConstraints.right(thumbControlData._draggingConstraints.left());
                    thumbControlData._draggingConstraints.top(scrollBarTrack._position._y);
                    thumbControlData._draggingConstraints.bottom(thumbControlData._draggingConstraints.top() + trackRemnantSize);
                }
                prepareControlData(thumbControlData, prepareControlDataParamForThumb);

                // @중요
                // Calculate position from internal value
                thumbControlData._position._y = scrollBarTrack._position._y + (thumbControlData._controlValue._thumbData._thumbAt * trackRemnantSize);

                mint::RenderingBase::Color thumbColor;
                processScrollableControl(thumbControlData, getNamedColor(NamedColor::ScrollBarThumb), getNamedColor(NamedColor::ScrollBarThumb).scaledRgb(1.25f), thumbColor);

                const float mouseWheelScroll = getMouseWheelScroll(scrollBarParent);
                const float thumbAtRatio = (trackRemnantSize < 1.0f) ? 0.0f : mint::Math::saturate((thumbControlData._position._y - thumbControlData._draggingConstraints.top() + mouseWheelScroll) / trackRemnantSize);
                thumbControlData._controlValue._thumbData._thumbAt = thumbAtRatio;
                scrollBarParent._childDisplayOffset._y = -thumbAtRatio * (totalLength - visibleLength); // Scrolling!

                // Rendering thumb
                {
                    const float rectLength = thumbSize - radius * 2.0f;
                    shapeFontRendererContext.setClipRect(thumbControlData.getClipRect());
                    shapeFontRendererContext.setColor(thumbColor);

                    mint::Float4 thumbRenderPosition = mint::Float4(thumbControlData._position._x + radius, thumbControlData._position._y + radius, 0.0f, 1.0f);

                    // Upper half circle
                    shapeFontRendererContext.setPosition(thumbRenderPosition);
                    shapeFontRendererContext.drawHalfCircle(radius, 0.0f);

                    // Rect
                    if (0.0f < rectLength)
                    {
                        thumbRenderPosition._y += rectLength * 0.5f;
                        shapeFontRendererContext.setPosition(thumbRenderPosition);
                        shapeFontRendererContext.drawRectangle(thumbControlData._displaySize - mint::Float2(0.0f, radius * 2.0f), 0.0f, 0.0f);
                    }

                    // Lower half circle
                    if (0.0f < rectLength)
                    {
                        thumbRenderPosition._y += rectLength * 0.5f;
                    }
                    shapeFontRendererContext.setPosition(thumbRenderPosition);
                    shapeFontRendererContext.drawHalfCircle(radius, mint::Math::kPi);
                }
            }
            else if (scrollBarType == ScrollBarType::Horz)
            {
                ControlData& thumbControlData = createOrGetControlData(generateControlKeyString(scrollBarParent, L"ScrollBarHorzThumb", thumbControlType), thumbControlType);
                PrepareControlDataParam prepareControlDataParamForThumb;
                {
                    prepareControlDataParamForThumb._alwaysResetDisplaySize = true;
                    prepareControlDataParamForThumb._initialDisplaySize._x = thumbSize;
                    prepareControlDataParamForThumb._initialDisplaySize._y = kScrollBarThickness;

                    prepareControlDataParamForThumb._desiredPositionInParent = getControlPositionInParentSpace(scrollBarTrack);
                    prepareControlDataParamForThumb._desiredPositionInParent._y -= kScrollBarThickness * 0.5f;

                    prepareControlDataParamForThumb._parentHashKeyOverride = scrollBarParent.getHashKey();
                    prepareControlDataParamForThumb._ignoreMeForContentAreaSize = true;
                    prepareControlDataParamForThumb._clipRectUsage = ClipRectUsage::ParentsOwn;

                    thumbControlData._isDraggable = true;
                    thumbControlData._draggingConstraints.left(scrollBarTrack._position._x);
                    thumbControlData._draggingConstraints.right(thumbControlData._draggingConstraints.left() + trackRemnantSize);
                    thumbControlData._draggingConstraints.top(scrollBarTrack._position._y - kScrollBarThickness * 0.5f);
                    thumbControlData._draggingConstraints.bottom(thumbControlData._draggingConstraints.top());
                }
                prepareControlData(thumbControlData, prepareControlDataParamForThumb);

                // @중요
                // Calculate position from internal value
                thumbControlData._position._x = scrollBarTrack._position._x + (thumbControlData._controlValue._thumbData._thumbAt * trackRemnantSize);

                mint::RenderingBase::Color thumbColor;
                processScrollableControl(thumbControlData, getNamedColor(NamedColor::ScrollBarThumb), getNamedColor(NamedColor::ScrollBarThumb).scaledRgb(1.25f), thumbColor);

                const float thumbAtRatio = (trackRemnantSize < 1.0f) ? 0.0f : mint::Math::saturate((thumbControlData._position._x - thumbControlData._draggingConstraints.left()) / trackRemnantSize);
                thumbControlData._controlValue._thumbData._thumbAt = thumbAtRatio;
                scrollBarParent._childDisplayOffset._x = -thumbAtRatio * (totalLength - visibleLength + ((scrollBarType == ScrollBarType::Both) ? kScrollBarThickness : 0.0f)); // Scrolling!

                // Rendering thumb
                {
                    const float rectLength = thumbSize - radius * 2.0f;
                    shapeFontRendererContext.setClipRect(thumbControlData.getClipRect());
                    shapeFontRendererContext.setColor(thumbColor);

                    mint::Float4 thumbRenderPosition = mint::Float4(thumbControlData._position._x + radius, thumbControlData._position._y + radius, 0.0f, 1.0f);

                    // Left half circle
                    shapeFontRendererContext.setPosition(thumbRenderPosition);
                    shapeFontRendererContext.drawHalfCircle(radius, +mint::Math::kPiOverTwo);

                    // Rect
                    if (0.0f < rectLength)
                    {
                        thumbRenderPosition._x += rectLength * 0.5f;
                        shapeFontRendererContext.setPosition(thumbRenderPosition);
                        shapeFontRendererContext.drawRectangle(thumbControlData._displaySize - mint::Float2(radius * 2.0f, 0.0f), 0.0f, 0.0f);
                    }

                    // Right half circle
                    if (0.0f < rectLength)
                    {
                        thumbRenderPosition._x += rectLength * 0.5f;
                    }
                    shapeFontRendererContext.setPosition(thumbRenderPosition);
                    shapeFontRendererContext.drawHalfCircle(radius, -mint::Math::kPiOverTwo);
                }
            }
        }

        void GuiContext::processDock(const ControlData& controlData, mint::RenderingBase::ShapeFontRendererContext& rendererContext)
        {
            if (controlData._dockingControlType == DockingControlType::Dock || controlData._dockingControlType == DockingControlType::DockerDock)
            {
                for (DockingMethod dockingMethodIter = static_cast<DockingMethod>(0); dockingMethodIter != DockingMethod::COUNT; dockingMethodIter = static_cast<DockingMethod>(static_cast<uint32>(dockingMethodIter) + 1))
                {
                    const DockDatum& dockDatum = controlData.getDockDatum(dockingMethodIter);
                    if (dockDatum.hasDockedControls() == true)
                    {
                        const mint::Float2& dockSize = controlData.getDockSize(dockingMethodIter);
                        const mint::Float2& dockPosition = controlData.getDockPosition(dockingMethodIter);

                        if (_mouseStates.isButtonDownThisFrame() == true)
                        {
                            if (ControlCommonHelpers::isInControl(_mouseStates.getButtonDownPosition(), dockPosition, mint::Float2::kZero, dockSize) == true)
                            {
                                if (isDescendantControlInclusive(controlData, _controlInteractionStates.getFocusedControlHashKey()) == false)
                                {
                                    setControlFocused(controlData);
                                }
                            }
                        }

                        rendererContext.setClipRect(controlData.getClipRectForDocks());
                        
                        rendererContext.setColor(getNamedColor(NamedColor::Dock));
                        rendererContext.setPosition(mint::Float4(dockPosition._x + dockSize._x * 0.5f, dockPosition._y + dockSize._y * 0.5f, 0, 0));

                        rendererContext.drawRectangle(dockSize, 0.0f, 0.0f);
                    }
                }
            }
        }

        void GuiContext::endControlInternal(const ControlType controlType)
        {
            MINT_ASSERT("김장원", _controlStackPerFrame.back()._controlType == controlType, "begin 과 end 의 ControlType 이 다릅니다!!!");
            _controlStackPerFrame.pop_back();
        }

        void GuiContext::setClipRectForMe(ControlData& controlData, const mint::Rect& clipRect)
        {
            controlData.setClipRectXXX(clipRect);

            const ControlData& parentControlData = getControlData(controlData.getParentHashKey());
            if (parentControlData.isTypeOf(ControlType::Window) == true && controlData.isTypeOf(ControlType::Window) == true)
            {
                if (controlData.isDocking() == true)
                {
                    controlData.setClipRectXXX(parentControlData.getClipRectForDocks());
                }
                else
                {
                    controlData.setClipRectXXX(parentControlData.getClipRect());
                }
            }
        }

        void GuiContext::setClipRectForDocks(ControlData& controlData, const mint::Rect& clipRect)
        {
            controlData.setClipRectForDocksXXX(clipRect);
        }

        void GuiContext::setClipRectForChildren(ControlData& controlData, const mint::Rect& clipRect)
        {
            controlData.setClipRectForChildrenXXX(clipRect);
        }

        mint::Float2 GuiContext::beginTitleBar(const wchar_t* const windowTitle, const mint::Float2& titleBarSize, const mint::Rect& innerPadding, VisibleState& inoutParentVisibleState)
        {
            static constexpr ControlType controlType = ControlType::TitleBar;

            ControlData& controlData = createOrGetControlData(windowTitle, controlType);
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
                    const float textWidth = calculateTextWidth(windowTitle, mint::StringUtil::wcslen(windowTitle));
                    const mint::Float2& displaySizeOverride = mint::Float2(textWidth + 16.0f, controlData._displaySize._y);
                    prepareControlDataParam._initialDisplaySize = displaySizeOverride;
                    prepareControlDataParam._desiredPositionInParent._x = parentDockDatum.getDockedControlTitleBarOffset(dockedControlIndex);
                    prepareControlDataParam._desiredPositionInParent._y = 0.0f;
                }
                else
                {
                    prepareControlDataParam._initialDisplaySize = titleBarSize;
                    prepareControlDataParam._deltaInteractionSize = mint::Float2(-innerPadding.right() - kDefaultRoundButtonRadius * 2.0f, 0.0f);
                }
                prepareControlDataParam._alwaysResetDisplaySize = true;
                prepareControlDataParam._alwaysResetPosition = true;
                prepareControlDataParam._clipRectUsage = ClipRectUsage::ParentsOwn;
            }
            prepareControlData(controlData, prepareControlDataParam);
            
            mint::RenderingBase::Color finalBackgroundColor;
            const bool isFocused = processFocusControl(controlData, getNamedColor(NamedColor::TitleBarFocused), getNamedColor(NamedColor::TitleBarOutOfFocus), finalBackgroundColor);
            if (isParentControlDocking == true)
            {
                if (_controlInteractionStates.isControlPressed(controlData) == true)
                {
                    ControlData& dockControlData = getControlData(parentControlData.getDockControlHashKey());
                    DockDatum& dockDatum = dockControlData.getDockDatum(parentControlData._lastDockingMethod);
                    dockDatum._dockedControlIndexShown = dockDatum.getDockedControlIndex(parentControlData.getHashKey());
                    
                    setControlFocused(dockControlData);
                }
            }

            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            rendererContext.setClipRect(controlData.getClipRect());
            rendererContext.setPosition(getControlCenterPosition(controlData));
            if (isParentControlDocking == true)
            {
                const ControlData& dockControlData = getControlData(parentControlData.getDockControlHashKey());
                const bool isParentControlShownInDock = dockControlData.isShowingInDock(parentControlData);
                if (_controlInteractionStates.isControlHovered(controlData) == true)
                {
                    rendererContext.setColor(((isParentControlShownInDock == true) ? getNamedColor(NamedColor::ShownInDock) : getNamedColor(NamedColor::ShownInDock).addedRgb(32)));
                }
                else
                {
                    rendererContext.setColor(((isParentControlShownInDock == true) ? getNamedColor(NamedColor::ShownInDock) : getNamedColor(NamedColor::ShownInDock).addedRgb(16)));
                }

                rendererContext.drawRectangle(controlData._displaySize, 0.0f, 0.0f);
            }
            else
            {
                rendererContext.setColor(finalBackgroundColor);

                rendererContext.drawHalfRoundedRectangle(controlData._displaySize, (kDefaultRoundnessInPixel * 2.0f / controlData._displaySize.minElement()), mint::Math::kPi);

                rendererContext.setColor(mint::RenderingBase::Color(127, 127, 127));
                rendererContext.drawLine(controlData._position + mint::Float2(0.0f, titleBarSize._y), controlData._position + mint::Float2(controlData._displaySize._x, titleBarSize._y), 1.0f);
            }

            const mint::Float4& titleBarTextPosition = mint::Float4(controlData._position._x, controlData._position._y, 0.0f, 1.0f) + mint::Float4(innerPadding.left(), titleBarSize._y * 0.5f, 0.0f, 0.0f);
            const bool needToColorFocused_ = needToColorFocused(parentControlData);
            if (isParentControlDocking == true)
            {
                const ControlData& dockControlData = getControlData(parentControlData.getDockControlHashKey());
                const bool isParentControlShownInDock = dockControlData.isShowingInDock(parentControlData);

                rendererContext.setTextColor((isParentControlShownInDock == true) ? getNamedColor(NamedColor::ShownInDockFont) : getNamedColor(NamedColor::LightFont));
            }
            else
            {
                rendererContext.setTextColor((needToColorFocused_ == true) ? getNamedColor(NamedColor::LightFont) : getNamedColor(NamedColor::DarkFont));
            }
            rendererContext.drawDynamicText(windowTitle, titleBarTextPosition, 
                mint::RenderingBase::FontRenderingOption(mint::RenderingBase::TextRenderDirectionHorz::Rightward, mint::RenderingBase::TextRenderDirectionVert::Centered, 0.9375f));

            _controlStackPerFrame.push_back(ControlStackData(controlData));

            // Close button
            if (parentControlData.isDocking() == false)
            {
                // 중요
                nextNoAutoPositioned();
                nextControlPosition(mint::Float2(titleBarSize._x - kDefaultRoundButtonRadius * 2.0f - innerPadding.right(), (titleBarSize._y - kDefaultRoundButtonRadius * 2.0f) * 0.5f));

                if (pushRoundButton(windowTitle, mint::RenderingBase::Color(1.0f, 0.375f, 0.375f)) == true)
                {
                    inoutParentVisibleState = mint::Gui::VisibleState::Invisible;
                }
            }

            // Window Offset 재조정!!
            parentControlData.setOffsetY_XXX(titleBarSize._y + parentControlData.getInnerPadding().top());

            return titleBarSize;
        }

        const bool GuiContext::pushRoundButton(const wchar_t* const windowTitle, const mint::RenderingBase::Color& color)
        {
            static constexpr ControlType controlType = ControlType::RoundButton;

            const ControlData& parentWindowData = getParentWindowControlData();

            const float radius = kDefaultRoundButtonRadius;
            ControlData& controlData = createOrGetControlData(windowTitle, controlType);

            PrepareControlDataParam prepareControlDataParam;
            {
                prepareControlDataParam._parentHashKeyOverride = parentWindowData.getHashKey();
                prepareControlDataParam._initialDisplaySize = mint::Float2(radius * 2.0f);
                prepareControlDataParam._clipRectUsage = ClipRectUsage::ParentsOwn;
            }
            prepareControlData(controlData, prepareControlDataParam);
            
            mint::RenderingBase::Color controlColor;
            const bool isClicked = processClickControl(controlData, color, color.scaledRgb(1.5f), color.scaledRgb(0.75f), controlColor);

            const mint::Float4& controlCenterPosition = getControlCenterPosition(controlData);
            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            rendererContext.setClipRect(controlData.getClipRect());
            rendererContext.setColor(controlColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawCircle(radius);

            return isClicked;
        }

        void GuiContext::pushTooltipWindow(const wchar_t* const tooltipText, const mint::Float2& position)
        {
            static constexpr ControlType controlType = ControlType::TooltipWindow;
            static constexpr float kTooltipFontScale = kFontScaleC;
            const float tooltipWindowPadding = 8.0f;

            ControlData& controlData = createOrGetControlData(tooltipText, controlType, L"TooltipWindow");
            PrepareControlDataParam prepareControlDataParam;
            {
                const float tooltipTextWidth = calculateTextWidth(tooltipText, mint::StringUtil::wcslen(tooltipText)) * kTooltipFontScale;
                prepareControlDataParam._initialDisplaySize = mint::Float2(tooltipTextWidth + tooltipWindowPadding * 2.0f, _fontSize * kTooltipFontScale + tooltipWindowPadding);
                prepareControlDataParam._desiredPositionInParent = position;
                prepareControlDataParam._alwaysResetParent = true;
                prepareControlDataParam._alwaysResetDisplaySize = true;
                prepareControlDataParam._alwaysResetPosition = true;
                prepareControlDataParam._parentHashKeyOverride = _controlInteractionStates.getTooltipParentWindowHashKey();
                prepareControlDataParam._clipRectUsage = ClipRectUsage::ParentsOwn;
            }
            nextNoAutoPositioned();
            prepareControlData(controlData, prepareControlDataParam);
            
            mint::RenderingBase::Color dummyColor;
            processShowOnlyControl(controlData, dummyColor);

            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(RendererContextLayer::TopMost);
            rendererContext.setClipRect(controlData.getClipRect());
            
            const mint::Float4& controlCenterPosition = getControlCenterPosition(controlData);
            rendererContext.setColor(getNamedColor(NamedColor::TooltipBackground));
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(controlData._displaySize, (kDefaultRoundnessInPixel / controlData._displaySize.minElement()) * 0.75f, 0.0f, 0.0f);

            const mint::Float4& textPosition = mint::Float4(controlData._position._x, controlData._position._y, 0.0f, 1.0f) + mint::Float4(tooltipWindowPadding, prepareControlDataParam._initialDisplaySize._y * 0.5f, 0.0f, 0.0f);
            rendererContext.setClipRect(controlData.getClipRect());
            rendererContext.setTextColor(getNamedColor(NamedColor::DarkFont));
            rendererContext.drawDynamicText(tooltipText, textPosition, 
                mint::RenderingBase::FontRenderingOption(mint::RenderingBase::TextRenderDirectionHorz::Rightward, mint::RenderingBase::TextRenderDirectionVert::Centered, kTooltipFontScale));
        }

        const wchar_t* GuiContext::generateControlKeyString(const wchar_t* const name, const ControlType controlType) const noexcept
        {
            return generateControlKeyString(getControlStackTopXXX(), name, controlType);
        }

        const wchar_t* GuiContext::generateControlKeyString(const ControlData& parentControlData, const wchar_t* const name, const ControlType controlType) const noexcept
        {
            static std::wstring hashKeyWstring;
            hashKeyWstring.clear();
            hashKeyWstring.append(std::to_wstring(parentControlData.getHashKey()));
            hashKeyWstring.append(name);
            hashKeyWstring.append(std::to_wstring(static_cast<uint16>(controlType)));
            return hashKeyWstring.c_str();
        }

        const uint64 GuiContext::generateControlHashKeyXXX(const wchar_t* const text, const ControlType controlType) const noexcept
        {
            static std::wstring hashKeyWstring;
            hashKeyWstring.clear();
            hashKeyWstring.append(text);
            hashKeyWstring.append(std::to_wstring(static_cast<uint16>(controlType)));
            return mint::computeHash(hashKeyWstring.c_str());
        }

        ControlData& GuiContext::createOrGetControlData(const wchar_t* const text, const ControlType controlType, const wchar_t* const hashGenerationKeyOverride) noexcept
        {
            const uint64 hashKey = generateControlHashKeyXXX((hashGenerationKeyOverride == nullptr) ? text : hashGenerationKeyOverride, controlType);
            auto found = _controlIdMap.find(hashKey);
            if (found.isValid() == false)
            {
                const ControlData& stackTopControlData = getControlStackTopXXX();
                ControlData newControlData{ hashKey, stackTopControlData.getHashKey(), controlType };
                newControlData._text = text;

                _controlIdMap.insert(hashKey, std::move(newControlData));
            }

            ControlData& controlData = _controlIdMap.at(hashKey);
            if (controlData._updateCount < 3)
            {
                ++controlData._updateCount;
            }
            return controlData;
        }

        const ControlData& GuiContext::getParentWindowControlData() const noexcept
        {
            MINT_ASSERT("김장원", _controlStackPerFrame.empty() == false, "Control 스택이 비어있을 때 호출되면 안 됩니다!!!");

            return getParentWindowControlData(getControlData(_controlStackPerFrame.back()._hashKey));
        }

        const ControlData& GuiContext::getParentWindowControlData(const ControlData& controlData) const noexcept
        {
            return getParentWindowControlDataInternal(controlData.getParentHashKey());
        }

        const ControlData& GuiContext::getParentWindowControlDataInternal(const uint64 hashKey) const noexcept
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

        const bool GuiContext::isThisControlPressed() const noexcept
        {
            return _controlInteractionStates.isControlPressed(getControlStackTopXXX());
        }

        const bool GuiContext::isFocusedControlInputBox() const noexcept
        {
            return (_controlInteractionStates.hasFocusedControl()) ? getControlData(_controlInteractionStates.getFocusedControlHashKey()).isInputBoxType() : false;
        }

        void GuiContext::setControlFocused(const ControlData& controlData) noexcept
        {
            if (controlData._isFocusable == true)
            {
                if (controlData._needDoubleClickToFocus == true)
                {
                    if (_mouseStates.isDoubleClicked() == true)
                    {
                        _controlInteractionStates.setControlFocused(controlData);
                    }
                }
                else
                {
                    _controlInteractionStates.setControlFocused(controlData);
                }
            }
        }

        void GuiContext::setControlHovered(const ControlData& controlData) noexcept
        {
            _controlInteractionStates.setControlHovered(controlData);
        }

        void GuiContext::setControlPressed(const ControlData& controlData) noexcept
        {
            if (_controlInteractionStates.setControlPressed(controlData) == true)
            {
                const ControlData& closestFocusableAncestor = getClosestFocusableAncestorControlInclusive(controlData);
                setControlFocused(closestFocusableAncestor);
            }
        }
        
        void GuiContext::setControlClicked(const ControlData& controlData) noexcept
        {
            if (_controlInteractionStates.setControlClicked(controlData) == true)
            {
                const ControlData& closestFocusableAncestor = getClosestFocusableAncestorControlInclusive(controlData);
                setControlFocused(closestFocusableAncestor);
            }
        }

        void GuiContext::prepareControlData(ControlData& controlData, const PrepareControlDataParam& prepareControlDataParam) noexcept
        {
            const bool isNewData = controlData._displaySize.isNan();
            if ((isNewData == true) || (prepareControlDataParam._alwaysResetParent == true))
            {
                const ControlData& stackTopControlData = getControlStackTopXXX();
                const uint64 parentHashKey = (prepareControlDataParam._parentHashKeyOverride == 0) ? stackTopControlData.getHashKey() : prepareControlDataParam._parentHashKeyOverride;
                controlData.setParentHashKeyXXX(parentHashKey);

                if (isNewData == true)
                {
                    controlData._resizingMask = prepareControlDataParam._initialResizingMask;

                    // 중요!!!
                    controlData.setClipRectXXX(_clipRectFullScreen);
                    controlData.setClipRectForChildrenXXX(_clipRectFullScreen);
                    controlData.setClipRectForDocksXXX(_clipRectFullScreen);
                }
            }

            ControlData& parentControlData = getControlData(controlData.getParentHashKey());
            controlData.updatePerFrameWithParent(isNewData, prepareControlDataParam, parentControlData);

            // 부모와 동일한 RendererContextLayer 가 되도록!
            controlData._rendererContextLayer = parentControlData._rendererContextLayer;
            
            // Display size
            if (isNewData == true || prepareControlDataParam._alwaysResetDisplaySize == true)
            {
                const float maxDisplaySizeX = parentControlData._displaySize._x - ((_nextControlStates._nextNoAutoPositioned == false) ? (parentControlData.getInnerPadding().left() * 2.0f) : 0.0f);
                if (_nextControlStates._nextControlSizeNonContrainedToParent == true)
                {
                    controlData._displaySize._x = (_nextControlStates._nextDesiredControlSize._x <= 0.0f) ? prepareControlDataParam._initialDisplaySize._x : _nextControlStates._nextDesiredControlSize._x;
                    controlData._displaySize._y = (_nextControlStates._nextDesiredControlSize._y <= 0.0f) ? prepareControlDataParam._initialDisplaySize._y : _nextControlStates._nextDesiredControlSize._y;
                }
                else
                {
                    controlData._displaySize._x = (_nextControlStates._nextDesiredControlSize._x <= 0.0f) 
                        ? mint::min(maxDisplaySizeX, prepareControlDataParam._initialDisplaySize._x) 
                        : ((_nextControlStates._nextSizingForced == true) 
                            ? _nextControlStates._nextDesiredControlSize._x 
                            : mint::min(maxDisplaySizeX, _nextControlStates._nextDesiredControlSize._x));
                    controlData._displaySize._y = (_nextControlStates._nextDesiredControlSize._y <= 0.0f)
                        ? prepareControlDataParam._initialDisplaySize._y 
                        : ((_nextControlStates._nextSizingForced == true) 
                            ? _nextControlStates._nextDesiredControlSize._y 
                            : mint::max(prepareControlDataParam._initialDisplaySize._y, _nextControlStates._nextDesiredControlSize._y));
                }
            }

            // Position, Parent offset, Parent child at, Parent content area size
            const bool isAutoPositioned = (_nextControlStates._nextNoAutoPositioned == false);
            if (isAutoPositioned == true)
            {
                mint::Float2& parentControlChildAt = const_cast<mint::Float2&>(parentControlData.getChildAt());
                mint::Float2& parentControlNextChildOffset = const_cast<mint::Float2&>(parentControlData.getNextChildOffset());
                const float parentControlPreviousNextChildOffsetX = parentControlNextChildOffset._x;

                const bool isSameLineAsPreviousControl = (_nextControlStates._nextSameLine == true);
                if (isSameLineAsPreviousControl == true)
                {
                    const float intervalX = (true == _nextControlStates._nextNoInterval) ? 0.0f : kDefaultIntervalX;
                    parentControlChildAt._x += (parentControlNextChildOffset._x + intervalX);

                    parentControlNextChildOffset = controlData._displaySize;
                }
                else
                {
                    parentControlChildAt._x = parentControlData._position._x + parentControlData.getInnerPadding().left() + parentControlData._childDisplayOffset._x; // @중요
                    parentControlChildAt._x += parentControlData.getDockSizeIfHosting(DockingMethod::LeftSide)._x;

                    parentControlChildAt._y += parentControlNextChildOffset._y;

                    parentControlNextChildOffset = controlData._displaySize;
                }

                const bool addIntervalY = (_nextControlStates._nextNoAutoPositioned == false && prepareControlDataParam._noIntervalForNextSibling == false);
                if (addIntervalY == true)
                {
                    const float intervalY = (true == _nextControlStates._nextNoInterval) ? 0.0f : kDefaultIntervalY;
                    parentControlNextChildOffset._y += intervalY;
                }

                // Parent content area size
                mint::Float2& parentControlContentAreaSize = const_cast<mint::Float2&>(parentControlData.getContentAreaSize());
                if (prepareControlDataParam._ignoreMeForContentAreaSize == false)
                {
                    if (true == _nextControlStates._nextSameLine)
                    {
                        if (parentControlContentAreaSize._x == 0.0f)
                        {
                            // 최초 _nextSameLine 시 바로 왼쪽 컨트롤의 크기도 추가해줘야 한다!
                            parentControlContentAreaSize._x = parentControlPreviousNextChildOffsetX;
                        }
                        parentControlContentAreaSize._x += controlData._displaySize._x + kDefaultIntervalX;
                    }

                    parentControlContentAreaSize._y += (true == _nextControlStates._nextSameLine) ? 0.0f : controlData._displaySize._y;
                    parentControlContentAreaSize._y += (true == addIntervalY) ? kDefaultIntervalY : 0.0f;
                }

                controlData._position = parentControlChildAt;
                controlData._position += prepareControlDataParam._offset;
            }
            else
            {
                // NO Auto-positioned

                if (prepareControlDataParam._alwaysResetPosition == true || isNewData == true)
                {
                    controlData._position = parentControlData._position;

                    if (prepareControlDataParam._desiredPositionInParent.isNan() == true)
                    {
                        controlData._position += _nextControlStates._nextControlPosition;
                    }
                    else
                    {
                        controlData._position += prepareControlDataParam._desiredPositionInParent;
                    }
                }
            }

            // Child at
            calculateControlChildAt(controlData);
        }

        void GuiContext::calculateControlChildAt(ControlData& controlData) noexcept
        {
            const MenuBarType currentMenuBarType = controlData._controlValue._commonData._menuBarType;
            mint::Float2& controlChildAt = const_cast<mint::Float2&>(controlData.getChildAt());
            controlChildAt = controlData._position + controlData._childDisplayOffset +
                ((_nextControlStates._nextNoAutoPositioned == false)
                    ? mint::Float2(controlData.getInnerPadding().left(), controlData.getInnerPadding().top())
                    : mint::Float2::kZero) +
                mint::Float2(0.0f, (MenuBarType::None != currentMenuBarType) ? kMenuBarBaseSize._y : 0.0f);

            const DockDatum& dockDatumTopSide = controlData.getDockDatum(DockingMethod::TopSide);
            if (dockDatumTopSide.hasDockedControls() == true)
            {
                // 맨 처음 Child Control 만 내려주면 된다!!
                controlChildAt._y += controlData.getDockSize(DockingMethod::TopSide)._y + controlData.getInnerPadding().top();
            }
        }

        const bool GuiContext::processClickControl(ControlData& controlData, const mint::RenderingBase::Color& normalColor, const mint::RenderingBase::Color& hoverColor, const mint::RenderingBase::Color& pressedColor, mint::RenderingBase::Color& outBackgroundColor) noexcept
        {
            processControlInteractionInternal(controlData);

            outBackgroundColor = normalColor;

            const bool isClicked = _controlInteractionStates.isControlClicked(controlData);
            if (_controlInteractionStates.isControlHovered(controlData) == true)
            {
                outBackgroundColor = hoverColor;
            }
            if (_controlInteractionStates.isControlPressed(controlData) == true || isClicked == true)
            {
                outBackgroundColor = pressedColor;
            }

            if (needToColorFocused(controlData) == false)
            {
                // Out-of-focus alpha
                outBackgroundColor.scaleA(kDefaultOutOfFocusAlpha);
            }

            processControlCommon(controlData);

            return isClicked;
        }

        const bool GuiContext::processFocusControl(ControlData& controlData, const mint::RenderingBase::Color& focusedColor, const mint::RenderingBase::Color& nonFocusedColor, mint::RenderingBase::Color& outBackgroundColor) noexcept
        {
            processControlInteractionInternal(controlData, false);

            const uint64 controlHashKey = (0 != controlData._delegateHashKey) ? controlData._delegateHashKey : controlData.getHashKey();

            // Check new focus
            if (_draggedControlHashKey == 0 && _resizedControlHashKey == 0 && controlData._isFocusable == true &&
                (_mouseStates.isButtonDownThisFrame() == true 
                    && (_controlInteractionStates.isControlPressed(controlData) == true || _controlInteractionStates.isControlClicked(controlData) == true)))
            {
                if (controlData._needDoubleClickToFocus == true)
                {
                    if (_mouseStates.isDoubleClicked() == true)
                    {
                        // Focus entered
                        setControlFocused(controlData);
                    }
                }
                else
                {
                    // Focus entered
                    setControlFocused(controlData);
                }
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

            processControlCommon(controlData);

            return _controlInteractionStates.isControlFocused(controlData);
        }

        void GuiContext::processShowOnlyControl(ControlData& controlData, mint::RenderingBase::Color& outBackgroundColor, const bool setMouseInteractionDone) noexcept
        {
            processControlInteractionInternal(controlData, setMouseInteractionDone);

            if (needToColorFocused(controlData) == true)
            {
                outBackgroundColor.scaleA(kDefaultFocusedAlpha);
            }
            else
            {
                outBackgroundColor.scaleA(kDefaultOutOfFocusAlpha);
            }

            processControlCommon(controlData);
        }

        const bool GuiContext::processScrollableControl(ControlData& controlData, const mint::RenderingBase::Color& normalColor, const mint::RenderingBase::Color& dragColor, mint::RenderingBase::Color& outBackgroundColor) noexcept
        {
            processControlInteractionInternal(controlData);

            outBackgroundColor = normalColor;

            const bool isHovered = _controlInteractionStates.isControlHovered(controlData);
            const bool isPressed = _controlInteractionStates.isControlPressed(controlData);
            const bool isDragging = isControlBeingDragged(controlData);
            if (isHovered == true || isPressed == true || isDragging == true)
            {
                outBackgroundColor = dragColor;
            }

            if (needToColorFocused(controlData) == false)
            {
                outBackgroundColor.scaleA(kDefaultOutOfFocusAlpha);
            }

            processControlCommon(controlData);

            return isPressed;
        }
        
        const bool GuiContext::processToggleControl(ControlData& controlData, const mint::RenderingBase::Color& normalColor, const mint::RenderingBase::Color& hoverColor, const mint::RenderingBase::Color& toggledColor, mint::RenderingBase::Color& outBackgroundColor) noexcept
        {
            processControlInteractionInternal(controlData);

            const bool isClicked = _controlInteractionStates.isControlClicked(controlData);
            if (isClicked == true)
            {
                controlData._controlValue._booleanData.toggle();
            }

            const bool isToggled = controlData._controlValue._booleanData.get();
            const bool isHovered = _controlInteractionStates.isControlHovered(controlData);
            outBackgroundColor = (isToggled == true) ? toggledColor : (isHovered == true) ? hoverColor : normalColor;

            if (needToColorFocused(controlData) == false)
            {
                outBackgroundColor.scaleA(kDefaultOutOfFocusAlpha);
            }

            processControlCommon(controlData);
            return isClicked;
        }
        
        void GuiContext::processControlInteractionInternal(ControlData& controlData, const bool setMouseInteractionDone) noexcept
        {
            const uint64 controlHashKey = controlData.getHashKey();
            if (isInteractingInternal(controlData) == false || _controlInteractionStates.isMouseInteractionDoneThisFrame() == true)
            {
                _controlInteractionStates.resetHoverIf(controlData);
                _controlInteractionStates.resetPressIf(controlData);
                return;
            }

            const ControlData& parentControlData = getControlData(controlData.getParentHashKey());
            const bool isMouseInParentInteractionArea = ControlCommonHelpers::isInControlInteractionArea(_mouseStates.getPosition(), parentControlData);
            const bool isMouseInInteractionArea = ControlCommonHelpers::isInControlInteractionArea(_mouseStates.getPosition(), controlData);
            const bool meetsAreaCondition = (controlData._isInteractableOutsideParent == true || isMouseInParentInteractionArea == true) && (isMouseInInteractionArea == true);
            const bool meetsInteractionCondition = (shouldInteract(_mouseStates.getPosition(), controlData) == true || controlData.isRootControl() == true);
            if (meetsAreaCondition == true && meetsInteractionCondition == true)
            {
                // Hovered (at least)

                if (setMouseInteractionDone == true)
                {
                    _controlInteractionStates.setMouseInteractionDoneThisFrame();
                }

                if (_controlInteractionStates.isControlHovered(controlData) == false && controlData._isFocusable == false)
                {
                    setControlHovered(controlData);
                }

                // Click Event 가 발생했을 때도 Pressed 상태 유지!
                if (_mouseStates.isButtonDownUp() == false && _mouseStates.isButtonDown() == false)
                {
                    _controlInteractionStates.resetPressIf(controlData);
                }
                
                // Pressed (Mouse down)
                const bool isMouseDownInInteractionArea = ControlCommonHelpers::isInControlInteractionArea(_mouseStates.getButtonDownPosition(), controlData);
                if (isMouseDownInInteractionArea == true)
                {
                    if (_mouseStates.isButtonDownThisFrame() == true || _mouseStates.isDoubleClicked() == true)
                    {
                        setControlPressed(controlData);
                    }

                    // Clicked (only in interaction area)
                    if (_mouseStates.isButtonDownUp() == true)
                    {
                        setControlClicked(controlData);
                    }
                }
            }
            else
            {
                // Not interacting

                _controlInteractionStates.resetHoverIf(controlData);
                _controlInteractionStates.resetPressIf(controlData);
            }
        }

        void GuiContext::processControlCommon(ControlData& controlData) noexcept
        {
            checkControlResizing(controlData);
            checkControlHoveringForTooltip(controlData);

            processControlResizingInternal(controlData);
            processControlDraggingInternal(controlData);
            processControlDockingInternal(controlData);

            _nextControlStates.reset();
        }

        void GuiContext::checkControlResizing(ControlData& controlData) noexcept
        {
            if (_resizedControlHashKey == 0 && isInteractingInternal(controlData) == true)
            {
                mint::Window::CursorType newCursorType;
                ResizingMask resizingMask;
                const bool isResizable = controlData.isResizable();
                if (controlData.isResizable() == true 
                    && ControlCommonHelpers::isInControlBorderArea(_mouseStates.getPosition(), controlData, newCursorType, resizingMask, _resizingMethod) == true)
                {
                    if (controlData._resizingMask.overlaps(resizingMask) == true)
                    {
                        _mouseStates._cursorType = newCursorType;

                        _controlInteractionStates.setMouseInteractionDoneThisFrame();
                    }
                }
            }
        }

        void GuiContext::checkControlHoveringForTooltip(ControlData& controlData) noexcept
        {
            const bool isHovered = _controlInteractionStates.isControlHovered(controlData);
            if (_nextControlStates._nextTooltipText != nullptr && isHovered == true 
                && _controlInteractionStates.isHoveringMoreThan(1000) == true)
            {
                _controlInteractionStates.setTooltipData(_mouseStates, _nextControlStates._nextTooltipText, getParentWindowControlData(controlData).getHashKey());
            }
        }

        void GuiContext::processControlResizingInternal(ControlData& controlData) noexcept
        {
            ControlData& changeTargetControlData = (controlData._delegateHashKey == 0) ? controlData : getControlData(controlData._delegateHashKey);
            const bool isResizing = isControlBeingResized(changeTargetControlData);
            if (isResizing == true)
            {
                if (_isResizeBegun == true)
                {
                    _resizedControlInitialPosition = changeTargetControlData._position;
                    _resizedControlInitialDisplaySize = changeTargetControlData._displaySize;

                    _isResizeBegun = false;
                }

                mint::Float2& changeTargetControlDisplaySize = const_cast<mint::Float2&>(changeTargetControlData._displaySize);

                const mint::Float2 mouseDragDelta = _mouseStates.getMouseDragDelta();
                const float flipHorz = (_resizingMethod == ResizingMethod::RepositionHorz || _resizingMethod == ResizingMethod::RepositionBoth) ? -1.0f : +1.0f;
                const float flipVert = (_resizingMethod == ResizingMethod::RepositionVert || _resizingMethod == ResizingMethod::RepositionBoth) ? -1.0f : +1.0f;
                if (_mouseStates.isCursor(mint::Window::CursorType::SizeVert) == false)
                {
                    const float newPositionX = _resizedControlInitialPosition._x - mouseDragDelta._x * flipHorz;
                    const float newDisplaySizeX = _resizedControlInitialDisplaySize._x + mouseDragDelta._x * flipHorz;

                    if (changeTargetControlData.getDisplaySizeMin()._x + changeTargetControlData.getHorzDockSizeSum() < newDisplaySizeX)
                    {
                        if (flipHorz < 0.0f)
                        {
                            changeTargetControlData._position._x = newPositionX;
                        }
                        changeTargetControlDisplaySize._x = newDisplaySizeX;
                    }
                }
                if (_mouseStates.isCursor(mint::Window::CursorType::SizeHorz) == false)
                {
                    const float newPositionY = _resizedControlInitialPosition._y - mouseDragDelta._y * flipVert;
                    const float newDisplaySizeY = _resizedControlInitialDisplaySize._y + mouseDragDelta._y * flipVert;

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

                _controlInteractionStates.setMouseInteractionDoneThisFrame();
            }
        }

        void GuiContext::processControlDraggingInternal(ControlData& controlData) noexcept
        {
            ControlData& changeTargetControlData = (controlData._delegateHashKey == 0) ? controlData : getControlData(controlData._delegateHashKey);
            const bool isDragging = isControlBeingDragged(controlData);
            if (isDragging == true)
            {
                if (_isDragBegun == true)
                {
                    _draggedControlInitialPosition = changeTargetControlData._position;

                    _isDragBegun = false;
                }

                const mint::Float2 mouseDragDelta = _mouseStates.getMouseDragDelta();
                const mint::Float2 originalPosition = changeTargetControlData._position;
                if (changeTargetControlData._draggingConstraints.isNan() == true)
                {
                    changeTargetControlData._position = _draggedControlInitialPosition + mouseDragDelta;
                }
                else
                {
                    const mint::Float2& naivePosition = _draggedControlInitialPosition + mouseDragDelta;
                    changeTargetControlData._position._x = mint::min(mint::max(changeTargetControlData._draggingConstraints.left(), naivePosition._x), changeTargetControlData._draggingConstraints.right());
                    changeTargetControlData._position._y = mint::min(mint::max(changeTargetControlData._draggingConstraints.top(), naivePosition._y), changeTargetControlData._draggingConstraints.bottom());
                }

                if (changeTargetControlData.isDocking() == true)
                {
                    // Docking 중이었으면 마우스로 바로 옮길 수 없도록!! (Dock 에 좀 더 오래 붙어있도록)

                    changeTargetControlData._position = originalPosition;

                    ControlData& dockControlData = getControlData(changeTargetControlData.getDockControlHashKey());
                    DockDatum& dockDatum = dockControlData.getDockDatum(changeTargetControlData._lastDockingMethod);
                    const mint::Float2& dockSize = dockControlData.getDockSize(changeTargetControlData._lastDockingMethod);
                    const mint::Float2& dockPosition = dockControlData.getDockPosition(changeTargetControlData._lastDockingMethod);
                    const mint::Rect dockRect{ dockPosition, dockSize };
                    bool needToDisconnectFromDock = true;
                    if (dockRect.contains(_mouseStates.getPosition()) == true)
                    {
                        needToDisconnectFromDock = false;

                        const mint::Rect dockTitleBarAreaRect{ dockPosition, mint::Float2(dockSize._x, kTitleBarBaseSize._y) };
                        if (dockTitleBarAreaRect.contains(_mouseStates.getPosition()) == true)
                        {
                            const float titleBarOffset = _mouseStates.getPosition()._x - dockTitleBarAreaRect.left();
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
                    changeTargetControlData._currentFrameDeltaPosition = changeTargetControlData._position - originalPosition;
                }

                _controlInteractionStates.setMouseInteractionDoneThisFrame();
            }
        }

        void GuiContext::processControlDockingInternal(ControlData& controlData) noexcept
        {
            ControlData& changeTargetControlData = (controlData._delegateHashKey == 0) ? controlData : getControlData(controlData._delegateHashKey);
            const bool isDragging = isControlBeingDragged(controlData);
            
            static constexpr mint::RenderingBase::Color color = mint::RenderingBase::Color(100, 110, 160);
            mint::RenderingBase::ShapeFontRendererContext& rendererContext = getRendererContext(RendererContextLayer::TopMost);
            std::function fnRenderDockingBox = [&](const mint::Rect& boxRect, const ControlData& parentControlData)
            {
                const mint::Float4& parentControlCenterPosition = getControlCenterPosition(parentControlData);
                mint::Float4 renderPosition = parentControlCenterPosition;
                renderPosition._x = boxRect.center()._x;
                renderPosition._y = boxRect.center()._y;
                rendererContext.setClipRect(parentControlData.getClipRect());

                const bool isMouseInBoxRect = boxRect.contains(_mouseStates.getPosition());
                rendererContext.setColor(((isMouseInBoxRect == true) ? color.scaledRgb(1.5f) : color));
                rendererContext.setPosition(renderPosition);
                rendererContext.drawRectangle(boxRect.size(), kDockingInteractionDisplayBorderThickness, 0.0f);
            };
            std::function fnRenderPreview = [&](const mint::Rect& previewRect)
            {
                rendererContext.setClipRect(0);
                rendererContext.setColor(color.scaledA(0.5f));
                rendererContext.setPosition(mint::Float4(previewRect.center()._x, previewRect.center()._y, 0.0f, 1.0f));
                rendererContext.drawRectangle(previewRect.size(), 0.0f, 0.0f);
            };

            ControlData& parentControlData = getControlData(changeTargetControlData.getParentHashKey());
            if ((changeTargetControlData.hasChildWindow() == false) && 
                (changeTargetControlData._dockingControlType == DockingControlType::Docker || changeTargetControlData._dockingControlType == DockingControlType::DockerDock) &&
                (parentControlData._dockingControlType == DockingControlType::Dock || parentControlData._dockingControlType == DockingControlType::DockerDock) &&
                ControlCommonHelpers::isInControlInteractionArea(_mouseStates.getPosition(), changeTargetControlData) == true)
            {
                const mint::Float4& parentControlCenterPosition = getControlCenterPosition(parentControlData);
                const float previewShortLengthMax = 160.0f;
                const float previewShortLength = mint::min(parentControlData._displaySize._x * 0.5f, previewShortLengthMax);

                mint::Rect interactionBoxRect;
                mint::Rect previewRect;
                
                // 초기화
                if (_mouseStates.isButtonDownUp() == false)
                {
                    changeTargetControlData._lastDockingMethodCandidate = DockingMethod::COUNT;
                }

                // Top
                {
                    interactionBoxRect.left(parentControlCenterPosition._x - kDockingInteractionLong * 0.5f);
                    interactionBoxRect.right(interactionBoxRect.left() + kDockingInteractionLong);
                    interactionBoxRect.top(parentControlData._position._y + parentControlData.getDockOffsetSize()._y + kDockingInteractionOffset);
                    interactionBoxRect.bottom(interactionBoxRect.top() + kDockingInteractionShort);

                    const mint::Float2& dockPosition = parentControlData.getDockPosition(DockingMethod::TopSide);
                    previewRect.position(dockPosition);
                    previewRect.right(previewRect.left() + parentControlData._displaySize._x);
                    previewRect.bottom(previewRect.top() + previewShortLength);

                    if (isDragging == true)
                    {
                        fnRenderDockingBox(interactionBoxRect, parentControlData);

                        DockDatum& parentControlDockDatum = parentControlData.getDockDatum(DockingMethod::TopSide);
                        if (changeTargetControlData._lastDockingMethodCandidate == DockingMethod::COUNT && interactionBoxRect.contains(_mouseStates.getPosition()) == true)
                        {
                            changeTargetControlData._lastDockingMethodCandidate = DockingMethod::TopSide;

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

                    const mint::Float2& dockPosition = parentControlData.getDockPosition(DockingMethod::BottomSide);
                    previewRect.position(dockPosition);
                    previewRect.right(previewRect.left() + parentControlData._displaySize._x);
                    previewRect.bottom(previewRect.top() + previewShortLength);

                    if (isDragging == true)
                    {
                        fnRenderDockingBox(interactionBoxRect, parentControlData);

                        DockDatum& parentControlDockDatum = parentControlData.getDockDatum(DockingMethod::BottomSide);
                        if (changeTargetControlData._lastDockingMethodCandidate == DockingMethod::COUNT && interactionBoxRect.contains(_mouseStates.getPosition()) == true)
                        {
                            changeTargetControlData._lastDockingMethodCandidate = DockingMethod::BottomSide;

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

                    const mint::Float2& dockPosition = parentControlData.getDockPosition(DockingMethod::LeftSide);
                    previewRect.position(dockPosition);
                    previewRect.right(previewRect.left() + previewShortLength);
                    previewRect.bottom(previewRect.top() + parentControlData._displaySize._y - parentControlData.getDockOffsetSize()._y);

                    if (isDragging == true)
                    {
                        fnRenderDockingBox(interactionBoxRect, parentControlData);

                        DockDatum& parentControlDockDatum = parentControlData.getDockDatum(DockingMethod::LeftSide);
                        if (changeTargetControlData._lastDockingMethodCandidate == DockingMethod::COUNT && interactionBoxRect.contains(_mouseStates.getPosition()) == true)
                        {
                            changeTargetControlData._lastDockingMethodCandidate = DockingMethod::LeftSide;
                            
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

                    const mint::Float2& dockPosition = parentControlData.getDockPosition(DockingMethod::RightSide);
                    previewRect.position(dockPosition);
                    previewRect.right(previewRect.left() + previewShortLength);
                    previewRect.bottom(previewRect.top() + parentControlData._displaySize._y - parentControlData.getDockOffsetSize()._y);

                    if (isDragging == true)
                    {
                        fnRenderDockingBox(interactionBoxRect, parentControlData);

                        DockDatum& parentControlDockDatum = parentControlData.getDockDatum(DockingMethod::RightSide);
                        if (changeTargetControlData._lastDockingMethodCandidate == DockingMethod::COUNT && interactionBoxRect.contains(_mouseStates.getPosition()) == true)
                        {
                            changeTargetControlData._lastDockingMethodCandidate = DockingMethod::RightSide;

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

                if (_mouseStates.isButtonDownUp() == true && changeTargetControlData._lastDockingMethodCandidate != DockingMethod::COUNT)
                {
                    if (changeTargetControlData.isDocking() == false)
                    {
                        // Docking 시작.

                        dock(changeTargetControlData.getHashKey(), parentControlData.getHashKey());

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
            parentControlDockDatum._dockedControlHashArray.push_back(dockedControlData.getHashKey());

            dockedControlData._resizingMask = ResizingMask::fromDockingMethod(dockedControlData._lastDockingMethod);
            dockedControlData._position = dockControlData.getDockPosition(dockedControlData._lastDockingMethod);
            dockedControlData.connectToDock(dockControlHashKey);

            parentControlDockDatum._dockedControlIndexShown = parentControlDockDatum.getDockedControlIndex(dockedControlData.getHashKey());

            updateDockDatum(dockControlHashKey);

            // 내가 Focus 였다면 Dock 을 가진 컨트롤로 옮기자!
            if (_controlInteractionStates.isControlFocused(dockedControlData) == true)
            {
                setControlFocused(dockControlData);
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
                dockDatum._dockedControlHashArray.erase(indexToErase);
            }
            else
            {
                MINT_LOG_ERROR("김장원", "Docked Control 이 Parent 의 Child Array 에 없는 상황입니다!!!");
            }

            dockedControlData.swapDockingStateContext();

            _draggedControlInitialPosition = dockedControlData._position;
            setControlFocused(dockedControlData);

            const uint64 dockControlHashKeyCopy = dockedControlData.getDockControlHashKey();

            dockedControlData.disconnectFromDock();
            dockDatum._dockedControlIndexShown = mint::min(dockDatum._dockedControlIndexShown, static_cast<int32>(dockDatum._dockedControlHashArray.size() - 1));
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
                    const float titleBarWidth = calculateTextWidth(title, mint::StringUtil::wcslen(title)) + 16.0f;
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
            if (_controlInteractionStates.hasFocusedControl() == true && isAncestorControlFocusedInclusiveXXX(controlData) == false)
            {
                // Focus 가 있는 Control 이 존재하지만 내가 Focus 는 아닌 경우

                mint::Window::CursorType dummyCursorType;
                ResizingMethod dummyResizingMethod;
                ResizingMask dummyResizingMask;
                const ControlData& focusedControlData = getControlData(_controlInteractionStates.getFocusedControlHashKey());
                if (ControlCommonHelpers::isInControlInteractionArea(_mouseStates.getPosition(), focusedControlData) == true 
                    || ControlCommonHelpers::isInControlBorderArea(_mouseStates.getPosition(), focusedControlData, dummyCursorType, dummyResizingMask, dummyResizingMethod) == true)
                {
                    // 마우스가 Focus Control 과 상호작용할 경우 나와는 상호작용하지 않는것으로 판단!!
                    return false;
                }
            }
            return true;
        }

        const bool GuiContext::isControlBeingDragged(const ControlData& controlData) const noexcept
        {
            if (_mouseStates.isButtonDown() == false)
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

                if (_mouseStates.isButtonDown() == true 
                    && ControlCommonHelpers::isInControlInteractionArea(_mouseStates.getPosition(), controlData) == true
                    && ControlCommonHelpers::isInControlInteractionArea(_mouseStates.getButtonDownPosition(), controlData) == true)
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

        const bool GuiContext::isControlBeingResized(const ControlData& controlData) const noexcept
        {
            if (_mouseStates.isButtonDown() == false)
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

                mint::Window::CursorType newCursorType;
                ResizingMask resizingMask;
                if (_mouseStates.isButtonDown() == true 
                    && ControlCommonHelpers::isInControlBorderArea(_mouseStates.getPosition(), controlData, newCursorType, resizingMask, _resizingMethod) == true
                    && ControlCommonHelpers::isInControlBorderArea(_mouseStates.getButtonDownPosition(), controlData, newCursorType, resizingMask, _resizingMethod) == true)
                {
                    if (controlData._resizingMask.overlaps(resizingMask) == true)
                    {
                        _resizedControlHashKey = controlData.getHashKey();
                        _isResizeBegun = true;
                        _mouseStates._cursorType = newCursorType;
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
        
        const bool GuiContext::isAncestorControlFocusedInclusiveXXX(const ControlData& controlData) const noexcept
        {
            if (_controlInteractionStates.isControlFocused(controlData) == true)
            {
                return true;
            }
            return isAncestorControlFocused(controlData);
        }

        const bool GuiContext::isAncestorControlInclusive(const ControlData& controlData, const uint64 ancestorCandidateHashKey) const noexcept
        {
            return isAncestorControlRecursiveXXX(controlData.getHashKey(), ancestorCandidateHashKey);
        }

        const bool GuiContext::isAncestorControlRecursiveXXX(const uint64 currentControlHashKey, const uint64 ancestorCandidateHashKey) const noexcept
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
            return isAncestorControlRecursiveXXX(parentControlHashKey, ancestorCandidateHashKey);
        }

        const bool GuiContext::isDescendantControlInclusive(const ControlData& controlData, const uint64 descendantCandidateHashKey) const noexcept
        {
            return ((0 == descendantCandidateHashKey) ? false : isDescendantControlRecursiveXXX(controlData.getHashKey(), descendantCandidateHashKey));
        }

        const bool GuiContext::isDescendantControlRecursiveXXX(const uint64 currentControlHashKey, const uint64 descendantCandidateHashKey) const noexcept
        {
            if (currentControlHashKey == descendantCandidateHashKey)
            {
                return true;
            }

            const ControlData& controlData = getControlData(currentControlHashKey);
            const auto& previousChildControlDataHashKeyArray = controlData.getPreviousChildControlDataHashKeyArray();
            const uint32 previousChildControlCount = previousChildControlDataHashKeyArray.size();
            for (uint32 previousChildControlIndex = 0; previousChildControlIndex < previousChildControlCount; ++previousChildControlIndex)
            {
                const uint64 previousChildControlHashKey = previousChildControlDataHashKeyArray[previousChildControlIndex];
                if (isDescendantControlRecursiveXXX(previousChildControlHashKey, descendantCandidateHashKey) == true)
                {
                    return true;
                }
            }

            return false;
        }

        const bool GuiContext::isParentControlRoot(const ControlData& controlData) const noexcept
        {
            const ControlData& parentControlData = getControlData(controlData.getParentHashKey());
            return parentControlData.isTypeOf(ControlType::ROOT);
        }

        const bool GuiContext::isAncestorControlFocused(const ControlData& controlData) const noexcept
        {
            return isAncestorControlTargetRecursiveXXX(controlData.getParentHashKey(), _controlInteractionStates.getFocusedControlHashKey());
        }

        const bool GuiContext::isAncestorControlPressed(const ControlData& controlData) const noexcept
        {
            return isAncestorControlTargetRecursiveXXX(controlData.getParentHashKey(), _controlInteractionStates.getPressedControlHashKey());
        }

        const bool GuiContext::isAncestorControlTargetRecursiveXXX(const uint64 hashKey, const uint64 targetHashKey) const noexcept
        {
            if (hashKey == 0)
            {
                return false;
            }

            if (hashKey == targetHashKey)
            {
                return true;
            }

            const uint64 parentHashKey = getControlData(hashKey).getParentHashKey();
            return isAncestorControlTargetRecursiveXXX(parentHashKey, targetHashKey);
        }

        const bool GuiContext::needToColorFocused(const ControlData& controlData) const noexcept
        {
            // #0. Child of docking control
            const bool hasDockingAncestorInclusive = hasDockingAncestorControlInclusive(controlData);
            if (hasDockingAncestorInclusive == true)
            {
                return true;
            }

            // #1. Focused
            const ControlData& closestFocusableAncestorInclusive = getClosestFocusableAncestorControlInclusive(controlData);
            const bool isFocused = _controlInteractionStates.isControlFocused(closestFocusableAncestorInclusive);
            if (isFocused == true)
            {
                return true;
            }

            // #2. Child Control Focused
            const bool isDescendantFocused = isDescendantControlFocusedInclusive(closestFocusableAncestorInclusive);
            if (isDescendantFocused == true)
            {
                return true;
            }

            // #3. Docking
            const bool isDocking = closestFocusableAncestorInclusive.isDocking();
            const ControlData& dockControlData = getControlData(closestFocusableAncestorInclusive.getDockControlHashKey());
            return (isDocking == true && (dockControlData.isRootControl() == true || _controlInteractionStates.isControlFocused(dockControlData) == true || isDescendantControlFocusedInclusive(dockControlData) == true));
        }

        const bool GuiContext::isDescendantControlFocusedInclusive(const ControlData& controlData) const noexcept
        {
            return isDescendantControlInclusive(controlData, _controlInteractionStates.getFocusedControlHashKey());
        }

        const bool GuiContext::isDescendantControlHoveredInclusive(const ControlData& controlData) const noexcept
        {
            return isDescendantControlInclusive(controlData, _controlInteractionStates.getHoveredControlHashKey());
        }

        const bool GuiContext::isDescendantControlPressedInclusive(const ControlData& controlData) const noexcept
        {
            return isDescendantControlInclusive(controlData, _controlInteractionStates.getPressedControlHashKey());
        }

        const bool GuiContext::isDescendantControlPressed(const ControlData& controlData) const noexcept
        {
            const auto& previousChildControlDataHashKeyArray = controlData.getPreviousChildControlDataHashKeyArray();
            const uint32 previousChildControlCount = previousChildControlDataHashKeyArray.size();
            for (uint32 previousChildControlIndex = 0; previousChildControlIndex < previousChildControlCount; ++previousChildControlIndex)
            {
                const uint64 previousChildControlHashKey = previousChildControlDataHashKeyArray[previousChildControlIndex];
                if (isDescendantControlRecursiveXXX(previousChildControlHashKey, _controlInteractionStates.getPressedControlHashKey()) == true)
                {
                    return true;
                }
            }
            return false;
        }

        const bool GuiContext::isDescendantControlHovered(const ControlData& controlData) const noexcept
        {
            const auto& previousChildControlDataHashKeyArray = controlData.getPreviousChildControlDataHashKeyArray();
            const uint32 previousChildControlCount = previousChildControlDataHashKeyArray.size();
            for (uint32 previousChildControlIndex = 0; previousChildControlIndex < previousChildControlCount; ++previousChildControlIndex)
            {
                const uint64 previousChildControlHashKey = previousChildControlDataHashKeyArray[previousChildControlIndex];
                if (isDescendantControlRecursiveXXX(previousChildControlHashKey, _controlInteractionStates.getHoveredControlHashKey()) == true)
                {
                    return true;
                }
            }
            return false;
        }

        const ControlData& GuiContext::getClosestFocusableAncestorControlInclusive(const ControlData& controlData) const noexcept
        {
            if (controlData.getHashKey() <= 1)
            {
                // ROOT
                return controlData;
            }

            if (controlData._isFocusable == true && controlData.isDocking() == false)
            {
                return controlData;
            }

            return getClosestFocusableAncestorControlInclusive(getControlData(controlData.getParentHashKey()));
        }

        const bool GuiContext::hasDockingAncestorControlInclusive(const ControlData& controlData) const noexcept
        {
            if (controlData.getHashKey() <= 1)
            {
                // ROOT
                return false;
            }

            if (controlData.isDocking() == true)
            {
                return true;
            }

            return hasDockingAncestorControlInclusive(getControlData(controlData.getParentHashKey()));
        }

        const float GuiContext::getMouseWheelScroll(const ControlData& scrollParentControlData) const noexcept
        {
            float result = 0.0f;
            if (0.0f != _mouseStates._mouseWheel 
                && ControlCommonHelpers::isInControlInteractionArea(_mouseStates.getPosition(), scrollParentControlData) == true)
            {
                result = _mouseStates._mouseWheel * kMouseWheelScrollScale;
                _mouseStates._mouseWheel = 0.0f;
            }
            return result;
        }

        mint::RenderingBase::ShapeFontRendererContext& GuiContext::getRendererContext(const ControlData& controlData) noexcept
        {
            return getRendererContext(controlData._rendererContextLayer);
        }

        mint::RenderingBase::ShapeFontRendererContext& GuiContext::getRendererContext(const RendererContextLayer rendererContextLayer) noexcept
        {
            return _rendererContexts[static_cast<int32>(rendererContextLayer)];
        }

        const RendererContextLayer GuiContext::getUpperRendererContextLayer(const ControlData& controlData) noexcept
        {
            const int32 index = mint::min(static_cast<int32>(controlData._rendererContextLayer) + 1, static_cast<int32>(RendererContextLayer::TopMost) - 1);
            return static_cast<RendererContextLayer>(index);
        }

        void GuiContext::render()
        {
            MINT_ASSERT("김장원", _controlStackPerFrame.empty() == true, "begin 과 end 호출 횟수가 맞지 않습니다!!!");

            _graphicDevice->getWindow()->setCursorType(_mouseStates._cursorType);

            if (_controlInteractionStates.needToShowTooltip() == true)
            {
                pushTooltipWindow(_controlInteractionStates.getTooltipText()
                    , _controlInteractionStates.getTooltipWindowPosition(getControlData(_controlInteractionStates.getTooltipParentWindowHashKey())));
            }

            // Viewport setting
            _graphicDevice->useScissorRectangles();

            // Layer 순서대로!
            for (int32 rendererContextIndex = 0; rendererContextIndex < getRendererContextLayerCount(); rendererContextIndex++)
            {
                _rendererContexts[rendererContextIndex].renderAndFlush();
            }
            
            // Viewport setting
            _graphicDevice->useFullScreenViewport();

            resetPerFrameStates();
        }

        void GuiContext::resetPerFrameStates()
        {
            _controlInteractionStates.resetPerFrameStates(_mouseStates);

            _controlStackPerFrame.clear();

            _rootControlData.clearPerFrameData();

            if (_resizedControlHashKey == 0)
            {
                _mouseStates._cursorType = mint::Window::CursorType::Arrow;
            }

            _keyCode = mint::Window::EventData::KeyCode::NONE;

            // 다음 프레임에서 가장 먼저 렌더링 되는 것!!
            processDock(_rootControlData, getRendererContext(RendererContextLayer::Background));

            if (0 < _updateScreenSizeCounter)
            {
                --_updateScreenSizeCounter;
            }
        }
    }
}

#include <stdafx.h>
#include <MintRenderingBase/Include/GUI/GUIContext.h>

#include <functional>

#include <MintContainer/Include/Hash.hpp>
#include <MintContainer/Include/String.hpp>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/BitVector.hpp>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/HashMap.hpp>

#include <MintReflection/Include/Reflection.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/GUI/ControlData.hpp>
#include <MintRenderingBase/Include/GUI/InputHelpers.hpp>

#include <MintPlatform/Include/WindowsWindow.h>
#include <MintPlatform/Include/InputContext.h>

#include <MintLibrary/Include/ScopedCPUProfiler.h>


namespace mint
{
    namespace GUI
    {
        namespace ControlCommonHelpers
        {
            MINT_INLINE const bool isInControl(const Float2& screenPosition, const Float2& controlPosition, const Float2& controlPositionOffset, const Float2& interactionSize) noexcept
            {
                const Float2 max = controlPosition + controlPositionOffset + interactionSize;
                if (controlPosition._x + controlPositionOffset._x <= screenPosition._x && screenPosition._x <= max._x &&
                    controlPosition._y + controlPositionOffset._y <= screenPosition._y && screenPosition._y <= max._y)
                {
                    return true;
                }
                return false;
            }

            MINT_INLINE const bool isInControlInteractionArea(const Float2& screenPosition, const ControlData& controlData) noexcept
            {
                if (controlData.isDockHosting() == true)
                {
                    const Float2 positionOffset{ controlData.getDockZoneSize(DockZone::LeftSide)._x, controlData.getDockZoneSize(DockZone::TopSide)._y };
                    return ControlCommonHelpers::isInControl(screenPosition, controlData._position, positionOffset, controlData.getInteractionSize());
                }
                return ControlCommonHelpers::isInControl(screenPosition, controlData._position, Float2::kZero, controlData.getInteractionSize());
            }

            MINT_INLINE const bool isInControlBorderArea(const Float2& screenPosition, const ControlData& controlData, Window::CursorType& outCursorType, ResizingMask& outResizingMask, ResizingMethod& outResizingMethod) noexcept
            {
                const Float2 extendedPosition = controlData._position - Float2(kHalfBorderThickness);
                const Float2 extendedInteractionSize = controlData.getInteractionSize() + Float2(kHalfBorderThickness * 2.0f);
                const Float2 originalMax = controlData._position + controlData.getInteractionSize();
                if (ControlCommonHelpers::isInControl(screenPosition, extendedPosition, Float2::kZero, extendedInteractionSize) == true)
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
                        outCursorType = Window::CursorType::SizeLeftTilted;
                    }
                    else if (bottomLeftOrTopRight == true)
                    {
                        outCursorType = Window::CursorType::SizeRightTilted;
                    }
                    else if (leftOrRight == true)
                    {
                        outCursorType = Window::CursorType::SizeHorz;
                    }
                    else if (topOrBottom == true)
                    {
                        outCursorType = Window::CursorType::SizeVert;
                    }

                    const bool result = (leftOrRight || topOrBottom);
                    return result;
                }
                return false;
            }
        }


        GUIContext::GUIContext(Rendering::GraphicDevice& graphicDevice)
            : _graphicDevice{ graphicDevice }
            , _fontSize{ 0.0f }
            , _rendererContexts{ _graphicDevice, _graphicDevice, _graphicDevice, _graphicDevice, _graphicDevice }
            , _updateScreenSizeCounter{ 0 }
            , _isDragBegun{ false }
            , _draggedControlID{ 0 }
            , _isResizeBegun{ false }
            , _resizedControlID{ 0 }
            , _resizingMethod{ ResizingMethod::ResizeOnly }
            , _caretBlinkIntervalMs{ 0 }
            , _wcharInput{ L'\0'}
            , _wcharInputCandidate{ L'\0'}
            , _keyCode{ Platform::KeyCode::NONE }
        {
            setNamedColor(NamedColor::NormalState, Rendering::Color(45, 47, 49));
            setNamedColor(NamedColor::HoverState, getNamedColor(NamedColor::NormalState).addedRgb(0.25f));
            setNamedColor(NamedColor::PressedState, Rendering::Color(80, 100, 120));

            setNamedColor(NamedColor::WindowFocused, Rendering::Color(3, 5, 7));
            setNamedColor(NamedColor::WindowOutOfFocus, getNamedColor(NamedColor::WindowFocused));
            setNamedColor(NamedColor::Dock, getNamedColor(NamedColor::NormalState));
            setNamedColor(NamedColor::ShownInDock, Rendering::Color(23, 25, 27));
            setNamedColor(NamedColor::HighlightColor, Rendering::Color(100, 180, 255));

            setNamedColor(NamedColor::TitleBarFocused, getNamedColor(NamedColor::WindowFocused));
            setNamedColor(NamedColor::TitleBarOutOfFocus, getNamedColor(NamedColor::WindowOutOfFocus));

            setNamedColor(NamedColor::TooltipBackground, Rendering::Color(200, 255, 220));

            setNamedColor(NamedColor::ScrollBarTrack, Rendering::Color(80, 82, 84));
            setNamedColor(NamedColor::ScrollBarThumb, getNamedColor(NamedColor::ScrollBarTrack).addedRgb(0.25f));

            setNamedColor(NamedColor::LightFont, Rendering::Color(233, 235, 237));
            setNamedColor(NamedColor::DarkFont, getNamedColor(NamedColor::LightFont).addedRgb(-0.75f));
            setNamedColor(NamedColor::ShownInDockFont, getNamedColor(NamedColor::HighlightColor));
        }

        GUIContext::~GUIContext()
        {
            __noop;
        }

        void GUIContext::initialize()
        {
            _fontSize = static_cast<float>(_graphicDevice.getFontRendererContext().getFontSize());
            
            _caretBlinkIntervalMs = _graphicDevice.getWindow().getCaretBlinkIntervalMs();

            const Rendering::FontRendererContext::FontData& fontData = _graphicDevice.getFontRendererContext().getFontData();
            for (int32 rendererContextIndex = 0; rendererContextIndex < getRendererContextLayerCount(); rendererContextIndex++)
            {
                if (_rendererContexts[rendererContextIndex].initializeFontData(fontData) == false)
                {
                    MINT_ASSERT("김장원", false, "ShapeFontRendererContext::initializeFont() 에 실패했습니다!");
                }

                _rendererContexts[rendererContextIndex].initializeShaders();
                _rendererContexts[rendererContextIndex].setUseMultipleViewports();
            }

            const Float2& windowSize = Float2(_graphicDevice.getWindowSize());
            _rootControlData = ControlData(ControlID(1), ControlID(0), GUI::ControlType::ROOT, windowSize);
            _rootControlData._option._isFocusable = false;

            updateScreenSize(windowSize);

            _controlMetaStateSet.resetPerFrame();
            resetPerFrameStates();
        }

        void GUIContext::updateScreenSize(const Float2& newScreenSize)
        {
            _clipRectFullScreen = _graphicDevice.getFullScreenClipRect();

            _rootControlData._size = newScreenSize;
            _rootControlData.setAllClipRects(_clipRectFullScreen);

            _updateScreenSizeCounter = 2;
        }

        void GUIContext::processEvent() noexcept
        {
            // 초기화
            _mouseStates.resetPerFrame();
            
            Platform::InputContext& inputContext = Platform::InputContext::getInstance();
            if (inputContext.isMousePointerMoved() == true)
            {
                _mouseStates.setPosition(inputContext.getMousePosition());
            }
            else if (inputContext.isMouseButtonPressed() == true)
            {
                const Platform::MouseState mouseState = inputContext.getMouseState();
                MINT_LOG("김장원", "Event: Mouse Pressed");

                _mouseStates.setButtonDown(mouseState._pressedButton);
                _mouseStates.setButtonDownPosition(inputContext.getMousePosition());
            }
            else if (inputContext.isMouseButtonReleased() == true)
            {
                const Platform::MouseState mouseState = inputContext.getMouseState();
                MINT_LOG("김장원", "Event: Mouse Released");

                if (_taskWhenMouseUp.isSet())
                {
                    updateDockZoneData(_taskWhenMouseUp.getUpdateDockZoneData());
                }

                _mouseStates.setButtonUp(mouseState._releasedButton);
                _mouseStates.setButtonUpPosition(inputContext.getMousePosition());
            }
            else if (inputContext.isMouseWheelScrolled() == true)
            {
                _mouseStates._mouseWheel = inputContext.getMouseWheelScroll();
            }
            else if (inputContext.isMouseButtonDoubleClicked() == true)
            {
                const Platform::MouseState mouseState = inputContext.getMouseState();
                _mouseStates.setDoubleClicked(mouseState._doubleClickedButton);
                _mouseStates.setButtonDownPosition(inputContext.getMousePosition());
            }
            else if (inputContext.isKeyInputCharacter() == true)
            {
                _wcharInputCandidate = L'\0';

                _wcharInput = inputContext.getKeyboardState()._character;
            }
            else if (inputContext.isKeyInputCharacterCandidate() == true)
            {
                _wcharInputCandidate = inputContext.getKeyboardState()._characterCandidate;
            }
            else if (inputContext.isKeyPressed() == true)
            {
                _keyCode = inputContext.getKeyboardState()._pressedKeyCode;

                if (isFocusedControlInputBox() == true && Platform::isKeyCodeAlnum(_keyCode) == true)
                {
                    _keyCode = Platform::KeyCode::NONE;
                }
            }

            // Root 컨트롤의 Interaction 이 가장 먼저 처리되어야 한다!
            processControlInteractionInternal(_rootControlData, false);
        }

        const bool GUIContext::shouldInteract(const Float2& screenPosition, const ControlData& controlData) const noexcept
        {
            const ControlType controlType = controlData.getControlType();
            const ControlData& parentControlData = getControlData(controlData.getParentID());
            if (controlType == ControlType::Window || parentControlData.hasChildWindow() == false)
            {
                return true;
            }

            // ParentControlData 가 Root 거나 Window 일 때만 여기에 온다.
            const Vector<ControlID>& parentChildControlIDs = parentControlData.getChildControlIDs();
            const uint32 parentChildControlCount = parentChildControlIDs.size();
            for (uint32 parentChildControlIndex = 0; parentChildControlIndex < parentChildControlCount; ++parentChildControlIndex)
            {
                const ControlID& parentChildControlID = parentChildControlIDs[parentChildControlIndex];
                const ControlData& parentChildControlData = getControlData(parentChildControlID);
                if (parentChildControlData.isTypeOf(ControlType::Window) == false)
                {
                    continue;
                }

                if (ControlCommonHelpers::isInControlInteractionArea(screenPosition, parentChildControlData) == true)
                {
                    return false;
                }
            }
            return true;
        }

        void GUIContext::makeTestWindow(VisibleState& inoutVisibleState)
        {
            GUI::WindowParam windowParam;
            windowParam._position = Float2(200.0f, 50.0f);
            windowParam._scrollBarType = GUI::ScrollBarType::Both;
            _controlMetaStateSet.nextSize(Float2(500.0f, 500.0f), true);
            if (beginWindow(MINT_GUI_CONTROL(L"TestWindow", windowParam, inoutVisibleState)) == true)
            {
                static VisibleState childWindowVisibleState0;
                static VisibleState childWindowVisibleState1;


                if (beginMenuBar(MINT_GUI_CONTROL(L"")) == true)
                {
                    if (beginMenuBarItem(MINT_GUI_CONTROL(L"파일")) == true)
                    {
                        if (beginMenuItem(MINT_GUI_CONTROL(L"새로 만들기")) == true)
                        {
                            if (beginMenuItem(MINT_GUI_CONTROL(L"T")) == true)
                            {
                                endMenuItem();
                            }

                            if (beginMenuItem(MINT_GUI_CONTROL(L"TT")) == true)
                            {
                                endMenuItem();
                            }

                            endMenuItem();
                        }

                        if (beginMenuItem(MINT_GUI_CONTROL(L"불러오기")) == true)
                        {
                            if (beginMenuItem(MINT_GUI_CONTROL(L"ABC")) == true)
                            {
                                if (beginMenuItem(MINT_GUI_CONTROL(L"Nest")) == true)
                                {
                                    endMenuItem();
                                }

                                endMenuItem();
                            }

                            if (beginMenuItem(MINT_GUI_CONTROL(L"DEF")) == true)
                            {
                                endMenuItem();
                            }

                            endMenuItem();
                        }

                        if (beginMenuItem(MINT_GUI_CONTROL(L"내보내기")) == true)
                        {
                            endMenuItem();
                        }

                        endMenuBarItem();
                    }

                    if (beginMenuBarItem(MINT_GUI_CONTROL(L"도움말")) == true)
                    {
                        endMenuBarItem();
                    }

                    endMenuBar();
                }

                _controlMetaStateSet.nextTooltip(L"툴팁 테스트!");

                if (beginButton(MINT_GUI_CONTROL(L"테스트")) == true)
                {
                    endButton();
                }

                if (beginCheckBox(MINT_GUI_CONTROL(L"체크박스")) == true)
                {
                    endCheckBox();
                }

                {
                    GUI::SliderParam sliderParam;
                    _controlMetaStateSet.nextSize(Float2(32.0f, 0.0f));
                    float value = 0.0f;
                    if (beginSlider(MINT_GUI_CONTROL(sliderParam, value)) == true)
                    {
                        endSlider();
                    }
                }


                _controlMetaStateSet.nextSameLine();
                if (beginButton(MINT_GUI_CONTROL(L"ChildWindow0")) == true)
                {
                    childWindowVisibleState0 = VisibleState::VisibleOpen;

                    endButton();
                }

                _controlMetaStateSet.nextSameLine();
                if (beginButton(MINT_GUI_CONTROL(L"ChildWindow1")) == true)
                {
                    childWindowVisibleState1 = VisibleState::VisibleOpen;

                    endButton();
                }

                if (beginButton(MINT_GUI_CONTROL(L"테스트2")) == true)
                {
                    endButton();
                }

                if (beginButton(MINT_GUI_CONTROL(L"테스트3")) == true)
                {
                    endButton();
                }

                static StringW textBoxContent;
                {
                    GUI::TextBoxParam textBoxParam;
                    _controlMetaStateSet.nextSize(Float2(240.0f, 24.0f));
                    textBoxParam._alignmentHorz = GUI::TextAlignmentHorz::Center;
                    if (beginTextBox(MINT_GUI_CONTROL(textBoxParam, textBoxContent)) == true)
                    {
                        endTextBox();
                    }
                }

                GUI::ListViewParam listViewParam;
                int16 listViewSelectedItemIndex = 0;
                if (beginListView(MINT_GUI_CONTROL(listViewSelectedItemIndex, listViewParam)) == true)
                {
                    makeListItem(MINT_GUI_CONTROL(L"아이템1"));
                    makeListItem(MINT_GUI_CONTROL(L"아이템2"));
                    makeListItem(MINT_GUI_CONTROL(L"아이템3"));
                    makeListItem(MINT_GUI_CONTROL(L"아이템4"));
                    makeListItem(MINT_GUI_CONTROL(L"아이템5"));
                    makeListItem(MINT_GUI_CONTROL(L"아이템6"));

                    endListView();
                }

                {
                    GUI::WindowParam testWindowParam;
                    _controlMetaStateSet.nextSize(Float2(200.0f, 240.0f), true);
                    testWindowParam._scrollBarType = GUI::ScrollBarType::Both;
                    testWindowParam._initialDockZone = GUI::DockZone::BottomSide;
                    if (beginWindow(MINT_GUI_CONTROL(L"1ST", testWindowParam, childWindowVisibleState0)))
                    {
                        if (beginButton(MINT_GUI_CONTROL(L"테스트!!")) == true)
                        {
                            endButton();
                        }

                        endWindow();
                    }
                }

                {
                    GUI::WindowParam testWindowParam;
                    _controlMetaStateSet.nextSize(Float2(100.0f, 100.0f));
                    testWindowParam._position._x = 10.0f;
                    testWindowParam._position._y = 60.0f;
                    testWindowParam._initialDockZone = GUI::DockZone::BottomSide;
                    if (beginWindow(MINT_GUI_CONTROL(L"2NDDD", testWindowParam, childWindowVisibleState1)))
                    {
                        if (beginButton(MINT_GUI_CONTROL(L"YEAH")) == true)
                        {
                            endButton();
                        }

                        endWindow();
                    }
                }

                endWindow();
            }
        }

        void GUIContext::makeTestDockedWindow(VisibleState& inoutVisibleState)
        {
            GUI::WindowParam windowParam;
            _controlMetaStateSet.nextSize(Float2(300.0f, 400.0f), true);
            windowParam._position = Float2(20.0f, 50.0f);
            windowParam._initialDockZone = GUI::DockZone::RightSide;
            windowParam._initialDockingSize._x = 240.0f;
            if (beginWindow(MINT_GUI_CONTROL(L"TestDockedWindow", windowParam, inoutVisibleState)) == true)
            {
                if (beginButton(MINT_GUI_CONTROL(L"버튼이요")) == true)
                {
                    bool a = true;
                    endButton();
                }

                _controlMetaStateSet.nextSameLine();

                _controlMetaStateSet.nextTooltip(L"Button B Toolip!!");

                if (beginButton(MINT_GUI_CONTROL(L"Button B")) == true)
                {
                    endButton();
                }

                if (beginButton(MINT_GUI_CONTROL(L"Another")) == true)
                {
                    endButton();
                }

                makeLabel(MINT_GUI_CONTROL(L"A label!"));

                _controlMetaStateSet.nextSameLine();

                if (beginButton(MINT_GUI_CONTROL(L"Fourth")) == true)
                {
                    endButton();
                }

                endWindow();
            }
        }

        void GUIContext::makeDebugControlDataViewer(VisibleState& inoutVisibleState)
        {
            GUI::WindowParam windowParam;
            _controlMetaStateSet.nextSize(Float2(300.0f, 400.0f), true);
            windowParam._position = Float2(20.0f, 50.0f);
            if (beginWindow(MINT_GUI_CONTROL(L"ControlData Viewer", windowParam, inoutVisibleState)) == true)
            {
                if (isValidControl(_viewerTargetControlID) == true)
                {
                    ScopeStringW<300> buffer;
                    const ControlData& controlData = getControlData(_viewerTargetControlID);
                    
                    formatString(buffer, L"Control ID Map Size: %u", _controlIDMap.size());
                    makeLabel(MINT_GUI_CONTROL(buffer.c_str()));

                    formatString(buffer, L"ID: %llX", controlData.getID());
                    makeLabel(MINT_GUI_CONTROL(buffer.c_str()));
                    
                    formatString(buffer, L"Control Type: (%s)", getControlTypeWideString(controlData.getControlType()));
                    makeLabel(MINT_GUI_CONTROL(buffer.c_str()));
                    
                    formatString(buffer, L"Text: %s", controlData._text.c_str());
                    makeLabel(MINT_GUI_CONTROL(buffer.c_str()));

                    formatString(buffer, L"Position: (%f, %f)", controlData._position._x, controlData._position._y);
                    makeLabel(MINT_GUI_CONTROL(buffer.c_str()));

                    formatString(buffer, L"InteractionSize: (%f, %f)", controlData.getInteractionSize()._x, controlData.getInteractionSize()._y);
                    makeLabel(MINT_GUI_CONTROL(buffer.c_str()));

                    makeFromReflectionClass(MINT_GUI_CONTROL(controlData.getReflectionData(), &controlData));
                }
                
                endWindow();
            }
        }

        void GUIContext::makeFromReflectionClass(const char* const file, const int line, const ReflectionData& reflectionData, const void* const reflectionClass)
        {
            ScopeStringA<300> bufferA;
            ScopeStringW<300> bufferW;

            const uint32 memberCount = reflectionData._memberTypeDatas.size();
            for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
            {
                TypeBaseData* const memberTypeData = reflectionData._memberTypeDatas[memberIndex];
                const char* const member = reinterpret_cast<const char*>(reflectionClass) + memberTypeData->_offset;
                if (memberTypeData->_typeName == "Float2")
                {
                    const Float2& memberCasted = *reinterpret_cast<const Float2*>(member);
                    formatString(bufferA, "%s: (%f, %f)", memberTypeData->_declarationName.c_str(), memberCasted._x, memberCasted._y);
                    
                    StringUtil::convertScopeStringAToScopeStringW(bufferA, bufferW);
                    makeLabel(file, line, bufferW.c_str());
                }
                else if (memberTypeData->_typeName == "uint64")
                {
                    const uint64 memberCasted = *reinterpret_cast<const uint64*>(member);
                    formatString(bufferA, "%s: %llu", memberTypeData->_declarationName.c_str(), memberCasted);
                    
                    StringUtil::convertScopeStringAToScopeStringW(bufferA, bufferW);
                    makeLabel(file, line, bufferW.c_str());
                }
                else if (memberTypeData->_typeName == "StringW")
                {
                    const StringW& memberCasted = *reinterpret_cast<const StringW*>(member);
                    ScopeStringW<300> bufferWTemp;
                    bufferA = memberTypeData->_declarationName.c_str();
                    StringUtil::convertScopeStringAToScopeStringW(bufferA, bufferWTemp);
                    formatString(bufferW, L"%s: %s", bufferWTemp.c_str(), memberCasted.c_str());
                    makeLabel(file, line, bufferW.c_str());
                }
                else
                {
                    continue;
                }
            }
        }

        const bool GUIContext::beginWindow(const char* const file, const int line, const wchar_t* const title, const WindowParam& windowParam, VisibleState& inoutVisibleState)
        {
            static constexpr ControlType controlType = ControlType::Window;
            
            _controlMetaStateSet.nextOffAutoPosition();

            const ControlID windowControlID = issueControlID(file, line, controlType, title);
            
            ControlData& windowControlData = accessControlData(windowControlID);
            windowControlData._dockContext._dockingControlType = DockingControlType::DockerDock;
            windowControlData._option._isFocusable = true;
            windowControlData._controlValue._windowData._titleBarThickness = kTitleBarBaseThickness;
            if (windowControlData.updateVisibleState(inoutVisibleState) == true && windowControlData.isControlVisible() == true)
            {
                setControlFocused(windowControlData);
            }
            dockWindowOnceInitially(windowControlData, windowParam._initialDockZone, windowParam._initialDockingSize);

            PrepareControlDataParam prepareControlDataParam;
            {
                const float titleWidth = computeTextWidth(title, StringUtil::length(title));
                prepareControlDataParam._initialResizingMask.setAllTrue();
                prepareControlDataParam._desiredPositionInParent = windowParam._position;
                prepareControlDataParam._innerPadding = kWindowInnerPadding;
                prepareControlDataParam._minSize._x = titleWidth + kTitleBarInnerPadding.horz() + kDefaultRoundButtonRadius * 2.0f;
                prepareControlDataParam._minSize._y = windowControlData._size._y - windowControlData.computeInnerDisplaySize()._y + 16.0f;
                prepareControlDataParam._alwaysResetDisplaySize = false; // 중요!!!
                prepareControlDataParam._alwaysResetPosition = false;
                prepareControlDataParam._clipRectUsage = ClipRectUsage::Own;
                prepareControlDataParam._deltaInteractionSizeByDock._x = -windowControlData.getHorzDockZoneSize();
                prepareControlDataParam._deltaInteractionSizeByDock._y = -windowControlData.getVertDockZoneSize();
            }
            prepareControlData(windowControlData, prepareControlDataParam);

            updateWindowPositionByParentWindow(windowControlData);
            
            updateDockingWindowDisplay(windowControlData);

            const bool needToProcessControl = needToProcessWindowControl(windowControlData);
            {
                Rendering::Color finalBackgroundColor;
                const bool isFocused = (needToProcessControl == true) 
                    ? processFocusControl(windowControlData, getNamedColor(NamedColor::WindowFocused), getNamedColor(NamedColor::WindowOutOfFocus), finalBackgroundColor)
                    : false;
                
                // Viewport & Scissor rectangle
                {
                    const ControlData& parentControlData = getControlData(windowControlData.getParentID());
                    const bool isParentAlsoWindow = parentControlData.isTypeOf(ControlType::Window);
                    {
                        Rect clipRectForMe = windowControlData.getRect();
                        if (isParentAlsoWindow == true)
                        {
                            clipRectForMe.clipBy(parentControlData.getClipRects()._forDocks);

                            if (windowControlData.isDocking() == true)
                            {
                                windowControlData.setClipRectForMe(parentControlData.getClipRects()._forDocks);
                            }
                            else
                            {
                                windowControlData.setClipRectForMe(parentControlData.getClipRects()._forMe);
                            }
                        }
                        else
                        {
                            windowControlData.setClipRectForMe(clipRectForMe);
                        }
                    }
                    {
                        Rect clipRectForDocks = windowControlData.getControlPaddedRect();
                        clipRectForDocks.top() += static_cast<LONG>(kTitleBarBaseThickness);
                        if (isParentAlsoWindow == true)
                        {
                            clipRectForDocks.clipBy(parentControlData.getClipRects()._forDocks);
                        }
                        windowControlData.setClipRectForDocks(clipRectForDocks);
                    }
                    {
                        const bool hasScrollBarVert = windowControlData._controlValue._commonData.isScrollBarEnabled(ScrollBarType::Vert);
                        const bool hasScrollBarHorz = windowControlData._controlValue._commonData.isScrollBarEnabled(ScrollBarType::Horz);

                        Rect clipRectForChildren = windowControlData.getControlPaddedRect();
                        clipRectForChildren.top() += static_cast<LONG>(windowControlData._controlValue._windowData._titleBarThickness + windowControlData.getMenuBarThickness()._y + windowControlData.getDockZoneSize(DockZone::TopSide)._y);
                        clipRectForChildren.left() += static_cast<LONG>(windowControlData.getDockZoneSize(DockZone::LeftSide)._x);
                        clipRectForChildren.right() -= static_cast<LONG>(((hasScrollBarVert == true) ? kScrollBarThickness : 0.0f) + windowControlData.getDockZoneSize(DockZone::RightSide)._x);
                        clipRectForChildren.bottom() -= static_cast<LONG>(((hasScrollBarHorz == true) ? kScrollBarThickness : 0.0f) + windowControlData.getDockZoneSize(DockZone::BottomSide)._y);
                        if (isParentAlsoWindow == true)
                        {
                            clipRectForChildren.clipBy(parentControlData.getClipRects()._forMe);
                        }
                        windowControlData.setClipRectForChildren(clipRectForChildren);
                    }
                }

                if (needToProcessControl == true)
                {
                    const bool isAncestorFocused = isAncestorControlFocused(windowControlData);
                    windowControlData._rendererContextLayer = (isFocused || isAncestorFocused) ? RendererContextLayer::Foreground : RendererContextLayer::Background;
                    
                    Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(windowControlData);
                    rendererContext.setClipRect(windowControlData.getClipRects()._forMe);

                    const Float4& windowCenterPosition = windowControlData.getControlCenterPosition();
                    rendererContext.setColor(finalBackgroundColor);
                    rendererContext.setPosition(windowCenterPosition + Float4(0, windowControlData._controlValue._windowData._titleBarThickness * 0.5f, 0, 0));
                    if (windowControlData.isDocking() == true)
                    {
                        Rendering::Color inDockColor = getNamedColor(NamedColor::ShownInDock);
                        inDockColor.a(finalBackgroundColor.a());
                        rendererContext.setColor(inDockColor);
                        rendererContext.drawRectangle(windowControlData._size - Float2(0, windowControlData._controlValue._windowData._titleBarThickness), 0.0f, 0.0f);
                    }
                    else
                    {
                        rendererContext.drawHalfRoundedRectangle(windowControlData._size - Float2(0, windowControlData._controlValue._windowData._titleBarThickness), (kDefaultRoundnessInPixel * 2.0f / windowControlData._size.minElement()), 0.0f);
                    }

                    processDock(windowControlData, rendererContext);
                    _controlStackPerFrame.push_back(ControlStackData(windowControlData));
                }
            }
            
            if (windowControlData.isControlVisible() == true)
            {
                {
                    _controlMetaStateSet.nextOffAutoPosition(); // 중요

                    const Float2 titleBarSize = Float2(windowControlData._size._x, windowControlData._controlValue._windowData._titleBarThickness);
                    beginTitleBar(windowControlData.getID(), title, titleBarSize, kTitleBarInnerPadding, inoutVisibleState);
                    endTitleBar();
                }

                if (windowParam._scrollBarType != ScrollBarType::None)
                {
                    makeScrollBar(windowControlData.getID(), windowParam._scrollBarType);
                }
            }
            
            return needToProcessControl;
        }

        void GUIContext::dockWindowOnceInitially(ControlData& windowControlData, const DockZone dockZone, const Float2& initialDockingSize)
        {
            MINT_ASSERT("김장원", windowControlData.isTypeOf(ControlType::Window) == true, "Window 가 아니면 사용하면 안 됩니다!");

            // Initial docking
            if (windowControlData._updateCount == 2 && dockZone != DockZone::COUNT)
            {
                windowControlData._dockContext._lastDockZoneCandidate = dockZone;

                ControlData& parentControlData = accessControlData(windowControlData.getParentID());
                if (dockZone == DockZone::LeftSide || dockZone == DockZone::RightSide)
                {
                    parentControlData.setDockZoneSize(dockZone, Float2(initialDockingSize._x, parentControlData._size._y));
                }
                else
                {
                    parentControlData.setDockZoneSize(dockZone, Float2(parentControlData._size._x, initialDockingSize._y));
                }

                dock(windowControlData.getID(), parentControlData.getID());
            }
        }

        void GUIContext::updateWindowPositionByParentWindow(ControlData& windowControlData) noexcept
        {
            MINT_ASSERT("김장원", windowControlData.isTypeOf(ControlType::Window) == true, "Window 가 아니면 사용하면 안 됩니다!");

            const ControlData& parentControlData = getControlData(windowControlData.getParentID());
            const bool isParentAlsoWindow = parentControlData.isTypeOf(ControlType::Window);
            if (isParentAlsoWindow == true)
            {
                // 부모 윈도우가 이동한 만큼 내 위치도 이동!
                windowControlData._position += parentControlData._perFrameData._deltaPosition;

                // 계층 가장 아래 Window 까지 전파되도록
                windowControlData._perFrameData._deltaPosition = parentControlData._perFrameData._deltaPosition;
            }
        }

        void GUIContext::updateDockingWindowDisplay(ControlData& windowControlData) noexcept
        {
            MINT_ASSERT("김장원", windowControlData.isTypeOf(ControlType::Window) == true, "Window 가 아니면 사용하면 안 됩니다!");

            if (windowControlData.isDocking() == true)
            {
                const ControlData& dockControlData = getControlData(windowControlData.getDockControlID());
                if (_updateScreenSizeCounter > 0)
                {
                    windowControlData._position = dockControlData.getDockZonePosition(windowControlData._dockContext._lastDockZone);
                    windowControlData._size = dockControlData.getDockZoneSize(windowControlData._dockContext._lastDockZone);
                }
            }
        }

        const bool GUIContext::needToProcessWindowControl(const ControlData& windowControlData) const noexcept
        {
            MINT_ASSERT("김장원", windowControlData.isTypeOf(ControlType::Window) == true, "Window 가 아니면 사용하면 안 됩니다!");

            const bool isDocking = windowControlData.isDocking();
            bool needToProcessControl = windowControlData.isControlVisible();
            if (isDocking == true)
            {
                const ControlData& dockControlData = getControlData(windowControlData.getDockControlID());
                const bool isFocusedDocker = dockControlData.isFocusedDocker(windowControlData);
                needToProcessControl &= (isDocking && isFocusedDocker);
            }
            return needToProcessControl;
        }

        const bool GUIContext::beginButton(const char* const file, const int line, const wchar_t* const text)
        {
            static constexpr ControlType controlType = ControlType::Button;
            
            const ControlID controlID = issueControlID(file, line, controlType, text);
            
            ControlData& controlData = accessControlData(controlID);
            PrepareControlDataParam prepareControlDataParam;
            {
                const float textWidth = computeTextWidth(text, StringUtil::length(text));
                prepareControlDataParam._autoComputedDisplaySize = Float2(textWidth + 24, _fontSize + 12);
            }
            prepareControlData(controlData, prepareControlDataParam);
        
            Rendering::Color finalBackgroundColor;
            const bool isClicked = processClickControl(controlData, getNamedColor(NamedColor::NormalState), getNamedColor(NamedColor::HoverState), getNamedColor(NamedColor::PressedState), finalBackgroundColor);
            
            const Float4& controlCenterPosition = controlData.getControlCenterPosition();
            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            rendererContext.setClipRect(controlData.getClipRects()._forMe);
            rendererContext.setColor(finalBackgroundColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(controlData._size, (kDefaultRoundnessInPixel * 2.0f / controlData._size.minElement()), 0.0f, 0.0f);

            rendererContext.setTextColor(getNamedColor(NamedColor::LightFont) * Rendering::Color(1.0f, 1.0f, 1.0f, finalBackgroundColor.a()));
            rendererContext.drawDynamicText(text, controlCenterPosition, 
                Rendering::FontRenderingOption(Rendering::TextRenderDirectionHorz::Centered, Rendering::TextRenderDirectionVert::Centered, kFontScaleB));

            if (isClicked == true)
            {
                _controlStackPerFrame.push_back(ControlStackData(controlData));
            }
            return isClicked;
        }

        const bool GUIContext::beginCheckBox(const char* const file, const int line, const wchar_t* const text, bool* const outIsChecked)
        {
            static constexpr ControlType controlType = ControlType::CheckBox;

            const ControlID controlID = issueControlID(file, line, controlType, text);
            
            ControlData& controlData = accessControlData(controlID);
            PrepareControlDataParam prepareControlDataParam;
            {
                prepareControlDataParam._autoComputedDisplaySize = kCheckBoxSize;
            }
            prepareControlData(controlData, prepareControlDataParam);

            Rendering::Color finalBackgroundColor;
            const bool isClicked = processToggleControl(controlData, getNamedColor(NamedColor::NormalState), getNamedColor(NamedColor::NormalState), getNamedColor(NamedColor::HighlightColor), finalBackgroundColor);
            const bool isChecked = controlData._controlValue._booleanData.get();
            if (outIsChecked != nullptr)
            {
                *outIsChecked = isChecked;
            }

            const Float4& controlCenterPosition = controlData.getControlCenterPosition();
            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            rendererContext.setClipRect(controlData.getClipRects()._forMe);
            rendererContext.setColor(finalBackgroundColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(controlData._size, (kDefaultRoundnessInPixel / controlData._size.minElement()), 0.0f, 0.0f);

            if (isChecked == true)
            {
                Float2 p0 = Float2(controlCenterPosition._x - 1.0f, controlCenterPosition._y + 4.0f);
                rendererContext.setColor(getNamedColor(NamedColor::LightFont));
                rendererContext.drawLine(p0, p0 + Float2(-4.0f, -5.0f), 2.0f);
                rendererContext.drawLine(p0, p0 + Float2(+7.0f, -8.0f), 2.0f);
            }

            rendererContext.setTextColor(getNamedColor(NamedColor::LightFont) * Rendering::Color(1.0f, 1.0f, 1.0f, finalBackgroundColor.a()));
            rendererContext.drawDynamicText(text, controlCenterPosition + Float4(kCheckBoxSize._x * 0.75f, 0.0f, 0.0f, 0.0f), 
                Rendering::FontRenderingOption(Rendering::TextRenderDirectionHorz::Rightward, Rendering::TextRenderDirectionVert::Centered, kFontScaleB));

            if (isClicked == true)
            {
                _controlStackPerFrame.push_back(ControlStackData(controlData));
            }
            return isClicked;
        }

        void GUIContext::makeLabel(const char* const file, const int line, const wchar_t* const text, const LabelParam& labelParam)
        {
            static constexpr ControlType controlType = ControlType::Label;

            const ControlID controlID = issueControlID(file, line, controlType, text);
            
            ControlData& controlData = accessControlData(controlID);
            PrepareControlDataParam prepareControlDataParam;
            {
                const float textWidth = computeTextWidth(text, StringUtil::length(text));
                prepareControlDataParam._autoComputedDisplaySize = Float2(textWidth + labelParam._paddingForAutoSize._x, _fontSize + labelParam._paddingForAutoSize._y);
                prepareControlDataParam._offset = labelParam._common._offset;
            }
            prepareControlData(controlData, prepareControlDataParam);
            
            Rendering::Color colorWithAlpha = Rendering::Color(255, 255, 255);
            processShowOnlyControl(controlData, colorWithAlpha);

            const Float4& controlCenterPosition = controlData.getControlCenterPosition();
            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            rendererContext.setClipRect(controlData.getClipRects()._forMe);
            rendererContext.setColor(labelParam._common._backgroundColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRectangle(controlData._size, 0.0f, 0.0f);

            rendererContext.setTextColor((labelParam._common._fontColor.isTransparent() == true) ? getNamedColor(NamedColor::LightFont) * colorWithAlpha : labelParam._common._fontColor);
            const Float4 textPosition = labelComputeTextPosition(labelParam, controlData);
            const Rendering::FontRenderingOption fontRenderingOption = labelGetFontRenderingOption(labelParam, controlData);
            rendererContext.drawDynamicText(text, textPosition, fontRenderingOption);
        }

        Float4 GUIContext::labelComputeTextPosition(const LabelParam& labelParam, const ControlData& labelControlData) const noexcept
        {
            MINT_ASSERT("김장원", labelControlData.isTypeOf(ControlType::Label) == true, "Label 이 아니면 사용하면 안 됩니다!");

            Float4 textPosition = labelControlData.getControlCenterPosition();
            if (labelParam._alignmentHorz != TextAlignmentHorz::Center)
            {
                if (labelParam._alignmentHorz == TextAlignmentHorz::Left)
                {
                    textPosition._x = labelControlData._position._x;
                }
                else
                {
                    textPosition._x = labelControlData._position._x + labelControlData._size._x;
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
                    textPosition._y = labelControlData._position._y + labelControlData._size._y;
                }
            }
            return textPosition;
        }

        Rendering::FontRenderingOption GUIContext::labelGetFontRenderingOption(const LabelParam& labelParam, const ControlData& labelControlData) const noexcept
        {
            MINT_ASSERT("김장원", labelControlData.isTypeOf(ControlType::Label) == true, "Label 이 아니면 사용하면 안 됩니다!");

            Rendering::TextRenderDirectionHorz textRenderDirectionHorz = Rendering::TextRenderDirectionHorz::Centered;
            Rendering::TextRenderDirectionVert textRenderDirectionVert = Rendering::TextRenderDirectionVert::Centered;
            if (labelParam._alignmentHorz != TextAlignmentHorz::Center)
            {
                if (labelParam._alignmentHorz == TextAlignmentHorz::Left)
                {
                    textRenderDirectionHorz = Rendering::TextRenderDirectionHorz::Rightward;
                }
                else
                {
                    textRenderDirectionHorz = Rendering::TextRenderDirectionHorz::Leftward;
                }
            }
            if (labelParam._alignmentVert != TextAlignmentVert::Middle)
            {
                if (labelParam._alignmentVert == TextAlignmentVert::Top)
                {
                    textRenderDirectionVert = Rendering::TextRenderDirectionVert::Downward;
                }
                else
                {
                    textRenderDirectionVert = Rendering::TextRenderDirectionVert::Upward;
                }
            }
            return Rendering::FontRenderingOption(textRenderDirectionHorz, textRenderDirectionVert, kFontScaleB);
        }

        const bool GUIContext::beginSlider(const char* const file, const int line, const SliderParam& sliderParam, float& outValue)
        {
            static constexpr ControlType trackControlType = ControlType::Slider;
            static constexpr ControlType thumbControlType = ControlType::SliderThumb;

            const ControlID trackControlID = issueControlID(file, line, trackControlType, nullptr);
            const ControlID thumbControlID = issueControlID(file, line, thumbControlType, nullptr);
            
            ControlData& trackControlData = accessControlData(trackControlID);
            PrepareControlDataParam prepareControlDataParamForTrack;
            {
                prepareControlDataParamForTrack._autoComputedDisplaySize = Float2(0.0f, kSliderThumbRadius * 2.0f);
            }
            prepareControlData(trackControlData, prepareControlDataParamForTrack);
            
            Rendering::Color trackColor = getNamedColor(NamedColor::HoverState);
            processShowOnlyControl(trackControlData, trackColor, false);

            bool isChanged = false;
            {
                _controlMetaStateSet.nextOffAutoPosition();

                const float sliderValidLength = trackControlData._size._x - kSliderThumbRadius * 2.0f;
                ControlData& thumbControlData = accessControlData(thumbControlID);
                thumbControlData._position._x = trackControlData._position._x + trackControlData._controlValue._thumbData._thumbAt * sliderValidLength;
                thumbControlData._position._y = trackControlData._position._y + trackControlData._size._y * 0.5f - thumbControlData._size._y * 0.5f;
                thumbControlData._option._isDraggable = true;
                thumbControlData._positionConstraintsForDragging.top(thumbControlData._position._y);
                thumbControlData._positionConstraintsForDragging.bottom(thumbControlData._positionConstraintsForDragging.top());
                thumbControlData._positionConstraintsForDragging.left(trackControlData._position._x);
                thumbControlData._positionConstraintsForDragging.right(thumbControlData._positionConstraintsForDragging.left() + sliderValidLength);
                PrepareControlDataParam prepareControlDataParamForThumb;
                {
                    const ControlData& parentWindowControlData = getParentWindowControlData(trackControlData);

                    prepareControlDataParamForThumb._autoComputedDisplaySize._x = kSliderThumbRadius * 2.0f;
                    prepareControlDataParamForThumb._autoComputedDisplaySize._y = kSliderThumbRadius * 2.0f;
                    prepareControlDataParamForThumb._alwaysResetPosition = false;
                    prepareControlDataParamForThumb._desiredPositionInParent = trackControlData._position - parentWindowControlData._position;
                }
                prepareControlData(thumbControlData, prepareControlDataParamForThumb);
                
                Rendering::Color thumbColor;
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

        void GUIContext::sliderDrawTrack(const SliderParam& sliderParam, const ControlData& trackControlData, const Rendering::Color& trackColor) noexcept
        {
            MINT_ASSERT("김장원", trackControlData.isTypeOf(ControlType::Slider) == true, "Slider (Track) 이 아니면 사용하면 안 됩니다!");

            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(trackControlData);
            const float trackRadius = kSliderTrackThicknes * 0.5f;
            const float trackRectLength = trackControlData._size._x - trackRadius * 2.0f;

            const float thumbAt = trackControlData._controlValue._thumbData._thumbAt;
            const float sliderValidLength = trackControlData._size._x - kSliderThumbRadius * 2.0f;
            const float trackRectLeftLength = thumbAt * sliderValidLength;
            const float trackRectRightLength = trackRectLength - trackRectLeftLength;

            const Float4& trackCenterPosition = trackControlData.getControlCenterPosition();
            Float4 trackRenderPosition = trackCenterPosition - Float4(trackRectLength * 0.5f, 0.0f, 0.0f, 0.0f);

            // Left(or Upper) half circle
            rendererContext.setClipRect(trackControlData.getClipRects()._forMe);
            rendererContext.setColor(getNamedColor(NamedColor::HighlightColor));
            rendererContext.setPosition(trackRenderPosition);
            rendererContext.drawHalfCircle(trackRadius, +Math::kPiOverTwo);

            // Left rect
            trackRenderPosition._x += trackRectLeftLength * 0.5f;
            rendererContext.setPosition(trackRenderPosition);
            rendererContext.drawRectangle(Float2(trackRectLeftLength, kSliderTrackThicknes), 0.0f, 0.0f);
            trackRenderPosition._x += trackRectLeftLength * 0.5f;

            // Right rect
            rendererContext.setColor(trackColor);
            trackRenderPosition._x += trackRectRightLength * 0.5f;
            rendererContext.setPosition(trackRenderPosition);
            rendererContext.drawRectangle(Float2(trackRectRightLength, kSliderTrackThicknes), 0.0f, 0.0f);
            trackRenderPosition._x += trackRectRightLength * 0.5f;

            // Right(or Lower) half circle
            rendererContext.setPosition(trackRenderPosition);
            rendererContext.drawHalfCircle(trackRadius, -Math::kPiOverTwo);
        }

        void GUIContext::sliderDrawThumb(const SliderParam& sliderParam, const ControlData& thumbControlData, const Rendering::Color& thumbColor) noexcept
        {
            MINT_ASSERT("김장원", thumbControlData.isTypeOf(ControlType::SliderThumb) == true, "Slider Thumb 이 아니면 사용하면 안 됩니다!");

            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(thumbControlData);
            const Float4& thumbCenterPosition = thumbControlData.getControlCenterPosition();
            rendererContext.setPosition(thumbCenterPosition);
            rendererContext.setColor(Rendering::Color::kWhite.scaledA(thumbColor.a()));
            rendererContext.drawCircle(kSliderThumbRadius);

            rendererContext.setColor(thumbColor);
            rendererContext.drawCircle(kSliderThumbRadius - 2.0f);
        }

        const bool GUIContext::beginTextBox(const char* const file, const int line, const TextBoxParam& textBoxParam, StringW& outText)
        {
            static constexpr ControlType controlType = ControlType::TextBox;
            
            const ControlID controlID = issueControlID(file, line, controlType, nullptr);
            
            ControlData& controlData = accessControlData(controlID);
            controlData._option._isFocusable = true;
            PrepareControlDataParam prepareControlDataParam;
            prepareControlDataParam._offset = textBoxParam._common._offset;
            prepareControlDataParam._autoComputedDisplaySize._y = _fontSize;
            prepareControlData(controlData, prepareControlDataParam);
            
            Rendering::Color finalBackgroundColor;
            const bool wasFocused = _controlInteractionStateSet.isControlFocused(controlData);
            const bool isFocused = processFocusControl(controlData, textBoxParam._common._backgroundColor, textBoxParam._common._backgroundColor.addedRgb(-0.125f), finalBackgroundColor);
            {
                const ControlData& parentControlData = getControlData(controlData.getParentID());
                Rect clipRectForMe = controlData.getRect();
                clipRectForMe.clipBy(parentControlData.getClipRects()._forChildren);
                controlData.setClipRectForMe(clipRectForMe);
            }

            const wchar_t inputCandidate[2]{ _wcharInputCandidate, L'\0' };
            const float inputCandidateWidth = ((isFocused == true) && (_wcharInputCandidate >= 32)) ? computeTextWidth(inputCandidate, 1) : 0.0f;
            const uint16 textLength = static_cast<uint16>(outText.length());
            Float4 textRenderOffset;
            if (controlData._controlValue._textBoxData._textDisplayOffset == 0)
            {
                const float fullTextWidth = computeTextWidth(outText.c_str(), textLength);
                if (textBoxParam._alignmentHorz == TextAlignmentHorz::Center)
                {
                    textRenderOffset._x = (controlData._size._x - fullTextWidth - inputCandidateWidth) * 0.5f;
                }
                else if (textBoxParam._alignmentHorz == TextAlignmentHorz::Right)
                {
                    textRenderOffset._x = controlData._size._x - fullTextWidth - inputCandidateWidth;
                }
            }

            // Input 처리
            if (isFocused == true)
            {
                textBoxProcessInput(wasFocused, textBoxParam._textInputMode, controlData, textRenderOffset, outText);
            }

            // Caret 의 렌더링 위치가 TextBox 를 벗어나는 경우 처리!!
            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            GUI::InputBoxHelpers::updateTextDisplayOffset(rendererContext, textLength, kTextBoxBackSpaceStride, controlData, inputCandidateWidth);

            // Box 렌더링
            const Float4& controlCenterPosition = controlData.getControlCenterPosition();
            rendererContext.setClipRect(controlData.getClipRects()._forMe);
            rendererContext.setColor(finalBackgroundColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(controlData._size, (textBoxParam._roundnessInPixel / controlData._size.minElement()), 0.0f, 0.0f);

            // Text, Caret, Selection 렌더링
            const bool needToRenderInputCandidate = (isFocused == true && _wcharInputCandidate >= 32);
            if (needToRenderInputCandidate == true)
            {
                GUI::InputBoxHelpers::drawTextWithInputCandidate(rendererContext, textBoxParam._common, textRenderOffset, isFocused, _fontSize, _wcharInputCandidate, controlData, outText);
            }
            else
            {
                GUI::InputBoxHelpers::drawTextWithoutInputCandidate(rendererContext, textBoxParam._common, textRenderOffset, isFocused, _fontSize, true, controlData, outText);
            }
            GUI::InputBoxHelpers::drawSelection(rendererContext, textRenderOffset, isFocused, _fontSize, getNamedColor(NamedColor::HighlightColor).addedRgb(-0.375f).scaledA(0.25f), controlData, outText);

            return false;
        }
        
        void GUIContext::textBoxProcessInput(const bool wasControlFocused, const TextInputMode textInputMode, ControlData& controlData, Float4& textRenderOffset, StringW& outText) noexcept
        {
            GUI::InputBoxHelpers::updateCaretState(_caretBlinkIntervalMs, controlData);

            TextBoxProcessInputResult result;
            if (_mouseStates.isButtonDown(Platform::MouseButton::Left) == true || _mouseStates.isButtonDownThisFrame(Platform::MouseButton::Left) == true)
            {
                GUI::InputBoxHelpers::processDefaultMouseInputs(_mouseStates, getRendererContext(controlData), controlData, textRenderOffset, outText, result);
            }
            else if (_mouseStates.isDoubleClicked(Platform::MouseButton::Left) == true)
            {
                GUI::InputBoxHelpers::selectAll(controlData, outText);
            }
            else
            {
                const Window::IWindow& window = _graphicDevice.getWindow();
                GUI::InputBoxHelpers::processDefaultKeyboardInputs(&window, getRendererContext(controlData), controlData, textInputMode, kTextBoxMaxTextLength, _keyCode,
                    _wcharInput, _wcharInputCandidate, textRenderOffset, outText, result);
            }

            if (wasControlFocused == false)
            {
                GUI::InputBoxHelpers::refreshCaret(controlData);
            }

            if (result._clearKeyCode == true)
            {
                _keyCode = Platform::KeyCode::NONE;
            }
            if (result._clearWcharInput == true)
            {
                _wcharInput = L'\0';
            }
        }

        const bool GUIContext::beginValueSlider(const char* const file, const int line, const CommonControlParam& commonControlParam, const float roundnessInPixel, const int32 decimalDigits, float& value)
        {
            static constexpr ControlType controlType = ControlType::ValueSlider;

            const ControlID controlID = issueControlID(file, line, controlType, nullptr);
            
            ControlData& controlData = accessControlData(controlID);
            controlData._option._isFocusable = true;
            controlData._option._needDoubleClickToFocus = true;
            PrepareControlDataParam prepareControlDataParam;
            prepareControlDataParam._offset = commonControlParam._offset;
            prepareControlDataParam._autoComputedDisplaySize._y = _fontSize;
            prepareControlData(controlData, prepareControlDataParam);

            Rendering::Color finalBackgroundColor;
            const bool wasFocused = _controlInteractionStateSet.isControlFocused(controlData);
            const bool isFocused = processFocusControl(controlData, commonControlParam._backgroundColor, commonControlParam._backgroundColor.addedRgb(-0.125f), finalBackgroundColor);
            {
                const ControlData& parentControlData = getControlData(controlData.getParentID());
                Rect clipRectForMe = controlData.getRect();
                clipRectForMe.clipBy(parentControlData.getClipRects()._forChildren);
                controlData.setClipRectForMe(clipRectForMe);
            }

            constexpr uint32 kTextBufferSize = 255;
            if (isFocused == false)
            {
                wchar_t format[kTextBufferSize];
                wchar_t buffer[kTextBufferSize];
                formatString(format, L"%%.%df", decimalDigits);
                formatString(buffer, format, value);
                controlData._text = buffer;
            }
            const uint16 textLength = static_cast<uint16>(controlData._text.length());
            Float4 textRenderOffset;
            if (controlData._controlValue._textBoxData._textDisplayOffset == 0)
            {
                const float fullTextWidth = computeTextWidth(controlData._text.c_str(), textLength);
                
                // 가운데 정렬!
                textRenderOffset._x = (controlData._size._x - fullTextWidth) * 0.5f;
            }

            // Input 처리
            valueSliderProcessInput(wasFocused, controlData, textRenderOffset, value, controlData._text);

            if (wasFocused == false && isFocused == true)
            {
                GUI::InputBoxHelpers::selectAll(controlData, controlData._text);
            }

            // Caret 의 렌더링 위치가 TextBox 를 벗어나는 경우 처리!!
            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            GUI::InputBoxHelpers::updateTextDisplayOffset(rendererContext, textLength, kTextBoxBackSpaceStride, controlData);

            // Box 렌더링
            const Float4& controlCenterPosition = controlData.getControlCenterPosition();
            rendererContext.setClipRect(controlData.getClipRects()._forMe);
            rendererContext.setColor(finalBackgroundColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(controlData._size, (roundnessInPixel / controlData._size.minElement()), 0.0f, 0.0f);

            // Text, Caret, Selection 렌더링
            GUI::InputBoxHelpers::drawTextWithoutInputCandidate(rendererContext, commonControlParam, textRenderOffset, isFocused, _fontSize, true, controlData, controlData._text);
            GUI::InputBoxHelpers::drawSelection(rendererContext, textRenderOffset, isFocused, _fontSize, getNamedColor(NamedColor::HighlightColor).addedRgb(-0.375f).scaledA(0.25f), controlData, controlData._text);
            return false;
        }

        const bool GUIContext::beginLabeledValueSlider(const char* const file, const int line, const wchar_t* const labelText, const LabelParam& labelParam, const CommonControlParam& valueSliderParam, const float labelWidth, const float roundnessInPixel, const int32 decimalDigits, float& value)
        {
            LabelParam labelParamModified = labelParam;
            labelParamModified._alignmentHorz = GUI::TextAlignmentHorz::Center;
            const Float2 desiredSize = _controlMetaStateSet.getNextDesiredSize();
            _controlMetaStateSet.pushSize(Float2(labelWidth, desiredSize._y));
            makeLabel(file, line, labelText, labelParamModified);
            _controlMetaStateSet.popSize();
            
            _controlMetaStateSet.nextSameLine();
            _controlMetaStateSet.nextOffInterval();
            
            _controlMetaStateSet.pushSize(Float2(desiredSize._x - labelWidth, desiredSize._y));
            const bool result = beginValueSlider(file, line, valueSliderParam, roundnessInPixel, decimalDigits, value);
            _controlMetaStateSet.popSize();

            return result;
        }

        void GUIContext::valueSliderProcessInput(const bool wasControlFocused, ControlData& controlData, Float4& textRenderOffset, float& value, StringW& outText) noexcept
        {
            GUI::InputBoxHelpers::updateCaretState(_caretBlinkIntervalMs, controlData);

            if (_controlInteractionStateSet.isControlFocused(controlData) == true)
            {
                const Window::IWindow& window = _graphicDevice.getWindow();
                TextBoxProcessInputResult result;
                if (_mouseStates.isButtonDown(Platform::MouseButton::Left) == true || _mouseStates.isButtonDownThisFrame(Platform::MouseButton::Left) == true)
                {
                    GUI::InputBoxHelpers::processDefaultMouseInputs(_mouseStates, getRendererContext(controlData), controlData, textRenderOffset, outText, result);
                }
                else
                {
                    const TextInputMode kTextInputMode = TextInputMode::NumberOnly;
                    GUI::InputBoxHelpers::processDefaultKeyboardInputs(&window, getRendererContext(controlData), controlData, kTextInputMode, kTextBoxMaxTextLength, _keyCode,
                        _wcharInput, _wcharInputCandidate, textRenderOffset, outText, result);
                }

                if (wasControlFocused == false)
                {
                    GUI::InputBoxHelpers::refreshCaret(controlData);
                }

                if (result._clearKeyCode == true)
                {
                    _keyCode = Platform::KeyCode::NONE;
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
                    try
                    {
                        value = StringUtil::convertStringWToFloat(outText);
                    }
                    catch (std::invalid_argument e)
                    {
                        __noop;
                    }
                }
            }
            else
            {
                if (_mouseStates.isButtonDown(Platform::MouseButton::Left) == true 
                    && ControlCommonHelpers::isInControlInteractionArea(_mouseStates.getButtonDownPosition(), controlData) == true)
                {
                    const Float2 dragDelta = _mouseStates.getMouseDragDelta();
                    value += (dragDelta._x - dragDelta._y) * 0.1f;

                    _mouseStates.setButtonDownPositionCopy(_mouseStates.getPosition());
                }
            }
        }
        
        const bool GUIContext::beginListView(const char* const file, const int line, int16& outSelectedListItemIndex, const ListViewParam& listViewParam)
        {
            static constexpr ControlType controlType = ControlType::ListView;
            
            const ControlID controlID = issueControlID(file, line, controlType, nullptr);
            
            ControlData& controlData = accessControlData(controlID);
            controlData._option._isFocusable = false;

            PrepareControlDataParam prepareControlDataParam;
            {
                prepareControlDataParam._autoComputedDisplaySize._x = 160.0f;
                prepareControlDataParam._autoComputedDisplaySize._y = 100.0f;
            }
            prepareControlData(controlData, prepareControlDataParam);

            if (controlData.getChildControlCount() == 0)
            {
                controlData._controlValue._itemData.deselect();
            }
            outSelectedListItemIndex = controlData._controlValue._itemData.getSelectedItemIndex();

            Rendering::Color finalBackgroundColor = getNamedColor(NamedColor::LightFont);
            processShowOnlyControl(controlData, finalBackgroundColor, false);

            {
                const ControlData& parentControlData = getControlData(controlData.getParentID());
                Rect clipRectForMe = controlData.getRect();
                clipRectForMe.clipBy(parentControlData.getClipRects()._forChildren);
                controlData.setClipRectForMe(clipRectForMe);
            }
            {
                Rect clipRectForChildren = controlData.getRect();
                const float halfRoundnessInPixel = kDefaultRoundnessInPixel * 0.5f;
                const float quarterRoundnessInPixel = halfRoundnessInPixel * 0.5f;
                clipRectForChildren.left(clipRectForChildren.left() + static_cast<LONG>(quarterRoundnessInPixel));
                clipRectForChildren.right(clipRectForChildren.right() - static_cast<LONG>(halfRoundnessInPixel));

                const ControlData& parentControlData = getControlData(controlData.getParentID());
                clipRectForChildren.clipBy(parentControlData.getClipRects()._forChildren);
                controlData.setClipRectForChildren(clipRectForChildren);
            }

            const Float4& controlCenterPosition = controlData.getControlCenterPosition();
            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            rendererContext.setClipRect(controlData.getClipRects()._forMe);
            rendererContext.setColor(finalBackgroundColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(controlData._size, (kDefaultRoundnessInPixel / controlData._size.minElement()), 0.0f, 0.0f);
            
            if (listViewParam._useScrollBar == true)
            {
                controlData._controlValue._commonData.enableScrollBar(GUI::ScrollBarType::Vert);
            }
            else
            {
                controlData._controlValue._commonData.disableScrollBar(GUI::ScrollBarType::Vert);
            }
            _controlStackPerFrame.push_back(ControlStackData(controlData));
            return true;
        }

        void GUIContext::endListView()
        {
            ControlData& controlData = accessControlStackTopXXX();
            const bool hasScrollBarVert = controlData._controlValue._commonData.isScrollBarEnabled(ScrollBarType::Vert);
            if (hasScrollBarVert == true)
            {
                makeScrollBar(controlData.getID(), GUI::ScrollBarType::Vert);
            }

            endControlInternal(ControlType::ListView);
        }

        void GUIContext::makeListItem(const char* const file, const int line, const wchar_t* const text)
        {
            static constexpr ControlType controlType = ControlType::ListItem;
            
            const ControlID controlID = issueControlID(file, line, controlType, text);
            
            ControlData& controlData = accessControlData(controlID);
            controlData._option._isFocusable = false;

            ControlData& parentControlData = accessControlData(controlData.getParentID());
            PrepareControlDataParam prepareControlDataParam;
            {
                prepareControlDataParam._autoComputedDisplaySize._x = parentControlData._size._x;
                prepareControlDataParam._autoComputedDisplaySize._y = _fontSize + 12.0f;
                prepareControlDataParam._innerPadding.left(prepareControlDataParam._autoComputedDisplaySize._y * 0.25f);
                prepareControlDataParam._clipRectUsage = GUI::ClipRectUsage::ParentsChild;
                _controlMetaStateSet.nextOffInterval();
            }
            prepareControlData(controlData, prepareControlDataParam);

            const int16 parentSelectedItemIndex = parentControlData._controlValue._itemData.getSelectedItemIndex();
            const int16 myIndex = parentControlData.getLastAddedChildIndex();
            Rendering::Color finalColor;
            const Rendering::Color inputColor = (parentSelectedItemIndex == myIndex) ? getNamedColor(NamedColor::HighlightColor) : getNamedColor(NamedColor::LightFont);
            const bool isClicked = processClickControl(controlData, inputColor, inputColor, inputColor, finalColor);
            if (isClicked == true)
            {
                parentControlData._controlValue._itemData.select(myIndex);
            }

            const Float4& controlCenterPosition = controlData.getControlCenterPosition();
            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            rendererContext.setClipRect(controlData.getClipRects()._forMe);
            rendererContext.setColor(finalColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(controlData._size, (kDefaultRoundnessInPixel / controlData._size.minElement()), 0.0f, 0.0f);

            const Float2& controlLeftCenterPosition = controlData.getControlLeftCenterPosition();
            rendererContext.setTextColor(getNamedColor(NamedColor::DarkFont));
            rendererContext.drawDynamicText(text, Float4(controlLeftCenterPosition._x + controlData.getInnerPadding().left(), controlLeftCenterPosition._y, 0, 0),
                Rendering::FontRenderingOption(Rendering::TextRenderDirectionHorz::Rightward, Rendering::TextRenderDirectionVert::Centered));
        }

        const bool GUIContext::beginMenuBar(const char* const file, const int line, const wchar_t* const name)
        {
            static constexpr ControlType controlType = ControlType::MenuBar;

            _controlMetaStateSet.nextOffAutoPosition();

            const ControlID menuBarID = issueControlID(file, line, controlType, nullptr);
            
            ControlData& menuBar = accessControlData(menuBarID);
            ControlData& menuBarParent = accessControlData(menuBar.getParentID());
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
                prepareControlDataParam._autoComputedDisplaySize._x = menuBarParent._size._x;
                prepareControlDataParam._autoComputedDisplaySize._y = kMenuBarBaseSize._y;
                prepareControlDataParam._desiredPositionInParent._x = 0.0f;
                prepareControlDataParam._desiredPositionInParent._y = (isMenuBarParentWindow == true) ? kTitleBarBaseThickness : 0.0f;
                prepareControlDataParam._clipRectUsage = ClipRectUsage::ParentsOwn;
            }
            prepareControlData(menuBar, prepareControlDataParam);

            const bool wasToggled = menuBar._controlValue._booleanData.get();
            const Float2 interactionSize = Float2(menuBar._controlValue._itemData._itemSize._x, menuBar.getInteractionSize()._y);
            if (_controlInteractionStateSet.hasPressedControl() 
                && ControlCommonHelpers::isInControl(_mouseStates.getPosition(), menuBar._position, Float2::kZero, interactionSize) == false)
            {
                menuBar._controlValue._booleanData.set(false);
            }
            menuBar._controlValue._itemData._itemSize._x = 0.0f;

            const bool isToggled = menuBar._controlValue._booleanData.get();
            const uint32 childCount = static_cast<uint32>(menuBar.getChildControlCount());
            if ((childCount == 0 || isToggled == false) && wasToggled == false)
            {
                // wasToggled 덕분에 다음 프레임에 -1 로 세팅된다. 한 번은 자식 함수들이 쭉 호출된다는 뜻!

                menuBar._controlValue._itemData.deselect();
            }

            Rendering::Color color = getNamedColor(NamedColor::NormalState);
            processShowOnlyControl(menuBar, color, false);
            if (isMenuBarParentRoot == true)
            {
                color.a(1.0f);
            }

            const Float4& controlCenterPosition = menuBar.getControlCenterPosition();
            menuBar._rendererContextLayer = getUpperRendererContextLayer(menuBarParent);
            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(menuBar);
            rendererContext.setClipRect(menuBar.getClipRects()._forMe);
            rendererContext.setColor(color);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(menuBar._size, 0.0f, 0.0f, 0.0f);

            _controlStackPerFrame.push_back(ControlStackData(menuBar));
            return true;
        }

        const bool GUIContext::beginMenuBarItem(const char* const file, const int line, const wchar_t* const text)
        {
            static constexpr ControlType controlType = ControlType::MenuBarItem;

            _controlMetaStateSet.nextOffAutoPosition();

            if (accessControlStackTopXXX().isTypeOf(ControlType::MenuBar) == false)
            {
                MINT_LOG_ERROR("김장원", "MenuBarItem 은 MenuBar 컨트롤의 자식으로만 사용할 수 있습니다!");
                return false;
            }

            const ControlID menuBarItemID = issueControlID(file, line, controlType, text);
            
            ControlData& menuBarItem = accessControlData(menuBarItemID);
            ControlData& menuBar = accessControlData(menuBarItem.getParentID());
            PrepareControlDataParam prepareControlDataParam;
            {
                const uint32 textLength = StringUtil::length(text);
                const float textWidth = computeTextWidth(text, textLength);
                prepareControlDataParam._autoComputedDisplaySize._x = textWidth + kMenuBarItemTextSpace;
                prepareControlDataParam._autoComputedDisplaySize._y = kMenuBarBaseSize._y;
                prepareControlDataParam._desiredPositionInParent._x = menuBar._controlValue._itemData._itemSize._x;
                prepareControlDataParam._desiredPositionInParent._y = 0.0f;
                prepareControlDataParam._clipRectUsage = ClipRectUsage::ParentsOwn;
            }
            prepareControlData(menuBarItem, prepareControlDataParam);
            menuBar._controlValue._itemData._itemSize._x += menuBarItem._size._x;
            menuBarItem._controlValue._itemData._itemSize._y = 0.0f;

            const int16 menuBarSelectedItemIndex = menuBar._controlValue._itemData.getSelectedItemIndex();
            const int16 myIndex = menuBar.getLastAddedChildIndex();
            const bool wasMeSelected = (menuBarSelectedItemIndex == myIndex);
            Rendering::Color finalBackgroundColor;
            const Rendering::Color& normalColor = (wasMeSelected == true) ? getNamedColor(NamedColor::PressedState) : getNamedColor(NamedColor::NormalState);
            const Rendering::Color& hoverColor = (wasMeSelected == true) ? getNamedColor(NamedColor::PressedState) : getNamedColor(NamedColor::HoverState);
            const Rendering::Color& pressedColor = (wasMeSelected == true) ? getNamedColor(NamedColor::PressedState) : getNamedColor(NamedColor::PressedState);
            const bool isClicked = processClickControl(menuBarItem, normalColor, hoverColor, pressedColor, finalBackgroundColor);
            const bool isParentAncestorPressed = isAncestorControlPressed(menuBar);
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
            if (_controlInteractionStateSet.isControlHovered(menuBarItem) == true && isParentControlToggled == true)
            {
                menuBar._controlValue._itemData.select(myIndex);
            }
            
            const Float4& controlCenterPosition = menuBarItem.getControlCenterPosition();
            
            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(menuBarItem);
            rendererContext.setClipRect(menuBarItem.getClipRects()._forMe);
            rendererContext.setColor(finalBackgroundColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(menuBarItem._size, 0.0f, 0.0f, 0.0f);

            const Float2& controlLeftCenterPosition = menuBarItem.getControlLeftCenterPosition();
            rendererContext.setTextColor(getNamedColor(NamedColor::LightFont));
            rendererContext.drawDynamicText(text, Float4(controlLeftCenterPosition._x + menuBarItem.getInnerPadding().left() + menuBarItem._size._x * 0.5f, controlLeftCenterPosition._y, 0, 0),
                Rendering::FontRenderingOption(Rendering::TextRenderDirectionHorz::Centered, Rendering::TextRenderDirectionVert::Centered));

            const bool isMeSelected = (menuBarSelectedItemIndex == myIndex);
            const bool result = (isClicked || isMeSelected || (isParentAncestorPressed && wasMeSelected));
            if (result == true)
            {
                _controlStackPerFrame.push_back(ControlStackData(menuBarItem));
            }
            return result;
        }

        const bool GUIContext::beginMenuItem(const char* const file, const int line, const wchar_t* const text)
        {
            static constexpr ControlType controlType = ControlType::MenuItem;

            _controlMetaStateSet.nextOffAutoPosition();
            _controlMetaStateSet.nextOffSizeContraintToParent();

            const ControlID menuItemID = issueControlID(file, line, controlType, text);
            
            ControlData& menuItem = accessControlData(menuItemID);
            menuItem._option._isInteractableOutsideParent = true;

            ControlData& menuItemParent = accessControlData(menuItem.getParentID());
            const ControlType parentControlType = menuItemParent.getControlType();
            const bool isParentControlMenuItem = (parentControlType == ControlType::MenuItem);
            if (parentControlType != ControlType::MenuBarItem && isParentControlMenuItem == false)
            {
                MINT_LOG_ERROR("김장원", "MenuItem 은 MenuBarItem 이나 MenuItem 컨트롤의 자식으로만 사용할 수 있습니다!");
                return false;
            }

            PrepareControlDataParam prepareControlDataParam;
            {
                prepareControlDataParam._autoComputedDisplaySize._x = menuItemParent._controlValue._itemData._itemSize._x;
                prepareControlDataParam._autoComputedDisplaySize._y = kMenuBarBaseSize._y;
                prepareControlDataParam._innerPadding.left(kMenuItemSpaceLeft);
                prepareControlDataParam._desiredPositionInParent._x = (isParentControlMenuItem == true) ? menuItemParent._size._x : 0.0f;
                prepareControlDataParam._desiredPositionInParent._y = menuItemParent._controlValue._itemData._itemSize._y + ((isParentControlMenuItem == true) ? 0.0f : prepareControlDataParam._autoComputedDisplaySize._y);
            }
            prepareControlData(menuItem, prepareControlDataParam);

            const uint32 textLength = StringUtil::length(text);
            const float textWidth = computeTextWidth(text, textLength);
            menuItemParent._controlValue._itemData._itemSize._x = max(menuItemParent._controlValue._itemData._itemSize._x, textWidth + kMenuItemSpaceRight);
            menuItemParent._controlValue._itemData._itemSize._y += menuItem._size._y;
            menuItem._controlValue._itemData._itemSize._y = 0.0f;

            const bool isDescendantHovered = isDescendantControlHoveredInclusive(menuItem);
            Rendering::Color finalBackgroundColor;
            {
                const Rendering::Color& normalColor = getNamedColor((isDescendantHovered == true) ? NamedColor::HoverState : NamedColor::NormalState);
                const Rendering::Color& hoverColor = getNamedColor(NamedColor::HoverState);
                const Rendering::Color& pressedColor = getNamedColor(NamedColor::PressedState);
                processClickControl(menuItem, normalColor, hoverColor, pressedColor, finalBackgroundColor);
                finalBackgroundColor.a(1.0f);
            }
            const bool isHovered = _controlInteractionStateSet.isControlHovered(menuItem);
            const bool isPresssed = _controlInteractionStateSet.isControlPressed(menuItem);
            const bool& isToggled = menuItem._controlValue._booleanData.get();
            const int16 myIndex = menuItemParent.getLastAddedChildIndex();
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

            const Float4& controlCenterPosition = menuItem.getControlCenterPosition();
            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(menuItem);
            rendererContext.setClipRect(menuItem.getClipRects()._forMe);
            rendererContext.setColor(finalBackgroundColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(menuItem._size, 0.0f, 0.0f, 0.0f);

            const uint16 maxChildCount = menuItem.getMaxChildControlCount();
            if (maxChildCount > 0)
            {
                const Float2& controlRightCenterPosition = menuItem.getControlRightCenterPosition();
                Float2 a = controlRightCenterPosition + Float2(-14, -5);
                Float2 b = controlRightCenterPosition + Float2( -4,  0);
                Float2 c = controlRightCenterPosition + Float2(-14, +5);
                rendererContext.setColor(getNamedColor((isToggled == true) ? NamedColor::HighlightColor : NamedColor::LightFont));
                rendererContext.drawSolidTriangle(a, b, c);
            }

            const Float2& controlLeftCenterPosition = menuItem.getControlLeftCenterPosition();
            rendererContext.setTextColor(getNamedColor(NamedColor::LightFont));
            rendererContext.drawDynamicText(text, Float4(controlLeftCenterPosition._x + menuItem.getInnerPadding().left(), controlLeftCenterPosition._y, 0, 0), 
                Rendering::FontRenderingOption(Rendering::TextRenderDirectionHorz::Rightward, Rendering::TextRenderDirectionVert::Centered));

            // (previousMaxChildCount) 최초 업데이트 시 Child 가 다 등록되어야 하므로 controlData._updateCount 를 이용한다.
            const bool result = (isToggled || isPresssed || menuItem._updateCount <= 1);
            if (result == true)
            {
                _controlStackPerFrame.push_back(ControlStackData(menuItem));
            }
            return result;
        }

        void GUIContext::makeScrollBar(const ControlID parentControlID, const ScrollBarType scrollBarType)
        {
            const bool useVertical = (scrollBarType == ScrollBarType::Vert || scrollBarType == ScrollBarType::Both);
            if (useVertical == true)
            {
                _makeScrollBarVert(parentControlID);
            }

            const bool useHorizontal = (scrollBarType == ScrollBarType::Horz || scrollBarType == ScrollBarType::Both);
            if (useHorizontal == true)
            {
                _makeScrollBarHorz(parentControlID);
            }
        }

        void GUIContext::_makeScrollBarVert(const ControlID parentControlID) noexcept
        {
            ScrollBarTrackParam scrollBarTrackParam;
            float parentWindowScrollDisplayHeight = 0.0f;
            {
                const ControlData& parentControlData = getControlData(parentControlID);
                parentWindowScrollDisplayHeight = parentControlData.computeScrollDisplayHeight();
                const bool isParentControlWindow = parentControlData.isTypeOf(GUI::ControlType::Window);
                const float titleBarOffsetX = (isParentControlWindow) ? kHalfBorderThickness * 2.0f : kScrollBarThickness * 0.5f;
                const float titleBarOffsetY = (isParentControlWindow) ? parentControlData.getMenuBarThickness()._y + parentControlData._controlValue._windowData._titleBarThickness : 0.0f;

                _controlMetaStateSet.nextSize(Float2(kScrollBarThickness, parentWindowScrollDisplayHeight));
                scrollBarTrackParam._positionInParent._x = parentControlData._size._x - titleBarOffsetX;
                scrollBarTrackParam._positionInParent._y = parentControlData.getInnerPadding().top() + titleBarOffsetY;
            }

            bool hasExtraSize = false;
            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(getControlData(parentControlID));
            ControlData& scrollBarTrack = __makeScrollBarTrack(parentControlID, ScrollBarType::Vert, scrollBarTrackParam, rendererContext, hasExtraSize);
            ControlData& parentControlData = accessControlData(parentControlID);
            if (hasExtraSize == true)
            {
                parentControlData._controlValue._commonData.enableScrollBar(ScrollBarType::Vert);

                __makeScrollBarThumb(scrollBarTrack.getID(), ScrollBarType::Vert, parentWindowScrollDisplayHeight, parentControlData.getContentAreaSize()._y, rendererContext);
            }
            else
            {
                parentControlData._controlValue._commonData.disableScrollBar(ScrollBarType::Vert);
                parentControlData._childDisplayOffset._y = 0.0f; // Scrolling!
            }
        }

        void GUIContext::_makeScrollBarHorz(const ControlID parentControlID) noexcept
        {
            ScrollBarTrackParam scrollBarTrackParam;
            float parentWindowScrollDisplayWidth = 0.0f;
            {
                const ControlData& parentControlData = getControlData(parentControlID);
                parentWindowScrollDisplayWidth = parentControlData.computeScrollDisplayWidth();
                const Float2& menuBarThicknes = parentControlData.getMenuBarThickness();

                _controlMetaStateSet.nextSize(Float2(parentWindowScrollDisplayWidth, kScrollBarThickness));
                scrollBarTrackParam._positionInParent._x = parentControlData.getInnerPadding().left() + menuBarThicknes._x;
                scrollBarTrackParam._positionInParent._y = parentControlData._size._y - kHalfBorderThickness * 2.0f;
            }
            
            bool hasExtraSize = false;
            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(getControlData(parentControlID));
            ControlData& scrollBarTrack = __makeScrollBarTrack(parentControlID, ScrollBarType::Horz, scrollBarTrackParam, rendererContext, hasExtraSize);
            ControlData& parentControlData = accessControlData(parentControlID);
            if (hasExtraSize == true)
            {
                parentControlData._controlValue._commonData.enableScrollBar(ScrollBarType::Horz);

                __makeScrollBarThumb(scrollBarTrack.getID(), ScrollBarType::Horz, parentWindowScrollDisplayWidth, parentControlData.getContentAreaSize()._x, rendererContext);
            }
            else
            {
                parentControlData._controlValue._commonData.disableScrollBar(ScrollBarType::Horz);
                parentControlData._childDisplayOffset._x = 0.0f; // Scrolling!
            }
        }

        ControlData& GUIContext::__makeScrollBarTrack(const ControlID parentControlID, const ScrollBarType scrollBarType, const ScrollBarTrackParam& scrollBarTrackParam, Rendering::ShapeFontRendererContext& shapeFontRendererContext, bool& outHasExtraSize)
        {
            static constexpr ControlType trackControlType = ControlType::ScrollBar;
            MINT_ASSERT("김장원", (scrollBarType != ScrollBarType::Both) && (scrollBarType != ScrollBarType::None), "잘못된 scrollBarType 입력값입니다.");

            outHasExtraSize = false;
            _controlMetaStateSet.nextOffAutoPosition();

            const bool isVert = (scrollBarType == ScrollBarType::Vert);
            const ControlID trackControlID = issueControlID(parentControlID, trackControlType, (isVert ? L"VERT" : L"HORZ"), nullptr);

            const ControlData& parentControlData = getControlData(parentControlID);
            ControlData& trackControlData = accessControlData(trackControlID);
            PrepareControlDataParam prepareControlDataParamForTrack;
            {
                //prepareControlDataParamForTrack._autoComputedDisplaySize = trackControlData._size;
                prepareControlDataParamForTrack._desiredPositionInParent = scrollBarTrackParam._positionInParent;
                if (isVert == true)
                {
                    prepareControlDataParamForTrack._desiredPositionInParent._x -= parentControlData.getDockZoneSize(DockZone::RightSide)._x;
                    prepareControlDataParamForTrack._desiredPositionInParent._y += parentControlData.getDockZoneSize(DockZone::TopSide)._y;
                }
                else
                {
                    prepareControlDataParamForTrack._desiredPositionInParent._x += parentControlData.getDockZoneSize(DockZone::LeftSide)._x;
                    prepareControlDataParamForTrack._desiredPositionInParent._y -= parentControlData.getDockZoneSize(DockZone::BottomSide)._y;
                }
                prepareControlDataParamForTrack._parentIDOverride = parentControlData.getID();
                prepareControlDataParamForTrack._alwaysResetPosition = true;
                prepareControlDataParamForTrack._ignoreMeForContentAreaSize = true;
                prepareControlDataParamForTrack._clipRectUsage = ClipRectUsage::ParentsOwn;
            }
            prepareControlData(trackControlData, prepareControlDataParamForTrack);

            Rendering::Color trackColor = getNamedColor(NamedColor::ScrollBarTrack);
            processShowOnlyControl(trackControlData, trackColor, false);

            // Vertical Track
            if (isVert)
            {
                const float extraSize = parentControlData.getContentAreaSize()._y - parentControlData.computeScrollDisplayHeight();
                if (extraSize >= 0.0f)
                {
                    // Rendering track
                    const float radius = kScrollBarThickness * 0.5f;
                    {
                        const float rectLength = trackControlData._size._y - radius * 2.0f;
                        shapeFontRendererContext.setClipRect(trackControlData.getClipRects()._forMe);
                        shapeFontRendererContext.setColor(trackColor);

                        Float4 trackRenderPosition = Float4(trackControlData._position._x, trackControlData._position._y + radius, 0.0f, 1.0f);

                        // Upper half circle
                        shapeFontRendererContext.setPosition(trackRenderPosition);
                        shapeFontRendererContext.drawHalfCircle(radius, 0.0f);

                        // Rect
                        if (rectLength > 0.0f)
                        {
                            trackRenderPosition._y += rectLength * 0.5f;
                            shapeFontRendererContext.setPosition(trackRenderPosition);
                            shapeFontRendererContext.drawRectangle(trackControlData._size - Float2(0.0f, radius * 2.0f), 0.0f, 0.0f);
                        }

                        // Lower half circle
                        if (rectLength > 0.0f)
                        {
                            trackRenderPosition._y += rectLength * 0.5f;
                        }
                        shapeFontRendererContext.setPosition(trackRenderPosition);
                        shapeFontRendererContext.drawHalfCircle(radius, Math::kPi);
                    }

                    outHasExtraSize = true;
                }
            }
            else // Horz
            {
                const float extraSize = parentControlData.getContentAreaSize()._x - parentControlData.computeScrollDisplayWidth();
                if (extraSize >= 0.0f)
                {
                    // Rendering track
                    const float radius = kScrollBarThickness * 0.5f;
                    {
                        const float rectLength = trackControlData._size._x - radius * 2.0f;
                        shapeFontRendererContext.setClipRect(trackControlData.getClipRects()._forMe);
                        shapeFontRendererContext.setColor(trackColor);

                        Float4 trackRenderPosition = Float4(trackControlData._position._x + radius, trackControlData._position._y, 0.0f, 1.0f);

                        // Left half circle
                        shapeFontRendererContext.setPosition(trackRenderPosition);
                        shapeFontRendererContext.drawHalfCircle(radius, +Math::kPiOverTwo);

                        // Rect
                        if (rectLength > 0.0f)
                        {
                            trackRenderPosition._x += rectLength * 0.5f;
                            shapeFontRendererContext.setPosition(trackRenderPosition);
                            shapeFontRendererContext.drawRectangle(trackControlData._size - Float2(radius * 2.0f, 0.0f), 0.0f, 0.0f);
                        }

                        // Right half circle
                        if (rectLength > 0.0f)
                        {
                            trackRenderPosition._x += rectLength * 0.5f;
                        }
                        shapeFontRendererContext.setPosition(trackRenderPosition);
                        shapeFontRendererContext.drawHalfCircle(radius, -Math::kPiOverTwo);
                    }

                    outHasExtraSize = true;
                }
            }

            return trackControlData;
        }

        void GUIContext::__makeScrollBarThumb(const ControlID parentControlID, const ScrollBarType scrollBarType, const float visibleLength, const float totalLength, Rendering::ShapeFontRendererContext& shapeFontRendererContext)
        {
            static constexpr ControlType thumbControlType = ControlType::ScrollBarThumb;
            
            _controlMetaStateSet.nextOffAutoPosition();

            const bool isVert = (scrollBarType == ScrollBarType::Vert);
            const ControlID thumbControlID = issueControlID(parentControlID, thumbControlType, (isVert ? L"VERT" : L"HORZ"), nullptr);
            
            const float radius = kScrollBarThickness * 0.5f;
            const float thumbSizeRatio = (visibleLength / totalLength);
            const float thumbSize = visibleLength * thumbSizeRatio - radius * 2.0f;
            const float trackRemnantSize = std::abs(visibleLength - thumbSize);
            ControlData& scrollBarTrack = accessControlData(parentControlID);
            ControlData& scrollBarParent = accessControlData(scrollBarTrack.getParentID());
            ControlData& thumbControlData = accessControlData(thumbControlID);
            if (isVert)
            {
                PrepareControlDataParam prepareControlDataParamForThumb;
                {
                    prepareControlDataParamForThumb._autoComputedDisplaySize._x = kScrollBarThickness;
                    prepareControlDataParamForThumb._autoComputedDisplaySize._y = thumbSize;

                    prepareControlDataParamForThumb._desiredPositionInParent = getControlPositionInParentSpace(scrollBarTrack);
                    prepareControlDataParamForThumb._desiredPositionInParent._x -= kScrollBarThickness * 0.5f;

                    prepareControlDataParamForThumb._parentIDOverride = scrollBarParent.getID();
                    prepareControlDataParamForThumb._ignoreMeForContentAreaSize = true;
                    prepareControlDataParamForThumb._clipRectUsage = ClipRectUsage::ParentsOwn;

                    thumbControlData._option._isDraggable = true;
                    thumbControlData._positionConstraintsForDragging.left(scrollBarTrack._position._x - kScrollBarThickness * 0.5f);
                    thumbControlData._positionConstraintsForDragging.right(thumbControlData._positionConstraintsForDragging.left());
                    thumbControlData._positionConstraintsForDragging.top(scrollBarTrack._position._y);
                    thumbControlData._positionConstraintsForDragging.bottom(thumbControlData._positionConstraintsForDragging.top() + trackRemnantSize);
                }
                prepareControlData(thumbControlData, prepareControlDataParamForThumb);

                // @중요
                // Compute position from internal value
                thumbControlData._position._y = scrollBarTrack._position._y + (thumbControlData._controlValue._thumbData._thumbAt * trackRemnantSize);

                Rendering::Color thumbColor;
                processScrollableControl(thumbControlData, getNamedColor(NamedColor::ScrollBarThumb), getNamedColor(NamedColor::ScrollBarThumb).scaledRgb(1.25f), thumbColor);

                const float mouseWheelScroll = getMouseWheelScroll(scrollBarParent);
                const float thumbAtRatio = (trackRemnantSize < 1.0f) ? 0.0f : Math::saturate((thumbControlData._position._y - thumbControlData._positionConstraintsForDragging.top() + mouseWheelScroll) / trackRemnantSize);
                thumbControlData._controlValue._thumbData._thumbAt = thumbAtRatio;
                scrollBarParent._childDisplayOffset._y = -thumbAtRatio * (totalLength - visibleLength); // Scrolling!

                // Rendering thumb
                {
                    const float rectLength = thumbSize - radius * 2.0f;
                    shapeFontRendererContext.setClipRect(thumbControlData.getClipRects()._forMe);
                    shapeFontRendererContext.setColor(thumbColor);

                    Float4 thumbRenderPosition = Float4(thumbControlData._position._x + radius, thumbControlData._position._y + radius, 0.0f, 1.0f);

                    // Upper half circle
                    shapeFontRendererContext.setPosition(thumbRenderPosition);
                    shapeFontRendererContext.drawHalfCircle(radius, 0.0f);

                    // Rect
                    if (rectLength > 0.0f)
                    {
                        thumbRenderPosition._y += rectLength * 0.5f;
                        shapeFontRendererContext.setPosition(thumbRenderPosition);
                        shapeFontRendererContext.drawRectangle(thumbControlData._size - Float2(0.0f, radius * 2.0f), 0.0f, 0.0f);
                    }

                    // Lower half circle
                    if (rectLength > 0.0f)
                    {
                        thumbRenderPosition._y += rectLength * 0.5f;
                    }
                    shapeFontRendererContext.setPosition(thumbRenderPosition);
                    shapeFontRendererContext.drawHalfCircle(radius, Math::kPi);
                }
            }
            else // Horz
            {
                PrepareControlDataParam prepareControlDataParamForThumb;
                {
                    prepareControlDataParamForThumb._autoComputedDisplaySize._x = thumbSize;
                    prepareControlDataParamForThumb._autoComputedDisplaySize._y = kScrollBarThickness;

                    prepareControlDataParamForThumb._desiredPositionInParent = getControlPositionInParentSpace(scrollBarTrack);
                    prepareControlDataParamForThumb._desiredPositionInParent._y -= kScrollBarThickness * 0.5f;

                    prepareControlDataParamForThumb._parentIDOverride = scrollBarParent.getID();
                    prepareControlDataParamForThumb._ignoreMeForContentAreaSize = true;
                    prepareControlDataParamForThumb._clipRectUsage = ClipRectUsage::ParentsOwn;

                    thumbControlData._option._isDraggable = true;
                    thumbControlData._positionConstraintsForDragging.left(scrollBarTrack._position._x);
                    thumbControlData._positionConstraintsForDragging.right(thumbControlData._positionConstraintsForDragging.left() + trackRemnantSize);
                    thumbControlData._positionConstraintsForDragging.top(scrollBarTrack._position._y - kScrollBarThickness * 0.5f);
                    thumbControlData._positionConstraintsForDragging.bottom(thumbControlData._positionConstraintsForDragging.top());
                }
                prepareControlData(thumbControlData, prepareControlDataParamForThumb);

                // @중요
                // Compute position from internal value
                thumbControlData._position._x = scrollBarTrack._position._x + (thumbControlData._controlValue._thumbData._thumbAt * trackRemnantSize);

                Rendering::Color thumbColor;
                processScrollableControl(thumbControlData, getNamedColor(NamedColor::ScrollBarThumb), getNamedColor(NamedColor::ScrollBarThumb).scaledRgb(1.25f), thumbColor);

                const float thumbAtRatio = (trackRemnantSize < 1.0f) ? 0.0f : Math::saturate((thumbControlData._position._x - thumbControlData._positionConstraintsForDragging.left()) / trackRemnantSize);
                thumbControlData._controlValue._thumbData._thumbAt = thumbAtRatio;
                scrollBarParent._childDisplayOffset._x = -thumbAtRatio * (totalLength - visibleLength + ((scrollBarType == ScrollBarType::Both) ? kScrollBarThickness : 0.0f)); // Scrolling!

                // Rendering thumb
                {
                    const float rectLength = thumbSize - radius * 2.0f;
                    shapeFontRendererContext.setClipRect(thumbControlData.getClipRects()._forMe);
                    shapeFontRendererContext.setColor(thumbColor);

                    Float4 thumbRenderPosition = Float4(thumbControlData._position._x + radius, thumbControlData._position._y + radius, 0.0f, 1.0f);

                    // Left half circle
                    shapeFontRendererContext.setPosition(thumbRenderPosition);
                    shapeFontRendererContext.drawHalfCircle(radius, +Math::kPiOverTwo);

                    // Rect
                    if (rectLength > 0.0f)
                    {
                        thumbRenderPosition._x += rectLength * 0.5f;
                        shapeFontRendererContext.setPosition(thumbRenderPosition);
                        shapeFontRendererContext.drawRectangle(thumbControlData._size - Float2(radius * 2.0f, 0.0f), 0.0f, 0.0f);
                    }

                    // Right half circle
                    if (rectLength > 0.0f)
                    {
                        thumbRenderPosition._x += rectLength * 0.5f;
                    }
                    shapeFontRendererContext.setPosition(thumbRenderPosition);
                    shapeFontRendererContext.drawHalfCircle(radius, -Math::kPiOverTwo);
                }
            }
        }

        void GUIContext::processDock(const ControlData& controlData, Rendering::ShapeFontRendererContext& rendererContext)
        {
            if (controlData._dockContext._dockingControlType == DockingControlType::Dock || controlData._dockContext._dockingControlType == DockingControlType::DockerDock)
            {
                for (DockZone dockZoneIter = static_cast<DockZone>(0); dockZoneIter != DockZone::COUNT; dockZoneIter = static_cast<DockZone>(static_cast<uint32>(dockZoneIter) + 1))
                {
                    const DockZoneData& dockZoneData = controlData.getDockZoneData(dockZoneIter);
                    if (dockZoneData.hasDockedControls() == true)
                    {
                        const Float2& dockZoneSize = controlData.getDockZoneSize(dockZoneIter);
                        const Float2& dockZonePosition = controlData.getDockZonePosition(dockZoneIter);

                        if (_mouseStates.isButtonDownThisFrame(Platform::MouseButton::Left) == true)
                        {
                            if (ControlCommonHelpers::isInControl(_mouseStates.getButtonDownPosition(), dockZonePosition, Float2::kZero, dockZoneSize) == true)
                            {
                                if (isDescendantControlInclusive(controlData, _controlInteractionStateSet.getFocusedControlID()) == false)
                                {
                                    setControlFocused(controlData);
                                }
                            }
                        }

                        rendererContext.setClipRect(controlData.getClipRects()._forDocks);
                        
                        rendererContext.setColor(getNamedColor(NamedColor::Dock));
                        rendererContext.setPosition(Float4(dockZonePosition._x + dockZoneSize._x * 0.5f, dockZonePosition._y + dockZoneSize._y * 0.5f, 0, 0));

                        rendererContext.drawRectangle(dockZoneSize, 0.0f, 0.0f);
                    }
                }
            }
        }

        void GUIContext::endControlInternal(const ControlType controlType)
        {
            MINT_ASSERT("김장원", _controlStackPerFrame.back()._controlType == controlType, "begin 과 end 의 ControlType 이 다릅니다!!!");

            _controlStackPerFrame.pop_back();
        }

        Float2 GUIContext::beginTitleBar(const ControlID parentControlID, const wchar_t* const windowTitle, const Float2& titleBarSize, const Rect& innerPadding, VisibleState& inoutParentVisibleState)
        {
            static constexpr ControlType controlType = ControlType::TitleBar;

            const ControlID controlID = issueControlID(parentControlID, controlType, L"TITLEBAR", windowTitle);
            
            ControlData& controlData = accessControlData(controlID);
            controlData._option._isDraggable = true;
            controlData._delegateControlID = controlData.getParentID();
            ControlData& parentWindowControlData = accessControlData(controlData.getParentID());
            const bool isParentControlDocking = parentWindowControlData.isDocking();
            PrepareControlDataParam prepareControlDataParam;
            {
                if (isParentControlDocking == true)
                {
                    const ControlData& dockControlData = getControlData(parentWindowControlData.getDockControlID());
                    const DockZoneData& parentDockZoneData = dockControlData.getDockZoneData(parentWindowControlData._dockContext._lastDockZone);
                    const int32 dockedControlIndex = parentDockZoneData.getDockedControlIndex(parentWindowControlData.getID());
                    const float textWidth = computeTextWidth(windowTitle, StringUtil::length(windowTitle));
                    const Float2& displaySizeOverride = Float2(textWidth + 16.0f, controlData._size._y);
                    prepareControlDataParam._autoComputedDisplaySize = displaySizeOverride;
                    prepareControlDataParam._desiredPositionInParent._x = parentDockZoneData.getDockedControlTitleBarOffset(dockedControlIndex);
                    prepareControlDataParam._desiredPositionInParent._y = 0.0f;
                }
                else
                {
                    prepareControlDataParam._autoComputedDisplaySize = titleBarSize;
                    prepareControlDataParam._deltaInteractionSize = Float2(-innerPadding.right() - kDefaultRoundButtonRadius * 2.0f, 0.0f);
                }
                prepareControlDataParam._alwaysResetPosition = true;
                prepareControlDataParam._clipRectUsage = ClipRectUsage::ParentsOwn;
            }
            prepareControlData(controlData, prepareControlDataParam);
            
            Rendering::Color finalBackgroundColor;
            const bool isFocused = processFocusControl(controlData, getNamedColor(NamedColor::TitleBarFocused), getNamedColor(NamedColor::TitleBarOutOfFocus), finalBackgroundColor);
            if (isParentControlDocking == true)
            {
                if (_controlInteractionStateSet.isControlPressed(controlData) == true)
                {
                    ControlData& dockControlData = accessControlData(parentWindowControlData.getDockControlID());
                    DockZoneData& dockZoneData = dockControlData.getDockZoneData(parentWindowControlData._dockContext._lastDockZone);
                    dockZoneData._focusedDockedControlIndex = dockZoneData.getDockedControlIndex(parentWindowControlData.getID());
                    
                    setControlFocused(dockControlData);
                }
            }

            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            rendererContext.setClipRect(controlData.getClipRects()._forMe);
            rendererContext.setPosition(controlData.getControlCenterPosition());
            if (isParentControlDocking == true)
            {
                const ControlData& dockControlData = getControlData(parentWindowControlData.getDockControlID());
                const bool isParentControlFocusedDocker = dockControlData.isFocusedDocker(parentWindowControlData);
                if (_controlInteractionStateSet.isControlHovered(controlData) == true)
                {
                    rendererContext.setColor(((isParentControlFocusedDocker == true) ? getNamedColor(NamedColor::ShownInDock) : getNamedColor(NamedColor::ShownInDock).addedRgb(32)));
                }
                else
                {
                    rendererContext.setColor(((isParentControlFocusedDocker == true) ? getNamedColor(NamedColor::ShownInDock) : getNamedColor(NamedColor::ShownInDock).addedRgb(16)));
                }

                rendererContext.drawRectangle(controlData._size, 0.0f, 0.0f);
            }
            else
            {
                rendererContext.setColor(finalBackgroundColor);

                rendererContext.drawHalfRoundedRectangle(controlData._size, (kDefaultRoundnessInPixel * 2.0f / controlData._size.minElement()), Math::kPi);

                rendererContext.setColor(Rendering::Color(127, 127, 127));
                rendererContext.drawLine(controlData._position + Float2(0.0f, titleBarSize._y), controlData._position + Float2(controlData._size._x, titleBarSize._y), 1.0f);
            }

            const Float4& titleBarTextPosition = Float4(controlData._position._x, controlData._position._y, 0.0f, 1.0f) + Float4(innerPadding.left(), titleBarSize._y * 0.5f, 0.0f, 0.0f);
            const bool needToColorFocused_ = needToColorFocused(parentWindowControlData);
            if (isParentControlDocking == true)
            {
                const ControlData& dockControlData = getControlData(parentWindowControlData.getDockControlID());
                const bool isParentControlFocusedDocker = dockControlData.isFocusedDocker(parentWindowControlData);

                rendererContext.setTextColor((isParentControlFocusedDocker == true) ? getNamedColor(NamedColor::ShownInDockFont) : getNamedColor(NamedColor::LightFont));
            }
            else
            {
                rendererContext.setTextColor((needToColorFocused_ == true) ? getNamedColor(NamedColor::LightFont) : getNamedColor(NamedColor::DarkFont));
            }
            rendererContext.drawDynamicText(windowTitle, titleBarTextPosition, 
                Rendering::FontRenderingOption(Rendering::TextRenderDirectionHorz::Rightward, Rendering::TextRenderDirectionVert::Centered, 0.9375f));

            _controlStackPerFrame.push_back(ControlStackData(controlData));

            // Close button
            if (parentWindowControlData.isDocking() == false)
            {
                // 중요
                _controlMetaStateSet.nextOffAutoPosition();
                _controlMetaStateSet.nextPosition(Float2(titleBarSize._x - kDefaultRoundButtonRadius * 2.0f - innerPadding.right(), (titleBarSize._y - kDefaultRoundButtonRadius * 2.0f) * 0.5f));

                if (makeRoundButton(controlData.getID(), L"CLOSEBUTTON", windowTitle, Rendering::Color(1.0f, 0.375f, 0.375f)) == true)
                {
                    inoutParentVisibleState = GUI::VisibleState::Invisible;
                }
            }

            return titleBarSize;
        }

        const bool GUIContext::makeRoundButton(const ControlID parentControlID, const wchar_t* const identifier, const wchar_t* const windowTitle, const Rendering::Color& color)
        {
            static constexpr ControlType controlType = ControlType::RoundButton;

            const ControlID controlID = issueControlID(parentControlID, controlType, identifier, windowTitle);

            const float radius = kDefaultRoundButtonRadius;
            const ControlData& parentWindowData = getParentWindowControlData(getControlData(_controlStackPerFrame.back()._id));
            ControlData& controlData = accessControlData(controlID);
            PrepareControlDataParam prepareControlDataParam;
            {
                prepareControlDataParam._parentIDOverride = parentWindowData.getID();
                prepareControlDataParam._autoComputedDisplaySize = Float2(radius * 2.0f);
                prepareControlDataParam._clipRectUsage = ClipRectUsage::ParentsOwn;
            }
            prepareControlData(controlData, prepareControlDataParam);
            
            Rendering::Color controlColor;
            const bool isClicked = processClickControl(controlData, color, color.scaledRgb(1.5f), color.scaledRgb(0.75f), controlColor);

            const Float4& controlCenterPosition = controlData.getControlCenterPosition();
            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            rendererContext.setClipRect(controlData.getClipRects()._forMe);
            rendererContext.setColor(controlColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawCircle(radius);

            return isClicked;
        }

        void GUIContext::makeTooltipWindow(const ControlID parentControlID, const wchar_t* const tooltipText, const Float2& position)
        {
            static constexpr ControlType controlType = ControlType::TooltipWindow;
            static constexpr float kTooltipFontScale = kFontScaleC;
            const float tooltipWindowPadding = 8.0f;

            const ControlID controlID = issueControlID(parentControlID, controlType, L"TOOLTIPWINDOW", tooltipText);
            
            ControlData& controlData = accessControlData(controlID);
            PrepareControlDataParam prepareControlDataParam;
            {
                const float tooltipTextWidth = computeTextWidth(tooltipText, StringUtil::length(tooltipText)) * kTooltipFontScale;
                prepareControlDataParam._autoComputedDisplaySize = Float2(tooltipTextWidth + tooltipWindowPadding * 2.0f, _fontSize * kTooltipFontScale + tooltipWindowPadding);
                prepareControlDataParam._desiredPositionInParent = position;
                prepareControlDataParam._alwaysResetParent = true;
                prepareControlDataParam._alwaysResetPosition = true;
                prepareControlDataParam._parentIDOverride = _controlInteractionStateSet.getTooltipParentWindowID();
                prepareControlDataParam._clipRectUsage = ClipRectUsage::ParentsOwn;
            }
            _controlMetaStateSet.nextOffAutoPosition();
            prepareControlData(controlData, prepareControlDataParam);
            
            Rendering::Color dummyColor;
            processShowOnlyControl(controlData, dummyColor);

            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(RendererContextLayer::TopMost);
            rendererContext.setClipRect(controlData.getClipRects()._forMe);
            
            const Float4& controlCenterPosition = controlData.getControlCenterPosition();
            rendererContext.setColor(getNamedColor(NamedColor::TooltipBackground));
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(controlData._size, (kDefaultRoundnessInPixel / controlData._size.minElement()) * 0.75f, 0.0f, 0.0f);

            const Float4& textPosition = Float4(controlData._position._x, controlData._position._y, 0.0f, 1.0f) + Float4(tooltipWindowPadding, prepareControlDataParam._autoComputedDisplaySize._y * 0.5f, 0.0f, 0.0f);
            rendererContext.setClipRect(controlData.getClipRects()._forMe);
            rendererContext.setTextColor(getNamedColor(NamedColor::DarkFont));
            rendererContext.drawDynamicText(tooltipText, textPosition, 
                Rendering::FontRenderingOption(Rendering::TextRenderDirectionHorz::Rightward, Rendering::TextRenderDirectionVert::Centered, kTooltipFontScale));
        }

        const ControlID GUIContext::issueControlID(const ControlID parentControlID, const ControlType controlType, const wchar_t* const identifier, const wchar_t* const text) noexcept
        {
            const ControlID controlID = _generateControlIDXXX(parentControlID, controlType, identifier);
            return _createControlDataInternalXXX(controlID, controlType, text);
        }

        const ControlID GUIContext::issueControlID(const char* const file, const int line, const ControlType controlType, const wchar_t* const text) noexcept
        {
            const ControlID controlID = _generateControlIDXXX(file, line, controlType);
            return _createControlDataInternalXXX(controlID, controlType, text);
        }

        const ControlID GUIContext::_createControlDataInternalXXX(const ControlID& controlID, const ControlType controlType, const wchar_t* const text) noexcept
        {
            auto found = _controlIDMap.find(controlID);
            if (found.isValid() == false)
            {
                const ControlData& stackTopControlData = getControlStackTopXXX();
                ControlData newControlData{ controlID, stackTopControlData.getID(), controlType };
                if (text != nullptr)
                {
                    newControlData._text = text;
                }

                _controlIDMap.insert(controlID, std::move(newControlData));
            }

            ControlData& controlData = _controlIDMap.at(controlID);
            if (controlData._updateCount < 3)
            {
                ++controlData._updateCount;
            }
            return controlID;
        }

        const ControlID GUIContext::_generateControlIDXXX(const ControlID& parentControlID, const ControlType controlType, const wchar_t* const identifier) const noexcept
        {
            static StringW idWstring;
            idWstring.clear();
            idWstring.append(StringUtil::convertToStringW(parentControlID.getRawValue()));
            idWstring.append(StringUtil::convertToStringW(static_cast<uint16>(controlType)));
            idWstring.append(identifier);
            return ControlID(computeHash(idWstring.c_str()));
        }

        const ControlID GUIContext::_generateControlIDXXX(const char* const file, const int line, const ControlType controlType) const noexcept
        {
            static StringW idWstring;
            idWstring.clear();
            StringUtil::convertStringAToStringW(file, idWstring);
            idWstring.append(line);
            idWstring.append(StringUtil::convertToStringW(static_cast<uint16>(controlType)));
            return ControlID(computeHash(idWstring.c_str()));
        }

        const ControlData& GUIContext::getParentWindowControlData(const ControlData& controlData) const noexcept
        {
            return getParentWindowControlDataInternal(controlData.getParentID());
        }

        const ControlData& GUIContext::getParentWindowControlDataInternal(const ControlID& id) const noexcept
        {
            if (id.isValid() == false)
            {
                return _rootControlData;
            }

            const ControlData& controlData = getControlData(id);
            if (controlData.getControlType() == ControlType::Window)
            {
                return controlData;
            }

            return getParentWindowControlDataInternal(controlData.getParentID());
        }

        const float GUIContext::getCurrentAvailableDisplaySizeX() const noexcept
        {
            const ControlData& parentControlData = getControlStackTopXXX();
            const float maxDisplaySizeX = parentControlData._size._x - ((_controlMetaStateSet.getNextUseAutoPosition() == true) 
                ? parentControlData.getInnerPadding().left() * 2.0f 
                : 0.0f);
            return maxDisplaySizeX;
        }

        const float GUIContext::getCurrentSameLineIntervalX() const noexcept
        {
            const float intervalX = (_controlMetaStateSet.getNextUseInterval() == true) ? kDefaultIntervalX : 0.0f;
            return intervalX;
        }

        const bool GUIContext::isThisControlPressed() const noexcept
        {
            return _controlInteractionStateSet.isControlPressed(getControlStackTopXXX());
        }

        const ControlAccessData& GUIContext::getThisControlAccessData() const noexcept
        {
            return getControlStackTopXXX()._controlAccessData;
        }

        const bool GUIContext::isFocusedControlInputBox() const noexcept
        {
            return (_controlInteractionStateSet.hasFocusedControl()) ? getControlData(_controlInteractionStateSet.getFocusedControlID()).isInputBoxType() : false;
        }

        void GUIContext::setControlFocused(const ControlData& controlData) noexcept
        {
            if (controlData._option._isFocusable == true)
            {
                if (controlData._option._needDoubleClickToFocus == true)
                {
                    if (_mouseStates.isDoubleClicked(Platform::MouseButton::Left) == true)
                    {
                        _controlInteractionStateSet.setControlFocused(controlData);
                    }
                }
                else
                {
                    _controlInteractionStateSet.setControlFocused(controlData);
                }
            }
        }

        void GUIContext::setControlHovered(const ControlData& controlData) noexcept
        {
            _controlInteractionStateSet.setControlHovered(controlData);
        }

        void GUIContext::setControlPressed(const ControlData& controlData) noexcept
        {
            if (_controlInteractionStateSet.setControlPressed(controlData) == true)
            {
                const ControlData& closestFocusableAncestor = getClosestFocusableAncestorControlInclusive(controlData);
                setControlFocused(closestFocusableAncestor);
            }
        }
        
        void GUIContext::setControlClicked(const ControlData& controlData) noexcept
        {
            if (_controlInteractionStateSet.setControlClicked(controlData) == true)
            {
                const ControlData& closestFocusableAncestor = getClosestFocusableAncestorControlInclusive(controlData);
                setControlFocused(closestFocusableAncestor);
            }
        }

        void GUIContext::prepareControlData(ControlData& controlData, const PrepareControlDataParam& prepareControlDataParam) noexcept
        {
            const bool isNewData = controlData._size.isNan();
            if ((isNewData == true) || (prepareControlDataParam._alwaysResetParent == true))
            {
                const ControlData& stackTopControlData = getControlStackTopXXX();
                const ControlID& parentID = (prepareControlDataParam._parentIDOverride.isValid() == true) ? prepareControlDataParam._parentIDOverride : stackTopControlData.getID();
                controlData.setParentID(parentID);

                if (isNewData == true)
                {
                    controlData._resizingMask = prepareControlDataParam._initialResizingMask;

                    // 중요!!!
                    controlData.setAllClipRects(_clipRectFullScreen);
                }
            }

            ControlData& parentControlData = accessControlData(controlData.getParentID());
            const bool computeSize = (isNewData == true || prepareControlDataParam._alwaysResetDisplaySize == true);
            controlData.updatePerFrame(prepareControlDataParam, parentControlData, _controlMetaStateSet, getCurrentAvailableDisplaySizeX(), computeSize);
            
            // 부모와 동일한 RendererContextLayer 가 되도록!
            controlData._rendererContextLayer = parentControlData._rendererContextLayer;
            
            // Position, Parent offset, Parent child at, Parent content area size
            const bool useAutoPosition = (_controlMetaStateSet.getNextUseAutoPosition() == true);
            if (useAutoPosition == true)
            {
                Float2& parentControlChildAt = const_cast<Float2&>(parentControlData.getChildAt());
                Float2& parentControlNextChildOffset = parentControlData._perFrameData._nextChildOffset;
                const float parentControlPreviousNextChildOffsetX = parentControlNextChildOffset._x;

                const bool isSameLineAsPreviousControl = (_controlMetaStateSet.getNextSameLine() == true);
                if (isSameLineAsPreviousControl == true)
                {
                    const float intervalX = getCurrentSameLineIntervalX();
                    parentControlChildAt._x += (parentControlNextChildOffset._x + intervalX);

                    parentControlNextChildOffset = controlData._size;
                }
                else
                {
                    parentControlChildAt._x = parentControlData._position._x + parentControlData.getInnerPadding().left() + parentControlData._childDisplayOffset._x; // @중요
                    parentControlChildAt._x += parentControlData.getDockZoneSize(DockZone::LeftSide)._x;

                    parentControlChildAt._y += parentControlNextChildOffset._y;

                    parentControlNextChildOffset = controlData._size;
                }

                const bool addIntervalY = (_controlMetaStateSet.getNextUseAutoPosition() == true && _controlMetaStateSet.getNextUseInterval() == true);
                if (addIntervalY == true)
                {
                    parentControlNextChildOffset._y += kDefaultIntervalY;
                }

                // Parent content area size
                Float2& parentControlContentAreaSize = parentControlData._perFrameData._contentAreaSize;
                if (prepareControlDataParam._ignoreMeForContentAreaSize == false)
                {
                    if (_controlMetaStateSet.getNextSameLine())
                    {
                        if (parentControlContentAreaSize._x == 0.0f)
                        {
                            // 최초 isSameLine() 시 바로 왼쪽 컨트롤의 크기도 추가해줘야 한다!
                            parentControlContentAreaSize._x = parentControlPreviousNextChildOffsetX;
                        }
                        parentControlContentAreaSize._x += controlData._size._x + kDefaultIntervalX;
                    }

                    parentControlContentAreaSize._y += (_controlMetaStateSet.getNextSameLine()) ? 0.0f : controlData._size._y;
                    parentControlContentAreaSize._y += (addIntervalY) ? kDefaultIntervalY : 0.0f;
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
                        controlData._position += _controlMetaStateSet.getNextDesiredPosition();
                    }
                    else
                    {
                        controlData._position += prepareControlDataParam._desiredPositionInParent;
                    }
                }
            }

            // Child at
            computeControlChildAt(controlData);
        }

        void GUIContext::computeControlChildAt(ControlData& controlData) noexcept
        {
            const MenuBarType currentMenuBarType = controlData._controlValue._commonData._menuBarType;
            Float2& controlChildAt = const_cast<Float2&>(controlData.getChildAt());
            controlChildAt = controlData._position + controlData._childDisplayOffset +
                ((_controlMetaStateSet.getNextUseAutoPosition() == true)
                    ? Float2(controlData.getInnerPadding().left(), controlData.getInnerPadding().top())
                    : Float2::kZero) +
                Float2(0.0f, (MenuBarType::None != currentMenuBarType) ? kMenuBarBaseSize._y : 0.0f);

            const DockZoneData& dockZoneDataTopSide = controlData.getDockZoneData(DockZone::TopSide);
            if (dockZoneDataTopSide.hasDockedControls() == true)
            {
                // 맨 처음 Child Control 만 내려주면 된다!!
                controlChildAt._y += controlData.getDockZoneSize(DockZone::TopSide)._y + controlData.getInnerPadding().top();
            }
        }

        const bool GUIContext::processClickControl(ControlData& controlData, const Rendering::Color& normalColor, const Rendering::Color& hoverColor, const Rendering::Color& pressedColor, Rendering::Color& outBackgroundColor) noexcept
        {
            processControlInteractionInternal(controlData);

            outBackgroundColor = normalColor;

            const bool isClicked = _controlInteractionStateSet.isControlClicked(controlData);
            if (_controlInteractionStateSet.isControlHovered(controlData) == true)
            {
                outBackgroundColor = hoverColor;
            }
            if (_controlInteractionStateSet.isControlPressed(controlData) == true || isClicked == true)
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

        const bool GUIContext::processFocusControl(ControlData& controlData, const Rendering::Color& focusedColor, const Rendering::Color& nonFocusedColor, Rendering::Color& outBackgroundColor) noexcept
        {
            processControlInteractionInternal(controlData, false);

            const ControlID& controlID = (controlData._delegateControlID.isValid() == true) ? controlData._delegateControlID : controlData.getID();

            // Check new focus
            if (_draggedControlID.isValid() == false && _resizedControlID.isValid() == false && controlData._option._isFocusable == true &&
                (_mouseStates.isButtonDownThisFrame(Platform::MouseButton::Left) == true
                    && (_controlInteractionStateSet.isControlPressed(controlData) == true || _controlInteractionStateSet.isControlClicked(controlData) == true)))
            {
                if (controlData._option._needDoubleClickToFocus == true)
                {
                    if (_mouseStates.isDoubleClicked(Platform::MouseButton::Left) == true)
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

            return _controlInteractionStateSet.isControlFocused(controlData);
        }

        void GUIContext::processShowOnlyControl(ControlData& controlData, Rendering::Color& outBackgroundColor, const bool setMouseInteractionDone) noexcept
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

        const bool GUIContext::processScrollableControl(ControlData& controlData, const Rendering::Color& normalColor, const Rendering::Color& dragColor, Rendering::Color& outBackgroundColor) noexcept
        {
            processControlInteractionInternal(controlData);

            outBackgroundColor = normalColor;

            const bool isHovered = _controlInteractionStateSet.isControlHovered(controlData);
            const bool isPressed = _controlInteractionStateSet.isControlPressed(controlData);
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
        
        const bool GUIContext::processToggleControl(ControlData& controlData, const Rendering::Color& normalColor, const Rendering::Color& hoverColor, const Rendering::Color& toggledColor, Rendering::Color& outBackgroundColor) noexcept
        {
            processControlInteractionInternal(controlData);

            const bool isClicked = _controlInteractionStateSet.isControlClicked(controlData);
            if (isClicked == true)
            {
                controlData._controlValue._booleanData.toggle();
            }

            const bool isToggled = controlData._controlValue._booleanData.get();
            const bool isHovered = _controlInteractionStateSet.isControlHovered(controlData);
            outBackgroundColor = (isToggled == true) ? toggledColor : (isHovered == true) ? hoverColor : normalColor;

            if (needToColorFocused(controlData) == false)
            {
                outBackgroundColor.scaleA(kDefaultOutOfFocusAlpha);
            }

            processControlCommon(controlData);
            return isClicked;
        }
        
        void GUIContext::processControlInteractionInternal(ControlData& controlData, const bool setMouseInteractionDone) noexcept
        {
            const ControlID& controlID = controlData.getID();
            if (isInteractingInternal(controlData) == false || _controlInteractionStateSet.isMouseInteractionDoneThisFrame() == true)
            {
                _controlInteractionStateSet.resetHoverIf(controlData);
                _controlInteractionStateSet.resetPressIf(controlData);
                return;
            }

            const ControlData& parentControlData = getControlData(controlData.getParentID());
            const bool isMouseInParentInteractionArea = ControlCommonHelpers::isInControlInteractionArea(_mouseStates.getPosition(), parentControlData);
            const bool isMouseInInteractionArea = ControlCommonHelpers::isInControlInteractionArea(_mouseStates.getPosition(), controlData);
            const bool meetsAreaCondition = (controlData._option._isInteractableOutsideParent == true || isMouseInParentInteractionArea == true) && (isMouseInInteractionArea == true);
            const bool meetsInteractionCondition = (shouldInteract(_mouseStates.getPosition(), controlData) == true || controlData.isRootControl() == true);
            if (meetsAreaCondition == true && meetsInteractionCondition == true)
            {
                // Hovered (at least)

                if (setMouseInteractionDone == true)
                {
                    _controlInteractionStateSet.setMouseInteractionDoneThisFrame();
                }

                if (_controlInteractionStateSet.isControlHovered(controlData) == false && controlData._option._isFocusable == false)
                {
                    setControlHovered(controlData);
                }

                // Click Event 가 발생했을 때도 Pressed 상태 유지!
                if (_mouseStates.isButtonDownUp(Platform::MouseButton::Left) == false 
                    && _mouseStates.isButtonDown(Platform::MouseButton::Left) == false)
                {
                    _controlInteractionStateSet.resetPressIf(controlData);
                }
                
                // Pressed (Mouse down)
                const bool isMouseDownInInteractionArea = ControlCommonHelpers::isInControlInteractionArea(_mouseStates.getButtonDownPosition(), controlData);
                if (isMouseDownInInteractionArea == true)
                {
                    if (_mouseStates.isButtonDownThisFrame(Platform::MouseButton::Left) == true 
                        || _mouseStates.isDoubleClicked(Platform::MouseButton::Left) == true)
                    {
                        setControlPressed(controlData);
                    }

                    if (_mouseStates.isButtonDownThisFrame(Platform::MouseButton::Right) == true)
                    {
                        _viewerTargetControlID = controlData.getID();
                    }

                    // Clicked (only in interaction area)
                    if (_mouseStates.isButtonDownUp(Platform::MouseButton::Left) == true)
                    {
                        setControlClicked(controlData);
                    }
                }
            }
            else
            {
                // Not interacting

                _controlInteractionStateSet.resetHoverIf(controlData);
                _controlInteractionStateSet.resetPressIf(controlData);
            }
        }

        void GUIContext::processControlCommon(ControlData& controlData) noexcept
        {
            checkControlResizing(controlData);
            checkControlHoveringForTooltip(controlData);

            processControlResizingInternal(controlData);
            processControlDraggingInternal(controlData);
            processControlDockingInternal(controlData);

            _controlMetaStateSet.resetPerFrame();
        }

        void GUIContext::checkControlResizing(ControlData& controlData) noexcept
        {
            if (_resizedControlID.isValid() == false && isInteractingInternal(controlData) == true)
            {
                Window::CursorType newCursorType;
                ResizingMask resizingMask;
                const bool isResizable = controlData.isResizable();
                if (controlData.isResizable() == true 
                    && ControlCommonHelpers::isInControlBorderArea(_mouseStates.getPosition(), controlData, newCursorType, resizingMask, _resizingMethod) == true)
                {
                    if (controlData._resizingMask.overlaps(resizingMask) == true)
                    {
                        _mouseStates._cursorType = newCursorType;

                        _controlInteractionStateSet.setMouseInteractionDoneThisFrame();
                    }
                }
            }
        }

        void GUIContext::checkControlHoveringForTooltip(ControlData& controlData) noexcept
        {
            const bool isHovered = _controlInteractionStateSet.isControlHovered(controlData);
            if (_controlMetaStateSet.getNextTooltipText() != nullptr && isHovered == true
                && _controlInteractionStateSet.isHoveringMoreThan(1000) == true)
            {
                _controlInteractionStateSet.setTooltipData(_mouseStates, _controlMetaStateSet.getNextTooltipText(), getParentWindowControlData(controlData).getID());
            }
        }

        void GUIContext::processControlResizingInternal(ControlData& controlData) noexcept
        {
            ControlData& changeTargetControlData = (controlData._delegateControlID.isValid() == false) ? controlData : accessControlData(controlData._delegateControlID);
            const bool isResizing = isControlBeingResized(changeTargetControlData);
            if (isResizing == true)
            {
                if (_isResizeBegun == true)
                {
                    _resizedControlInitialPosition = changeTargetControlData._position;
                    _resizedControlInitialSize = changeTargetControlData._size;

                    _isResizeBegun = false;
                }

                Float2& changeTargetControlDisplaySize = const_cast<Float2&>(changeTargetControlData._size);

                const Float2 mouseDragDelta = _mouseStates.getMouseDragDelta();
                const float flipHorz = (_resizingMethod == ResizingMethod::RepositionHorz || _resizingMethod == ResizingMethod::RepositionBoth) ? -1.0f : +1.0f;
                const float flipVert = (_resizingMethod == ResizingMethod::RepositionVert || _resizingMethod == ResizingMethod::RepositionBoth) ? -1.0f : +1.0f;
                const Float2 resizeMinSize = changeTargetControlData.getResizeMinSize();
                if (_mouseStates.isCursor(Window::CursorType::SizeVert) == false)
                {
                    const float newPositionX = _resizedControlInitialPosition._x - mouseDragDelta._x * flipHorz;
                    const float newSizeX = _resizedControlInitialSize._x + mouseDragDelta._x * flipHorz;
                    if (resizeMinSize._x < newSizeX)
                    {
                        if (flipHorz < 0.0f)
                        {
                            changeTargetControlData._position._x = newPositionX;
                        }
                        changeTargetControlDisplaySize._x = newSizeX;
                    }
                }
                if (_mouseStates.isCursor(Window::CursorType::SizeHorz) == false)
                {
                    const float newPositionY = _resizedControlInitialPosition._y - mouseDragDelta._y * flipVert;
                    const float newSizeY = _resizedControlInitialSize._y + mouseDragDelta._y * flipVert;
                    if (resizeMinSize._y < newSizeY)
                    {
                        if (flipVert < 0.0f)
                        {
                            changeTargetControlData._position._y = newPositionY;
                        }
                        changeTargetControlDisplaySize._y = newSizeY;
                    }
                }

                if (changeTargetControlData.isDocking() == true)
                {
                    // 내가 Docking 중인 컨트롤이라면 Dock Control 의 Dock 크기도 같이 변경해줘야 한다.

                    const ControlID& dockControlID = changeTargetControlData.getDockControlID();
                    ControlData& dockControlData = accessControlData(dockControlID);
                    dockControlData.setDockZoneSize(changeTargetControlData._dockContext._lastDockZone, changeTargetControlDisplaySize);
                    updateDockZoneData(dockControlID);
                }
                else if (changeTargetControlData._dockContext._dockingControlType == DockingControlType::Dock 
                    || changeTargetControlData._dockContext._dockingControlType == DockingControlType::DockerDock)
                {
                    // 내가 DockHosting 중일 수 있음

                    updateDockZoneData(changeTargetControlData.getID());
                }

                _controlInteractionStateSet.setMouseInteractionDoneThisFrame();
            }
        }

        void GUIContext::processControlDraggingInternal(ControlData& controlData) noexcept
        {
            const bool isDragging = isControlBeingDragged(controlData);
            if (isDragging == false)
            {
                return;
            }
            
            ControlData& targetControlData = (controlData._delegateControlID.isValid() == false) ? controlData : accessControlData(controlData._delegateControlID);
            if (_isDragBegun == true)
            {
                _draggedControlInitialPosition = targetControlData._position;
                _isDragBegun = false;
            }

            const Float2 positionOld = targetControlData._position;
            const Float2& mouseDragDelta = _mouseStates.getMouseDragDelta();
            const Float2 positionCandidate = _draggedControlInitialPosition + mouseDragDelta;
            const Rect& positionConstraints = targetControlData._positionConstraintsForDragging;
            targetControlData._position = (positionConstraints.isNan()) ? positionCandidate : positionConstraints.bound(positionCandidate);

            if (targetControlData.isDocking() == true)
            {
                // Docking 중이었으면 마우스로 바로 옮길 수 없도록!! (Dock 에 좀 더 오래 붙어있도록)

                targetControlData._position = positionOld;

                ControlData& dockControlData = accessControlData(targetControlData.getDockControlID());
                DockZoneData& dockZoneData = dockControlData.getDockZoneData(targetControlData._dockContext._lastDockZone);
                const Float2& dockZoneSize = dockControlData.getDockZoneSize(targetControlData._dockContext._lastDockZone);
                const Float2& dockZonePosition = dockControlData.getDockZonePosition(targetControlData._dockContext._lastDockZone);
                const Rect dockRect{ dockZonePosition, dockZoneSize };
                bool needToDisconnectFromDock = true;
                const Float2& mousePosition = _mouseStates.getPosition();
                if (dockRect.contains(mousePosition))
                {
                    needToDisconnectFromDock = false;

                    const Rect dockTitleBarAreaRect{ dockZonePosition, Float2(dockZoneSize._x, kTitleBarBaseThickness) };
                    if (dockTitleBarAreaRect.contains(mousePosition))
                    {
                        const float titleBarOffset = mousePosition._x - dockTitleBarAreaRect.left();
                        const int32 targetDockedControlindex = dockZoneData.getDockedControlIndexByMousePosition(titleBarOffset);
                        if (targetDockedControlindex >= 0)
                        {
                            const int32 originalDockedControlIndex = dockZoneData.getDockedControlIndex(targetControlData.getID());
                            if (originalDockedControlIndex != targetDockedControlindex)
                            {
                                dockZoneData.swapDockedControlsXXX(originalDockedControlIndex, targetDockedControlindex);
                                dockZoneData._focusedDockedControlIndex = targetDockedControlindex;

                                _taskWhenMouseUp.setUpdateDockZoneData(dockControlData.getID());
                                updateDockZoneData(dockControlData.getID(), false);
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

                    undock(targetControlData.getID());
                }
            }
            else
            {
                // Set delta position
                targetControlData._perFrameData._deltaPosition = targetControlData._position - positionOld;
            }

            _controlInteractionStateSet.setMouseInteractionDoneThisFrame();
        }

        void GUIContext::processControlDockingInternal(ControlData& controlData) noexcept
        {
            ControlData& changeTargetControlData = (controlData._delegateControlID.isValid() == false) ? controlData : accessControlData(controlData._delegateControlID);
            const bool isDragging = isControlBeingDragged(controlData);
            
            static constexpr Rendering::Color color = Rendering::Color(100, 110, 160);
            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(RendererContextLayer::TopMost);
            std::function fnRenderDockingBox = [&](const Rect& boxRect, const ControlData& parentControlData)
            {
                const Float4& parentControlCenterPosition = parentControlData.getControlCenterPosition();
                Float4 renderPosition = parentControlCenterPosition;
                renderPosition._x = boxRect.center()._x;
                renderPosition._y = boxRect.center()._y;
                rendererContext.setClipRect(parentControlData.getClipRects()._forMe);

                const bool isMouseInBoxRect = boxRect.contains(_mouseStates.getPosition());
                rendererContext.setColor(((isMouseInBoxRect == true) ? color.scaledRgb(1.5f) : color));
                rendererContext.setPosition(renderPosition);
                rendererContext.drawRectangle(boxRect.size(), kDockingInteractionDisplayBorderThickness, 0.0f);
            };
            std::function fnRenderPreview = [&](const Rect& previewRect)
            {
                rendererContext.setClipRect(0);
                rendererContext.setColor(color.scaledA(0.5f));
                rendererContext.setPosition(Float4(previewRect.center()._x, previewRect.center()._y, 0.0f, 1.0f));
                rendererContext.drawRectangle(previewRect.size(), 0.0f, 0.0f);
            };

            ControlData& parentControlData = accessControlData(changeTargetControlData.getParentID());
            if ((changeTargetControlData.hasChildWindow() == false) 
                && (changeTargetControlData._dockContext._dockingControlType == DockingControlType::Docker 
                    || changeTargetControlData._dockContext._dockingControlType == DockingControlType::DockerDock) 
                && (parentControlData._dockContext._dockingControlType == DockingControlType::Dock 
                    || parentControlData._dockContext._dockingControlType == DockingControlType::DockerDock) 
                && ControlCommonHelpers::isInControlInteractionArea(_mouseStates.getPosition(), changeTargetControlData) == true)
            {
                const Float4& parentControlCenterPosition = parentControlData.getControlCenterPosition();
                const float previewShortLengthMax = 160.0f;
                const float previewShortLength = min(parentControlData._size._x * 0.5f, previewShortLengthMax);

                Rect interactionBoxRect;
                Rect previewRect;
                
                // 초기화
                if (_mouseStates.isButtonDownUp(Platform::MouseButton::Left) == false)
                {
                    changeTargetControlData._dockContext._lastDockZoneCandidate = DockZone::COUNT;
                }

                // Top
                {
                    interactionBoxRect.left(parentControlCenterPosition._x - kDockingInteractionLong * 0.5f);
                    interactionBoxRect.right(interactionBoxRect.left() + kDockingInteractionLong);
                    interactionBoxRect.top(parentControlData._position._y + parentControlData.getDockOffsetSize()._y + kDockingInteractionOffset);
                    interactionBoxRect.bottom(interactionBoxRect.top() + kDockingInteractionShort);

                    const Float2& dockZonePosition = parentControlData.getDockZonePosition(DockZone::TopSide);
                    previewRect.position(dockZonePosition);
                    previewRect.right(previewRect.left() + parentControlData._size._x);
                    previewRect.bottom(previewRect.top() + previewShortLength);

                    if (isDragging == true)
                    {
                        fnRenderDockingBox(interactionBoxRect, parentControlData);

                        DockZoneData& parentControlDockZoneData = parentControlData.getDockZoneData(DockZone::TopSide);
                        if (changeTargetControlData._dockContext._lastDockZoneCandidate == DockZone::COUNT 
                            && interactionBoxRect.contains(_mouseStates.getPosition()) == true)
                        {
                            changeTargetControlData._dockContext._lastDockZoneCandidate = DockZone::TopSide;

                            if (parentControlDockZoneData.isRawDockSizeSet() == true)
                            {
                                previewRect.right(previewRect.left() + parentControlData.getDockZoneSize(DockZone::TopSide)._x);
                                previewRect.bottom(previewRect.top() + parentControlData.getDockZoneSize(DockZone::TopSide)._y);

                                fnRenderPreview(previewRect);
                            }
                            else
                            {
                                parentControlDockZoneData.setRawDockSize(previewRect.size());

                                fnRenderPreview(previewRect);
                            }
                        }
                    }
                }

                // Bottom
                {
                    interactionBoxRect.left(parentControlCenterPosition._x - kDockingInteractionLong * 0.5f);
                    interactionBoxRect.right(interactionBoxRect.left() + kDockingInteractionLong);
                    interactionBoxRect.bottom(parentControlData._position._y + parentControlData._size._y - kDockingInteractionOffset);
                    interactionBoxRect.top(interactionBoxRect.bottom() - kDockingInteractionShort);

                    const Float2& dockZonePosition = parentControlData.getDockZonePosition(DockZone::BottomSide);
                    previewRect.position(dockZonePosition);
                    previewRect.right(previewRect.left() + parentControlData._size._x);
                    previewRect.bottom(previewRect.top() + previewShortLength);

                    if (isDragging == true)
                    {
                        fnRenderDockingBox(interactionBoxRect, parentControlData);

                        DockZoneData& parentControlDockZoneData = parentControlData.getDockZoneData(DockZone::BottomSide);
                        if (changeTargetControlData._dockContext._lastDockZoneCandidate == DockZone::COUNT 
                            && interactionBoxRect.contains(_mouseStates.getPosition()) == true)
                        {
                            changeTargetControlData._dockContext._lastDockZoneCandidate = DockZone::BottomSide;

                            if (parentControlDockZoneData.isRawDockSizeSet() == true)
                            {
                                previewRect.right(previewRect.left() + parentControlData.getDockZoneSize(DockZone::BottomSide)._x);
                                previewRect.bottom(previewRect.top() + parentControlData.getDockZoneSize(DockZone::BottomSide)._y);

                                fnRenderPreview(previewRect);
                            }
                            else
                            {
                                parentControlDockZoneData.setRawDockSize(previewRect.size());

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

                    const Float2& dockZonePosition = parentControlData.getDockZonePosition(DockZone::LeftSide);
                    previewRect.position(dockZonePosition);
                    previewRect.right(previewRect.left() + previewShortLength);
                    previewRect.bottom(previewRect.top() + parentControlData._size._y - parentControlData.getDockOffsetSize()._y);

                    if (isDragging == true)
                    {
                        fnRenderDockingBox(interactionBoxRect, parentControlData);

                        DockZoneData& parentControlDockZoneData = parentControlData.getDockZoneData(DockZone::LeftSide);
                        if (changeTargetControlData._dockContext._lastDockZoneCandidate == DockZone::COUNT 
                            && interactionBoxRect.contains(_mouseStates.getPosition()) == true)
                        {
                            changeTargetControlData._dockContext._lastDockZoneCandidate = DockZone::LeftSide;
                            
                            if (parentControlDockZoneData.isRawDockSizeSet() == true)
                            {
                                previewRect.right(previewRect.left() + parentControlData.getDockZoneSize(DockZone::LeftSide)._x);
                                previewRect.bottom(previewRect.top() + parentControlData.getDockZoneSize(DockZone::LeftSide)._y);

                                fnRenderPreview(previewRect);
                            }
                            else
                            {
                                parentControlDockZoneData.setRawDockSize(previewRect.size());

                                fnRenderPreview(previewRect);
                            }
                        }
                    }
                }

                // Right
                {
                    interactionBoxRect.right(parentControlData._position._x + parentControlData._size._x - kDockingInteractionOffset);
                    interactionBoxRect.left(interactionBoxRect.right() - kDockingInteractionShort);
                    interactionBoxRect.top(parentControlCenterPosition._y - kDockingInteractionLong * 0.5f);
                    interactionBoxRect.bottom(interactionBoxRect.top() + kDockingInteractionLong);

                    const Float2& dockZonePosition = parentControlData.getDockZonePosition(DockZone::RightSide);
                    previewRect.position(dockZonePosition);
                    previewRect.right(previewRect.left() + previewShortLength);
                    previewRect.bottom(previewRect.top() + parentControlData._size._y - parentControlData.getDockOffsetSize()._y);

                    if (isDragging == true)
                    {
                        fnRenderDockingBox(interactionBoxRect, parentControlData);

                        DockZoneData& parentControlDockZoneData = parentControlData.getDockZoneData(DockZone::RightSide);
                        if (changeTargetControlData._dockContext._lastDockZoneCandidate == DockZone::COUNT 
                            && interactionBoxRect.contains(_mouseStates.getPosition()) == true)
                        {
                            changeTargetControlData._dockContext._lastDockZoneCandidate = DockZone::RightSide;

                            if (parentControlDockZoneData.isRawDockSizeSet() == true)
                            {
                                previewRect.right(previewRect.left() + parentControlData.getDockZoneSize(DockZone::RightSide)._x);
                                previewRect.bottom(previewRect.top() + parentControlData.getDockZoneSize(DockZone::RightSide)._y);

                                fnRenderPreview(previewRect);
                            }
                            else
                            {
                                parentControlDockZoneData.setRawDockSize(previewRect.size());

                                fnRenderPreview(previewRect);
                            }
                        }
                    }
                }

                if (_mouseStates.isButtonDownUp(Platform::MouseButton::Left) == true 
                    && changeTargetControlData._dockContext._lastDockZoneCandidate != DockZone::COUNT)
                {
                    if (changeTargetControlData.isDocking() == false)
                    {
                        // Docking 시작.

                        dock(changeTargetControlData.getID(), parentControlData.getID());

                        _draggedControlID.reset();
                    }
                }
            }
        }

        void GUIContext::dock(const ControlID& dockedControlID, const ControlID& dockControlID) noexcept
        {
            ControlData& dockedControlData = accessControlData(dockedControlID);
            dockedControlData.swapDockingStateContext();

            if (dockedControlData._dockContext._lastDockZone != dockedControlData._dockContext._lastDockZoneCandidate)
            {
                dockedControlData._dockContext._lastDockZone = dockedControlData._dockContext._lastDockZoneCandidate;

                dockedControlData._dockContext._lastDockZoneCandidate = DockZone::COUNT;
            }

            ControlData& dockControlData = accessControlData(dockControlID);
            DockZoneData& parentControlDockZoneData = dockControlData.getDockZoneData(dockedControlData._dockContext._lastDockZone);
            if (dockedControlData._dockContext._lastDockZone != dockedControlData._dockContext._lastDockZoneCandidate)
            {
                dockedControlData._size = dockControlData.getDockZoneSize(dockedControlData._dockContext._lastDockZone);
            }
            parentControlDockZoneData._dockedControlIDArray.push_back(dockedControlData.getID());

            dockedControlData._resizingMask = ResizingMask::fromDockZone(dockedControlData._dockContext._lastDockZone);
            dockedControlData._position = dockControlData.getDockZonePosition(dockedControlData._dockContext._lastDockZone);
            dockedControlData.connectToDock(dockControlID);

            parentControlDockZoneData._focusedDockedControlIndex = parentControlDockZoneData.getDockedControlIndex(dockedControlData.getID());

            updateDockZoneData(dockControlID);

            // 내가 Focus 였다면 Dock 을 가진 컨트롤로 옮기자!
            if (_controlInteractionStateSet.isControlFocused(dockedControlData) == true)
            {
                setControlFocused(dockControlData);
            }
        }

        void GUIContext::undock(const ControlID& dockedControlID) noexcept
        {
            ControlData& dockedControlData = accessControlData(dockedControlID);
            ControlData& dockControlData = accessControlData(dockedControlData.getDockControlID());
            DockZoneData& dockZoneData = dockControlData.getDockZoneData(dockedControlData._dockContext._lastDockZone);
            const uint32 changeTargetParentDockedControlCount = static_cast<uint32>(dockZoneData._dockedControlIDArray.size());
            int32 indexToErase = -1;
            for (uint32 iter = 0; iter < changeTargetParentDockedControlCount; ++iter)
            {
                if (dockZoneData._dockedControlIDArray[iter] == dockedControlData.getID())
                {
                    indexToErase = static_cast<int32>(iter);
                }
            }
            if (indexToErase >= 0)
            {
                dockZoneData._dockedControlIDArray.erase(indexToErase);
            }
            else
            {
                MINT_LOG_ERROR("김장원", "Docked Control 이 Parent 의 Child Array 에 없는 상황입니다!!!");
            }

            dockedControlData.swapDockingStateContext();

            _draggedControlInitialPosition = dockedControlData._position;
            setControlFocused(dockedControlData);

            const ControlID dockControlIDCopy = dockedControlData.getDockControlID();

            dockedControlData.disconnectFromDock();
            dockZoneData._focusedDockedControlIndex = min(dockZoneData._focusedDockedControlIndex, static_cast<int32>(dockZoneData._dockedControlIDArray.size() - 1));
            dockedControlData._dockContext._lastDockZoneCandidate = DockZone::COUNT;

            updateDockZoneData(dockControlIDCopy);
        }

        void GUIContext::updateDockZoneData(const ControlID& dockControlID, const bool updateWidthArray) noexcept
        {
            ControlData& dockControlData = accessControlData(dockControlID);
            for (DockZone dockZoneIter = static_cast<DockZone>(0); dockZoneIter != DockZone::COUNT;
                dockZoneIter = static_cast<DockZone>(static_cast<uint32>(dockZoneIter) + 1))
            {
                DockZoneData& dockZoneData = dockControlData.getDockZoneData(dockZoneIter);
                const uint32 dockedControlCount = static_cast<uint32>(dockZoneData._dockedControlIDArray.size());
                dockZoneData._dockedControlTitleBarOffsetArray.resize(dockedControlCount);
                dockZoneData._dockedControlTitleBarWidthArray.resize(dockedControlCount);

                float titleBarWidthSum = 0.0f;
                for (uint32 dockedControlIndex = 0; dockedControlIndex < dockedControlCount; ++dockedControlIndex)
                {
                    ControlData& dockedControlData = accessControlData(dockZoneData._dockedControlIDArray[dockedControlIndex]);
                    dockedControlData._size = dockControlData.getDockZoneSize(dockZoneIter);
                    dockedControlData._position = dockControlData.getDockZonePosition(dockZoneIter);
                    
                    const wchar_t* const title = dockedControlData._text.c_str();
                    const float titleBarWidth = computeTextWidth(title, StringUtil::length(title)) + 16.0f;
                    dockZoneData._dockedControlTitleBarOffsetArray[dockedControlIndex] = titleBarWidthSum;
                    if (updateWidthArray)
                    {
                        dockZoneData._dockedControlTitleBarWidthArray[dockedControlIndex] = titleBarWidth;
                    }
                    titleBarWidthSum += titleBarWidth;
                }
            }
        }

        const bool GUIContext::isInteractingInternal(const ControlData& controlData) const noexcept
        {
            if (_controlInteractionStateSet.hasFocusedControl() == true && isAncestorControlFocusedInclusiveXXX(controlData) == false)
            {
                // Focus 가 있는 Control 이 존재하지만 내가 Focus 는 아닌 경우

                Window::CursorType dummyCursorType;
                ResizingMethod dummyResizingMethod;
                ResizingMask dummyResizingMask;
                const ControlData& focusedControlData = getControlData(_controlInteractionStateSet.getFocusedControlID());
                if (ControlCommonHelpers::isInControlInteractionArea(_mouseStates.getPosition(), focusedControlData) == true 
                    || ControlCommonHelpers::isInControlBorderArea(_mouseStates.getPosition(), focusedControlData, dummyCursorType, dummyResizingMask, dummyResizingMethod) == true)
                {
                    // 마우스가 Focus Control 과 상호작용할 경우 나와는 상호작용하지 않는것으로 판단!!
                    return false;
                }
            }
            return true;
        }

        const bool GUIContext::isControlBeingDragged(const ControlData& controlData) const noexcept
        {
            if (_mouseStates.isButtonDown(Platform::MouseButton::Left) == false)
            {
                _draggedControlID.reset();
                return false;
            }

            if (_draggedControlID.isValid() == false)
            {
                if (_resizedControlID.isValid() == true || controlData._option._isDraggable == false || isInteractingInternal(controlData) == false)
                {
                    return false;
                }

                if (_mouseStates.isButtonDown(Platform::MouseButton::Left) == true
                    && ControlCommonHelpers::isInControlInteractionArea(_mouseStates.getPosition(), controlData) == true
                    && ControlCommonHelpers::isInControlInteractionArea(_mouseStates.getButtonDownPosition(), controlData) == true)
                {
                    // Drag 시작
                    _isDragBegun = true;
                    _draggedControlID = controlData.getID();
                    return true;
                }
            }
            else
            {
                if (controlData.getID() == _draggedControlID)
                {
                    return true;
                }
            }
            return false;
        }

        const bool GUIContext::isControlBeingResized(const ControlData& controlData) const noexcept
        {
            if (_mouseStates.isButtonDown(Platform::MouseButton::Left) == false)
            {
                _resizedControlID.reset();
                return false;
            }

            if (_resizedControlID.isValid() == false)
            {
                if (_draggedControlID.isValid() == true || controlData.isResizable() == false || isInteractingInternal(controlData) == false)
                {
                    return false;
                }

                Window::CursorType newCursorType;
                ResizingMask resizingMask;
                if (_mouseStates.isButtonDown(Platform::MouseButton::Left) == true
                    && ControlCommonHelpers::isInControlBorderArea(_mouseStates.getPosition(), controlData, newCursorType, resizingMask, _resizingMethod) == true
                    && ControlCommonHelpers::isInControlBorderArea(_mouseStates.getButtonDownPosition(), controlData, newCursorType, resizingMask, _resizingMethod) == true)
                {
                    if (controlData._resizingMask.overlaps(resizingMask) == true)
                    {
                        _resizedControlID = controlData.getID();
                        _isResizeBegun = true;
                        _mouseStates._cursorType = newCursorType;
                        return true;
                    }
                }
            }
            else
            {
                if (controlData.getID() == _resizedControlID)
                {
                    return true;
                }
            }
            return false;
        }
        
        const bool GUIContext::isAncestorControlFocusedInclusiveXXX(const ControlData& controlData) const noexcept
        {
            if (_controlInteractionStateSet.isControlFocused(controlData) == true)
            {
                return true;
            }
            return isAncestorControlFocused(controlData);
        }

        const bool GUIContext::isAncestorControlInclusive(const ControlData& controlData, const ControlID& ancestorCandidateID) const noexcept
        {
            return isAncestorControlRecursiveXXX(controlData.getID(), ancestorCandidateID);
        }

        const bool GUIContext::isAncestorControlRecursiveXXX(const ControlID& currentControlID, const ControlID& ancestorCandidateID) const noexcept
        {
            if (currentControlID.isValid() == false)
            {
                return false;
            }

            if (currentControlID == ancestorCandidateID)
            {
                return true;
            }

            const ControlID& parentControlID = getControlData(currentControlID).getParentID();
            return isAncestorControlRecursiveXXX(parentControlID, ancestorCandidateID);
        }

        const bool GUIContext::isDescendantControlInclusive(const ControlData& controlData, const ControlID& descendantCandidateID) const noexcept
        {
            return ((descendantCandidateID.isValid() == false) ? false : isDescendantControlRecursiveXXX(controlData.getID(), descendantCandidateID));
        }

        const bool GUIContext::isDescendantControlRecursiveXXX(const ControlID& currentControlID, const ControlID& descendantCandidateID) const noexcept
        {
            if (currentControlID == descendantCandidateID)
            {
                return true;
            }

            const ControlData& controlData = getControlData(currentControlID);
            const auto& previousChildControlIDs = controlData.getChildControlIDs();
            const uint32 previousChildControlCount = previousChildControlIDs.size();
            for (uint32 previousChildControlIndex = 0; previousChildControlIndex < previousChildControlCount; ++previousChildControlIndex)
            {
                const ControlID& previousChildControlID = previousChildControlIDs[previousChildControlIndex];
                if (isDescendantControlRecursiveXXX(previousChildControlID, descendantCandidateID) == true)
                {
                    return true;
                }
            }

            return false;
        }

        const bool GUIContext::isParentControlRoot(const ControlData& controlData) const noexcept
        {
            const ControlData& parentControlData = getControlData(controlData.getParentID());
            return parentControlData.isTypeOf(ControlType::ROOT);
        }

        const bool GUIContext::isAncestorControlFocused(const ControlData& controlData) const noexcept
        {
            return isAncestorControlTargetRecursiveXXX(controlData.getParentID(), _controlInteractionStateSet.getFocusedControlID());
        }

        const bool GUIContext::isAncestorControlPressed(const ControlData& controlData) const noexcept
        {
            return isAncestorControlTargetRecursiveXXX(controlData.getParentID(), _controlInteractionStateSet.getPressedControlID());
        }

        const bool GUIContext::isAncestorControlTargetRecursiveXXX(const ControlID& id, const ControlID& targetID) const noexcept
        {
            if (id.isValid() == false)
            {
                return false;
            }

            if (id == targetID)
            {
                return true;
            }

            const ControlID& parentID = getControlData(id).getParentID();
            return isAncestorControlTargetRecursiveXXX(parentID, targetID);
        }

        const bool GUIContext::needToColorFocused(const ControlData& controlData) const noexcept
        {
            // #0. Child of docking control
            const bool hasDockingAncestorInclusive = hasDockingAncestorControlInclusive(controlData);
            if (hasDockingAncestorInclusive == true)
            {
                return true;
            }

            // #1. Focused
            const ControlData& closestFocusableAncestorInclusive = getClosestFocusableAncestorControlInclusive(controlData);
            const bool isFocused = _controlInteractionStateSet.isControlFocused(closestFocusableAncestorInclusive);
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
            const ControlData& dockControlData = getControlData(closestFocusableAncestorInclusive.getDockControlID());
            return (isDocking == true && (dockControlData.isRootControl() == true || _controlInteractionStateSet.isControlFocused(dockControlData) == true || isDescendantControlFocusedInclusive(dockControlData) == true));
        }

        const bool GUIContext::isDescendantControlFocusedInclusive(const ControlData& controlData) const noexcept
        {
            return isDescendantControlInclusive(controlData, _controlInteractionStateSet.getFocusedControlID());
        }

        const bool GUIContext::isDescendantControlHoveredInclusive(const ControlData& controlData) const noexcept
        {
            return isDescendantControlInclusive(controlData, _controlInteractionStateSet.getHoveredControlID());
        }

        const bool GUIContext::isDescendantControlPressedInclusive(const ControlData& controlData) const noexcept
        {
            return isDescendantControlInclusive(controlData, _controlInteractionStateSet.getPressedControlID());
        }

        const bool GUIContext::isDescendantControlPressed(const ControlData& controlData) const noexcept
        {
            const auto& previousChildControlIDs = controlData.getChildControlIDs();
            const uint32 previousChildControlCount = previousChildControlIDs.size();
            for (uint32 previousChildControlIndex = 0; previousChildControlIndex < previousChildControlCount; ++previousChildControlIndex)
            {
                const ControlID& previousChildControlID = previousChildControlIDs[previousChildControlIndex];
                if (isDescendantControlRecursiveXXX(previousChildControlID, _controlInteractionStateSet.getPressedControlID()) == true)
                {
                    return true;
                }
            }
            return false;
        }

        const bool GUIContext::isDescendantControlHovered(const ControlData& controlData) const noexcept
        {
            const auto& previousChildControlIDs = controlData.getChildControlIDs();
            const uint32 previousChildControlCount = previousChildControlIDs.size();
            for (uint32 previousChildControlIndex = 0; previousChildControlIndex < previousChildControlCount; ++previousChildControlIndex)
            {
                const ControlID& previousChildControlID = previousChildControlIDs[previousChildControlIndex];
                if (isDescendantControlRecursiveXXX(previousChildControlID, _controlInteractionStateSet.getHoveredControlID()) == true)
                {
                    return true;
                }
            }
            return false;
        }

        const ControlData& GUIContext::getClosestFocusableAncestorControlInclusive(const ControlData& controlData) const noexcept
        {
            if (controlData.getID().getRawValue() <= 1)
            {
                // ROOT
                return controlData;
            }

            if (controlData._option._isFocusable == true && controlData.isDocking() == false)
            {
                return controlData;
            }

            return getClosestFocusableAncestorControlInclusive(getControlData(controlData.getParentID()));
        }

        const bool GUIContext::hasDockingAncestorControlInclusive(const ControlData& controlData) const noexcept
        {
            if (controlData.getID().getRawValue() <= 1)
            {
                // ROOT
                return false;
            }

            if (controlData.isDocking() == true)
            {
                return true;
            }

            return hasDockingAncestorControlInclusive(getControlData(controlData.getParentID()));
        }

        const float GUIContext::getMouseWheelScroll(const ControlData& scrollParentControlData) const noexcept
        {
            float result = 0.0f;
            if (_mouseStates._mouseWheel != 0.0f
                && ControlCommonHelpers::isInControlInteractionArea(_mouseStates.getPosition(), scrollParentControlData) == true)
            {
                result = _mouseStates._mouseWheel * kMouseWheelScrollScale;
                _mouseStates._mouseWheel = 0.0f;
            }
            return result;
        }

        Rendering::ShapeFontRendererContext& GUIContext::getRendererContext(const ControlData& controlData) noexcept
        {
            return getRendererContext(controlData._rendererContextLayer);
        }

        Rendering::ShapeFontRendererContext& GUIContext::getRendererContext(const RendererContextLayer rendererContextLayer) noexcept
        {
            return _rendererContexts[static_cast<int32>(rendererContextLayer)];
        }

        const RendererContextLayer GUIContext::getUpperRendererContextLayer(const ControlData& controlData) noexcept
        {
            const int32 index = min(static_cast<int32>(controlData._rendererContextLayer) + 1, static_cast<int32>(RendererContextLayer::TopMost) - 1);
            return static_cast<RendererContextLayer>(index);
        }

        void GUIContext::render()
        {
            MINT_ASSERT("김장원", _controlStackPerFrame.empty() == true, "begin 과 end 호출 횟수가 맞지 않습니다!!!");

            _graphicDevice.accessWindow().setCursorType(_mouseStates._cursorType);

            if (_controlInteractionStateSet.needToShowTooltip() == true)
            {
                makeTooltipWindow(_rootControlData.getID(), _controlInteractionStateSet.getTooltipText()
                    , _controlInteractionStateSet.getTooltipWindowPosition(getControlData(_controlInteractionStateSet.getTooltipParentWindowID()))
                );
            }

            // Viewport setting
            _graphicDevice.useScissorRectangles();

            // Layer 순서대로!
            for (int32 rendererContextIndex = 0; rendererContextIndex < getRendererContextLayerCount(); rendererContextIndex++)
            {
                _rendererContexts[rendererContextIndex].renderAndFlush();
            }
            
            // Viewport setting
            _graphicDevice.useFullScreenViewport();

            resetPerFrameStates();
        }

        void GUIContext::resetPerFrameStates()
        {
            _controlInteractionStateSet.resetPerFrameStates(_mouseStates);

            _controlStackPerFrame.clear();

            _rootControlData.clearPerFrameData();

            if (_resizedControlID.isValid() == false)
            {
                _mouseStates._cursorType = Window::CursorType::Arrow;
            }

            _keyCode = Platform::KeyCode::NONE;

            // 다음 프레임에서 가장 먼저 렌더링 되는 것!!
            processDock(_rootControlData, getRendererContext(RendererContextLayer::Background));

            if (_updateScreenSizeCounter > 0)
            {
                --_updateScreenSizeCounter;
            }
        }
    }
}

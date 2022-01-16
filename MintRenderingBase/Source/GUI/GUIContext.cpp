#include <stdafx.h>
#include <MintRenderingBase/Include/GUI/GUIContext.h>

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

#pragma optimize("",off)

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

            MINT_INLINE const bool isInControlInnerInteractionArea(const Float2& screenPosition, const ControlData& controlData) noexcept
            {
                if (controlData.isDockHosting() == true)
                {
                    const Float2 positionOffset{ controlData.getDockZoneSize(DockZone::LeftSide)._x, controlData.getDockZoneSize(DockZone::TopSide)._y };
                    return ControlCommonHelpers::isInControl(screenPosition, controlData._position, positionOffset, controlData.getInnerInteractionSize());
                }
                return ControlCommonHelpers::isInControl(screenPosition, controlData._position, Float2::kZero, controlData.getInnerInteractionSize());
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

                if (ControlCommonHelpers::isInControlInnerInteractionArea(screenPosition, parentChildControlData) == true)
                {
                    return false;
                }
            }
            return true;
        }

        const bool GUIContext::beginWindow(const char* const file, const int line, const wchar_t* const title, const WindowParam& windowParam, VisibleState& inoutVisibleState)
        {
            static constexpr ControlType controlType = ControlType::Window;
            
            _controlMetaStateSet.nextOffAutoPosition();

            const ControlID windowControlID = issueControlID(file, line, controlType, title);
            
            ControlData& windowControlData = accessControlData(windowControlID);
            if (windowControlData.needInitialization())
            {
                windowControlData._option._isFocusable = true;
                windowControlData._controlValue._windowData._titleBarThickness = kTitleBarBaseThickness;
                windowControlData._dockContext._dockingControlType = DockingControlType::DockerDock;
            }
            if (windowControlData.updateVisibleState(inoutVisibleState) == true && windowControlData.isControlVisible() == true)
            {
                setControlFocused(windowControlData);
            }
            windowDockInitially(windowControlData, windowParam._initialDockZone, windowParam._initialDockingSize);

            ControlData::UpdateParam updateParam;
            {
                const float titleWidth = computeTextWidth(title, StringUtil::length(title));
                updateParam._initialResizingMask.setAllTrue();
                updateParam._desiredPositionInParent = windowParam._position;
                updateParam._innerPadding = kWindowInnerPadding;
                updateParam._minSize._x = titleWidth + kTitleBarInnerPadding.horz() + kDefaultRoundButtonRadius * 2.0f;
                updateParam._minSize._y = windowControlData._size._y - windowControlData.computeInnerDisplaySize()._y + 16.0f;
                updateParam._alwaysResetDisplaySize = false; // 중요!!!
                updateParam._alwaysResetPosition = false;
                updateParam._clipRectUsage = ClipRectUsage::Own;
                updateParam._deltaInteractionSizeByDock._x = -windowControlData.getHorzDockZoneSize();
                updateParam._deltaInteractionSizeByDock._y = -windowControlData.getVertDockZoneSize();
            }
            updateControlData(windowControlData, updateParam);

            windowUpdatePositionByParentWindow(windowControlData);
            
            windowUpdateDockingWindowDisplay(windowControlData);

            const bool needToProcessControl = windowNeedToProcessControl(windowControlData);
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
                        clipRectForDocks.top() += static_cast<LONG>(windowControlData._controlValue._windowData._titleBarThickness);
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
                    const bool isAncestorFocused = isAncestorControlInteractionState(windowControlData, ControlInteractionState::Focused);
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
                }
            }
            
            if (windowControlData.isControlVisible() == true)
            {
                // 중요
                _controlMetaStateSet.nextOffAutoPosition();

                const Float2 titleBarSize = Float2(windowControlData._size._x, windowControlData._controlValue._windowData._titleBarThickness);
                if (beginTitleBar(windowControlData.getID(), title, titleBarSize, kTitleBarInnerPadding, inoutVisibleState))
                {
                    endTitleBar();
                }

                if (windowParam._scrollBarType != ScrollBarType::None)
                {
                    makeScrollBar(windowControlData.getID(), windowParam._scrollBarType);
                }
            }

            // ControlStackTop 에 WindowControl 이 추가되는 시점이 아래인 것에 주의!!!
            return beginControlInternal(controlType, windowControlID, needToProcessControl);
        }

        void GUIContext::windowDockInitially(ControlData& windowControlData, const DockZone dockZone, const Float2& initialDockingSize)
        {
            MINT_ASSERT("김장원", windowControlData.isTypeOf(ControlType::Window) == true, "Window 가 아니면 사용하면 안 됩니다!");

            if (dockZone == DockZone::COUNT)
            {
                return;
            }

            if (windowControlData._updateCount != 2)
            {
                return;
            }

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

        void GUIContext::windowUpdatePositionByParentWindow(ControlData& windowControlData) noexcept
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

        void GUIContext::windowUpdateDockingWindowDisplay(ControlData& windowControlData) noexcept
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

        const bool GUIContext::windowNeedToProcessControl(const ControlData& windowControlData) const noexcept
        {
            MINT_ASSERT("김장원", windowControlData.isTypeOf(ControlType::Window) == true, "Window 가 아니면 사용하면 안 됩니다!");

            const bool isDocking = windowControlData.isDocking();
            if (isDocking == false)
            {
                return windowControlData.isControlVisible();
            }

            const ControlData& dockControlData = getControlData(windowControlData.getDockControlID());
            const bool isFocusedDocker = dockControlData.isFocusedDocker(windowControlData);
            return (windowControlData.isControlVisible() && isFocusedDocker);
        }

        const bool GUIContext::beginButton(const char* const file, const int line, const wchar_t* const text)
        {
            static constexpr ControlType controlType = ControlType::Button;
            
            const ControlID controlID = issueControlID(file, line, controlType, text);
            
            ControlData& controlData = accessControlData(controlID);
            ControlData::UpdateParam updateParam;
            {
                const float textWidth = computeTextWidth(text, StringUtil::length(text));
                updateParam._autoComputedDisplaySize = Float2(textWidth + 24, _fontSize + 12);
            }
            updateControlData(controlData, updateParam);
        
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

            return beginControlInternal(controlType, controlID, isClicked);
        }

        const bool GUIContext::beginCheckBox(const char* const file, const int line, const wchar_t* const text, bool* const outIsChecked)
        {
            static constexpr ControlType controlType = ControlType::CheckBox;

            const ControlID controlID = issueControlID(file, line, controlType, text);
            
            ControlData& controlData = accessControlData(controlID);
            ControlData::UpdateParam updateParam;
            {
                updateParam._autoComputedDisplaySize = kCheckBoxSize;
            }
            updateControlData(controlData, updateParam);

            Rendering::Color finalBackgroundColor;
            const bool isClicked = processToggleControl(controlData, getNamedColor(NamedColor::NormalState), getNamedColor(NamedColor::NormalState), getNamedColor(NamedColor::HighlightColor), finalBackgroundColor);
            
            const Float4& controlCenterPosition = controlData.getControlCenterPosition();
            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(controlData);
            rendererContext.setClipRect(controlData.getClipRects()._forMe);
            
            // draw background
            rendererContext.setColor(finalBackgroundColor);
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(controlData._size, (kDefaultRoundnessInPixel / controlData._size.minElement()), 0.0f, 0.0f);

            const bool isChecked = controlData._controlValue._booleanData.get();
            if (outIsChecked != nullptr)
            {
                *outIsChecked = isChecked;
            }
            if (isChecked)
            {
                // draw check mark
                Float2 p0 = Float2(controlCenterPosition._x - 1.0f, controlCenterPosition._y + 4.0f);
                rendererContext.setColor(getNamedColor(NamedColor::LightFont));
                rendererContext.drawLine(p0, p0 + Float2(-4.0f, -5.0f), 2.0f);
                rendererContext.drawLine(p0, p0 + Float2(+7.0f, -8.0f), 2.0f);
            }

            // draw text
            rendererContext.setTextColor(getNamedColor(NamedColor::LightFont) * Rendering::Color(1.0f, 1.0f, 1.0f, finalBackgroundColor.a()));
            rendererContext.drawDynamicText(text, controlCenterPosition + Float4(kCheckBoxSize._x * 0.75f, 0.0f, 0.0f, 0.0f), 
                Rendering::FontRenderingOption(Rendering::TextRenderDirectionHorz::Rightward, Rendering::TextRenderDirectionVert::Centered, kFontScaleB));

            return beginControlInternal(controlType, controlID, isClicked);
        }

        void GUIContext::makeLabel(const char* const file, const int line, const wchar_t* const text, const LabelParam& labelParam)
        {
            static constexpr ControlType controlType = ControlType::Label;

            const ControlID controlID = issueControlID(file, line, controlType, text);
            
            ControlData& controlData = accessControlData(controlID);
            ControlData::UpdateParam updateParam;
            {
                const float textWidth = computeTextWidth(text, StringUtil::length(text));
                updateParam._autoComputedDisplaySize = Float2(textWidth + labelParam._paddingForAutoSize._x, _fontSize + labelParam._paddingForAutoSize._y);
                updateParam._offset = labelParam._common._offset;
            }
            updateControlData(controlData, updateParam);
            
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
            const Rendering::FontRenderingOption fontRenderingOption = labelMakeFontRenderingOption(labelParam, controlData);
            rendererContext.drawDynamicText(text, textPosition, fontRenderingOption);
        }

        Float4 GUIContext::labelComputeTextPosition(const LabelParam& labelParam, const ControlData& labelControlData) const noexcept
        {
            MINT_ASSERT("김장원", labelControlData.isTypeOf(ControlType::Label) == true, "Label 이 아니면 사용하면 안 됩니다!");

            Float4 textPosition = labelControlData.getControlCenterPosition();
            if (labelParam._alignmentHorz != TextAlignmentHorz::Center)
            {
                textPosition._x = labelControlData._position._x;
                if (labelParam._alignmentHorz == TextAlignmentHorz::Right)
                {
                    textPosition._x += labelControlData._size._x;
                }
            }
            if (labelParam._alignmentVert != TextAlignmentVert::Middle)
            {
                textPosition._y = labelControlData._position._y;
                if (labelParam._alignmentVert == TextAlignmentVert::Bottom)
                {
                    textPosition._y += labelControlData._size._y;
                }
            }
            return textPosition;
        }

        Rendering::FontRenderingOption GUIContext::labelMakeFontRenderingOption(const LabelParam& labelParam, const ControlData& labelControlData) const noexcept
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
            ControlData::UpdateParam updateParamForTrack;
            {
                updateParamForTrack._autoComputedDisplaySize = Float2(0.0f, kSliderThumbRadius * 2.0f);
            }
            updateControlData(trackControlData, updateParamForTrack);
            
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
                ControlData::UpdateParam updateParamForThumb;
                {
                    const ControlData& parentWindowControlData = getParentWindowControlData(trackControlData);

                    updateParamForThumb._autoComputedDisplaySize._x = kSliderThumbRadius * 2.0f;
                    updateParamForThumb._autoComputedDisplaySize._y = kSliderThumbRadius * 2.0f;
                    updateParamForThumb._alwaysResetPosition = false;
                    updateParamForThumb._desiredPositionInParent = trackControlData._position - parentWindowControlData._position;
                }
                updateControlData(thumbControlData, updateParamForThumb);
                
                Rendering::Color thumbColor;
                processScrollableControl(thumbControlData, getNamedColor(NamedColor::HighlightColor), getNamedColor(NamedColor::HighlightColor).addedRgb(0.125f), thumbColor);

                const float thumbAt = (thumbControlData._position._x - trackControlData._position._x) / sliderValidLength;
                if (trackControlData._controlValue._thumbData._thumbAt != thumbAt)
                {
                    isChanged = true;
                }
                trackControlData._controlValue._thumbData._thumbAt = thumbAt;
                outValue = thumbAt;
                
                // 반드시 thumbAt 이 갱신된 이후에 draw 를 한다.
                sliderDrawTrack(sliderParam, trackControlData, trackColor);
                sliderDrawThumb(sliderParam, thumbControlData, thumbColor);
            }
            
            return beginControlInternal(trackControlType, trackControlID, isChanged);
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
            ControlData::UpdateParam updateParam;
            updateParam._offset = textBoxParam._common._offset;
            updateParam._autoComputedDisplaySize._y = _fontSize;
            updateControlData(controlData, updateParam);
            
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

            return beginControlInternal(controlType, controlID, isFocused);
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
            ControlData::UpdateParam updateParam;
            updateParam._offset = commonControlParam._offset;
            updateParam._autoComputedDisplaySize._y = _fontSize;
            updateControlData(controlData, updateParam);

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
            
            return beginControlInternal(controlType, controlID, isFocused);
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
            const ControlType controlType = ControlType::ValueSlider;
            const ControlID controlID = issueControlID(file, line, controlType, nullptr);
            
            _controlMetaStateSet.popSize();

            return beginControlInternal(controlType, controlID, result);
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
                    && ControlCommonHelpers::isInControlInnerInteractionArea(_mouseStates.getButtonDownPosition(), controlData) == true)
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

            ControlData::UpdateParam updateParam;
            {
                updateParam._autoComputedDisplaySize._x = 160.0f;
                updateParam._autoComputedDisplaySize._y = 100.0f;
            }
            updateControlData(controlData, updateParam);

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
            return beginControlInternal(controlType, controlID, true);
        }

        void GUIContext::endListView()
        {
            const ControlData& controlData = getControlStackTopXXX();
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
            ControlData::UpdateParam updateParam;
            {
                updateParam._autoComputedDisplaySize._x = parentControlData._size._x;
                updateParam._autoComputedDisplaySize._y = _fontSize + 12.0f;
                updateParam._innerPadding.left(updateParam._autoComputedDisplaySize._y * 0.25f);
                updateParam._clipRectUsage = GUI::ClipRectUsage::ParentsChild;
                _controlMetaStateSet.nextOffInterval();
            }
            updateControlData(controlData, updateParam);

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

            const ControlID controlID = issueControlID(file, line, controlType, nullptr);
            
            ControlData& menuBar = accessControlData(controlID);
            ControlData& menuBarParent = accessControlData(menuBar.getParentID());
            const bool isMenuBarParentRoot = menuBarParent.isTypeOf(ControlType::ROOT);
            const bool isMenuBarParentWindow = menuBarParent.isTypeOf(ControlType::Window);
            if (isMenuBarParentRoot == false && isMenuBarParentWindow == false)
            {
                MINT_LOG_ERROR("김장원", "MenuBar 는 Window 나 Root 컨트롤의 자식으로만 사용할 수 있습니다!");
                return false;
            }
            menuBarParent._controlValue._commonData._menuBarType = MenuBarType::Top; // TODO...

            ControlData::UpdateParam updateParam;
            {
                updateParam._autoComputedDisplaySize._x = menuBarParent._size._x;
                updateParam._autoComputedDisplaySize._y = kMenuBarBaseSize._y;
                updateParam._desiredPositionInParent._x = 0.0f;
                updateParam._desiredPositionInParent._y = (isMenuBarParentWindow == true) ? menuBarParent._controlValue._windowData._titleBarThickness : 0.0f;
                updateParam._clipRectUsage = ClipRectUsage::ParentsOwn;
            }
            updateControlData(menuBar, updateParam);

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

            return beginControlInternal(controlType, controlID, true);
        }

        const bool GUIContext::beginMenuBarItem(const char* const file, const int line, const wchar_t* const text)
        {
            static constexpr ControlType controlType = ControlType::MenuBarItem;

            _controlMetaStateSet.nextOffAutoPosition();

            if (getControlStackTopXXX().isTypeOf(ControlType::MenuBar) == false)
            {
                MINT_LOG_ERROR("김장원", "MenuBarItem 은 MenuBar 컨트롤의 자식으로만 사용할 수 있습니다!");
                return false;
            }

            const ControlID controlID = issueControlID(file, line, controlType, text);
            
            ControlData& menuBarItem = accessControlData(controlID);
            ControlData& menuBar = accessControlData(menuBarItem.getParentID());
            ControlData::UpdateParam updateParam;
            {
                const uint32 textLength = StringUtil::length(text);
                const float textWidth = computeTextWidth(text, textLength);
                updateParam._autoComputedDisplaySize._x = textWidth + kMenuBarItemTextSpace;
                updateParam._autoComputedDisplaySize._y = kMenuBarBaseSize._y;
                updateParam._desiredPositionInParent._x = menuBar._controlValue._itemData._itemSize._x;
                updateParam._desiredPositionInParent._y = 0.0f;
                updateParam._clipRectUsage = ClipRectUsage::ParentsOwn;
            }
            updateControlData(menuBarItem, updateParam);
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
            const bool isParentAncestorPressed = isAncestorControlInteractionState(menuBar, ControlInteractionState::Pressed);
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
            return beginControlInternal(controlType, controlID, result);
        }

        const bool GUIContext::beginMenuItem(const char* const file, const int line, const wchar_t* const text)
        {
            static constexpr ControlType controlType = ControlType::MenuItem;

            _controlMetaStateSet.nextOffAutoPosition();
            _controlMetaStateSet.nextOffSizeContraintToParent();

            const ControlID controlID = issueControlID(file, line, controlType, text);
            
            ControlData& menuItem = accessControlData(controlID);
            menuItem._option._isInteractableOutsideParent = true;

            ControlData& menuItemParent = accessControlData(menuItem.getParentID());
            const ControlType parentControlType = menuItemParent.getControlType();
            const bool isParentControlMenuItem = (parentControlType == ControlType::MenuItem);
            if (parentControlType != ControlType::MenuBarItem && isParentControlMenuItem == false)
            {
                MINT_LOG_ERROR("김장원", "MenuItem 은 MenuBarItem 이나 MenuItem 컨트롤의 자식으로만 사용할 수 있습니다!");
                return false;
            }

            ControlData::UpdateParam updateParam;
            {
                updateParam._autoComputedDisplaySize._x = menuItemParent._controlValue._itemData._itemSize._x;
                updateParam._autoComputedDisplaySize._y = kMenuBarBaseSize._y;
                updateParam._innerPadding.left(kMenuItemSpaceLeft);
                updateParam._desiredPositionInParent._x = (isParentControlMenuItem == true) ? menuItemParent._size._x : 0.0f;
                updateParam._desiredPositionInParent._y = menuItemParent._controlValue._itemData._itemSize._y + ((isParentControlMenuItem == true) ? 0.0f : updateParam._autoComputedDisplaySize._y);
            }
            updateControlData(menuItem, updateParam);

            const uint32 textLength = StringUtil::length(text);
            const float textWidth = computeTextWidth(text, textLength);
            menuItemParent._controlValue._itemData._itemSize._x = max(menuItemParent._controlValue._itemData._itemSize._x, textWidth + kMenuItemSpaceRight);
            menuItemParent._controlValue._itemData._itemSize._y += menuItem._size._y;
            menuItem._controlValue._itemData._itemSize._y = 0.0f;

            const bool isDescendantHovered = isThisOrDescendantControlInteractionState(menuItem, ControlInteractionState::Hovered);
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
            return beginControlInternal(controlType, controlID, result);
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
            ControlData::UpdateParam updateParamForTrack;
            {
                //updateParamForTrack._autoComputedDisplaySize = trackControlData._size;
                updateParamForTrack._desiredPositionInParent = scrollBarTrackParam._positionInParent;
                if (isVert == true)
                {
                    updateParamForTrack._desiredPositionInParent._x -= parentControlData.getDockZoneSize(DockZone::RightSide)._x;
                    updateParamForTrack._desiredPositionInParent._y += parentControlData.getDockZoneSize(DockZone::TopSide)._y;
                }
                else
                {
                    updateParamForTrack._desiredPositionInParent._x += parentControlData.getDockZoneSize(DockZone::LeftSide)._x;
                    updateParamForTrack._desiredPositionInParent._y -= parentControlData.getDockZoneSize(DockZone::BottomSide)._y;
                }
                updateParamForTrack._parentIDOverride = parentControlData.getID();
                updateParamForTrack._alwaysResetPosition = true;
                updateParamForTrack._ignoreMeForContentAreaSize = true;
                updateParamForTrack._clipRectUsage = ClipRectUsage::ParentsOwn;
            }
            updateControlData(trackControlData, updateParamForTrack);

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
                ControlData::UpdateParam updateParamForThumb;
                {
                    updateParamForThumb._autoComputedDisplaySize._x = kScrollBarThickness;
                    updateParamForThumb._autoComputedDisplaySize._y = thumbSize;

                    updateParamForThumb._desiredPositionInParent = getControlPositionInParentSpace(scrollBarTrack);
                    updateParamForThumb._desiredPositionInParent._x -= kScrollBarThickness * 0.5f;

                    updateParamForThumb._parentIDOverride = scrollBarParent.getID();
                    updateParamForThumb._ignoreMeForContentAreaSize = true;
                    updateParamForThumb._clipRectUsage = ClipRectUsage::ParentsOwn;

                    thumbControlData._option._isDraggable = true;
                    thumbControlData._positionConstraintsForDragging.left(scrollBarTrack._position._x - kScrollBarThickness * 0.5f);
                    thumbControlData._positionConstraintsForDragging.right(thumbControlData._positionConstraintsForDragging.left());
                    thumbControlData._positionConstraintsForDragging.top(scrollBarTrack._position._y);
                    thumbControlData._positionConstraintsForDragging.bottom(thumbControlData._positionConstraintsForDragging.top() + trackRemnantSize);
                }
                updateControlData(thumbControlData, updateParamForThumb);

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
                ControlData::UpdateParam updateParamForThumb;
                {
                    updateParamForThumb._autoComputedDisplaySize._x = thumbSize;
                    updateParamForThumb._autoComputedDisplaySize._y = kScrollBarThickness;

                    updateParamForThumb._desiredPositionInParent = getControlPositionInParentSpace(scrollBarTrack);
                    updateParamForThumb._desiredPositionInParent._y -= kScrollBarThickness * 0.5f;

                    updateParamForThumb._parentIDOverride = scrollBarParent.getID();
                    updateParamForThumb._ignoreMeForContentAreaSize = true;
                    updateParamForThumb._clipRectUsage = ClipRectUsage::ParentsOwn;

                    thumbControlData._option._isDraggable = true;
                    thumbControlData._positionConstraintsForDragging.left(scrollBarTrack._position._x);
                    thumbControlData._positionConstraintsForDragging.right(thumbControlData._positionConstraintsForDragging.left() + trackRemnantSize);
                    thumbControlData._positionConstraintsForDragging.top(scrollBarTrack._position._y - kScrollBarThickness * 0.5f);
                    thumbControlData._positionConstraintsForDragging.bottom(thumbControlData._positionConstraintsForDragging.top());
                }
                updateControlData(thumbControlData, updateParamForThumb);

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
            if (controlData._dockContext.isDockable() == false)
            {
                return;
            }
            
            for (DockZone dockZoneIter = static_cast<DockZone>(0); dockZoneIter != DockZone::COUNT; dockZoneIter = static_cast<DockZone>(static_cast<uint32>(dockZoneIter) + 1))
            {
                const DockZoneData& dockZoneData = controlData.getDockZoneData(dockZoneIter);
                if (dockZoneData.hasDockedControls() == false)
                {
                    continue;
                }

                const Float2& dockZoneSize = controlData.getDockZoneSize(dockZoneIter);
                const Float2& dockZonePosition = controlData.getDockZonePosition(dockZoneIter);
                rendererContext.setClipRect(controlData.getClipRects()._forDocks);
                rendererContext.setColor(getNamedColor(NamedColor::Dock));
                rendererContext.setPosition(Float4(dockZonePosition._x + dockZoneSize._x * 0.5f, dockZonePosition._y + dockZoneSize._y * 0.5f, 0, 0));
                rendererContext.drawRectangle(dockZoneSize, 0.0f, 0.0f);

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
            }
        }

        const bool GUIContext::beginControlInternal(const ControlType controlType, const ControlID controlID, const bool returnValue)
        {
            if (returnValue)
            {
                _controlStack.push_back(ControlStackData(controlType, controlID));
            }
            return returnValue;
        }

        void GUIContext::endControlInternal(const ControlType controlType)
        {
            MINT_ASSERT("김장원", _controlStack.back()._controlType == controlType, "begin 과 end 의 ControlType 이 다릅니다!!!");

            _controlStack.pop_back();
        }

        const bool GUIContext::beginTitleBar(const ControlID parentControlID, const wchar_t* const windowTitle, const Float2& titleBarSize, const Rect& innerPadding, VisibleState& inoutParentVisibleState)
        {
            static constexpr ControlType controlType = ControlType::TitleBar;

            const ControlID controlID = issueControlID(parentControlID, controlType, L"TITLEBAR", windowTitle);
            
            ControlData& controlData = accessControlData(controlID);
            controlData._option._isDraggable = true;
            controlData._delegateControlID = controlData.getParentID();
            ControlData& parentWindowControlData = accessControlData(controlData.getParentID());
            const bool isParentControlDocking = parentWindowControlData.isDocking();
            ControlData::UpdateParam updateParam;
            {
                if (isParentControlDocking == true)
                {
                    const ControlData& dockControlData = getControlData(parentWindowControlData.getDockControlID());
                    const DockZoneData& parentDockZoneData = dockControlData.getDockZoneData(parentWindowControlData._dockContext._lastDockZone);
                    const int32 dockedControlIndex = parentDockZoneData.getDockedControlIndex(parentWindowControlData.getID());
                    const float textWidth = computeTextWidth(windowTitle, StringUtil::length(windowTitle));
                    const Float2& displaySizeOverride = Float2(textWidth + 16.0f, controlData._size._y);
                    updateParam._autoComputedDisplaySize = displaySizeOverride;
                    updateParam._desiredPositionInParent._x = parentDockZoneData.getDockedControlTitleBarOffset(dockedControlIndex);
                    updateParam._desiredPositionInParent._y = 0.0f;
                }
                else
                {
                    updateParam._autoComputedDisplaySize = titleBarSize;
                    updateParam._deltaInteractionSize = Float2(-innerPadding.right() - kDefaultRoundButtonRadius * 2.0f, 0.0f);
                }
                updateParam._alwaysResetPosition = true;
                updateParam._clipRectUsage = ClipRectUsage::ParentsOwn;
            }
            updateControlData(controlData, updateParam);
            
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

            return beginControlInternal(controlType, controlID, true);
        }

        const bool GUIContext::makeRoundButton(const ControlID parentControlID, const wchar_t* const identifier, const wchar_t* const windowTitle, const Rendering::Color& color)
        {
            static constexpr ControlType controlType = ControlType::RoundButton;

            const ControlID controlID = issueControlID(parentControlID, controlType, identifier, windowTitle);

            const float radius = kDefaultRoundButtonRadius;
            const ControlData& parentWindowData = getParentWindowControlData(getControlData(parentControlID));
            ControlData& controlData = accessControlData(controlID);
            ControlData::UpdateParam updateParam;
            {
                updateParam._parentIDOverride = parentWindowData.getID();
                updateParam._autoComputedDisplaySize = Float2(radius * 2.0f);
                updateParam._clipRectUsage = ClipRectUsage::ParentsOwn;
            }
            updateControlData(controlData, updateParam);
            
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
            ControlData::UpdateParam updateParam;
            {
                const float tooltipTextWidth = computeTextWidth(tooltipText, StringUtil::length(tooltipText)) * kTooltipFontScale;
                updateParam._autoComputedDisplaySize = Float2(tooltipTextWidth + tooltipWindowPadding * 2.0f, _fontSize * kTooltipFontScale + tooltipWindowPadding);
                updateParam._desiredPositionInParent = position;
                updateParam._alwaysResetParent = true;
                updateParam._alwaysResetPosition = true;
                updateParam._parentIDOverride = _controlInteractionStateSet.getTooltipParentWindowID();
                updateParam._clipRectUsage = ClipRectUsage::ParentsOwn;
            }
            _controlMetaStateSet.nextOffAutoPosition();
            updateControlData(controlData, updateParam);
            
            Rendering::Color dummyColor;
            processShowOnlyControl(controlData, dummyColor);

            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(RendererContextLayer::TopMost);
            rendererContext.setClipRect(controlData.getClipRects()._forMe);
            
            const Float4& controlCenterPosition = controlData.getControlCenterPosition();
            rendererContext.setColor(getNamedColor(NamedColor::TooltipBackground));
            rendererContext.setPosition(controlCenterPosition);
            rendererContext.drawRoundedRectangle(controlData._size, (kDefaultRoundnessInPixel / controlData._size.minElement()) * 0.75f, 0.0f, 0.0f);

            const Float4& textPosition = Float4(controlData._position._x, controlData._position._y, 0.0f, 1.0f) + Float4(tooltipWindowPadding, updateParam._autoComputedDisplaySize._y * 0.5f, 0.0f, 0.0f);
            rendererContext.setClipRect(controlData.getClipRects()._forMe);
            rendererContext.setTextColor(getNamedColor(NamedColor::DarkFont));
            rendererContext.drawDynamicText(tooltipText, textPosition, 
                Rendering::FontRenderingOption(Rendering::TextRenderDirectionHorz::Rightward, Rendering::TextRenderDirectionVert::Centered, kTooltipFontScale));
        }

        const ControlID GUIContext::issueControlID(const ControlID parentControlID, const ControlType controlType, const wchar_t* const identifier, const wchar_t* const text) noexcept
        {
            const ControlID controlID = _generateControlIDXXX(parentControlID, controlType, identifier);
            return _createControlDataInternalXXX(controlID, parentControlID, controlType, text);
        }

        const ControlID GUIContext::issueControlID(const char* const file, const int line, const ControlType controlType, const wchar_t* const text) noexcept
        {
            const ControlID controlID = _generateControlIDXXX(file, line, controlType);
            const ControlID parentControlID = getControlStackTopXXX().getID();
            return _createControlDataInternalXXX(controlID, parentControlID, controlType, text);
        }

        const ControlID GUIContext::_createControlDataInternalXXX(const ControlID controlID, const ControlID parentControlID, const ControlType controlType, const wchar_t* const text) noexcept
        {
            auto found = _controlIDMap.find(controlID);
            if (found.isValid() == false)
            {
                ControlData newControlData{ controlID, parentControlID, controlType };
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
            if (controlData._option._isFocusable == false)
            {
                return;
            }

            if (controlData._option._needDoubleClickToFocus == true && _mouseStates.isDoubleClicked(Platform::MouseButton::Left) == true)
            {
                _controlInteractionStateSet.setControlFocused(controlData);
            }
            else
            {
                _controlInteractionStateSet.setControlFocused(controlData);
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

        void GUIContext::updateControlData(ControlData& controlData, const ControlData::UpdateParam& updateParam) noexcept
        {
            const bool isNewData = controlData._size.isNan();
            if ((isNewData == true) || (updateParam._alwaysResetParent == true))
            {
                if (updateParam._parentIDOverride.isValid())
                {
                    controlData.setParentID(updateParam._parentIDOverride);
                }

                if (isNewData == true)
                {
                    controlData._resizingMask = updateParam._initialResizingMask;

                    // 중요!!!
                    controlData.setAllClipRects(_clipRectFullScreen);
                }
            }

            ControlData& parentControlData = accessControlData(controlData.getParentID());
            const bool computeSize = (isNewData == true || updateParam._alwaysResetDisplaySize == true);
            controlData.updatePerFrame(updateParam, parentControlData, _controlMetaStateSet, getCurrentAvailableDisplaySizeX(), computeSize);
            
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
                if (updateParam._ignoreMeForContentAreaSize == false)
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
                controlData._position += updateParam._offset;
            }
            else
            {
                // NO Auto-positioned

                if (updateParam._alwaysResetPosition == true || isNewData == true)
                {
                    controlData._position = parentControlData._position;

                    if (updateParam._desiredPositionInParent.isNan() == true)
                    {
                        controlData._position += _controlMetaStateSet.getNextDesiredPosition();
                    }
                    else
                    {
                        controlData._position += updateParam._desiredPositionInParent;
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
            const bool isMouseInParentInteractionArea = ControlCommonHelpers::isInControlInnerInteractionArea(_mouseStates.getPosition(), parentControlData);
            const bool isMouseInInteractionArea = ControlCommonHelpers::isInControlInnerInteractionArea(_mouseStates.getPosition(), controlData);
            const bool meetsAreaCondition = (controlData._option._isInteractableOutsideParent == true || isMouseInParentInteractionArea == true) && (isMouseInInteractionArea == true);
            const bool meetsInteractionCondition = (shouldInteract(_mouseStates.getPosition(), controlData) == true || controlData.isRootControl() == true);
            if (meetsAreaCondition == false || meetsInteractionCondition == false)
            {
                // Not interacting
                _controlInteractionStateSet.resetHoverIf(controlData);
                _controlInteractionStateSet.resetPressIf(controlData);
                return;
            }

            // Hovered (at least)
            if (setMouseInteractionDone)
            {
                _controlInteractionStateSet.setMouseInteractionDoneThisFrame();
            }

            if (_controlInteractionStateSet.isControlHovered(controlData) == false && controlData._option._isFocusable == false)
            {
                setControlHovered(controlData);
            }

            // Click Event 가 발생했을 때도 Pressed 상태 유지!
            if (_mouseStates.isButtonDownUp(Platform::MouseButton::Left) == false && _mouseStates.isButtonDown(Platform::MouseButton::Left) == false)
            {
                _controlInteractionStateSet.resetPressIf(controlData);
            }

            if (ControlCommonHelpers::isInControlInnerInteractionArea(_mouseStates.getButtonDownPosition(), controlData) == true)
            {
                // Pressed (Mouse down)
                if (_mouseStates.isButtonDownThisFrame(Platform::MouseButton::Left) == true || _mouseStates.isDoubleClicked(Platform::MouseButton::Left) == true)
                {
                    setControlPressed(controlData);
                }

                // Clicked (only in interaction area)
                if (_mouseStates.isButtonDownUp(Platform::MouseButton::Left) == true)
                {
                    setControlClicked(controlData);
                }

                if (_mouseStates.isButtonDownThisFrame(Platform::MouseButton::Right) == true)
                {
                    _viewerTargetControlID = controlData.getID();
                }
            }
        }

        void GUIContext::processControlCommon(ControlData& controlData) noexcept
        {
            processControlCommon_updateMouseCursorForResizing(controlData);
            processControlCommon_updateTooltipData(controlData);

            processControlCommon_resize(controlData);
            processControlCommon_drag(controlData);
            processControlCommon_dock(controlData);

            _controlMetaStateSet.resetPerFrame();
        }

        void GUIContext::processControlCommon_updateMouseCursorForResizing(ControlData& controlData) noexcept
        {
            if (_resizedControlID.isValid())
            {
                return;
            }

            if (isInteractingInternal(controlData) == false)
            {
                return;
            }

            if (controlData.isResizable() == false)
            {
                return;
            }

            Window::CursorType newCursorType;
            ResizingMask resizingMask;
            if (ControlCommonHelpers::isInControlBorderArea(_mouseStates.getPosition(), controlData, newCursorType, resizingMask, _resizingMethod) == false)
            {
                return;
            }

            if (controlData._resizingMask.overlaps(resizingMask) == false)
            {
                return;
            }

            _mouseStates._cursorType = newCursorType;
            _controlInteractionStateSet.setMouseInteractionDoneThisFrame();
        }

        void GUIContext::processControlCommon_updateTooltipData(ControlData& controlData) noexcept
        {
            if (StringUtil::isNullOrEmpty(_controlMetaStateSet.getNextTooltipText()))
            {
                return;
            }

            if (_controlInteractionStateSet.isControlHovered(controlData) == false)
            {
                return;
            }
            
            // hover duration constraint
            if (_controlInteractionStateSet.isHoveringMoreThan(1000) == false)
            {
                return;
            }

            _controlInteractionStateSet.setTooltipData(_mouseStates, _controlMetaStateSet.getNextTooltipText(), getParentWindowControlData(controlData).getID());
        }

        void GUIContext::processControlCommon_resize(ControlData& controlData) noexcept
        {
            ControlData& targetControlData = (controlData._delegateControlID.isValid() == false) ? controlData : accessControlData(controlData._delegateControlID);
            if (isControlBeingResized(targetControlData) == false)
            {
                return;
            }

            if (_isResizeBegun == true)
            {
                _resizedControlInitialPosition = targetControlData._position;
                _resizedControlInitialSize = targetControlData._size;

                _isResizeBegun = false;
            }

            Float2& targetControlDisplaySize = const_cast<Float2&>(targetControlData._size);
            const Float2 mouseDragDelta = _mouseStates.getMouseDragDelta();
            const Float2 targetControlResizeMinSize = targetControlData.getResizeMinSize();
            const float flipHorz = (_resizingMethod == ResizingMethod::RepositionHorz || _resizingMethod == ResizingMethod::RepositionBoth) ? -1.0f : +1.0f;
            if (_mouseStates.isCursor(Window::CursorType::SizeVert) == false)
            {
                const float newPositionX = _resizedControlInitialPosition._x - mouseDragDelta._x * flipHorz;
                const float newSizeX = _resizedControlInitialSize._x + mouseDragDelta._x * flipHorz;
                if (targetControlResizeMinSize._x < newSizeX)
                {
                    if (flipHorz < 0.0f)
                    {
                        targetControlData._position._x = newPositionX;
                    }
                    targetControlDisplaySize._x = newSizeX;
                }
            }

            const float flipVert = (_resizingMethod == ResizingMethod::RepositionVert || _resizingMethod == ResizingMethod::RepositionBoth) ? -1.0f : +1.0f;
            if (_mouseStates.isCursor(Window::CursorType::SizeHorz) == false)
            {
                const float newPositionY = _resizedControlInitialPosition._y - mouseDragDelta._y * flipVert;
                const float newSizeY = _resizedControlInitialSize._y + mouseDragDelta._y * flipVert;
                if (targetControlResizeMinSize._y < newSizeY)
                {
                    if (flipVert < 0.0f)
                    {
                        targetControlData._position._y = newPositionY;
                    }
                    targetControlDisplaySize._y = newSizeY;
                }
            }

            if (targetControlData.isDocking() == true)
            {
                // TargetControl 이 Docking 중이라면, DockControl 의 DockZoneSize 도 같이 변경해줘야 한다.
                const ControlID& dockControlID = targetControlData.getDockControlID();
                ControlData& dockControlData = accessControlData(dockControlID);
                dockControlData.setDockZoneSize(targetControlData._dockContext._lastDockZone, targetControlDisplaySize);
                updateDockZoneData(dockControlID);
            }
            else if (targetControlData._dockContext.isDockable())
            {
                // 내가 DockHosting 중일 수 있음
                updateDockZoneData(targetControlData.getID());
            }

            _controlInteractionStateSet.setMouseInteractionDoneThisFrame();
        }

        void GUIContext::processControlCommon_drag(ControlData& controlData) noexcept
        {
            if (isControlBeingDragged(controlData) == false)
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

        void GUIContext::processControlCommon_dock(ControlData& controlData) noexcept
        {
            static constexpr Rendering::Color color = Rendering::Color(100, 110, 160);
            const bool isDragging = isControlBeingDragged(controlData);
            ControlData& changeTargetControlData = (controlData._delegateControlID.isValid() == false) ? controlData : accessControlData(controlData._delegateControlID);
            if (changeTargetControlData._dockContext.isDockable() == false)
            {
                return;
            }

            // No docking for controls that have child window.
            if (changeTargetControlData.hasChildWindow())
            {
                return;
            }

            // Mouse cursor position constraint
            if (ControlCommonHelpers::isInControlInnerInteractionArea(_mouseStates.getPosition(), changeTargetControlData) == false)
            {
                return;
            }

            ControlData& parentControlData = accessControlData(changeTargetControlData.getParentID());
            if (parentControlData._dockContext.isDockable() == false)
            {
                return;
            }

            // 초기화
            if (_mouseStates.isButtonDownUp(Platform::MouseButton::Left) == false)
            {
                changeTargetControlData._dockContext._lastDockZoneCandidate = DockZone::COUNT;
            }

            if (isDragging)
            {
                for (DockZone dockZoneIter = static_cast<DockZone>(0); dockZoneIter != DockZone::COUNT; dockZoneIter = static_cast<DockZone>(static_cast<uint32>(dockZoneIter) + 1))
                {
                    const Rect interactionBoxRect = processControlCommon_dock_makeInteractionBoxRect(dockZoneIter, parentControlData);
                    const Rect previewRect = processControlCommon_dock_makePreviewRect(dockZoneIter, interactionBoxRect, parentControlData);
                    DockZoneData& parentControlDockZoneData = parentControlData.getDockZoneData(dockZoneIter);
                    if (interactionBoxRect.contains(_mouseStates.getPosition()))
                    {
                        changeTargetControlData._dockContext._lastDockZoneCandidate = dockZoneIter;

                        if (parentControlDockZoneData.isRawDockSizeSet() == false)
                        {
                            parentControlDockZoneData.setRawDockSize(previewRect.size());
                        }
                    }

                    processControlCommon_dock_renderInteractionBox(color, interactionBoxRect, parentControlData);

                    if (interactionBoxRect.contains(_mouseStates.getPosition()))
                    {
                        processControlCommon_dock_renderPreviewBox(color, previewRect);
                    }
                }
            }

            if (_mouseStates.isButtonDownUp(Platform::MouseButton::Left) == true && changeTargetControlData._dockContext._lastDockZoneCandidate != DockZone::COUNT)
            {
                if (changeTargetControlData.isDocking() == false)
                {
                    // Docking 시작.

                    dock(changeTargetControlData.getID(), parentControlData.getID());

                    _draggedControlID.reset();
                }
            }
        }

        Rect GUIContext::processControlCommon_dock_makeInteractionBoxRect(const DockZone dockZone, ControlData& parentControlData) const noexcept
        {
            const Float4 parentControlCenterPosition = parentControlData.getControlCenterPosition();
            Rect interactionBoxRect;
            switch (dockZone)
            {
            case DockZone::LeftSide:
                interactionBoxRect.left(parentControlData._position._x + kDockingInteractionOffset);
                interactionBoxRect.right(interactionBoxRect.left() + kDockingInteractionShort);
                interactionBoxRect.top(parentControlCenterPosition._y - kDockingInteractionLong * 0.5f);
                interactionBoxRect.bottom(interactionBoxRect.top() + kDockingInteractionLong);
                break;
            case DockZone::RightSide:
                interactionBoxRect.right(parentControlData._position._x + parentControlData._size._x - kDockingInteractionOffset);
                interactionBoxRect.left(interactionBoxRect.right() - kDockingInteractionShort);
                interactionBoxRect.top(parentControlCenterPosition._y - kDockingInteractionLong * 0.5f);
                interactionBoxRect.bottom(interactionBoxRect.top() + kDockingInteractionLong);
                break;
            case DockZone::TopSide:
                interactionBoxRect.left(parentControlCenterPosition._x - kDockingInteractionLong * 0.5f);
                interactionBoxRect.right(interactionBoxRect.left() + kDockingInteractionLong);
                interactionBoxRect.top(parentControlData._position._y + parentControlData.getDockOffsetSize()._y + kDockingInteractionOffset);
                interactionBoxRect.bottom(interactionBoxRect.top() + kDockingInteractionShort);
                break;
            case DockZone::BottomSide:
                interactionBoxRect.left(parentControlCenterPosition._x - kDockingInteractionLong * 0.5f);
                interactionBoxRect.right(interactionBoxRect.left() + kDockingInteractionLong);
                interactionBoxRect.bottom(parentControlData._position._y + parentControlData._size._y - kDockingInteractionOffset);
                interactionBoxRect.top(interactionBoxRect.bottom() - kDockingInteractionShort);
                break;
            default:
                break;
            }
            return interactionBoxRect;
        }

        Rect GUIContext::processControlCommon_dock_makePreviewRect(const DockZone dockZone, const Rect& interactionBoxRect, ControlData& parentControlData) const noexcept
        {
            const float previewShortLengthMax = 160.0f;
            const float previewShortLength = min(parentControlData._size._x * 0.5f, previewShortLengthMax);

            Rect previewRect;
            const Float2& dockZonePosition = parentControlData.getDockZonePositionCached(dockZone);
            previewRect.position(dockZonePosition);
            
            const DockZoneData& dockZoneData = parentControlData.getDockZoneData(dockZone);
            if (dockZoneData.isRawDockSizeSet())
            {
                previewRect.right(previewRect.left() + parentControlData.getDockZoneSizeCached(dockZone)._x);
                previewRect.bottom(previewRect.top() + parentControlData.getDockZoneSizeCached(dockZone)._y);
                return previewRect;
            }

            switch (dockZone)
            {
            case DockZone::LeftSide:
                previewRect.right(previewRect.left() + previewShortLength);
                previewRect.bottom(previewRect.top() + parentControlData._size._y - parentControlData.getDockOffsetSize()._y);
                break;
            case DockZone::RightSide:
                previewRect.right(previewRect.left() + previewShortLength);
                previewRect.bottom(previewRect.top() + parentControlData._size._y - parentControlData.getDockOffsetSize()._y);
                break;
            case DockZone::TopSide:
                previewRect.right(previewRect.left() + parentControlData._size._x);
                previewRect.bottom(previewRect.top() + previewShortLength);
                break;
            case DockZone::BottomSide:
                previewRect.right(previewRect.left() + parentControlData._size._x);
                previewRect.bottom(previewRect.top() + previewShortLength);
                break;
            default:
                break;
            }
            return previewRect;
        }

        void GUIContext::processControlCommon_dock_renderInteractionBox(const Rendering::Color& color, const Rect& interactionBoxRect, const ControlData& parentControlData) noexcept
        {
            const bool isMouseInBoxRect = interactionBoxRect.contains(_mouseStates.getPosition());
            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(RendererContextLayer::TopMost);
            rendererContext.setClipRect(parentControlData.getClipRects()._forMe);
            rendererContext.setColor(((isMouseInBoxRect == true) ? color.scaledRgb(1.5f) : color));
            rendererContext.setPosition(Float4(interactionBoxRect.center()._x, interactionBoxRect.center()._y, 0.0f, 1.0f));
            rendererContext.drawRectangle(interactionBoxRect.size(), kDockingInteractionDisplayBorderThickness, 0.0f);
        }

        void GUIContext::processControlCommon_dock_renderPreviewBox(const Rendering::Color& color, const Rect& previewRect) noexcept
        {
            Rendering::ShapeFontRendererContext& rendererContext = getRendererContext(RendererContextLayer::TopMost);
            rendererContext.setClipRect(_clipRectFullScreen);
            rendererContext.setColor(color.scaledA(0.5f));
            rendererContext.setPosition(Float4(previewRect.center()._x, previewRect.center()._y, 0.0f, 1.0f));
            rendererContext.drawRectangle(previewRect.size(), 0.0f, 0.0f);
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
            if (_controlInteractionStateSet.hasFocusedControl() == true && _controlInteractionStateSet.isControlFocused(controlData) == false && 
                isAncestorControlInteractionState(controlData, ControlInteractionState::Focused) == false)
            {
                // Focus 가 있는 Control 이 존재하지만 내가 Focus 는 아닌 경우

                Window::CursorType dummyCursorType;
                ResizingMethod dummyResizingMethod;
                ResizingMask dummyResizingMask;
                const ControlData& focusedControlData = getControlData(_controlInteractionStateSet.getFocusedControlID());
                if (ControlCommonHelpers::isInControlInnerInteractionArea(_mouseStates.getPosition(), focusedControlData) == true 
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

            if (_draggedControlID.isValid())
            {
                return controlData.getID() == _draggedControlID;
            }

            if (_resizedControlID.isValid() == true || controlData._option._isDraggable == false || isInteractingInternal(controlData) == false)
            {
                return false;
            }

            if (ControlCommonHelpers::isInControlInnerInteractionArea(_mouseStates.getPosition(), controlData) == true &&
                ControlCommonHelpers::isInControlInnerInteractionArea(_mouseStates.getButtonDownPosition(), controlData) == true)
            {
                // Begin dragging
                _isDragBegun = true;
                _draggedControlID = controlData.getID();
                return true;
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

            if (_resizedControlID.isValid())
            {
                return controlData.getID() == _resizedControlID;
            }

            if (_draggedControlID.isValid() == true || controlData.isResizable() == false || isInteractingInternal(controlData) == false)
            {
                return false;
            }

            Window::CursorType newCursorType;
            ResizingMask resizingMask;
            if (ControlCommonHelpers::isInControlBorderArea(_mouseStates.getPosition(), controlData, newCursorType, resizingMask, _resizingMethod) == true &&
                ControlCommonHelpers::isInControlBorderArea(_mouseStates.getButtonDownPosition(), controlData, newCursorType, resizingMask, _resizingMethod) == true &&
                controlData._resizingMask.overlaps(resizingMask) == true)
            {
                // Begin resizing
                _resizedControlID = controlData.getID();
                _isResizeBegun = true;
                _mouseStates._cursorType = newCursorType;
                return true;
            }
            return false;
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

        const bool GUIContext::isAncestorControlInteractionState(const ControlData& controlData, const ControlInteractionState controlInteractionState) const noexcept
        {
            switch (controlInteractionState)
            {
            case ControlInteractionState::None:
                break;
            case ControlInteractionState::Hovered:
                return isAncestorControlInteractionState_recursive(controlData.getParentID(), _controlInteractionStateSet.getHoveredControlID());
            case ControlInteractionState::Pressed:
                return isAncestorControlInteractionState_recursive(controlData.getParentID(), _controlInteractionStateSet.getPressedControlID());
            case ControlInteractionState::Clicked:
                return isAncestorControlInteractionState_recursive(controlData.getParentID(), _controlInteractionStateSet.getClickedControlID());
            case ControlInteractionState::Focused:
                return isAncestorControlInteractionState_recursive(controlData.getParentID(), _controlInteractionStateSet.getFocusedControlID());
            default:
                break;
            }
            return false;
        }

        const bool GUIContext::isAncestorControlInteractionState_recursive(const ControlID& id, const ControlID& targetID) const noexcept
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
            return isAncestorControlInteractionState_recursive(parentID, targetID);
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

            // #2. Descendant Control Focused
            const bool isDescendantFocused = isThisOrDescendantControlInteractionState(closestFocusableAncestorInclusive, ControlInteractionState::Focused);
            if (isDescendantFocused == true)
            {
                return true;
            }

            // #3. Docking
            const bool isDocking = closestFocusableAncestorInclusive.isDocking();
            const ControlData& dockControlData = getControlData(closestFocusableAncestorInclusive.getDockControlID());
            return (isDocking == true && (dockControlData.isRootControl() == true || _controlInteractionStateSet.isControlFocused(dockControlData) == true || 
                isThisOrDescendantControlInteractionState(dockControlData, ControlInteractionState::Focused) == true));
        }

        const bool GUIContext::isThisOrDescendantControlInteractionState(const ControlData& controlData, const ControlInteractionState controlInteractionState) const noexcept
        {
            switch (controlInteractionState)
            {
            case ControlInteractionState::None:
                break;
            case ControlInteractionState::Hovered:
                return isDescendantControlInclusive(controlData, _controlInteractionStateSet.getHoveredControlID());
            case ControlInteractionState::Pressed:
                return isDescendantControlInclusive(controlData, _controlInteractionStateSet.getPressedControlID());
            case ControlInteractionState::Clicked:
                return isDescendantControlInclusive(controlData, _controlInteractionStateSet.getClickedControlID());
            case ControlInteractionState::Focused:
                return isDescendantControlInclusive(controlData, _controlInteractionStateSet.getFocusedControlID());
            default:
                break;
            }
            return false;
        }

        const bool GUIContext::isDescendantControlInteractionState(const ControlData& controlData, const ControlInteractionState controlInteractionState) const noexcept
        {
            ControlID controlIDToCompare;
            switch (controlInteractionState)
            {
            case ControlInteractionState::None:
                break;
            case ControlInteractionState::Hovered:
                controlIDToCompare = _controlInteractionStateSet.getHoveredControlID();
                break;
            case ControlInteractionState::Pressed:
                controlIDToCompare = _controlInteractionStateSet.getPressedControlID();
                break;
            case ControlInteractionState::Clicked:
                controlIDToCompare = _controlInteractionStateSet.getClickedControlID();
                break;
            case ControlInteractionState::Focused:
                controlIDToCompare = _controlInteractionStateSet.getFocusedControlID();
                break;
            default:
                break;
            }

            const auto& previousChildControlIDs = controlData.getChildControlIDs();
            const uint32 previousChildControlCount = previousChildControlIDs.size();
            for (uint32 previousChildControlIndex = 0; previousChildControlIndex < previousChildControlCount; ++previousChildControlIndex)
            {
                const ControlID& previousChildControlID = previousChildControlIDs[previousChildControlIndex];
                if (isDescendantControlRecursiveXXX(previousChildControlID, controlIDToCompare) == true)
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
            static constexpr float kNoMouseWheelValue = 0.0f;
            if (_mouseStates._mouseWheel == kNoMouseWheelValue)
            {
                return kNoMouseWheelValue;
            }

            if (ControlCommonHelpers::isInControlInnerInteractionArea(_mouseStates.getPosition(), scrollParentControlData) == false)
            {
                return kNoMouseWheelValue;
            }

            const float mouseWheelScroll = _mouseStates._mouseWheel * kMouseWheelScrollScale;
            _mouseStates._mouseWheel = kNoMouseWheelValue;
            return mouseWheelScroll;
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
            MINT_ASSERT("김장원", _controlStack.empty() == true, "begin 과 end 호출 횟수가 맞지 않습니다!!!");

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

            _controlStack.clear();

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

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
#include <MintRenderingBase/Include/GUI/GUIInteractionModule.hpp>

#include <MintPlatform/Include/WindowsWindow.h>
#include <MintPlatform/Include/InputContext.h>


namespace mint
{
    using Platform::InputContext;
    using Platform::MouseButton;
    using Window::CursorType;


    namespace Rendering
    {
        namespace GUI
        {
            GUIContext::GUIContext(GraphicDevice& graphicDevice)
                : _graphicDevice{ graphicDevice }
                , _rendererContext{ graphicDevice }
                , _fontSize{ 0.0f }
                , _currentCursor{ CursorType::Arrow }
            {
                __noop;
            }

            GUIContext::~GUIContext()
            {
                __noop;
            }

            void GUIContext::initialize()
            {
                //_caretBlinkIntervalMs = _graphicDevice.getWindow().getCaretBlinkIntervalMs();

                const FontData& fontData = _graphicDevice.getShapeRendererContext().getFontData();
                _fontSize = static_cast<float>(fontData._fontSize);
                if (_rendererContext.initializeFontData(fontData) == false)
                {
                    MINT_ASSERT(false, "ShapeRendererContext::initializeFont() 에 실패했습니다!");
                }

                _rendererContext.initializeShaders();
                _rendererContext.setUseMultipleViewports();

                const Float2& windowSize = Float2(_graphicDevice.getWindowSize());
                updateScreenSize(windowSize);

                // ROOT
                _rootControlID = ControlID(kUint64Max);
                ControlData& rootControlData = accessControlData(_rootControlID, ControlType::COUNT);
                rootControlData._absolutePosition = Float2::kZero;
                //rootControlData._size = windowSize;
                _controlStack.push_back(_rootControlID);
            }

            void GUIContext::processEvent() noexcept
            {
                const InputContext& inputContext = InputContext::getInstance();
                if (inputContext.isMouseButtonPressed())
                {
                    _mousePressedPosition = inputContext.getMousePosition();
                }
                if (inputContext.isMouseButtonReleased() == false && inputContext.isMouseButtonUp(MouseButton::Left) == true)
                {
                    _mousePressedPosition = Float2::kNan;
                }

                _currentCursor = CursorType::Arrow;
            }

            void GUIContext::updateScreenSize(const Float2& newScreenSize)
            {
                // TODO
            }

            void GUIContext::render() noexcept
            {
                MINT_ASSERT(_controlStack.size() <= 1, "begin- 호출 횟수가 end- 호출 횟수보다 많습니다!!!");

                if (_focusingModule.isInteracting())
                {
                    // FocusedWindow 가 다른 Window (priority 0) 에 비해 위에 그려지도록 priority 1 로 설정.
                    _rendererContext.accessLowLevelRenderer().setOrdinalRenderCommandGroupPriority(_focusingModule.getControlID().getRawID(), 1);
                }

                if (_debugSwitch._raw != 0)
                {
                    for (const ControlID& controlID : _controlIDsOfCurrentFrame)
                    {
                        ControlData& controlData = accessControlData(controlID);
                        debugRender_control(controlData);
                    }
                }
                _controlIDsOfCurrentFrame.clear();

                _graphicDevice.accessWindow().setCursorType(_currentCursor);

                _rendererContext.render();
                _rendererContext.flush();
            }

            void GUIContext::nextControlSameLine()
            {
                _nextControlDesc._sameLine = true;
            }

            void GUIContext::nextControlPosition(const Float2& position)
            {
                _nextControlDesc._position = position;
            }

            void GUIContext::nextControlSize(const Float2& contentSize)
            {
                _nextControlDesc._size = contentSize;
            }

            void GUIContext::nextControlMargin(const Rect& margin)
            {
                _nextControlDesc._margin = margin;
            }

            void GUIContext::nextControlPadding(const Rect& padding)
            {
                _nextControlDesc._padding = padding;
            }

            void GUIContext::makeLabel(const FileLine& fileLine, const LabelDesc& labelDesc)
            {
                static constexpr ControlType kControlType = ControlType::Label;
                const ControlID parentControlID = accessStackParentControlData().getID();
                const ControlID controlID = ControlData::generateID(fileLine, kControlType, labelDesc._text, parentControlID);
                _controlIDsOfCurrentFrame.push_back(controlID);

                ControlData& controlData = accessControlData(controlID, kControlType);
                controlData._parentID = parentControlID;
                controlData._text = labelDesc._text;
                updateControlData(controlData);
                makeLabel_render(labelDesc, controlData);
            }

            bool GUIContext::makeButton(const FileLine& fileLine, const ButtonDesc& buttonDesc)
            {
                static constexpr ControlType kControlType = ControlType::Button;
                const ControlID parentControlID = accessStackParentControlData().getID();
                const ControlID controlID = ControlData::generateID(fileLine, kControlType, buttonDesc._text, parentControlID);
                _controlIDsOfCurrentFrame.push_back(controlID);

                ControlData& controlData = accessControlData(controlID, kControlType);
                controlData._parentID = parentControlID;
                controlData._text = buttonDesc._text;
                updateControlData(controlData);
                makeButton_render(buttonDesc, controlData);
                return controlData._mouseInteractionState == ControlData::MouseInteractionState::Clicked;
            }

            bool GUIContext::beginWindow(const FileLine& fileLine, const WindowDesc& windowDesc)
            {
                static constexpr ControlType kControlType = ControlType::Window;
                const ControlID parentControlID = accessStackParentControlData().getID();
                const ControlID controlID = ControlData::generateID(fileLine, kControlType, windowDesc._title, parentControlID);
                _controlIDsOfCurrentFrame.push_back(controlID);
                
                if (_isInBeginWindow)
                {
                    MINT_NEVER;
                }
                _isInBeginWindow = true;
                _rendererContext.accessLowLevelRenderer().beginOrdinalRenderCommands(controlID.getRawID());

                ControlData& controlData = accessControlData(controlID, kControlType);
                controlData._parentID = parentControlID;
                controlData._text = windowDesc._title;
                ControlData& parentControlData = accessControlData(parentControlID);
                const bool isNewlyCreated = (controlData.getAccessCount() == 0);
                if (isNewlyCreated)
                {
                    ControlData::PerTypeData::WindowData& windowData = controlData._perTypeData._windowData;
                    windowData._titleBarHeight = _fontSize + _theme._titleBarPadding.vert();
                    controlData._resizingMask.setAllTrue();
                    controlData._resizableMinSize = Float2(windowData._titleBarHeight * 2.0f);
                    controlData._generalTraits._isFocusable = true;
                    controlData._generalTraits._isDraggable = true;

                    nextControlPosition(windowDesc._initialPosition);
                    nextControlSize(windowDesc._initialSize);
                }
                else
                {
                    nextControlPosition(controlData.computeRelativePosition(parentControlData));
                    nextControlSize(controlData._size);
                }
                // No margin for window controls
                nextControlMargin(Rect());

                updateControlData(controlData);
                beginWindow_render(controlData);
                _controlStack.push_back(controlID);

                {
                    ButtonDesc closeButtonDesc;
                    closeButtonDesc._text = L"@CloseButton";
                    closeButtonDesc._isRoundButton = true;
                    closeButtonDesc._customizeColor = true;
                    closeButtonDesc._customizedColorSet = _theme._closeButtonColorSet;
                    const float titleBarHeight = controlData._zones._titleBarZone.height();
                    const float radius = _theme._systemButtonRadius;
                    nextControlPosition(Float2(controlData._size._x - _theme._titleBarPadding.right() - radius * 2.0f, titleBarHeight * 0.5f - radius));
                    nextControlSize(Float2(radius * 2.0f));
                    if (makeButton(fileLine, closeButtonDesc))
                    {

                    }
                }

                return true;
            }

            void GUIContext::endWindow()
            {
                _isInBeginWindow = false;
                _rendererContext.accessLowLevelRenderer().endOrdinalRenderCommands();

                if (_controlStack.empty())
                {
                    MINT_ASSERT(false, "end- 호출 횟수가 begin- 호출 횟수보다 많습니다!");
                    return;
                }
                MINT_ASSERT(accessControlData(_controlStack.back()).getType() == ControlType::Window, "Control Stack 이 비정상적입니다!");

                _controlStack.pop_back();
            }

            void GUIContext::makeLabel_render(const LabelDesc& labelDesc, const ControlData& controlData)
            {
                const Color& backgroundColor = labelDesc.getBackgroundColor(_theme);
                if (backgroundColor.a() > 0.0f)
                {
                    _rendererContext.setColor(backgroundColor);
                    _rendererContext.setPosition(computeShapePosition(controlData.getID()));
                    _rendererContext.drawRectangle(controlData._size, 0.0f, 0.0f);
                }
                FontRenderingOption fontRenderingOption;
                fontRenderingOption._directionHorz = labelDesc._directionHorz;
                fontRenderingOption._directionVert = labelDesc._directionVert;
                drawText(controlData.getID(), labelDesc.getTextColor(_theme), fontRenderingOption);
            }

            void GUIContext::makeButton_render(const ButtonDesc& buttonDesc, const ControlData& controlData)
            {
                const HoverPressColorSet& hoverPressColorSet = (buttonDesc._customizeColor) ? buttonDesc._customizedColorSet : _theme._hoverPressColorSet;
                _rendererContext.setColor(hoverPressColorSet.chooseColorByInteractionState(controlData._mouseInteractionState));
                _rendererContext.setPosition(computeShapePosition(controlData.getID()));
                if (buttonDesc._isRoundButton)
                {
                    const float radius = controlData._size._x * 0.5f;
                    _rendererContext.drawCircle(radius);
                }
                else
                {
                    _rendererContext.drawRoundedRectangle(controlData._size, computeRoundness(controlData.getID()), 0.0f, 0.0f);

                    FontRenderingOption fontRenderingOption;
                    fontRenderingOption._directionHorz = TextRenderDirectionHorz::Centered;
                    fontRenderingOption._directionVert = TextRenderDirectionVert::Centered;
                    drawText(controlData.getID(), _theme._textColor, fontRenderingOption);
                }
            }

            void GUIContext::beginWindow_render(const ControlData& controlData)
            {
                _rendererContext.setPosition(computeShapePosition(controlData.getID()));

                const bool isFocused = _focusingModule.isInteractingWith(controlData.getID());
                const float titleBarHeight = controlData._zones._titleBarZone.height();
                StackVector<ShapeRendererContext::Split, 3> splits;
                splits.push_back(ShapeRendererContext::Split(titleBarHeight / controlData._size._y, (isFocused ? _theme._windowTitleBarFocusedColor : _theme._windowTitleBarUnfocusedColor)));
                splits.push_back(ShapeRendererContext::Split(1.0f, _theme._windowBackgroundColor));
                _rendererContext.drawRoundedRectangleVertSplit(controlData._size, _theme._roundnessInPixel, splits, 0.0f);

                FontRenderingOption titleBarFontRenderingOption;
                titleBarFontRenderingOption._directionHorz = TextRenderDirectionHorz::Rightward;
                titleBarFontRenderingOption._directionVert = TextRenderDirectionVert::Centered;
                const ControlData& parentControlData = accessControlData(controlData._parentID);
                const Float2 titleBarTextPosition = controlData.computeRelativePosition(parentControlData) + Float2(_theme._titleBarPadding.left(), 0.0f);
                const Float2 titleBarSize = Float2(controlData._size._x, titleBarHeight);
                drawText(titleBarTextPosition, titleBarSize, controlData._text, _theme._textColor, titleBarFontRenderingOption);
            }

            ControlData& GUIContext::accessControlData(const ControlID& controlID) const
            {
                static ControlData invalid;
                auto found = _controlDataMap.find(controlID);
                if (found.isValid())
                {
                    uint64& accessCount = const_cast<uint64&>(found._value->getAccessCount());
                    ++accessCount;
                    return *found._value;
                }
                return invalid;
            }

            ControlData& GUIContext::accessControlData(const ControlID& controlID, const ControlType controlType)
            {
                ControlData& controlData = accessControlData(controlID);
                if (controlData.getID() != controlID)
                {
                    _controlDataMap.insert(controlID, ControlData(controlID, controlType));
                    return *_controlDataMap.find(controlID)._value;
                }
                return controlData;
            }

            ControlID GUIContext::findAncestorWindowControl(const ControlID& controlID) const
            {
                ControlData& controlData = accessControlData(controlID);
                if (controlData.getType() == ControlType::Window)
                {
                    return controlID;
                }

                ControlID foundControlID = controlData._parentID;
                while (foundControlID.isValid())
                {
                    ControlData& currentControlData = accessControlData(foundControlID);
                    if (currentControlData.getType() == ControlType::Window)
                    {
                        return foundControlID;
                    }
                    foundControlID = currentControlData._parentID;
                }
                return _rootControlID;
            }

            ControlData& GUIContext::accessStackParentControlData()
            {
                return accessControlData(_controlStack.back());
            }

            void GUIContext::updateControlData(ControlData& controlData)
            {
                updateControlData_processResizing(controlData);
                updateControlData_processDragging(controlData);

                updateControlData_renderingData(controlData);
                updateControlData_interaction(controlData);
                updateControlData_resetNextControlDesc();
            }

            void GUIContext::updateControlData_processResizing(const ControlData& controlData)
            {
                if (_resizingModule.isInteractingWith(controlData.getID()) == false)
                {
                    return;
                }

                const InputContext& inputContext = InputContext::getInstance();
                const ControlData::ResizingFlags& resizingFlags = _resizingModule.getResizingFlags();
                selectResizingCursorType(resizingFlags);

                Float2 displacementSize = inputContext.getMousePosition() - _resizingModule.getMousePressedPosition();
                if (resizingFlags._left == false && resizingFlags._right == false)
                {
                    displacementSize._x = 0.0f;
                }
                if (resizingFlags._top == false && resizingFlags._bottom == false)
                {
                    displacementSize._y = 0.0f;
                }

                if (resizingFlags._left == true || resizingFlags._top == true)
                {
                    Float2 displacementPosition = displacementSize;
                    if (resizingFlags._left)
                    {
                        displacementSize._x *= -1.0f;
                    }
                    else
                    {
                        displacementPosition._x = 0.0f;
                    }

                    if (resizingFlags._top)
                    {
                        displacementSize._y *= -1.0f;
                    }
                    else
                    {
                        displacementPosition._y *= 0.0f;
                    }

                    const Float2 maxPosition = _resizingModule.getInitialControlPosition() + _resizingModule.getInitialControlSize() - controlData._resizableMinSize;
                    nextControlPosition(Float2::min(_resizingModule.getInitialControlPosition() + displacementPosition, maxPosition));
                }

                nextControlSize(Float2::max(_resizingModule.getInitialControlSize() + displacementSize, controlData._resizableMinSize));
            }

            void GUIContext::updateControlData_processDragging(const ControlData& controlData)
            {
                if (_draggingModule.isInteractingWith(controlData.getID()) == false)
                {
                    return;
                }

                const InputContext& inputContext = InputContext::getInstance();
                const Float2 displacement = inputContext.getMousePosition() - _draggingModule.getMousePressedPosition();
                const Float2 absolutePosition = _draggingModule.getInitialControlPosition() + displacement;
                const Float2 relativePosition = absolutePosition - accessControlData(controlData._parentID)._absolutePosition;
                nextControlPosition(relativePosition);
            }

            void GUIContext::updateControlData_renderingData(ControlData& controlData)
            {
                const Float2 controlRelativePosition = _nextControlDesc._position;
                const Float2 controlSize = _nextControlDesc._size;

                controlData._nextChildNextLinePosition = controlData._zones._contentZone.position();

                // Position
                const bool isAutoPositioned = controlRelativePosition.isNan();
                ControlData& parentControlData = accessControlData(controlData._parentID);
                const Float2& parentNextChildPosition = (_nextControlDesc._sameLine ? parentControlData._nextChildSameLinePosition : parentControlData._nextChildNextLinePosition);
                const Float2 relativePosition = (isAutoPositioned ? parentNextChildPosition : controlRelativePosition);
                controlData._absolutePosition = relativePosition;
                controlData._absolutePosition += parentControlData._absolutePosition;
                if (isAutoPositioned)
                {
                    // Only auto-positioned controls need margin
                    controlData._absolutePosition._x += _nextControlDesc._margin.left();
                    controlData._absolutePosition._y += _nextControlDesc._margin.top();
                }

                // Size
                const bool isAutoSized = controlSize.isNan();
                if (isAutoSized)
                {
                    const FontData& fontData = _rendererContext.getFontData();
                    const float textWidth = fontData.computeTextWidth(controlData._text, StringUtil::length(controlData._text));
                    controlData._size._x = textWidth + _nextControlDesc._padding.horz();
                    controlData._size._y = _fontSize + _nextControlDesc._padding.vert();
                }
                else
                {
                    controlData._size = controlSize;
                }

                if (isAutoPositioned)
                {
                    // If its position is specified, the control does not affect its parent's _nextChildSameLinePosition nor _nextChildNextLinePosition.
                    parentControlData._nextChildSameLinePosition = relativePosition + Float2(controlData._size._x + _nextControlDesc._margin.right(), 0.0f);

                    parentControlData._nextChildNextLinePosition._x = parentControlData._zones._contentZone.position()._x;
                    parentControlData._nextChildNextLinePosition._y = relativePosition._y + controlData._size._y + _nextControlDesc._margin.bottom();
                }

                parentControlData._zones._contentZone.expandRightBottom(Rect(relativePosition, controlData._size));

                controlData.updateZones();
            }

            void GUIContext::updateControlData_interaction(ControlData& controlData)
            {
                const InputContext& inputContext = InputContext::getInstance();
                controlData._mouseInteractionState = ControlData::MouseInteractionState::None;
                const Float2& mousePosition = inputContext.getMousePosition();
                if (_focusingModule.isInteracting())
                {
                    const ControlID ancestorWindowControlID = findAncestorWindowControl(controlData.getID());
                    if (_focusingModule.isInteractingWith(ancestorWindowControlID) == false)
                    {
                        // FocusedWindow 에 속하지 않은 ControlData 인데, FocusedWindow 영역 내에 Mouse 가 있는 경우,
                        // interaction 을 진행하지 않는다!
                        const ControlData& focusedControlData = accessControlData(_focusingModule.getControlID());
                        const Rect focusedControlRect = Rect(focusedControlData._absolutePosition, focusedControlData._size);
                        if (focusedControlRect.contains(mousePosition))
                        {
                            return;
                        }
                    }
                }

                const bool isMouseLeftUp = inputContext.isMouseButtonUp(MouseButton::Left);
                const bool isMousePositionIn = Rect(controlData._absolutePosition, controlData._size).contains(mousePosition);
                const Float2 relativePressedMousePosition = _mousePressedPosition - controlData._absolutePosition;
                if (isMousePositionIn)
                {
                    const bool isPressedMousePositionIn = Rect(Float2::kZero, controlData._size).contains(relativePressedMousePosition);
                    controlData._mouseInteractionState = isPressedMousePositionIn ? ControlData::MouseInteractionState::Pressing : ControlData::MouseInteractionState::Hovering;
                    if (isPressedMousePositionIn == true && isMouseLeftUp == true)
                    {
                        controlData._mouseInteractionState = ControlData::MouseInteractionState::Clicked;
                    }
                }

                updateControlData_interaction_focusing(controlData);
                updateControlData_interaction_resizing(controlData);
                updateControlData_interaction_dragging(controlData);
            }

            void GUIContext::updateControlData_interaction_focusing(ControlData& controlData)
            {
                if (controlData._generalTraits._isFocusable == false)
                {
                    return;
                }

                // 이미 Focus 되어 있는 컨트롤
                if (_focusingModule.isInteractingWith(controlData.getID()) == true)
                {
                    return;
                }

                if (controlData._mouseInteractionState == ControlData::MouseInteractionState::Clicked
                    || _draggingModule.isInteractingWith(controlData.getID()) == true
                    || _resizingModule.isInteractingWith(controlData.getID()) == true)
                {
                    _focusingModule.end();

                    InteractionMousePressModuleInput interactionMousePressModuleInput;
                    interactionMousePressModuleInput._controlID = controlData.getID();
                    interactionMousePressModuleInput._controlPosition = controlData._absolutePosition;
                    interactionMousePressModuleInput._mousePressedPosition = _mousePressedPosition;
                    _focusingModule.begin(interactionMousePressModuleInput);
                }
            }

            void GUIContext::updateControlData_interaction_resizing(ControlData& controlData)
            {
                // Dragging 중에는 Resizing 을 하지 않는다.
                if (_draggingModule.isInteracting())
                {
                    return;
                }

                const InputContext& inputContext = InputContext::getInstance();
                const bool isMouseLeftUp = inputContext.isMouseButtonUp(MouseButton::Left);
                if (isMouseLeftUp)
                {
                    _resizingModule.end();
                }

                if (controlData._resizingMask.isAllFalse())
                {
                    return;
                }

                Rect outerRect;
                Rect innerRect;
                ResizingModule::makeOuterAndInenrRects(controlData, _theme._outerResizingDistances, _theme._innerResizingDistances, outerRect, innerRect);
                const Float2& mousePosition = inputContext.getMousePosition();
                if (outerRect.contains(mousePosition) == true && innerRect.contains(mousePosition) == false)
                {
                    // Hover
                    const ControlData::ResizingFlags resizingInteraction = ResizingModule::makeResizingFlags(mousePosition, controlData, outerRect, innerRect);
                    selectResizingCursorType(resizingInteraction);
                }

                if (inputContext.isMouseButtonDown(MouseButton::Left))
                {
                    if (outerRect.contains(_mousePressedPosition) == true && innerRect.contains(_mousePressedPosition) == false)
                    {
                        ResizingModuleInput resizingModuleInput;
                        resizingModuleInput._controlID = controlData.getID();
                        resizingModuleInput._controlPosition = controlData._absolutePosition;
                        resizingModuleInput._controlSize = controlData._size;
                        resizingModuleInput._mousePressedPosition = _mousePressedPosition;
                        resizingModuleInput._resizingInteraction = ResizingModule::makeResizingFlags(_mousePressedPosition, controlData, outerRect, innerRect);
                        _resizingModule.begin(resizingModuleInput);
                    }
                }
                else
                {
                    _resizingModule.end();
                }
            }

            void GUIContext::updateControlData_interaction_dragging(ControlData& controlData)
            {
                if (controlData._generalTraits._isDraggable == false)
                {
                    return;
                }

                // Resizing 중에는 Dragging 을 하지 않는다.
                if (_resizingModule.isInteracting())
                {
                    return;
                }

                const InputContext& inputContext = InputContext::getInstance();
                const bool isMouseLeftUp = inputContext.isMouseButtonUp(MouseButton::Left);
                if (isMouseLeftUp)
                {
                    _draggingModule.end();
                    return;
                }

                // ParentControl 에 beginDragging 을 호출했지만 ChildControl 과도 Interaction 을 하고 있다면 ParentControl 에 endDragging 을 호출한다.
                const ControlData& parentControlData = accessControlData(controlData._parentID);
                if (_draggingModule.isInteractingWith(parentControlData.getID()))
                {
                    const Float2 draggingRelativePressedMousePosition = _draggingModule.computeRelativeMousePressedPosition() - controlData.computeRelativePosition(parentControlData);
                    if (controlData._zones._visibleContentZone.contains(draggingRelativePressedMousePosition))
                    {
                        _draggingModule.end();
                    }
                }

                // TODO: Draggable Control 에 대한 처리도 추가
                const Float2 relativePressedMousePosition = _mousePressedPosition - controlData._absolutePosition;
                if (controlData._zones._titleBarZone.contains(relativePressedMousePosition))
                {
                    InteractionMousePressModuleInput interactionMousePressModuleInput;
                    interactionMousePressModuleInput._controlID = controlData.getID();
                    interactionMousePressModuleInput._controlPosition = controlData._absolutePosition;
                    interactionMousePressModuleInput._mousePressedPosition = _mousePressedPosition;
                    _draggingModule.begin(interactionMousePressModuleInput);
                }
            }

            void GUIContext::updateControlData_resetNextControlDesc()
            {
                _nextControlDesc._sameLine = false;
                _nextControlDesc._position = Float2::kNan;
                _nextControlDesc._size = Float2::kNan;
                _nextControlDesc._margin = _theme._defaultMargin;
                _nextControlDesc._padding = _theme._defaultPadding;
            }

            void GUIContext::selectResizingCursorType(const ControlData::ResizingFlags& resizingFlags)
            {
                if ((resizingFlags._top && resizingFlags._left) || (resizingFlags._bottom && resizingFlags._right))
                {
                    _currentCursor = CursorType::SizeLeftTilted;
                }
                else if ((resizingFlags._top && resizingFlags._right) || (resizingFlags._bottom && resizingFlags._left))
                {
                    _currentCursor = CursorType::SizeRightTilted;
                }
                else if (resizingFlags._top || resizingFlags._bottom)
                {
                    _currentCursor = CursorType::SizeVert;
                }
                else if (resizingFlags._left || resizingFlags._right)
                {
                    _currentCursor = CursorType::SizeHorz;
                }
            }

            void GUIContext::drawText(const ControlID& controlID, const Color& color, const FontRenderingOption& fontRenderingOption)
            {
                const ControlData& controlData = accessControlData(controlID);
                drawText(controlData._absolutePosition, controlData._size, controlData._text, color, fontRenderingOption);
            }

            void GUIContext::drawText(const Float2& position, const Float2& size, const wchar_t* const text, const Color& color, const FontRenderingOption& fontRenderingOption)
            {
                _rendererContext.setTextColor(color);

                const Float2 halfSize = size * 0.5f;
                Float2 finalPosition = position + halfSize;
                if (fontRenderingOption._directionHorz != TextRenderDirectionHorz::Centered)
                {
                    finalPosition._x += (fontRenderingOption._directionHorz == TextRenderDirectionHorz::Rightward ? -halfSize._x : halfSize._x);
                }
                if (fontRenderingOption._directionVert != TextRenderDirectionVert::Centered)
                {
                    finalPosition._y += (fontRenderingOption._directionVert == TextRenderDirectionVert::Downward ? -halfSize._y : halfSize._y);
                }
                _rendererContext.drawDynamicText(text, Float4(finalPosition), fontRenderingOption);
            }

            Float4 GUIContext::computeShapePosition(const ControlID& controlID) const
            {
                const ControlData& controlData = accessControlData(controlID);
                return computeShapePosition(controlData._absolutePosition, controlData._size);
            }

            Float4 GUIContext::computeShapePosition(const Float2& position, const Float2& size) const
            {
                return Float4(position + size * 0.5f);
            }

            float GUIContext::computeRoundness(const ControlID& controlID) const
            {
                const ControlData& controlData = accessControlData(controlID);
                return _rendererContext.computeNormalizedRoundness(controlData._size.minElement(), _theme._roundnessInPixel);
            }

            void GUIContext::debugRender_control(const ControlData& controlData)
            {
                if (_debugSwitch._renderZoneOverlay)
                {
                    static const float OVERLAY_ALPHA = 0.125f;
                    const Float2 titleBarZoneSize = controlData._zones._titleBarZone.size();
                    if (titleBarZoneSize._x != 0.0f && titleBarZoneSize._y != 0.0f)
                    {
                        _rendererContext.setColor(Color(1.0f, 0.5f, 0.25f, OVERLAY_ALPHA));
                        _rendererContext.setPosition(computeShapePosition(controlData._absolutePosition + controlData._zones._titleBarZone.position(), titleBarZoneSize));
                        _rendererContext.drawRectangle(titleBarZoneSize, 0.0f, 0.0f);
                    }

                    const Float2 contentZoneSize = controlData._zones._contentZone.size();
                    if (contentZoneSize._x != 0.0f && contentZoneSize._y != 0.0f)
                    {
                        _rendererContext.setColor(Color(0.25f, 1.0f, 0.5f, OVERLAY_ALPHA));
                        _rendererContext.setPosition(computeShapePosition(controlData._absolutePosition + controlData._zones._contentZone.position(), contentZoneSize));
                        _rendererContext.drawRectangle(contentZoneSize, 0.0f, 0.0f);
                    }

                    const Float2 visibleContentZoneSize = controlData._zones._visibleContentZone.size();
                    if (visibleContentZoneSize._x != 0.0f && visibleContentZoneSize._y != 0.0f)
                    {
                        _rendererContext.setColor(Color(0.25f, 0.25f, 1.0f, OVERLAY_ALPHA));
                        _rendererContext.setPosition(computeShapePosition(controlData._absolutePosition + controlData._zones._visibleContentZone.position(), visibleContentZoneSize));
                        _rendererContext.drawRectangle(visibleContentZoneSize, 0.0f, 0.0f);
                    }
                }

                if (_debugSwitch._renderMousePoints)
                {
                    static const float POINT_RADIUS = 4.0f;
                    if (_draggingModule.isInteractingWith(controlData.getID()))
                    {
                        const InputContext& inputContext = InputContext::getInstance();
                        _rendererContext.setColor(Color::kBlue);
                        _rendererContext.setPosition(Float4(_draggingModule.getMousePressedPosition()));
                        _rendererContext.drawCircle(POINT_RADIUS);

                        _rendererContext.setColor(Color::kRed);
                        _rendererContext.setPosition(Float4(controlData._absolutePosition + _draggingModule.getMousePressedPosition() - _draggingModule.getInitialControlPosition()));
                        _rendererContext.drawCircle(POINT_RADIUS);
                    }
                }

                if (_debugSwitch._renderResizingArea && controlData._resizingMask.isAnyTrue())
                {
                    const Rect controlRect = Rect(controlData._absolutePosition, controlData._size);
                    Rect outerRect = controlRect;
                    outerRect.expandByQuantity(_theme._outerResizingDistances);
                    Rect innerRect = controlRect;
                    innerRect.shrinkByQuantity(_theme._innerResizingDistances);

                    _rendererContext.setColor(Color(0.0f, 0.0f, 1.0f, 0.25f));
                    _rendererContext.setPosition(computeShapePosition(outerRect.position(), outerRect.size()));
                    _rendererContext.drawRectangle(outerRect.size(), 0.0f, 0.0f);

                    _rendererContext.setColor(Color(0.0f, 1.0f, 0.0f, 0.25f));
                    _rendererContext.setPosition(computeShapePosition(innerRect.position(), innerRect.size()));
                    _rendererContext.drawRectangle(innerRect.size(), 0.0f, 0.0f);
                }
            }
        }
    }
}

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

#include <MintPlatform/Include/WindowsWindow.h>
#include <MintPlatform/Include/InputContext.h>


namespace mint
{
    using Platform::InputContext;
    using Platform::MouseButton;


    namespace Rendering
    {
        GUIContext::GUIContext(GraphicDevice& graphicDevice)
            : _graphicDevice{ graphicDevice }
            , _rendererContext{ graphicDevice }
            , _fontSize{ 0.0f }
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
            const ControlID rootControlID{ ControlID(kUint64Max) };
            ControlData& rootControlData = accessControlData(rootControlID, ControlType::COUNT);
            rootControlData._absolutePosition = Float2::kZero;
            _controlStack.push_back(rootControlID);
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
        }

        void GUIContext::updateScreenSize(const Float2& newScreenSize)
        {
            // TODO
        }

        void GUIContext::render() noexcept
        {
            MINT_ASSERT(_controlStack.size() <= 1, "begin- 호출 횟수가 end- 호출 횟수보다 많습니다!!!");

            _rendererContext.render();
            _rendererContext.flush();
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
            _nextControlDesc._renderingDesc._margin = margin;
        }

        void GUIContext::nextControlPadding(const Rect& padding)
        {
            _nextControlDesc._renderingDesc._padding = padding;
        }

        void GUIContext::makeLabel(const FileLine& fileLine, const LabelDesc& labelDesc)
        {
            static constexpr ControlType kControlType = ControlType::Label;
            ControlData& parentControlData = accessStackParentControlData();
            const ControlID controlID = ControlData::generateID(fileLine, kControlType, labelDesc._text, parentControlData.getID());
            ControlData& controlData = accessControlData(controlID, kControlType);

            ControlDesc controlDesc;
            updateControlData(labelDesc._text, controlDesc, controlData, parentControlData);
            makeLabel_render(controlDesc, labelDesc, controlData);
        }

        const bool GUIContext::makeButton(const FileLine& fileLine, const ButtonDesc& buttonDesc)
        {
            static constexpr ControlType kControlType = ControlType::Button;
            ControlData& parentControlData = accessStackParentControlData();
            const ControlID controlID = ControlData::generateID(fileLine, kControlType, buttonDesc._text, parentControlData.getID());
            ControlData& controlData = accessControlData(controlID, kControlType);

            ControlDesc controlDesc;
            updateControlData(buttonDesc._text, controlDesc, controlData, parentControlData);
            makeButton_render(controlDesc, buttonDesc, controlData);
            return controlData._interactionState == ControlData::InteractionState::Clicked;
        }

        const bool GUIContext::beginWindow(const FileLine& fileLine, const WindowDesc& windowDesc)
        {
            static constexpr ControlType kControlType = ControlType::Window;
            ControlData& parentControlData = accessStackParentControlData();
            const ControlID controlID = ControlData::generateID(fileLine, kControlType, windowDesc._title, parentControlData.getID());
            ControlData& controlData = accessControlData(controlID, kControlType);

            const bool isNewlyCreated = (controlData.getAccessCount() == 0);
            if (isNewlyCreated)
            {
                float& titleBarHeight = controlData._perTypeData._windowData._titleBarHeight;
                titleBarHeight = _fontSize + _theme._titleBarPadding.vert();

                controlData._resizingMask.setAllTrue();
            }
            nextControlPosition((isNewlyCreated ? windowDesc._initialPosition : controlData.computeRelativePosition(parentControlData)));
            nextControlSize((isNewlyCreated ? windowDesc._initialSize : controlData._size));
            // No margin for window controls
            nextControlMargin(Rect());

            ControlDesc controlDesc;
            updateControlData(windowDesc._title, controlDesc, controlData, parentControlData);
            beginWindow_render(controlDesc, controlData, parentControlData);

            _controlStack.push_back(controlID);

            {
                ButtonDesc closeButtonDesc;
                closeButtonDesc._text = L"@CloseButton";
                closeButtonDesc._isRoundButton = true;
                closeButtonDesc._customizeColor = true;
                closeButtonDesc._customizedColorSet = _theme._closeButtonColorSet;
                const float titleBarHeight = controlData._zones._titleBarZone.vert();
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
            if (_controlStack.empty())
            {
                MINT_ASSERT(false, "end- 호출 횟수가 begin- 호출 횟수보다 많습니다!");
                return;
            }
            MINT_ASSERT(accessControlData(_controlStack.back()).getType() == ControlType::Window, "Control Stack 이 비정상적입니다!");

            _controlStack.pop_back();
        }

        void GUIContext::makeLabel_render(const ControlDesc& controlDesc, const LabelDesc& labelDesc, const ControlData& controlData)
        {
            const Color& backgroundColor = labelDesc.getBackgroundColor(_theme);
            if (backgroundColor.a() > 0.0f)
            {
                _rendererContext.setColor(backgroundColor);
                _rendererContext.setPosition(computeShapePosition(controlDesc));
                _rendererContext.drawRectangle(controlData._size, 0.0f, 0.0f);
            }
            FontRenderingOption fontRenderingOption;
            fontRenderingOption._directionHorz = labelDesc._directionHorz;
            fontRenderingOption._directionVert = labelDesc._directionVert;
            drawText(controlDesc, labelDesc.getTextColor(_theme), fontRenderingOption);

            renderControlCommon(controlData);
        }

        void GUIContext::makeButton_render(const ControlDesc& controlDesc, const ButtonDesc& buttonDesc, const ControlData& controlData)
        {
            const HoverPressColorSet& hoverPressColorSet = (buttonDesc._customizeColor) ? buttonDesc._customizedColorSet : _theme._hoverPressColorSet;
            _rendererContext.setColor(hoverPressColorSet.chooseColorByInteractionState(controlData._interactionState));
            _rendererContext.setPosition(computeShapePosition(controlDesc));
            if (buttonDesc._isRoundButton)
            {
                const float radius = controlData._size._x * 0.5f;
                _rendererContext.drawCircle(radius);
            }
            else
            {
                _rendererContext.drawRoundedRectangle(controlData._size, computeRoundness(controlDesc), 0.0f, 0.0f);

                FontRenderingOption fontRenderingOption;
                fontRenderingOption._directionHorz = TextRenderDirectionHorz::Centered;
                fontRenderingOption._directionVert = TextRenderDirectionVert::Centered;
                drawText(controlDesc, _theme._textColor, fontRenderingOption);
            }

            renderControlCommon(controlData);
        }

        void GUIContext::beginWindow_render(const ControlDesc& controlDesc, const ControlData& controlData, const ControlData& parentControlData)
        {
            _rendererContext.setColor(_theme._windowBackgroundColor);
            _rendererContext.setPosition(computeShapePosition(controlDesc));

            const float titleBarHeight = controlData._zones._titleBarZone.vert();
            ScopeVector<ShapeRendererContext::Split, 3> splits;
            splits.push_back(ShapeRendererContext::Split(titleBarHeight / controlData._size._y, _theme._windowTitleBarFocusedColor));
            splits.push_back(ShapeRendererContext::Split(1.0f, _theme._windowBackgroundColor));
            _rendererContext.drawRoundedRectangleVertSplit(controlData._size, _theme._roundnessInPixel, splits, 0.0f);

            FontRenderingOption fontRenderingOption;
            fontRenderingOption._directionHorz = TextRenderDirectionHorz::Rightward;
            fontRenderingOption._directionVert = TextRenderDirectionVert::Centered;
            const Float2 titleBarTextPosition = controlData.computeRelativePosition(parentControlData) + Float2(_theme._titleBarPadding.left(), 0.0f);
            const Float2 titleBarSize = Float2(controlData._size._x, titleBarHeight);
            drawText(titleBarTextPosition, titleBarSize, controlDesc._renderingDesc._text, _theme._textColor, fontRenderingOption);

            renderControlCommon(controlData);
        }

        void GUIContext::renderControlCommon(const ControlData& controlData)
        {
            static bool RENDER_ZONE_OVERLAY = false;
            static bool RENDER_MOUSE_POINTS = false;
            static bool RENDER_RESIZING_AREA = false;

            static const float OVERLAY_ALPHA = 0.25f;
            static const float POINT_RADIUS = 4.0f;
            if (RENDER_ZONE_OVERLAY)
            {
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
            }

            if (RENDER_MOUSE_POINTS)
            {
                if (_draggingModule.isInteracting())
                {
                    _rendererContext.setColor(Color::kBlue);
                    _rendererContext.setPosition(Float4(_mousePressedPosition));
                    _rendererContext.drawCircle(POINT_RADIUS);

                    _rendererContext.setColor(Color::kRed);
                    _rendererContext.setPosition(Float4(_draggingModule.getMousePressedPosition()));
                    _rendererContext.drawCircle(POINT_RADIUS);
                }
            }

            if (RENDER_RESIZING_AREA)
            {
                const Rect controlRect = Rect(controlData._absolutePosition, controlData._size);
                Rect outerRect = controlRect;
                outerRect.expandByQuantity(_theme._outerResizingDistance);
                Rect innerRect = controlRect;
                innerRect.shrinkByQuantity(_theme._innerResizingDistance);

                _rendererContext.setColor(Color(0.0f, 0.0f, 1.0f, 0.25f));
                _rendererContext.setPosition(computeShapePosition(outerRect.position(), outerRect.size()));
                _rendererContext.drawRectangle(outerRect.size(), 0.0f, 0.0f);

                _rendererContext.setColor(Color(0.0f, 1.0f, 0.0f, 0.25f));
                _rendererContext.setPosition(computeShapePosition(innerRect.position(), innerRect.size()));
                _rendererContext.drawRectangle(innerRect.size(), 0.0f, 0.0f);
            }
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

        ControlData& GUIContext::accessStackParentControlData()
        {
            return accessControlData(_controlStack.back());
        }

        void GUIContext::updateControlData(const wchar_t* const text, ControlDesc& controlDesc, ControlData& controlData, ControlData& parentControlData)
        {
            controlDesc._controlID = controlData.getID();

            updateControlData_processResizing(controlData, parentControlData);
            updateControlData_processDragging(controlData, parentControlData);

            updateControlData_renderingData(text, controlDesc, controlData, parentControlData);
            updateControlData_interaction(controlDesc, controlData, parentControlData);
            updateControlData_resetNextControlDesc();
        }

        void GUIContext::updateControlData_processResizing(const ControlData& controlData, const ControlData& parentControlData)
        {
            if (_resizingModule.isInteracting(controlData) == false)
            {
                return;
            }

            const InputContext& inputContext = InputContext::getInstance();
            const ControlData::ResizingFlags& resizingFlags = _resizingModule.getResizingFlags();
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
                nextControlPosition(_resizingModule.getInitialControlPosition() + displacementPosition);
            }
            
            nextControlSize(_resizingModule.getInitialControlSize() + displacementSize);
        }

        void GUIContext::updateControlData_processDragging(const ControlData& controlData, const ControlData& parentControlData)
        {
            if (_draggingModule.isInteracting(controlData) == false)
            {
                return;
            }
            
            const InputContext& inputContext = InputContext::getInstance();
            const Float2 displacement = inputContext.getMousePosition() - _draggingModule.getMousePressedPosition();
            const Float2 absolutePosition = _draggingModule.getInitialControlPosition() + displacement;
            const Float2 relativePosition = absolutePosition - parentControlData._absolutePosition;
            nextControlPosition(relativePosition);
        }

        void GUIContext::updateControlData_renderingData(const wchar_t* const text, ControlDesc& controlDesc, ControlData& controlData, ControlData& parentControlData)
        {
            ControlRenderingDesc& controlRenderingDesc = controlDesc._renderingDesc;
            controlRenderingDesc = _nextControlDesc._renderingDesc;
            controlRenderingDesc._text = text;
            const Float2 controlRelativePosition = _nextControlDesc._position;
            const Float2 controlSize = _nextControlDesc._size;

            controlData._nextChildRelativePosition = controlData._zones._contentZone.position();

            // Position
            const bool isAutoPositioned = controlRelativePosition.isNan();
            const Float2 relativePosition = (isAutoPositioned ? parentControlData._nextChildRelativePosition : controlRelativePosition);
            controlData._absolutePosition = relativePosition;
            controlData._absolutePosition += parentControlData._absolutePosition;
            if (isAutoPositioned)
            {
                // Only auto-positioned controls need margin
                controlData._absolutePosition._x += controlRenderingDesc._margin.left();
                controlData._absolutePosition._y += controlRenderingDesc._margin.top();
            }

            // Size
            const bool isAutoSized = controlSize.isNan();
            if (isAutoSized)
            {
                const FontData& fontData = _rendererContext.getFontData();
                const float textWidth = fontData.computeTextWidth(text, StringUtil::length(text));
                controlData._size._x = textWidth + controlRenderingDesc._padding.horz();
                controlData._size._y = _fontSize + controlRenderingDesc._padding.vert();
            }
            else
            {
                controlData._size = controlSize;
            }

            if (isAutoPositioned)
            {
                // If its position is specified, the control does not affect its parent's _nextChildRelativePosition.
                parentControlData._nextChildRelativePosition = relativePosition + Float2(0.0f, controlData._size._y + controlRenderingDesc._margin.bottom());
            }

            parentControlData._zones._contentZone.expandRightBottom(Rect(relativePosition, controlData._size));

            controlData.updateZones();
        }

        void GUIContext::updateControlData_interaction(ControlDesc& controlDesc, ControlData& controlData, ControlData& parentControlData)
        {
            const InputContext& inputContext = InputContext::getInstance();

            ControlData::InteractionState& interactionState = controlData._interactionState;
            interactionState = ControlData::InteractionState::None;
            const Float2& mousePosition = inputContext.getMousePosition();
            const bool isMouseLeftUp = inputContext.isMouseButtonUp(MouseButton::Left);
            const bool isMousePositionIn = Rect(controlData._absolutePosition, controlData._size).contains(mousePosition);
            const Float2 relativePressedMousePosition = _mousePressedPosition - controlData._absolutePosition;
            if (isMousePositionIn)
            {
                const bool isPressedMousePositionIn = Rect(Float2::kZero, controlData._size).contains(relativePressedMousePosition);
                interactionState = isPressedMousePositionIn ? ControlData::InteractionState::Pressing : ControlData::InteractionState::Hovering;
                if (isPressedMousePositionIn == true && isMouseLeftUp == true)
                {
                    interactionState = ControlData::InteractionState::Clicked;
                }
            }

            updateControlData_interaction_resizing(controlData);
            updateControlData_interaction_dragging(controlData, parentControlData);
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
                return;
            }

            if (controlData._resizingMask.isAllFalse())
            {
                return;
            }
            
            const Rect controlRect = Rect(controlData._absolutePosition, controlData._size);
            Rect outerRect = controlRect;
            outerRect.expandByQuantity(_theme._outerResizingDistance);
            Rect innerRect = controlRect;
            innerRect.shrinkByQuantity(_theme._innerResizingDistance);
            if (outerRect.contains(_mousePressedPosition) == true && innerRect.contains(_mousePressedPosition) == false)
            {
                ControlData::ResizingFlags resizingInteraction;
                if (_mousePressedPosition._y >= outerRect.top() && _mousePressedPosition._y <= innerRect.top())
                {
                    resizingInteraction._top = true;
                }
                if (_mousePressedPosition._y <= outerRect.bottom() && _mousePressedPosition._y >= innerRect.bottom())
                {
                    resizingInteraction._bottom = true;
                }
                if (_mousePressedPosition._x >= outerRect.left() && _mousePressedPosition._x <= innerRect.left())
                {
                    resizingInteraction._left = true;
                }
                if (_mousePressedPosition._x <= outerRect.right() && _mousePressedPosition._x >= innerRect.right())
                {
                    resizingInteraction._right = true;
                }
                resizingInteraction.maskBy(controlData._resizingMask);

                _resizingModule.begin(controlData, _mousePressedPosition, resizingInteraction);
            }
        }

        void GUIContext::updateControlData_interaction_dragging(ControlData& controlData, const ControlData& parentControlData)
        {
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
            if (_draggingModule.isInteracting(parentControlData))
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
                _draggingModule.begin(controlData, _mousePressedPosition);
            }
        }

        void GUIContext::updateControlData_resetNextControlDesc()
        {
            _nextControlDesc._position = Float2::kNan;
            _nextControlDesc._size = Float2::kNan;
            _nextControlDesc._renderingDesc._margin = _theme._defaultMargin;
            _nextControlDesc._renderingDesc._padding = _theme._defaultPadding;
        }

        void GUIContext::drawText(const ControlDesc& controlDesc, const Color& color, const FontRenderingOption& fontRenderingOption)
        {
            const ControlData& controlData = accessControlData(controlDesc._controlID);
            drawText(controlData._absolutePosition, controlData._size, controlDesc._renderingDesc._text, color, fontRenderingOption);
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

        Float4 GUIContext::computeShapePosition(const ControlDesc& controlDesc) const
        {
            const ControlData& controlData = accessControlData(controlDesc._controlID);
            return computeShapePosition(controlData._absolutePosition, controlData._size);
        }

        Float4 GUIContext::computeShapePosition(const Float2& position, const Float2& size) const
        {
            return Float4(position + size * 0.5f);
        }

        const float GUIContext::computeRoundness(const ControlDesc& controlDesc) const
        {
            const ControlData& controlData = accessControlData(controlDesc._controlID);
            return _rendererContext.computeNormalizedRoundness(controlData._size.minElement(), _theme._roundnessInPixel);
        }

#pragma region InteractionModule
        const bool GUIContext::InteractionModule::isInteracting() const
        {
            return _controlID.isValid();
        }

        const bool GUIContext::InteractionModule::isInteracting(const ControlData& controlData) const
        {
            return _controlID == controlData.getID();
        }

        Float2 GUIContext::InteractionModule::computeRelativeMousePressedPosition() const
        {
            return _mousePressedPosition - _initialControlPosition;
        }

        const bool GUIContext::InteractionModule::beginInternal(const ControlData& controlData, const Float2& mousePressedPosition)
        {
            if (isInteracting())
            {
                return false;
            }

            _controlID = controlData.getID();
            _initialControlPosition = controlData._absolutePosition;
            _mousePressedPosition = mousePressedPosition;

            return true;
        }

        void GUIContext::InteractionModule::endInternal()
        {
            _controlID.invalidate();
        }
#pragma endregion

        void GUIContext::ResizingModule::begin(const ControlData& controlData, const Float2& mousePressedPosition, const ControlData::ResizingFlags& resizingFlags)
        {
            if (resizingFlags.isAllFalse() == true)
            {
                return;
            }
            
            if (beginInternal(controlData, mousePressedPosition) == false)
            {
                return;
            }

            _initialControlSize = controlData._size;
            _resizingFlags = resizingFlags;
        }
    }
}

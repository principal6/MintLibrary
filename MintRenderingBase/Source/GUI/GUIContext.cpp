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
            rootControlData._position = Float2::kZero;
            _controlStack.push_back(rootControlID);
        }

        void GUIContext::processEvent() noexcept
        {
            const InputContext& inputContext = InputContext::getInstance();
            if (inputContext.isMouseButtonPressed())
            {
                _mouseDownPosition = inputContext.getMousePosition();
            }
            if (inputContext.isMouseButtonReleased() == false && inputContext.isMouseButtonUp(MouseButton::Left) == true)
            {
                _mouseDownPosition = Float2::kNan;
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
            _nextControlRenderingDesc._position = position;
        }

        void GUIContext::nextControlSize(const Float2& contentSize)
        {
            _nextControlRenderingDesc._size = contentSize;
        }

        void GUIContext::makeLabel(const FileLine& fileLine, const LabelDesc& labelDesc)
        {
            static constexpr ControlType type = ControlType::Label;
            const ControlID ID = ControlData::generateID(fileLine, type, labelDesc._text);
            ControlData& controlData = accessControlData(ID, type);
            ControlData& parentControlData = accessStackParentControlData();

            ControlDesc controlDesc;
            fillControlDesc_controlRenderingDesc(labelDesc._text, controlDesc, controlData, parentControlData);
            //fillControlDesc_interactionState(controlDesc);

            makeLabel_render(controlDesc, labelDesc);
        }

        const bool GUIContext::makeButton(const FileLine& fileLine, const ButtonDesc& buttonDesc)
        {
            static constexpr ControlType type = ControlType::Button;
            const ControlID ID = ControlData::generateID(fileLine, type, buttonDesc._text);
            ControlData& controlData = accessControlData(ID, type);
            ControlData& parentControlData = accessStackParentControlData();

            ControlDesc controlDesc;
            fillControlDesc_controlRenderingDesc(buttonDesc._text, controlDesc, controlData, parentControlData);
            fillControlDesc_interactionState(controlDesc);

            makeButton_render(controlDesc, buttonDesc);
            return controlDesc._interactionState == InteractionState::Clicked;
        }

        const bool GUIContext::beginWindow(const FileLine& fileLine, const WindowDesc& windowDesc)
        {
            static constexpr ControlType type = ControlType::Window;
            const ControlID ID = ControlData::generateID(fileLine, type, windowDesc._title);
            ControlData& controlData = accessControlData(ID, type);
            ControlData& parentControlData = accessStackParentControlData();
            
            ControlDesc controlDesc;
            fillControlDesc_controlRenderingDesc(windowDesc._title, controlDesc, controlData, parentControlData);
            fillControlDesc_interactionState(controlDesc);
            
            beginWindow_render(controlDesc, controlData);

            {
                ButtonDesc closeButtonDesc;
                closeButtonDesc._text = L"#CloseButton";
                closeButtonDesc._isRoundButton = true;
                closeButtonDesc._customizeColor = true;
                closeButtonDesc._customizedColorSet = _theme._closeButtonColorSet;
                const float titleBarHeight = controlData.computeTitleBarZone().size()._y;
                const float radius = _theme._systemButtonRadius;
                nextControlPosition(controlData._position + Float2(controlData._size._x - _theme._titleBarPadding.right() - radius * 2.0f, titleBarHeight * 0.5f - radius));
                nextControlSize(Float2(radius * 2.0f));
                if (makeButton(fileLine, closeButtonDesc))
                {

                }
            }

            _controlStack.push_back(ID);
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

        void GUIContext::makeLabel_render(const ControlDesc& controlDesc, const LabelDesc& labelDesc)
        {
            const ControlRenderingDesc& controlRenderingDesc = controlDesc._controlRenderingDesc;
            const InteractionState& interactionState = controlDesc._interactionState;

            const Color& backgroundColor = labelDesc.getBackgroundColor(_theme);
            if (backgroundColor.a() > 0.0f)
            {
                _rendererContext.setColor(backgroundColor);
                _rendererContext.setPosition(computeShapePosition(controlRenderingDesc));
                _rendererContext.drawRectangle(controlRenderingDesc._size, controlRenderingDesc._borderThickness, 0.0f);
            }
            FontRenderingOption fontRenderingOption;
            fontRenderingOption._directionHorz = labelDesc._directionHorz;
            fontRenderingOption._directionVert = labelDesc._directionVert;
            drawText(controlRenderingDesc, labelDesc.getTextColor(_theme), fontRenderingOption);
        }

        void GUIContext::makeButton_render(const ControlDesc& controlDesc, const ButtonDesc& buttonDesc)
        {
            const ControlRenderingDesc& controlRenderingDesc = controlDesc._controlRenderingDesc;
            const InteractionState& interactionState = controlDesc._interactionState;

            const HoverPressColorSet& hoverPressColorSet = (buttonDesc._customizeColor) ? buttonDesc._customizedColorSet : _theme._hoverPressColorSet;
            _rendererContext.setColor(hoverPressColorSet.chooseColorByInteractionState(interactionState));
            _rendererContext.setPosition(computeShapePosition(controlRenderingDesc));
            if (buttonDesc._isRoundButton)
            {
                const float radius = controlRenderingDesc._size._x * 0.5f;
                _rendererContext.drawCircle(radius);
            }
            else
            {
                _rendererContext.drawRoundedRectangle(controlRenderingDesc._size, computeRoundness(controlRenderingDesc), controlRenderingDesc._borderThickness, 0.0f);

                FontRenderingOption fontRenderingOption;
                fontRenderingOption._directionHorz = TextRenderDirectionHorz::Centered;
                fontRenderingOption._directionVert = TextRenderDirectionVert::Centered;
                drawText(controlRenderingDesc, _theme._textColor, fontRenderingOption);
            }
        }

        void GUIContext::beginWindow_render(const ControlDesc& controlDesc, const ControlData& controlData)
        {
            const ControlRenderingDesc& controlRenderingDesc = controlDesc._controlRenderingDesc;
            const InteractionState& interactionState = controlDesc._interactionState;

            _rendererContext.setColor(_theme._windowBackgroundColor);
            _rendererContext.setPosition(computeShapePosition(controlRenderingDesc));
            
            const float titleBarHeight = controlData.computeTitleBarZone().vert();
            ScopeVector<ShapeRendererContext::Split, 3> splits;
            splits.push_back(ShapeRendererContext::Split(titleBarHeight / controlRenderingDesc._size._y, _theme._windowTitleBarFocusedColor));
            splits.push_back(ShapeRendererContext::Split(1.0f, _theme._windowBackgroundColor));
            _rendererContext.drawRoundedRectangleVertSplit(controlRenderingDesc._size, _theme._roundnessInPixel, splits, 0.0f);

            FontRenderingOption fontRenderingOption;
            fontRenderingOption._directionHorz = TextRenderDirectionHorz::Rightward;
            fontRenderingOption._directionVert = TextRenderDirectionVert::Centered;
            const Float2 titleBarTextPosition = controlData._position + Float2(_theme._titleBarPadding.left(), 0.0f);
            const Float2 titleBarSize = Float2(controlData._size._x, titleBarHeight);
            drawText(titleBarTextPosition, titleBarSize, controlRenderingDesc._text, _theme._textColor, fontRenderingOption);
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

        void GUIContext::fillControlDesc_controlRenderingDesc(const wchar_t* const text, ControlDesc& controlDesc, ControlData& controlData, ControlData& parentControlData)
        {
            ControlRenderingDesc& controlRenderingDesc = controlDesc._controlRenderingDesc;
            controlRenderingDesc = _nextControlRenderingDesc;
            controlRenderingDesc._text = text;

            // Reset _nextControlRenderingDesc
            {
                _nextControlRenderingDesc._borderThickness = _theme._defaultBorderThickness;
                _nextControlRenderingDesc._margin = _theme._defaultMargin;
                _nextControlRenderingDesc._padding = _theme._defaultPadding;
                _nextControlRenderingDesc._position = Float2::kNan;
                _nextControlRenderingDesc._size = Float2::kNan;
            }

            controlData._nextChildPosition = controlData.computeContentZone().position();

            if (controlData.getType() == ControlType::Window)
            {
                // Specified size and position are only applied the first time the control is created!

                if (controlData.getAccessCount() == 0)
                {
                    controlData._position = controlRenderingDesc._position;
                    controlData._size = controlRenderingDesc._size;

                    float& titleBarHeight = controlData._perTypeData._windowData._titleBarHeight;
                    titleBarHeight = _fontSize + _theme._titleBarPadding.vert();
                }
                else
                {
                    controlRenderingDesc._position = controlData._position;
                    controlRenderingDesc._size = controlData._size;
                }
            }
            else
            {
                const bool isPositionSpecified = controlRenderingDesc._position.isNan() == false;
                const bool isSizeSpecified = controlRenderingDesc._size.isNan() == false;
                if (isPositionSpecified)
                {
                    // Specified (just turning it into a relative position)
                    controlData._position = controlRenderingDesc._position;
                    controlRenderingDesc._position += parentControlData._position;
                }
                else
                {
                    // Automated
                    controlData._position = parentControlData._nextChildPosition;
                    controlRenderingDesc._position = controlData._position + parentControlData._position;
                    controlRenderingDesc._position._x += controlRenderingDesc._margin.left();
                    controlRenderingDesc._position._y += controlRenderingDesc._margin.top();
                }
                controlRenderingDesc._position._x += controlRenderingDesc._borderThickness;
                controlRenderingDesc._position._y += controlRenderingDesc._borderThickness;

                if (isSizeSpecified == false)
                {
                    // Automated
                    const FontData& fontData = _rendererContext.getFontData();
                    const float textWidth = fontData.computeTextWidth(text, StringUtil::length(text));
                    controlRenderingDesc._size._x = textWidth + controlRenderingDesc._padding.horz() + controlRenderingDesc._borderThickness * 2.0f;
                    controlRenderingDesc._size._y = _fontSize + controlRenderingDesc._padding.vert() + controlRenderingDesc._borderThickness * 2.0f;
                }
                
                controlData._size = controlRenderingDesc._size;

                if (isPositionSpecified == false)
                {
                    parentControlData._nextChildPosition = controlData._position + Float2(0.0f, controlData._size._y + controlRenderingDesc._margin.bottom());
                }
            }
        }

        void GUIContext::fillControlDesc_interactionState(ControlDesc& controlDesc) const
        {
            const InputContext& inputContext = InputContext::getInstance();

            const ControlRenderingDesc& controlRenderingDesc = controlDesc._controlRenderingDesc;
            InteractionState& interactionState = controlDesc._interactionState;
            interactionState = InteractionState::None;
            if (isMouseCursorInControl(controlRenderingDesc, inputContext.getMousePosition()))
            {
                interactionState = isMouseCursorInControl(controlRenderingDesc, _mouseDownPosition) ? InteractionState::Pressing : InteractionState::Hovering;

                if (inputContext.isMouseButtonUp(MouseButton::Left) && isMouseCursorInControl(controlRenderingDesc, _mouseDownPosition))
                {
                    interactionState = InteractionState::Clicked;
                }
            }
        }

        void GUIContext::drawText(const ControlRenderingDesc& controlRenderingDesc, const Color& color, const FontRenderingOption& fontRenderingOption)
        {
            drawText(controlRenderingDesc._position, controlRenderingDesc._size, controlRenderingDesc._text, color, fontRenderingOption);
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

        Float4 GUIContext::computeShapePosition(const ControlRenderingDesc& controlRenderingDesc) const
        {
            return Float4(controlRenderingDesc._position + controlRenderingDesc._size * 0.5f + Float2(controlRenderingDesc._borderThickness));
        }

        const float GUIContext::computeRoundness(const ControlRenderingDesc& controlRenderingDesc) const
        {
            return _rendererContext.computeNormalizedRoundness(controlRenderingDesc._size.minElement(), _theme._roundnessInPixel);
        }

        const bool GUIContext::isMouseCursorInControl(const ControlRenderingDesc& controlRenderingDesc, const Float2& mouseCurosrPosition) const
        {
            return Rect::fromPositionSize(controlRenderingDesc._position, controlRenderingDesc._size).contains(mouseCurosrPosition);
        }
    }
}

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
            ControlDesc controlDesc;
            fillControlDesc_controlRenderingDesc(labelDesc._text, controlDesc);

            ControlData& controlData = accessControlData(ID, type);
            
            makeLabel_render(controlDesc, labelDesc);
        }

        const bool GUIContext::makeButton(const FileLine& fileLine, const ButtonDesc& buttonDesc)
        {
            static constexpr ControlType type = ControlType::Button;
            const ControlID ID = ControlData::generateID(fileLine, type, buttonDesc._text);
            ControlDesc controlDesc;
            fillControlDesc_controlRenderingDesc(buttonDesc._text, controlDesc);

            ControlData& controlData = accessControlData(ID, type);

            fillControlDesc_interactionState(controlDesc);

            makeButton_render(controlDesc, buttonDesc);

            return controlDesc._interactionState == InteractionState::Clicked;
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

        ControlData& GUIContext::accessControlData(const ControlID& controlID, const ControlType controlType)
        {
            auto found = _controlDataMap.find(controlID);
            if (found.isValid())
            {
                return *found._value;
            }

            _controlDataMap.insert(controlID, ControlData(controlID, controlType));
            return *_controlDataMap.find(controlID)._value;
        }

        void GUIContext::fillControlDesc_controlRenderingDesc(const wchar_t* const text, ControlDesc& controlDesc)
        {
            controlDesc._controlRenderingDesc = _nextControlRenderingDesc;
            controlDesc._controlRenderingDesc._text = text;

            _nextControlRenderingDesc._borderThickness = _theme._defaultBorderThickness;
            _nextControlRenderingDesc._margin = _theme._defaultMargin;
            _nextControlRenderingDesc._padding = _theme._defaultPadding;
            
            _nextControlRenderingDesc._position = Float2::kNan;
            _nextControlRenderingDesc._size = Float2::kNan;
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
            _rendererContext.setTextColor(color);

            const Float2 halfSize = controlRenderingDesc._size * 0.5f;
            Float2 position = controlRenderingDesc._position + halfSize;
            if (fontRenderingOption._directionHorz != TextRenderDirectionHorz::Centered)
            {
                position._x += (fontRenderingOption._directionHorz == TextRenderDirectionHorz::Rightward ? -halfSize._x : halfSize._x);
            }
            if (fontRenderingOption._directionVert != TextRenderDirectionVert::Centered)
            {
                position._y += (fontRenderingOption._directionVert == TextRenderDirectionVert::Downward ? -halfSize._y : halfSize._y);
            }
            _rendererContext.drawDynamicText(controlRenderingDesc._text, Float4(position), fontRenderingOption);
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

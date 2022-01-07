#pragma once


#include <stdafx.h>
#include <MintRenderingBase/Include/GUI_/InputHelpers.h>

#include <MintContainer/Include/String.hpp>
#include <MintContainer/Include/StringUtil.hpp>

#include <MintRenderingBase/Include/GUI_/ControlData.hpp>
#include <MintRenderingBase/Include/ShapeFontRendererContext.h>

#include <MintPlatform/Include/InputContext.h>

#include <MintLibrary/Include/Profiler.h>


namespace mint
{
    namespace GUI
    {
#pragma region MouseStates
        MINT_INLINE void MouseStates::PerButtonStates::resetPerFrame() noexcept
        {
            _isButtonDownUp = false;
            _isButtonDownThisFrame = false;
            _isDoubleClicked = false;
        }

        MINT_INLINE void MouseStates::resetPerFrame() noexcept
        {
            for (uint32 buttonIndex = 0; buttonIndex < Platform::getMouseButtonCount(); buttonIndex++)
            {
                _perButtonStates[buttonIndex].resetPerFrame();
            }
        }

        MINT_INLINE void MouseStates::setPosition(const Float2& position) noexcept
        {
            _mousePosition = position;

            computeMouseDragDelta();
        }

        MINT_INLINE void MouseStates::setButtonDownPosition(const Float2& position) noexcept
        {
            _mouseDownPositionCopy = _mouseDownPosition = position;

            computeMouseDragDelta();
        }

        MINT_INLINE void MouseStates::setButtonDownPositionCopy(const Float2& position) noexcept
        {
            _mouseDownPositionCopy = position;

            computeMouseDragDelta();
        }

        MINT_INLINE void MouseStates::setButtonUpPosition(const Float2& position) noexcept
        {
            _mouseUpPosition = position;
        }

        MINT_INLINE void MouseStates::setButtonDown(const Platform::MouseButton mouseButton) noexcept
        {
            _perButtonStates[static_cast<uint32>(mouseButton)]._isButtonDown = true;
            _perButtonStates[static_cast<uint32>(mouseButton)]._isButtonDownThisFrame = true;
        }

        MINT_INLINE void MouseStates::setButtonUp(const Platform::MouseButton mouseButton) noexcept
        {
            if (_perButtonStates[static_cast<uint32>(mouseButton)]._isButtonDown == true)
            {
                _perButtonStates[static_cast<uint32>(mouseButton)]._isButtonDownUp = true;
            }
            _perButtonStates[static_cast<uint32>(mouseButton)]._isButtonDown = false;
        }

        MINT_INLINE void MouseStates::setDoubleClicked(const Platform::MouseButton mouseButton) noexcept
        {
            _perButtonStates[static_cast<uint32>(mouseButton)]._isDoubleClicked = true;
        }

        MINT_INLINE void MouseStates::computeMouseDragDelta() noexcept
        {
            _mouseDragDelta = _mousePosition - _mouseDownPositionCopy;
        }

        MINT_INLINE const Float2& MouseStates::getPosition() const noexcept
        {
            return _mousePosition;
        }

        MINT_INLINE const Float2& MouseStates::getButtonDownPosition() const noexcept
        {
            return _mouseDownPosition;
        }

        MINT_INLINE const Float2& MouseStates::getButtonUpPosition() const noexcept
        {
            return _mouseUpPosition;
        }

        MINT_INLINE const Float2& MouseStates::getMouseDragDelta() const noexcept
        {
            return _mouseDragDelta;
        }

        MINT_INLINE const bool MouseStates::isButtonDown(const Platform::MouseButton mouseButton) const noexcept
        {
            return _perButtonStates[static_cast<uint32>(mouseButton)]._isButtonDown;
        }

        MINT_INLINE const bool MouseStates::isButtonDownThisFrame(const Platform::MouseButton mouseButton) const noexcept
        {
            return _perButtonStates[static_cast<uint32>(mouseButton)]._isButtonDownThisFrame;
        }

        MINT_INLINE const bool MouseStates::isButtonDownUp(const Platform::MouseButton mouseButton) const noexcept
        {
            return _perButtonStates[static_cast<uint32>(mouseButton)]._isButtonDownUp;
        }

        MINT_INLINE const bool MouseStates::isDoubleClicked(const Platform::MouseButton mouseButton) const noexcept
        {
            return _perButtonStates[static_cast<uint32>(mouseButton)]._isDoubleClicked;
        }

        MINT_INLINE const bool MouseStates::isCursor(const Window::CursorType cursorType) const noexcept
        {
            return _cursorType == cursorType;
        }
#pragma endregion


        MINT_INLINE void InputBoxHelpers::updateCaretState(const uint32 caretBlinkIntervalMs, ControlData& controlData) noexcept
        {
            uint16& caretState = controlData._controlValue._textBoxData._caretState;
            uint64& lastCaretBlinkTimeMs = controlData._controlValue._textBoxData._lastCaretBlinkTimeMs;
            const uint64 currentTimeMs = Profiler::getCurrentTimeMs();
            if (lastCaretBlinkTimeMs + caretBlinkIntervalMs < currentTimeMs)
            {
                lastCaretBlinkTimeMs = currentTimeMs;

                caretState ^= 1;
            }
        }

        MINT_INLINE void InputBoxHelpers::refreshCaret(ControlData& controlData) noexcept
        {
            uint64& lastCaretBlinkTimeMs = controlData._controlValue._textBoxData._lastCaretBlinkTimeMs;
            uint16& caretState = controlData._controlValue._textBoxData._caretState;
            lastCaretBlinkTimeMs = Profiler::getCurrentTimeMs();
            caretState = 0;
        }

        inline void InputBoxHelpers::processDefaultMouseInputs(const MouseStates& mouseStates, const Rendering::ShapeFontRendererContext& rendererContext,
            ControlData& controlData, const Float4& textRenderOffset, const StringW& outText, TextBoxProcessInputResult& result) noexcept
        {
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            const float textDisplayOffset = controlData._controlValue._textBoxData._textDisplayOffset;
            const float positionInText = mouseStates.getPosition()._x - controlData._position._x + textDisplayOffset - textRenderOffset._x;
            const uint16 textLength = static_cast<uint16>(outText.length());
            const uint32 newCaretAt = rendererContext.computeIndexFromPositionInText(outText.c_str(), textLength, positionInText);
            if (mouseStates.isButtonDownThisFrame(Platform::MouseButton::Left) == true)
            {
                caretAt = newCaretAt;

                controlData._controlValue._textBoxData._selectionLength = 0;
                controlData._controlValue._textBoxData._selectionStart = caretAt;
            }
            else
            {
                if (newCaretAt != caretAt)
                {
                    updateSelection(caretAt, newCaretAt, controlData);

                    caretAt = newCaretAt;
                }
            }

            result._clearWcharInput = true;
            result._clearKeyCode = true;
        }

        inline void InputBoxHelpers::processDefaultKeyboardInputs(const Window::IWindow* const window, const Rendering::ShapeFontRendererContext& rendererContext,
            ControlData& controlData, const TextInputMode textInputMode, const uint32 maxTextLength, Platform::KeyCode& keyCode,
            wchar_t& wcharInput, const wchar_t wcharInputCandidate, const Float4& textRenderOffset, StringW& outText, TextBoxProcessInputResult& result) noexcept
        {
            Platform::InputContext& inputContext = Platform::InputContext::getInstance();
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            const bool isShiftKeyDown = inputContext.isKeyDown(Platform::KeyCode::Shift);
            const bool isControlKeyDown = inputContext.isKeyDown(Platform::KeyCode::Control);
            const uint16 oldCaretAt = controlData._controlValue._textBoxData._caretAt;
            if (wcharInputCandidate >= 32)
            {
                InputBoxHelpers::eraseSelection(controlData, outText);
            }
            else if (wcharInput != L'\0')
            {
                // 글자 입력 or 키 입력

                const bool isInputCharacter = (wcharInput >= 32);
                if (isInputCharacter == true)
                {
                    const uint16 futureCaretAt = InputBoxHelpers::computeCaretAtIfErasedSelection(controlData, outText);
                    if (InputBoxHelpers::isValidCharacterInput(wcharInput, futureCaretAt, textInputMode, outText) == true)
                    {
                        InputBoxHelpers::eraseSelection(controlData, outText);

                        if (InputBoxHelpers::insertWchar(wcharInput, caretAt, outText) == false)
                        {
                            window->showMessageBox(L"오류", InputBoxHelpers::getLengthErrorMessage(kTextBoxMaxTextLength), Window::MessageBoxType::Error);
                        }
                    }
                }
                else
                {
                    InputBoxHelpers::processAsciiControlFunctions(window, wcharInput, maxTextLength, controlData, outText);
                }

                result._clearWcharInput = true;
            }
            else
            {
                InputBoxHelpers::processKeyCodeCaretMovements(rendererContext, keyCode, controlData, outText);
            }

            // Caret 위치가 바뀐 경우 refresh
            if (oldCaretAt != caretAt)
            {
                InputBoxHelpers::refreshCaret(controlData);

                // Selection
                if (isShiftKeyDown == true && keyCode != Platform::KeyCode::NONE)
                {
                    InputBoxHelpers::updateSelection(oldCaretAt, caretAt, controlData);
                }
            }

            if (isShiftKeyDown == false && isControlKeyDown == false &&
                keyCode != Platform::KeyCode::NONE &&
                keyCode != Platform::KeyCode::Control &&
                keyCode != Platform::KeyCode::Alt)
            {
                InputBoxHelpers::deselect(controlData);
            }
        }

        inline void InputBoxHelpers::eraseBefore(ControlData& controlData, StringW& outText) noexcept
        {
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            const uint16 selectionLength = controlData._controlValue._textBoxData._selectionLength;
            if (selectionLength > 0)
            {
                eraseSelection(controlData, outText);
            }
            else
            {
                caretAt = min(caretAt, static_cast<uint16>(outText.length()));

                if (outText.empty() == false && caretAt > 0)
                {
                    outText.erase(caretAt - 1);

                    caretAt = max(caretAt - 1, 0);
                }
            }
        }

        MINT_INLINE void InputBoxHelpers::processAsciiControlFunctions(const Window::IWindow* const window, const wchar_t asciiCode, const uint32 maxTextLength, ControlData& controlData, StringW& outText) noexcept
        {
            Platform::InputContext& inputContext = Platform::InputContext::getInstance();
            const bool isControlKeyDown = inputContext.isKeyDown(Platform::KeyCode::Control);
            if (asciiCode == VK_BACK) // BackSpace
            {
                InputBoxHelpers::eraseBefore(controlData, outText);
            }
            else if (isControlKeyDown == true && asciiCode == 0x01) // Ctrl + A
            {
                InputBoxHelpers::selectAll(controlData, outText);
            }
            else if (isControlKeyDown == true && asciiCode == 0x03) // Ctrl + C
            {
                InputBoxHelpers::copySelection(window, controlData, outText);
            }
            else if (isControlKeyDown == true && asciiCode == 0x18) // Ctrl + X
            {
                InputBoxHelpers::cutSelection(window, controlData, outText);
            }
            else if (isControlKeyDown == true && asciiCode == 0x16) // Ctrl + V
            {
                InputBoxHelpers::paste(window, controlData, outText, getLengthErrorMessage(maxTextLength));
            }
        }

        inline void InputBoxHelpers::eraseAfter(ControlData& controlData, StringW& outText) noexcept
        {
            const uint16 selectionLength = controlData._controlValue._textBoxData._selectionLength;
            if (selectionLength > 0)
            {
                eraseSelection(controlData, outText);
            }
            else
            {
                const uint16 textLength = static_cast<uint16>(outText.length());
                uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
                if (textLength > 0 && caretAt < textLength)
                {
                    outText.erase(caretAt);

                    caretAt = min(caretAt, textLength);
                }
            }
        }

        MINT_INLINE void InputBoxHelpers::selectAll(ControlData& controlData, const StringW& outText) noexcept
        {
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            controlData._controlValue._textBoxData._selectionStart = 0;
            caretAt = controlData._controlValue._textBoxData._selectionLength = static_cast<uint16>(outText.length());
        }

        inline void InputBoxHelpers::copySelection(const Window::IWindow* const window, ControlData& controlData, const StringW& outText) noexcept
        {
            const uint16 selectionLength = controlData._controlValue._textBoxData._selectionLength;
            if (selectionLength == 0)
            {
                return;
            }

            const uint16 selectionStart = controlData._controlValue._textBoxData._selectionStart;
            window->textToClipboard(&outText[selectionStart], selectionLength);
        }

        MINT_INLINE void InputBoxHelpers::cutSelection(const Window::IWindow* const window, ControlData& controlData, StringW& outText) noexcept
        {
            copySelection(window, controlData, outText);

            eraseSelection(controlData, outText);
        }

        inline void InputBoxHelpers::paste(const Window::IWindow* const window, ControlData& controlData, StringW& outText, const wchar_t* const errorMessage) noexcept
        {
            StringW fromClipboard;
            window->textFromClipboard(fromClipboard);

            if (fromClipboard.empty() == true)
            {
                return;
            }

            eraseSelection(controlData, outText);

            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            if (insertWstring(fromClipboard, caretAt, outText) == false)
            {
                if (errorMessage != nullptr)
                {
                    window->showMessageBox(L"오류", errorMessage, Window::MessageBoxType::Error);
                }
            }
        }

        MINT_INLINE void InputBoxHelpers::processKeyCodeCaretMovements(const Rendering::ShapeFontRendererContext& rendererContext, const Platform::KeyCode keyCode,
            ControlData& controlData, StringW& outText) noexcept
        {
            if (keyCode == Platform::KeyCode::Left)
            {
                InputBoxHelpers::moveCaretToPrev(controlData);
            }
            else if (keyCode == Platform::KeyCode::Right)
            {
                InputBoxHelpers::moveCaretToNext(controlData, outText);
            }
            else if (keyCode == Platform::KeyCode::Home)
            {
                InputBoxHelpers::moveCaretToHead(controlData);
            }
            else if (keyCode == Platform::KeyCode::End)
            {
                InputBoxHelpers::moveCaretToTail(rendererContext, controlData, outText);
            }
            else if (keyCode == Platform::KeyCode::Delete)
            {
                InputBoxHelpers::eraseAfter(controlData, outText);
            }
        }

        MINT_INLINE void InputBoxHelpers::moveCaretToPrev(ControlData& controlData) noexcept
        {
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            caretAt = max(caretAt - 1, 0);
        }

        MINT_INLINE void InputBoxHelpers::moveCaretToNext(ControlData& controlData, const StringW& text) noexcept
        {
            const uint16 textLength = static_cast<uint16>(text.length());
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            caretAt = min(caretAt + 1, static_cast<int32>(textLength));
        }

        MINT_INLINE void InputBoxHelpers::moveCaretToHead(ControlData& controlData) noexcept
        {
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            caretAt = 0;

            float& textDisplayOffset = controlData._controlValue._textBoxData._textDisplayOffset;
            textDisplayOffset = 0.0f;
        }

        MINT_INLINE void InputBoxHelpers::moveCaretToTail(const Rendering::ShapeFontRendererContext& rendererContext, ControlData& controlData, const StringW& text) noexcept
        {
            const uint16 textLength = static_cast<uint16>(text.length());
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            caretAt = textLength;

            float& textDisplayOffset = controlData._controlValue._textBoxData._textDisplayOffset;
            const float textWidth = rendererContext.computeTextWidth(text.c_str(), textLength);
            textDisplayOffset = max(0.0f, textWidth - controlData._size._x);
        }

        inline const bool InputBoxHelpers::insertWchar(const wchar_t input, uint16& caretAt, StringW& outText) noexcept
        {
            if (outText.length() < kTextBoxMaxTextLength)
            {
                caretAt = min(caretAt, static_cast<uint16>(outText.length()));

                outText.insert(caretAt, input);

                ++caretAt;

                return true;
            }
            return false;
        }

        inline const bool InputBoxHelpers::insertWstring(const StringW& input, uint16& caretAt, StringW& outText)  noexcept
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

        inline const bool InputBoxHelpers::isValidCharacterInput(const wchar_t input, const uint16 caretAt, const TextInputMode textInputMode, const StringW& text) noexcept
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
                    if (text.find(input) == kStringNPos)
                    {
                        result = true;
                    }
                }
                else if (kMinusSign == input)
                {
                    if (text.find(input) == kStringNPos && caretAt == 0)
                    {
                        result = true;
                    }
                }
            }

            return result;
        }

        MINT_INLINE void InputBoxHelpers::deselect(ControlData& controlData) noexcept
        {
            uint16& selectionLength = controlData._controlValue._textBoxData._selectionLength;
            selectionLength = 0;
        }

        inline void InputBoxHelpers::eraseSelection(ControlData& controlData, StringW& outText) noexcept
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
            caretAt = min(static_cast<uint16>(caretAt - selectionLength), textLength);

            controlData._controlValue._textBoxData._selectionLength = 0;
        }

        MINT_INLINE const uint16 InputBoxHelpers::computeCaretAtIfErasedSelection(const ControlData& controlData, const StringW& outText) noexcept
        {
            uint16 caretAt = controlData._controlValue._textBoxData._caretAt;
            const uint16 selectionLength = controlData._controlValue._textBoxData._selectionLength;
            if (selectionLength > 0)
            {
                const uint16 textLength = static_cast<uint16>(outText.length() - selectionLength);
                caretAt = min(static_cast<uint16>(caretAt - selectionLength), textLength);
            }
            return caretAt;
        }

        inline void InputBoxHelpers::updateSelection(const uint16 oldCaretAt, const uint16 caretAt, ControlData& controlData) noexcept
        {
            uint16& selectionStart = controlData._controlValue._textBoxData._selectionStart;
            uint16& selectionLength = controlData._controlValue._textBoxData._selectionLength;

            if (selectionLength == 0)
            {
                // 새 Selection
                selectionStart = min(caretAt, oldCaretAt);
                selectionLength = max(caretAt, oldCaretAt) - selectionStart;
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
                        selectionStart = (isLeftWard == true) ? min(selectionStart, caretAt) : max(selectionStart, caretAt);
                        selectionLength = selectionEnd - selectionStart;
                    }
                    else
                    {
                        // from Tail
                        const uint16 selectionEnd = (isLeftWard == true) ? min(oldSelectionEnd, caretAt) : max(oldSelectionEnd, caretAt);
                        selectionStart = min(selectionStart, caretAt);
                        selectionLength = selectionEnd - selectionStart;
                    }
                }
            }
        }

        inline const wchar_t* InputBoxHelpers::getLengthErrorMessage(const uint32 maxLength) noexcept
        {
            static wchar_t message[256]{};
            formatString(message, 256, L"텍스트 길이가 %d 자를 넘을 수 없습니다!", maxLength);
            return message;
        }

        inline void InputBoxHelpers::updateTextDisplayOffset(const Rendering::ShapeFontRendererContext& rendererContext, const uint16 textLength, 
            const float backSpaceStride, ControlData& controlData, const float inputCandidateWidth) noexcept
        {
            const uint16 caretAt = controlData._controlValue._textBoxData._caretAt;
            const float textWidthTillCaret = rendererContext.computeTextWidth(controlData._text.c_str(), min(caretAt, textLength));
            float& textDisplayOffset = controlData._controlValue._textBoxData._textDisplayOffset;
            {
                const float deltaTextDisplayOffsetRight = (textWidthTillCaret + inputCandidateWidth - textDisplayOffset) - controlData._size._x;
                if (deltaTextDisplayOffsetRight > 0.0f)
                {
                    textDisplayOffset += deltaTextDisplayOffsetRight;
                }

                const float deltaTextDisplayOffsetLeft = (textWidthTillCaret + inputCandidateWidth - textDisplayOffset);
                if (deltaTextDisplayOffsetLeft < 0.0f)
                {
                    textDisplayOffset -= backSpaceStride;
                    textDisplayOffset = max(textDisplayOffset, 0.0f);
                }

                if (textWidthTillCaret + inputCandidateWidth < controlData._size._x)
                {
                    textDisplayOffset = 0.0f;
                }
            }
        }

        inline void InputBoxHelpers::drawTextWithInputCandidate(Rendering::ShapeFontRendererContext& rendererContext, const CommonControlParam& commonControlParam,
            const Float4& textRenderOffset, const bool isFocused, const float fontSize, const wchar_t inputCandiate, ControlData& controlData, StringW& outText) noexcept
        {
            MINT_ASSERT("김장원", controlData.isTypeOf(ControlType::TextBox) == true, "TextBox 가 아니면 사용하면 안 됩니다!");

            const Float2& controlLeftCenterPosition = controlData.getControlLeftCenterPosition();
            const float textDisplayOffset = controlData._controlValue._textBoxData._textDisplayOffset;
            const Float4 textRenderPosition = Float4(controlLeftCenterPosition._x - textDisplayOffset, controlLeftCenterPosition._y, 0, 0);

            // Text 렌더링 (Caret 이전)
            const uint16 caretAt = controlData._controlValue._textBoxData._caretAt;
            if (outText.empty() == false)
            {
                rendererContext.setTextColor(commonControlParam._fontColor);
                rendererContext.drawDynamicText(outText.c_str(), caretAt, textRenderPosition + textRenderOffset,
                    Rendering::FontRenderingOption(Rendering::TextRenderDirectionHorz::Rightward, Rendering::TextRenderDirectionVert::Centered));
            }

            // Input Candidate 렌더링
            const wchar_t inputCandidate[2]{ inputCandiate, L'\0' };
            const uint16 textLength = static_cast<uint16>(outText.length());
            const float textWidthTillCaret = rendererContext.computeTextWidth(outText.c_str(), min(caretAt, textLength));
            rendererContext.setTextColor(commonControlParam._fontColor);
            rendererContext.drawDynamicText(inputCandidate, Float4(controlLeftCenterPosition._x + textWidthTillCaret - textDisplayOffset, controlLeftCenterPosition._y, 0, 0) + textRenderOffset,
                Rendering::FontRenderingOption(Rendering::TextRenderDirectionHorz::Rightward, Rendering::TextRenderDirectionVert::Centered));

            // Text 렌더링 (Caret 이후)
            const float inputCandidateWidth = ((isFocused == true) && (inputCandiate >= 32)) ? rendererContext.computeTextWidth(inputCandidate, 1) : 0.0f;
            if (outText.empty() == false)
            {
                rendererContext.setTextColor(commonControlParam._fontColor);
                rendererContext.drawDynamicText(outText.c_str() + caretAt, textLength - caretAt, textRenderPosition + Float4(textWidthTillCaret + inputCandidateWidth, 0, 0, 0) + textRenderOffset,
                    Rendering::FontRenderingOption(Rendering::TextRenderDirectionHorz::Rightward, Rendering::TextRenderDirectionVert::Centered));
            }

            // Caret 렌더링 (Input Candidate 의 바로 뒤에!)
            const bool needToRenderCaret = (isFocused == true && controlData._controlValue._textBoxData._caretState == 0);
            if (needToRenderCaret == true)
            {
                const float caretHeight = fontSize;
                const Float2& p0 = Float2(controlLeftCenterPosition._x + textWidthTillCaret + inputCandidateWidth - textDisplayOffset + textRenderOffset._x, controlLeftCenterPosition._y - caretHeight * 0.5f);
                rendererContext.setColor(Rendering::Color::kBlack);
                rendererContext.drawLine(p0, p0 + Float2(0.0f, caretHeight), 2.0f);
            }
        }

        inline void InputBoxHelpers::drawTextWithoutInputCandidate(Rendering::ShapeFontRendererContext& rendererContext, const CommonControlParam& commonControlParam,
            const Float4& textRenderOffset, const bool isFocused, const float fontSize, const bool renderCaret, ControlData& controlData, StringW& outText) noexcept
        {
            MINT_ASSERT("김장원", controlData.isInputBoxType() == true, "호환되지 않는 컨트롤 타입입니다!");

            const Float2& controlLeftCenterPosition = controlData.getControlLeftCenterPosition();
            const float textDisplayOffset = controlData._controlValue._textBoxData._textDisplayOffset;
            const Float4 textRenderPosition = Float4(controlLeftCenterPosition._x - textDisplayOffset, controlLeftCenterPosition._y, 0, 0);

            // Text 전체 렌더링
            if (outText.empty() == false)
            {
                rendererContext.setTextColor(commonControlParam._fontColor);
                rendererContext.drawDynamicText(outText.c_str(), textRenderPosition + textRenderOffset,
                    Rendering::FontRenderingOption(Rendering::TextRenderDirectionHorz::Rightward, Rendering::TextRenderDirectionVert::Centered));
            }

            // Caret 렌더링
            const bool needToRenderCaret = (isFocused == true && controlData._controlValue._textBoxData._caretState == 0);
            if (renderCaret == true && needToRenderCaret == true)
            {
                const uint16 caretAt = controlData._controlValue._textBoxData._caretAt;
                const uint16 textLength = static_cast<uint16>(outText.length());
                const float textWidthTillCaret = rendererContext.computeTextWidth(outText.c_str(), min(caretAt, textLength));
                const float caretHeight = fontSize;
                const Float2& p0 = Float2(controlLeftCenterPosition._x + textWidthTillCaret - textDisplayOffset + textRenderOffset._x, controlLeftCenterPosition._y - caretHeight * 0.5f);
                rendererContext.setColor(Rendering::Color::kBlack);
                rendererContext.drawLine(p0, p0 + Float2(0.0f, caretHeight), 2.0f);
            }
        }

        inline void InputBoxHelpers::drawSelection(Rendering::ShapeFontRendererContext& rendererContext, const Float4& textRenderOffset, 
            const bool isFocused, const float fontSize, const Rendering::Color& selectionColor, ControlData& textBoxControlData, StringW& outText) noexcept
        {
            MINT_ASSERT("김장원", textBoxControlData.isInputBoxType() == true, "호환되지 않는 컨트롤 타입입니다!");

            if (isFocused == false)
            {
                return;
            }

            const uint16 selectionStart = textBoxControlData._controlValue._textBoxData._selectionStart;
            const uint16 selectionLength = textBoxControlData._controlValue._textBoxData._selectionLength;
            const uint16 selectionEnd = selectionStart + selectionLength;
            if (selectionLength > 0)
            {
                const Float2& controlLeftCenterPosition = textBoxControlData.getControlLeftCenterPosition();
                const float textDisplayOffset = textBoxControlData._controlValue._textBoxData._textDisplayOffset;
                const float textWidthTillSelectionStart = rendererContext.computeTextWidth(outText.c_str(), selectionStart);
                const float textWidthTillSelectionEnd = rendererContext.computeTextWidth(outText.c_str(), selectionEnd);
                const float textWidthSelection = textWidthTillSelectionEnd - textWidthTillSelectionStart;

                const Float4 selectionRenderPosition = Float4(controlLeftCenterPosition._x - textDisplayOffset + textWidthTillSelectionStart + textWidthSelection * 0.5f, controlLeftCenterPosition._y, 0, 0);
                rendererContext.setPosition(selectionRenderPosition + textRenderOffset);
                rendererContext.setColor(selectionColor);
                rendererContext.drawRectangle(Float2(textWidthSelection, fontSize * 1.25f), 0.0f, 0.0f);
            }
        }
    }
}

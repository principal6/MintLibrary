#pragma once


#include <stdafx.h>
#include <MintRenderingBase/Include/Gui/InputHelpers.h>

#include <MintContainer/Include/StringUtil.hpp>

#include <MintRenderingBase/Include/Gui/ControlData.hpp>

#include <MintLibrary/Include/Profiler.h>


namespace mint
{
    namespace Gui
    {
#pragma region MouseStates
        MINT_INLINE void MouseStates::resetPerFrame() noexcept
        {
            _isButtonDownUp = false;
            _isButtonDownThisFrame = false;
            _isDoubleClicked = false;
        }

        MINT_INLINE void MouseStates::setPosition(const mint::Float2& position) noexcept
        {
            _mousePosition = position;

            calculateMouseDragDelta();
        }

        MINT_INLINE void MouseStates::setButtonDownPosition(const mint::Float2& position) noexcept
        {
            _mouseDownPositionCopy = _mouseDownPosition = position;

            calculateMouseDragDelta();
        }

        MINT_INLINE void MouseStates::setButtonDownPositionCopy(const mint::Float2& position) noexcept
        {
            _mouseDownPositionCopy = position;

            calculateMouseDragDelta();
        }

        MINT_INLINE void MouseStates::setButtonUpPosition(const mint::Float2& position) noexcept
        {
            _mouseUpPosition = position;
        }

        MINT_INLINE void MouseStates::setButtonDown() noexcept
        {
            _isButtonDown = true;
            _isButtonDownThisFrame = true;
        }

        MINT_INLINE void MouseStates::setButtonUp() noexcept
        {
            if (_isButtonDown == true)
            {
                _isButtonDownUp = true;
            }
            _isButtonDown = false;
        }

        MINT_INLINE void MouseStates::setDoubleClicked() noexcept
        {
            _isDoubleClicked = true;
        }

        MINT_INLINE void MouseStates::calculateMouseDragDelta() noexcept
        {
            _mouseDragDelta = _mousePosition - _mouseDownPositionCopy;
        }

        MINT_INLINE const mint::Float2& MouseStates::getPosition() const noexcept
        {
            return _mousePosition;
        }

        MINT_INLINE const mint::Float2& MouseStates::getButtonDownPosition() const noexcept
        {
            return _mouseDownPosition;
        }

        MINT_INLINE const mint::Float2& MouseStates::getButtonUpPosition() const noexcept
        {
            return _mouseUpPosition;
        }

        MINT_INLINE const mint::Float2& MouseStates::getMouseDragDelta() const noexcept
        {
            return _mouseDragDelta;
        }

        MINT_INLINE const bool MouseStates::isButtonDown() const noexcept
        {
            return _isButtonDown;
        }

        MINT_INLINE const bool MouseStates::isButtonDownThisFrame() const noexcept
        {
            return _isButtonDownThisFrame;
        }

        MINT_INLINE const bool MouseStates::isButtonDownUp() const noexcept
        {
            return _isButtonDownUp;
        }

        MINT_INLINE const bool MouseStates::isDoubleClicked() const noexcept
        {
            return _isDoubleClicked;
        }

        MINT_INLINE const bool MouseStates::isCursor(const mint::Window::CursorType cursorType) const noexcept
        {
            return _cursorType == cursorType;
        }
#pragma endregion


        MINT_INLINE void InputBoxHelpers::updateCaretState(const uint32 caretBlinkIntervalMs, ControlData& controlData) noexcept
        {
            uint16& caretState = controlData._controlValue._textBoxData._caretState;
            uint64& lastCaretBlinkTimeMs = controlData._controlValue._textBoxData._lastCaretBlinkTimeMs;
            const uint64 currentTimeMs = mint::Profiler::getCurrentTimeMs();
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
            lastCaretBlinkTimeMs = mint::Profiler::getCurrentTimeMs();
            caretState = 0;
        }

        inline void InputBoxHelpers::processDefaultMouseInputs(const MouseStates& mouseStates, const mint::RenderingBase::ShapeFontRendererContext& rendererContext,
            ControlData& controlData, const mint::Float4& textRenderOffset, const std::wstring& outText, TextBoxProcessInputResult& result) noexcept
        {
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            const float textDisplayOffset = controlData._controlValue._textBoxData._textDisplayOffset;
            const float positionInText = mouseStates.getPosition()._x - controlData._position._x + textDisplayOffset - textRenderOffset._x;
            const uint16 textLength = static_cast<uint16>(outText.length());
            const uint32 newCaretAt = rendererContext.calculateIndexFromPositionInText(outText.c_str(), textLength, positionInText);
            if (mouseStates.isButtonDownThisFrame() == true)
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

        inline void InputBoxHelpers::processDefaultKeyboardInputs(const mint::Window::IWindow* const window, const mint::RenderingBase::ShapeFontRendererContext& rendererContext,
            ControlData& controlData, const TextInputMode textInputMode, const uint32 maxTextLength, mint::Window::EventData::KeyCode& keyCode,
            wchar_t& wcharInput, const wchar_t wcharInputCandidate, const mint::Float4& textRenderOffset, std::wstring& outText, TextBoxProcessInputResult& result) noexcept
        {
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            const bool isShiftKeyDown = window->isKeyDown(mint::Window::EventData::KeyCode::Shift);
            const bool isControlKeyDown = window->isKeyDown(mint::Window::EventData::KeyCode::Control);
            const uint16 oldCaretAt = controlData._controlValue._textBoxData._caretAt;
            if (32 <= wcharInputCandidate)
            {
                mint::Gui::InputBoxHelpers::eraseSelection(controlData, outText);
            }
            else if (wcharInput != L'\0')
            {
                // 글자 입력 or 키 입력

                const bool isInputCharacter = (32 <= wcharInput);
                if (isInputCharacter == true)
                {
                    const uint16 futureCaretAt = mint::Gui::InputBoxHelpers::calculateCaretAtIfErasedSelection(controlData, outText);
                    if (mint::Gui::InputBoxHelpers::isValidCharacterInput(wcharInput, futureCaretAt, textInputMode, outText) == true)
                    {
                        mint::Gui::InputBoxHelpers::eraseSelection(controlData, outText);

                        if (mint::Gui::InputBoxHelpers::insertWchar(wcharInput, caretAt, outText) == false)
                        {
                            window->showMessageBox(L"오류", mint::Gui::InputBoxHelpers::getLengthErrorMessage(kTextBoxMaxTextLength), mint::Window::MessageBoxType::Error);
                        }
                    }
                }
                else
                {
                    mint::Gui::InputBoxHelpers::processAsciiControlFunctions(window, wcharInput, maxTextLength, controlData, outText);
                }

                result._clearWcharInput = true;
            }
            else
            {
                mint::Gui::InputBoxHelpers::processKeyCodeCaretMovements(rendererContext, keyCode, controlData, outText);
            }

            // Caret 위치가 바뀐 경우 refresh
            if (oldCaretAt != caretAt)
            {
                mint::Gui::InputBoxHelpers::refreshCaret(controlData);

                // Selection
                if (isShiftKeyDown == true && keyCode != mint::Window::EventData::KeyCode::NONE)
                {
                    mint::Gui::InputBoxHelpers::updateSelection(oldCaretAt, caretAt, controlData);
                }
            }

            if (isShiftKeyDown == false && isControlKeyDown == false &&
                keyCode != mint::Window::EventData::KeyCode::NONE &&
                keyCode != mint::Window::EventData::KeyCode::Control &&
                keyCode != mint::Window::EventData::KeyCode::Alt)
            {
                mint::Gui::InputBoxHelpers::deselect(controlData);
            }
        }

        inline void InputBoxHelpers::eraseBefore(ControlData& controlData, std::wstring& outText) noexcept
        {
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            const uint16 selectionLength = controlData._controlValue._textBoxData._selectionLength;
            if (0 < selectionLength)
            {
                eraseSelection(controlData, outText);
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

        MINT_INLINE void InputBoxHelpers::processAsciiControlFunctions(const mint::Window::IWindow* const window, const wchar_t asciiCode, const uint32 maxTextLength, ControlData& controlData, std::wstring& outText) noexcept
        {
            const bool isControlKeyDown = window->isKeyDown(mint::Window::EventData::KeyCode::Control);
            if (asciiCode == VK_BACK) // BackSpace
            {
                mint::Gui::InputBoxHelpers::eraseBefore(controlData, outText);
            }
            else if (isControlKeyDown == true && asciiCode == 0x01) // Ctrl + A
            {
                mint::Gui::InputBoxHelpers::selectAll(controlData, outText);
            }
            else if (isControlKeyDown == true && asciiCode == 0x03) // Ctrl + C
            {
                mint::Gui::InputBoxHelpers::copySelection(window, controlData, outText);
            }
            else if (isControlKeyDown == true && asciiCode == 0x18) // Ctrl + X
            {
                mint::Gui::InputBoxHelpers::cutSelection(window, controlData, outText);
            }
            else if (isControlKeyDown == true && asciiCode == 0x16) // Ctrl + V
            {
                mint::Gui::InputBoxHelpers::paste(window, controlData, outText, getLengthErrorMessage(maxTextLength));
            }
        }

        inline void InputBoxHelpers::eraseAfter(ControlData& controlData, std::wstring& outText) noexcept
        {
            const uint16 selectionLength = controlData._controlValue._textBoxData._selectionLength;
            if (0 < selectionLength)
            {
                eraseSelection(controlData, outText);
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

        MINT_INLINE void InputBoxHelpers::selectAll(ControlData& controlData, const std::wstring& outText) noexcept
        {
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            controlData._controlValue._textBoxData._selectionStart = 0;
            caretAt = controlData._controlValue._textBoxData._selectionLength = static_cast<uint16>(outText.length());
        }

        inline void InputBoxHelpers::copySelection(const mint::Window::IWindow* const window, ControlData& controlData, const std::wstring& outText) noexcept
        {
            const uint16 selectionLength = controlData._controlValue._textBoxData._selectionLength;
            if (selectionLength == 0)
            {
                return;
            }

            const uint16 selectionStart = controlData._controlValue._textBoxData._selectionStart;
            window->textToClipboard(&outText[selectionStart], selectionLength);
        }

        MINT_INLINE void InputBoxHelpers::cutSelection(const mint::Window::IWindow* const window, ControlData& controlData, std::wstring& outText) noexcept
        {
            copySelection(window, controlData, outText);

            eraseSelection(controlData, outText);
        }

        inline void InputBoxHelpers::paste(const mint::Window::IWindow* const window, ControlData& controlData, std::wstring& outText, const wchar_t* const errorMessage) noexcept
        {
            std::wstring fromClipboard;
            window->textFromClipboard(fromClipboard);

            if (fromClipboard.empty() == true)
            {
                return;
            }

            eraseSelection(controlData, outText);

            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            if (false == insertWstring(fromClipboard, caretAt, outText))
            {
                if (errorMessage != nullptr)
                {
                    window->showMessageBox(L"오류", errorMessage, mint::Window::MessageBoxType::Error);
                }
            }
        }

        MINT_INLINE void InputBoxHelpers::processKeyCodeCaretMovements(const mint::RenderingBase::ShapeFontRendererContext& rendererContext, const mint::Window::EventData::KeyCode keyCode,
            ControlData& controlData, std::wstring& outText) noexcept
        {
            if (keyCode == mint::Window::EventData::KeyCode::Left)
            {
                mint::Gui::InputBoxHelpers::moveCaretToPrev(controlData);
            }
            else if (keyCode == mint::Window::EventData::KeyCode::Right)
            {
                mint::Gui::InputBoxHelpers::moveCaretToNext(controlData, outText);
            }
            else if (keyCode == mint::Window::EventData::KeyCode::Home)
            {
                mint::Gui::InputBoxHelpers::moveCaretToHead(controlData);
            }
            else if (keyCode == mint::Window::EventData::KeyCode::End)
            {
                mint::Gui::InputBoxHelpers::moveCaretToTail(rendererContext, controlData, outText);
            }
            else if (keyCode == mint::Window::EventData::KeyCode::Delete)
            {
                mint::Gui::InputBoxHelpers::eraseAfter(controlData, outText);
            }
        }

        MINT_INLINE void InputBoxHelpers::moveCaretToPrev(ControlData& controlData) noexcept
        {
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            caretAt = mint::max(caretAt - 1, 0);
        }

        MINT_INLINE void InputBoxHelpers::moveCaretToNext(ControlData& controlData, const std::wstring& text) noexcept
        {
            const uint16 textLength = static_cast<uint16>(text.length());
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            caretAt = mint::min(caretAt + 1, static_cast<int32>(textLength));
        }

        MINT_INLINE void InputBoxHelpers::moveCaretToHead(ControlData& controlData) noexcept
        {
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            caretAt = 0;

            float& textDisplayOffset = controlData._controlValue._textBoxData._textDisplayOffset;
            textDisplayOffset = 0.0f;
        }

        MINT_INLINE void InputBoxHelpers::moveCaretToTail(const mint::RenderingBase::ShapeFontRendererContext& rendererContext, ControlData& controlData, const std::wstring& text) noexcept
        {
            const uint16 textLength = static_cast<uint16>(text.length());
            uint16& caretAt = controlData._controlValue._textBoxData._caretAt;
            caretAt = textLength;

            float& textDisplayOffset = controlData._controlValue._textBoxData._textDisplayOffset;
            const float textWidth = rendererContext.calculateTextWidth(text.c_str(), textLength);
            textDisplayOffset = mint::max(0.0f, textWidth - controlData._displaySize._x);
        }

        inline const bool InputBoxHelpers::insertWchar(const wchar_t input, uint16& caretAt, std::wstring& outText) noexcept
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

        inline const bool InputBoxHelpers::insertWstring(const std::wstring& input, uint16& caretAt, std::wstring& outText)  noexcept
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

        inline const bool InputBoxHelpers::isValidCharacterInput(const wchar_t input, const uint16 caretAt, const TextInputMode textInputMode, const std::wstring& text) noexcept
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

        MINT_INLINE void InputBoxHelpers::deselect(ControlData& controlData) noexcept
        {
            uint16& selectionLength = controlData._controlValue._textBoxData._selectionLength;
            selectionLength = 0;
        }

        inline void InputBoxHelpers::eraseSelection(ControlData& controlData, std::wstring& outText) noexcept
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

        MINT_INLINE const uint16 InputBoxHelpers::calculateCaretAtIfErasedSelection(const ControlData& controlData, const std::wstring& outText) noexcept
        {
            uint16 caretAt = controlData._controlValue._textBoxData._caretAt;
            const uint16 selectionLength = controlData._controlValue._textBoxData._selectionLength;
            if (0 < selectionLength)
            {
                const uint16 textLength = static_cast<uint16>(outText.length() - selectionLength);
                caretAt = mint::min(static_cast<uint16>(caretAt - selectionLength), textLength);
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

        inline const wchar_t* InputBoxHelpers::getLengthErrorMessage(const uint32 maxLength) noexcept
        {
            static wchar_t message[256]{};
            mint::formatString(message, 256, L"텍스트 길이가 %d 자를 넘을 수 없습니다!", maxLength);
            return message;
        }

        inline void InputBoxHelpers::updateTextDisplayOffset(const mint::RenderingBase::ShapeFontRendererContext& rendererContext, const uint16 textLength, 
            const float backSpaceStride, ControlData& controlData, const float inputCandidateWidth) noexcept
        {
            const uint16 caretAt = controlData._controlValue._textBoxData._caretAt;
            const float textWidthTillCaret = rendererContext.calculateTextWidth(controlData._text.c_str(), mint::min(caretAt, textLength));
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
                    textDisplayOffset -= backSpaceStride;
                    textDisplayOffset = mint::max(textDisplayOffset, 0.0f);
                }

                if (textWidthTillCaret + inputCandidateWidth < controlData._displaySize._x)
                {
                    textDisplayOffset = 0.0f;
                }
            }
        }

        inline void InputBoxHelpers::drawTextWithInputCandidate(mint::RenderingBase::ShapeFontRendererContext& rendererContext, const CommonControlParam& commonControlParam,
            const mint::Float4& textRenderOffset, const bool isFocused, const float fontSize, const wchar_t inputCandiate, ControlData& controlData, std::wstring& outText) noexcept
        {
            MINT_ASSERT("김장원", controlData.isTypeOf(ControlType::TextBox) == true, "TextBox 가 아니면 사용하면 안 됩니다!");

            const mint::Float2& controlLeftCenterPosition = controlData.getControlLeftCenterPosition();
            const float textDisplayOffset = controlData._controlValue._textBoxData._textDisplayOffset;
            const mint::Float4 textRenderPosition = mint::Float4(controlLeftCenterPosition._x - textDisplayOffset, controlLeftCenterPosition._y, 0, 0);

            // Text 렌더링 (Caret 이전)
            const uint16 caretAt = controlData._controlValue._textBoxData._caretAt;
            if (outText.empty() == false)
            {
                rendererContext.setTextColor(commonControlParam._fontColor);
                rendererContext.drawDynamicText(outText.c_str(), caretAt, textRenderPosition + textRenderOffset,
                    mint::RenderingBase::FontRenderingOption(mint::RenderingBase::TextRenderDirectionHorz::Rightward, mint::RenderingBase::TextRenderDirectionVert::Centered));
            }

            // Input Candidate 렌더링
            const wchar_t inputCandidate[2]{ inputCandiate, L'\0' };
            const uint16 textLength = static_cast<uint16>(outText.length());
            const float textWidthTillCaret = rendererContext.calculateTextWidth(outText.c_str(), mint::min(caretAt, textLength));
            rendererContext.setTextColor(commonControlParam._fontColor);
            rendererContext.drawDynamicText(inputCandidate, mint::Float4(controlLeftCenterPosition._x + textWidthTillCaret - textDisplayOffset, controlLeftCenterPosition._y, 0, 0) + textRenderOffset,
                mint::RenderingBase::FontRenderingOption(mint::RenderingBase::TextRenderDirectionHorz::Rightward, mint::RenderingBase::TextRenderDirectionVert::Centered));

            // Text 렌더링 (Caret 이후)
            const float inputCandidateWidth = ((isFocused == true) && (32 <= inputCandiate)) ? rendererContext.calculateTextWidth(inputCandidate, 1) : 0.0f;
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
                const float caretHeight = fontSize;
                const mint::Float2& p0 = mint::Float2(controlLeftCenterPosition._x + textWidthTillCaret + inputCandidateWidth - textDisplayOffset + textRenderOffset._x, controlLeftCenterPosition._y - caretHeight * 0.5f);
                rendererContext.setColor(mint::RenderingBase::Color::kBlack);
                rendererContext.drawLine(p0, p0 + mint::Float2(0.0f, caretHeight), 2.0f);
            }
        }

        inline void InputBoxHelpers::drawTextWithoutInputCandidate(mint::RenderingBase::ShapeFontRendererContext& rendererContext, const CommonControlParam& commonControlParam,
            const mint::Float4& textRenderOffset, const bool isFocused, const float fontSize, const bool renderCaret, ControlData& controlData, std::wstring& outText) noexcept
        {
            MINT_ASSERT("김장원", controlData.isInputBoxType() == true, "호환되지 않는 컨트롤 타입입니다!");

            const mint::Float2& controlLeftCenterPosition = controlData.getControlLeftCenterPosition();
            const float textDisplayOffset = controlData._controlValue._textBoxData._textDisplayOffset;
            const mint::Float4 textRenderPosition = mint::Float4(controlLeftCenterPosition._x - textDisplayOffset, controlLeftCenterPosition._y, 0, 0);

            // Text 전체 렌더링
            if (outText.empty() == false)
            {
                rendererContext.setTextColor(commonControlParam._fontColor);
                rendererContext.drawDynamicText(outText.c_str(), textRenderPosition + textRenderOffset,
                    mint::RenderingBase::FontRenderingOption(mint::RenderingBase::TextRenderDirectionHorz::Rightward, mint::RenderingBase::TextRenderDirectionVert::Centered));
            }

            // Caret 렌더링
            const bool needToRenderCaret = (isFocused == true && controlData._controlValue._textBoxData._caretState == 0);
            if (renderCaret == true && needToRenderCaret == true)
            {
                const uint16 caretAt = controlData._controlValue._textBoxData._caretAt;
                const uint16 textLength = static_cast<uint16>(outText.length());
                const float textWidthTillCaret = rendererContext.calculateTextWidth(outText.c_str(), mint::min(caretAt, textLength));
                const float caretHeight = fontSize;
                const mint::Float2& p0 = mint::Float2(controlLeftCenterPosition._x + textWidthTillCaret - textDisplayOffset + textRenderOffset._x, controlLeftCenterPosition._y - caretHeight * 0.5f);
                rendererContext.setColor(mint::RenderingBase::Color::kBlack);
                rendererContext.drawLine(p0, p0 + mint::Float2(0.0f, caretHeight), 2.0f);
            }
        }

        inline void InputBoxHelpers::drawSelection(mint::RenderingBase::ShapeFontRendererContext& rendererContext, const mint::Float4& textRenderOffset, 
            const bool isFocused, const float fontSize, const mint::RenderingBase::Color& selectionColor, ControlData& textBoxControlData, std::wstring& outText) noexcept
        {
            MINT_ASSERT("김장원", textBoxControlData.isInputBoxType() == true, "호환되지 않는 컨트롤 타입입니다!");

            if (isFocused == false)
            {
                return;
            }

            const uint16 selectionStart = textBoxControlData._controlValue._textBoxData._selectionStart;
            const uint16 selectionLength = textBoxControlData._controlValue._textBoxData._selectionLength;
            const uint16 selectionEnd = selectionStart + selectionLength;
            if (0 < selectionLength)
            {
                const mint::Float2& controlLeftCenterPosition = textBoxControlData.getControlLeftCenterPosition();
                const float textDisplayOffset = textBoxControlData._controlValue._textBoxData._textDisplayOffset;
                const float textWidthTillSelectionStart = rendererContext.calculateTextWidth(outText.c_str(), selectionStart);
                const float textWidthTillSelectionEnd = rendererContext.calculateTextWidth(outText.c_str(), selectionEnd);
                const float textWidthSelection = textWidthTillSelectionEnd - textWidthTillSelectionStart;

                const mint::Float4 selectionRenderPosition = mint::Float4(controlLeftCenterPosition._x - textDisplayOffset + textWidthTillSelectionStart + textWidthSelection * 0.5f, controlLeftCenterPosition._y, 0, 0);
                rendererContext.setPosition(selectionRenderPosition + textRenderOffset);
                rendererContext.setColor(selectionColor);
                rendererContext.drawRectangle(mint::Float2(textWidthSelection, fontSize * 1.25f), 0.0f, 0.0f);
            }
        }
    }
}

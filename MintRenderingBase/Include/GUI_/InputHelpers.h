#pragma once


#ifndef MINT_GUI_INPUT_HELPERS_H
#define MINT_GUI_INPUT_HELPERS_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/String.h>

#include <MintRenderingBase/Include/GUI_/GUICommon_.h>
#include <MintPlatform/Include/IWindow.h>


namespace mint
{
    class Float4;


    namespace Rendering
    {
        class ShapeFontRendererContext;
    }


    namespace GUI
    {
        class ControlData;


        struct MouseStates
        {
            struct PerButtonStates
            {
                void                    resetPerFrame() noexcept;

                bool                    _isButtonDown = false;
                bool                    _isButtonDownThisFrame = false;
                bool                    _isButtonDownUp = false;
                bool                    _isDoubleClicked = false;
            };

        public:
            void                        resetPerFrame() noexcept;

        public:
            void                        setPosition(const Float2& position) noexcept;
            void                        setButtonDownPosition(const Float2& position) noexcept;
            void                        setButtonDownPositionCopy(const Float2& position) noexcept;
            void                        setButtonUpPosition(const Float2& position) noexcept;
            void                        setButtonDown(const Platform::MouseButton mouseButton) noexcept;
            void                        setButtonUp(const Platform::MouseButton mouseButton) noexcept;
            void                        setDoubleClicked(const Platform::MouseButton mouseButton) noexcept;

        private:
            void                        computeMouseDragDelta() noexcept;

        public:
            const Float2&               getPosition() const noexcept;
            const Float2&               getButtonDownPosition() const noexcept;
            const Float2&               getButtonUpPosition() const noexcept;
            const Float2&               getMouseDragDelta() const noexcept;
            const bool                  isButtonDown(const Platform::MouseButton mouseButton) const noexcept;
            const bool                  isButtonDownThisFrame(const Platform::MouseButton mouseButton) const noexcept;
            const bool                  isButtonDownUp(const Platform::MouseButton mouseButton) const noexcept;
            const bool                  isDoubleClicked(const Platform::MouseButton mouseButton) const noexcept;
            const bool                  isCursor(const Window::CursorType cursorType) const noexcept;

        private:
            Float2                      _mousePosition;
            Float2                      _mouseDownPosition;
            Float2                      _mouseDownPositionCopy;
            Float2                      _mouseUpPosition;
            Float2                      _mouseDragDelta;
            PerButtonStates             _perButtonStates[Platform::getMouseButtonCount()];

        public:
            mutable float               _mouseWheel;
            mutable Window::CursorType  _cursorType = Window::CursorType::Arrow; // per frame
        };


        struct TextBoxProcessInputResult
        {
            bool    _clearWcharInput = false;
            bool    _clearKeyCode = false;
        };


        class InputBoxHelpers abstract
        {
#pragma region Caret state
        public:
            static void             updateCaretState(const uint32 caretBlinkIntervalMs, ControlData& controlData) noexcept;
            static void             refreshCaret(ControlData& controlData) noexcept;
#pragma endregion

#pragma region Mouse
        public:
            static void             processDefaultMouseInputs(const MouseStates& mouseStates, const Rendering::ShapeFontRendererContext& rendererContext, 
                ControlData& controlData, const Float4& textRenderOffset, const StringW& outText, TextBoxProcessInputResult& result) noexcept;
#pragma endregion
        
#pragma region Keyboard
        public:
            static void             processDefaultKeyboardInputs(const Window::IWindow* const window, const Rendering::ShapeFontRendererContext& rendererContext, 
                ControlData& controlData, const TextInputMode textInputMode, const uint32 maxTextLength, Platform::KeyCode& keyCode,
                wchar_t& wcharInput, const wchar_t wcharInputCandidate, const Float4& textRenderOffset, StringW& outText, TextBoxProcessInputResult& result) noexcept;
#pragma endregion

#pragma region Keyboard - Control functions
        public:
            static void             processAsciiControlFunctions(const Window::IWindow* const window, const wchar_t asciiCode, const uint32 maxTextLength, ControlData& controlData, StringW& outText) noexcept;

        public:
            static void             eraseAfter(ControlData& controlData, StringW& outText) noexcept;
            static void             eraseBefore(ControlData& controlData, StringW& outText) noexcept;
            static void             selectAll(ControlData& controlData, const StringW& outText) noexcept;
            static void             copySelection(const Window::IWindow* const window, ControlData& controlData, const StringW& outText) noexcept;
            static void             cutSelection(const Window::IWindow* const window, ControlData& controlData, StringW& outText) noexcept;
            static void             paste(const Window::IWindow* const window, ControlData& controlData, StringW& outText, const wchar_t* const errorMessage = nullptr) noexcept;
#pragma endregion

#pragma region Keyboard - Caret movements
        public:
            static void             processKeyCodeCaretMovements(const Rendering::ShapeFontRendererContext& rendererContext, const Platform::KeyCode keyCode,
                ControlData& controlData, StringW& outText) noexcept;
        
        public:
            static void             moveCaretToPrev(ControlData& controlData) noexcept;
            static void             moveCaretToNext(ControlData& controlData, const StringW& text) noexcept;
            static void             moveCaretToHead(ControlData& controlData) noexcept;
            static void             moveCaretToTail(const Rendering::ShapeFontRendererContext& rendererContext, ControlData& controlData, const StringW& text) noexcept;
#pragma endregion

#pragma region Keyboard - Character input
        public:
            static const bool       insertWchar(const wchar_t input, uint16& caretAt, StringW& outText) noexcept;
            static const bool       insertWstring(const StringW& input, uint16& caretAt, StringW& outText) noexcept;
            static const bool       isValidCharacterInput(const wchar_t input, const uint16 caretAt, const TextInputMode textInputMode, const StringW& text) noexcept;
#pragma endregion

#pragma region Selection
        public:
            static void             deselect(ControlData& controlData) noexcept;
            static void             eraseSelection(ControlData& controlData, StringW& outText) noexcept;
            static const uint16     computeCaretAtIfErasedSelection(const ControlData& controlData, const StringW& outText) noexcept;
            static void             updateSelection(const uint16 oldCaretAt, const uint16 caretAt, ControlData& controlData) noexcept;
#pragma endregion

        public:
            static const wchar_t*   getLengthErrorMessage(const uint32 maxLength) noexcept;

        public:
            static void             updateTextDisplayOffset(const Rendering::ShapeFontRendererContext& rendererContext, const uint16 textLength, 
                const float backSpaceStride, ControlData& controlData, const float inputCandidateWidth = 0.0f) noexcept;
            static void             drawTextWithInputCandidate(Rendering::ShapeFontRendererContext& rendererContext, const CommonControlParam& commonControlParam,
                const Float4& textRenderOffset, const bool isFocused, const float fontSize, const wchar_t inputCandiate, ControlData& controlData, StringW& outText) noexcept;
            static void             drawTextWithoutInputCandidate(Rendering::ShapeFontRendererContext& rendererContext, const CommonControlParam& commonControlParam, 
                const Float4& textRenderOffset, const bool isFocused, const float fontSize, const bool renderCaret, ControlData& controlData, StringW& outText) noexcept;
            static void             drawSelection(Rendering::ShapeFontRendererContext& rendererContext, const Float4& textRenderOffset, 
                const bool isFocused, const float fontSize, const Rendering::Color& selectionColor, ControlData& textBoxControlData, StringW& outText) noexcept;
        };
    }
}


#endif // !MINT_GUI_INPUT_HELPERS_H

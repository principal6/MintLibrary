#pragma once


#ifndef MINT_GUI_INPUT_HELPERS_H
#define MINT_GUI_INPUT_HELPERS_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <string>

#include <MintRenderingBase/Include/Gui/GuiCommon.h>
#include <MintPlatform/Include/IWindow.h>


namespace mint
{
    class Float4;


    namespace Gui
    {
        class ControlData;


        struct MouseStates
        {
            struct PerButtonStates
            {
                void                            resetPerFrame() noexcept;

                bool                            _isButtonDown = false;
                bool                            _isButtonDownThisFrame = false;
                bool                            _isButtonDownUp = false;
                bool                            _isDoubleClicked = false;
            };

        public:
            void                                resetPerFrame() noexcept;

        public:
            void                                setPosition(const mint::Float2& position) noexcept;
            void                                setButtonDownPosition(const mint::Float2& position) noexcept;
            void                                setButtonDownPositionCopy(const mint::Float2& position) noexcept;
            void                                setButtonUpPosition(const mint::Float2& position) noexcept;
            void                                setButtonDown(const mint::Platform::MouseButton mouseButton) noexcept;
            void                                setButtonUp(const mint::Platform::MouseButton mouseButton) noexcept;
            void                                setDoubleClicked(const mint::Platform::MouseButton mouseButton) noexcept;

        private:
            void                                calculateMouseDragDelta() noexcept;

        public:
            const mint::Float2&                 getPosition() const noexcept;
            const mint::Float2&                 getButtonDownPosition() const noexcept;
            const mint::Float2&                 getButtonUpPosition() const noexcept;
            const mint::Float2&                 getMouseDragDelta() const noexcept;
            const bool                          isButtonDown(const mint::Platform::MouseButton mouseButton) const noexcept;
            const bool                          isButtonDownThisFrame(const mint::Platform::MouseButton mouseButton) const noexcept;
            const bool                          isButtonDownUp(const mint::Platform::MouseButton mouseButton) const noexcept;
            const bool                          isDoubleClicked(const mint::Platform::MouseButton mouseButton) const noexcept;
            const bool                          isCursor(const mint::Window::CursorType cursorType) const noexcept;

        private:
            mint::Float2                        _mousePosition;
            mint::Float2                        _mouseDownPosition;
            mint::Float2                        _mouseDownPositionCopy;
            mint::Float2                        _mouseUpPosition;
            mint::Float2                        _mouseDragDelta;
            PerButtonStates                     _perButtonStates[mint::Platform::getMouseButtonCount()];

        public:
            mutable float                       _mouseWheel;
            mutable mint::Window::CursorType    _cursorType = mint::Window::CursorType::Arrow; // per frame
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
            static void             processDefaultMouseInputs(const MouseStates& mouseStates, const mint::RenderingBase::ShapeFontRendererContext& rendererContext, 
                ControlData& controlData, const mint::Float4& textRenderOffset, const std::wstring& outText, TextBoxProcessInputResult& result) noexcept;
#pragma endregion
        
#pragma region Keyboard
        public:
            static void             processDefaultKeyboardInputs(const mint::Window::IWindow* const window, const mint::RenderingBase::ShapeFontRendererContext& rendererContext, 
                ControlData& controlData, const TextInputMode textInputMode, const uint32 maxTextLength, mint::Platform::KeyCode& keyCode,
                wchar_t& wcharInput, const wchar_t wcharInputCandidate, const mint::Float4& textRenderOffset, std::wstring& outText, TextBoxProcessInputResult& result) noexcept;
#pragma endregion

#pragma region Keyboard - Control functions
        public:
            static void             processAsciiControlFunctions(const mint::Window::IWindow* const window, const wchar_t asciiCode, const uint32 maxTextLength, ControlData& controlData, std::wstring& outText) noexcept;

        public:
            static void             eraseAfter(ControlData& controlData, std::wstring& outText) noexcept;
            static void             eraseBefore(ControlData& controlData, std::wstring& outText) noexcept;
            static void             selectAll(ControlData& controlData, const std::wstring& outText) noexcept;
            static void             copySelection(const mint::Window::IWindow* const window, ControlData& controlData, const std::wstring& outText) noexcept;
            static void             cutSelection(const mint::Window::IWindow* const window, ControlData& controlData, std::wstring& outText) noexcept;
            static void             paste(const mint::Window::IWindow* const window, ControlData& controlData, std::wstring& outText, const wchar_t* const errorMessage = nullptr) noexcept;
#pragma endregion

#pragma region Keyboard - Caret movements
        public:
            static void             processKeyCodeCaretMovements(const mint::RenderingBase::ShapeFontRendererContext& rendererContext, const mint::Platform::KeyCode keyCode,
                ControlData& controlData, std::wstring& outText) noexcept;
        
        public:
            static void             moveCaretToPrev(ControlData& controlData) noexcept;
            static void             moveCaretToNext(ControlData& controlData, const std::wstring& text) noexcept;
            static void             moveCaretToHead(ControlData& controlData) noexcept;
            static void             moveCaretToTail(const mint::RenderingBase::ShapeFontRendererContext& rendererContext, ControlData& controlData, const std::wstring& text) noexcept;
#pragma endregion

#pragma region Keyboard - Character input
        public:
            static const bool       insertWchar(const wchar_t input, uint16& caretAt, std::wstring& outText) noexcept;
            static const bool       insertWstring(const std::wstring& input, uint16& caretAt, std::wstring& outText) noexcept;
            static const bool       isValidCharacterInput(const wchar_t input, const uint16 caretAt, const TextInputMode textInputMode, const std::wstring& text) noexcept;
#pragma endregion

#pragma region Selection
        public:
            static void             deselect(ControlData& controlData) noexcept;
            static void             eraseSelection(ControlData& controlData, std::wstring& outText) noexcept;
            static const uint16     calculateCaretAtIfErasedSelection(const ControlData& controlData, const std::wstring& outText) noexcept;
            static void             updateSelection(const uint16 oldCaretAt, const uint16 caretAt, ControlData& controlData) noexcept;
#pragma endregion

        public:
            static const wchar_t*   getLengthErrorMessage(const uint32 maxLength) noexcept;

        public:
            static void             updateTextDisplayOffset(const mint::RenderingBase::ShapeFontRendererContext& rendererContext, const uint16 textLength, 
                const float backSpaceStride, ControlData& controlData, const float inputCandidateWidth = 0.0f) noexcept;
            static void             drawTextWithInputCandidate(mint::RenderingBase::ShapeFontRendererContext& rendererContext, const CommonControlParam& commonControlParam,
                const mint::Float4& textRenderOffset, const bool isFocused, const float fontSize, const wchar_t inputCandiate, ControlData& controlData, std::wstring& outText) noexcept;
            static void             drawTextWithoutInputCandidate(mint::RenderingBase::ShapeFontRendererContext& rendererContext, const CommonControlParam& commonControlParam, 
                const mint::Float4& textRenderOffset, const bool isFocused, const float fontSize, const bool renderCaret, ControlData& controlData, std::wstring& outText) noexcept;
            static void             drawSelection(mint::RenderingBase::ShapeFontRendererContext& rendererContext, const mint::Float4& textRenderOffset, 
                const bool isFocused, const float fontSize, const mint::RenderingBase::Color& selectionColor, ControlData& textBoxControlData, std::wstring& outText) noexcept;
        };
    }
}


#endif // !MINT_GUI_INPUT_HELPERS_H

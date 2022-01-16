#pragma once


#ifndef MINT_GUI_CONTROL_VALUE_H
#define MINT_GUI_CONTROL_VALUE_H


#include <MintRenderingBase/Include/GUI/GUICommon.h>


namespace mint
{
    namespace GUI
    {
        class ControlValue
        {
        public:
            struct CommonData
            {
                MenuBarType     _menuBarType;
                void            enableScrollBar(const ScrollBarType scrollBarType) noexcept;
                void            disableScrollBar(const ScrollBarType scrollBarType) noexcept;
                const bool      isScrollBarEnabled(const ScrollBarType scrollBarType) const noexcept;

            private:
                ScrollBarType   _scrollBarType;
            };

            struct TextBoxData
            {
                uint16      _caretAt = 0;
                uint16      _caretState = 0;
                uint16      _selectionStart = 0;
                uint16      _selectionLength = 0;
                float       _textDisplayOffset = 0.0f;
                uint64      _lastCaretBlinkTimeMs = 0;
            };

            struct WindowData
            {
                float       _titleBarThickness = 0.0f;
            };

            struct ThumbData
            {
                float       _thumbAt = 0.0f;
            };

            struct ItemData
            {
                Float2                  _itemSize;
                MINT_INLINE void        select(const int16 itemIndex) noexcept { _selectedItemIndex = itemIndex; }
                MINT_INLINE void        deselect() noexcept { _selectedItemIndex = -1; }
                MINT_INLINE const bool  isSelected(const int16 itemIndex) const noexcept { return (_selectedItemIndex == itemIndex); }
                MINT_INLINE int16       getSelectedItemIndex() const noexcept { return _selectedItemIndex; }

            private:
                int16                   _selectedItemIndex = 0;
            };

            struct BooleanData
            {
                MINT_INLINE void        toggle() noexcept { _value = !_value; }
                MINT_INLINE void        set(const bool value) noexcept { _value = value; }
                MINT_INLINE bool& get() noexcept { return _value; }

            private:
                bool                    _value = false;
            };

        public:
                                    ControlValue();
                                    ~ControlValue() = default;

        public:
            ControlValue&           operator=(const ControlValue& rhs) = default;
            ControlValue&           operator=(ControlValue&& rhs) noexcept = default;

        public:
            CommonData              _commonData;
            union
            {
                TextBoxData         _textBoxData;
                WindowData          _windowData;
                ThumbData           _thumbData;
                struct // 아래 두 개는 동시에 쓰이는 경우가 있기 때문에 이렇게 struct 로 묶어둬야 메모리 침범을 하지 않는다.
                {
                    ItemData        _itemData;
                    BooleanData     _booleanData;
                };
            };
        };
    }
}


#include <MintRenderingBase/Include/GUI/ControlValue.inl>


#endif // !MINT_GUI_CONTROL_VALUE_H

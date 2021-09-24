#pragma once


#ifndef MINT_GUI_COMMON_H
#define MINT_GUI_COMMON_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintMath/Include/Float4.h>


namespace mint
{
    namespace Gui
    {
        enum class ControlType : uint16
        {
            ROOT,

            Button,
            TitleBar, // PRIVATE
            RoundButton, // PRIVATE
            Window,
            TooltipWindow,
            Label,
            ScrollBar,
            ScrollBarThumb, // PRIVATE
            Slider,
            SliderThumb, // PRIVATE
            CheckBox,
            TextBox,
            ValueSlider,
            ListView,
            ListItem,
            MenuBar,
            MenuBarItem,
            MenuItem,

            COUNT
        };
        static constexpr const wchar_t* const kControlTypeString[] =
        {
            L"Root",
            L"Button",
            L"TitleBar",
            L"Round Button",
            L"Window",
            L"Tooltip Window",
            L"Label",
            L"ScrollBar",
            L"ScrollBar Thumb",
            L"Slider",
            L"Slider Thumb",
            L"CheckBox",
            L"TextBox",
            L"Value Slider",
            L"ListView",
            L"List Item",
            L"MenuBar",
            L"MenuBar Item",
            L"Menu Item",
        };
        static_assert(static_cast<uint32>(ControlType::COUNT) == ARRAYSIZE(kControlTypeString), "ControlType String Array Size Is Wrong!!!");
        static constexpr const wchar_t* getControlTypeWideString(const ControlType controlType) noexcept
        {
            return kControlTypeString[static_cast<uint32>(controlType)];
        }

        enum class VisibleState
        {
            Visible,
            VisibleOpen = Visible,

            VisibleFolded,

            Invisible
        };

        enum class ResizingMethod
        {
            ResizeOnly,
            RepositionHorz,
            RepositionVert,
            RepositionBoth,
        };

        enum class ScrollBarType : int16
        {
            None,
            Vert,
            Horz,
            Both
        };

        enum class MenuBarType : int16
        {
            None,
            Top,
            Bottom,
            Left,
            Right,
        };

        enum class DockingControlType
        {
            None,
            Dock,
            Docker,
            DockerDock
        };

        enum class DockingMethod
        {
            LeftSide,
            RightSide,
            TopSide,
            BottomSide,

            COUNT
        };

        enum class RendererContextLayer
        {
            Background,
            BackgroundTop,
            Foreground,
            ForegroundTop,
            TopMost,

            COUNT,
        };
        MINT_INLINE constexpr int32 getRendererContextLayerCount() noexcept
        {
            return static_cast<int32>(RendererContextLayer::COUNT);
        }

        enum class ClipRectUsage
        {
            ParentsOwn,
            ParentsChild,
            ParentsDock,
            Own,
        };

        enum class TextInputMode
        {
            General,
            NumberOnly,
        };

        enum class TextAlignmentHorz
        {
            Left,
            Center,
            Right
        };

        enum class TextAlignmentVert
        {
            Top,
            Middle,
            Bottom
        };

        struct CommonControlParam
        {
            CommonControlParam() : CommonControlParam(Float2::kNegativeOne) { __noop; }
            CommonControlParam(const Float2& size) : CommonControlParam(size, Rendering::Color::kWhite, Rendering::Color::kBlack) { __noop; }
            CommonControlParam(const Float2& size, const Rendering::Color& backgroundColor, const Rendering::Color& fontColor)
                : _size{ size }, _offset{ Float2::kNegativeOne }, _backgroundColor{ backgroundColor }, _fontColor{ fontColor } { __noop; }

            Float2              _size;
            Float2              _offset;
            Rendering::Color    _backgroundColor;
            Rendering::Color    _fontColor;
        };
    }
}


#endif // !MINT_GUI_COMMON_H

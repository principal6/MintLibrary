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
            ValueSliderFloat,
            ListView,
            ListItem,
            MenuBar,
            MenuBarItem,
            MenuItem,

            COUNT
        };

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
            CommonControlParam() : CommonControlParam(mint::Float2::kNegativeOne) { __noop; }
            CommonControlParam(const mint::Float2& size) : CommonControlParam(size, mint::Rendering::Color::kWhite, mint::Rendering::Color::kBlack) { __noop; }
            CommonControlParam(const mint::Float2& size, const mint::Rendering::Color& backgroundColor, const mint::Rendering::Color& fontColor)
                : _size{ size }, _offset{ mint::Float2::kNegativeOne }, _backgroundColor{ backgroundColor }, _fontColor{ fontColor } { __noop; }

            mint::Float2            _size;
            mint::Float2            _offset;
            mint::Rendering::Color  _backgroundColor;
            mint::Rendering::Color  _fontColor;
        };
    }
}


#endif // !MINT_GUI_COMMON_H

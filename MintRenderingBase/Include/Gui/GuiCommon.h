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
    }
}


#endif // !MINT_GUI_COMMON_H

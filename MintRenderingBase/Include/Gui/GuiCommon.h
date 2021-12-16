#pragma once


#ifndef MINT_GUI_COMMON_H
#define MINT_GUI_COMMON_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintMath/Include/Float4.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>


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
            CommonControlParam() : CommonControlParam(Rendering::Color::kWhite, Rendering::Color::kBlack) { __noop; }
            CommonControlParam(const Rendering::Color& backgroundColor, const Rendering::Color& fontColor)
                : _offset{ Float2::kNegativeOne }, _backgroundColor{ backgroundColor }, _fontColor{ fontColor } { __noop; }

            Float2              _offset;
            Rendering::Color    _backgroundColor;
            Rendering::Color    _fontColor;
        };


        static constexpr float          kDefaultIntervalX = 5.0f;
        static constexpr float          kDefaultIntervalY = 5.0f;
        static constexpr float          kDefaultRoundnessInPixel = 8.0f;
        static constexpr float          kDefaultFocusedAlpha = 0.875f;
        static constexpr float          kDefaultOutOfFocusAlpha = 0.5f;
        static constexpr float          kDefaultRoundButtonRadius = 7.0f;
        static constexpr float          kFontScaleA = 1.0f;
        static constexpr float          kFontScaleB = 0.875f;
        static constexpr float          kFontScaleC = 0.8125f;
        static constexpr Rect           kWindowInnerPadding = Rect(4.0f);
        static constexpr float          kScrollBarThickness = 8.0f;
        static constexpr Rect           kTitleBarInnerPadding = Rect(12.0f, 6.0f, 6.0f, 6.0f);
        static constexpr float          kTitleBarBaseThickness = Rendering::kDefaultFontSize + kTitleBarInnerPadding.vert();
        static constexpr Float2         kMenuBarBaseSize = Float2(0.0f, Rendering::kDefaultFontSize + 8.0f);
        static constexpr float          kMenuBarItemTextSpace = 24.0f;
        static constexpr float          kMenuItemSpaceLeft = 16.0f;
        static constexpr float          kMenuItemSpaceRight = 48.0f;
        static constexpr float          kHalfBorderThickness = 5.0f;
        static constexpr float          kSliderTrackThicknes = 6.0f;
        static constexpr float          kSliderThumbRadius = 8.0f;
        static constexpr float          kDockingInteractionShort = 30.0f;
        static constexpr float          kDockingInteractionLong = 40.0f;
        static constexpr float          kDockingInteractionDisplayBorderThickness = 2.0f;
        static constexpr float          kDockingInteractionOffset = 5.0f;
        static constexpr Float2         kCheckBoxSize = Float2(16.0f, 16.0f);
        static constexpr float          kMouseWheelScrollScale = -8.0f;
        static constexpr float          kTextBoxBackSpaceStride = 48.0f;
        static constexpr uint32         kTextBoxMaxTextLength = 2048;
    }
}


#endif // !MINT_GUI_COMMON_H

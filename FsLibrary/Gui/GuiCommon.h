#pragma once


#ifndef FS_GUI_COMMON_H
#define FS_GUI_COMMON_H


#include <CommonDefinitions.h>

#include <FsMath/Include/Float4.h>


namespace fs
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

			COUNT
		};

		enum class ControlState
		{
			Visible,

			VisibleOpen,
			VisibleClosed,

			Invisible
		};

		enum class ResizingMethod
		{
			ResizeOnly,
			RepositionHorz,
			RepositionVert,
			RepositionBoth,
		};

		enum class ScrollBarType
		{
			None,
			Vert,
			Horz,
			Both
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

			COUNT
		};


		class Rect
		{
		public:
			constexpr						Rect();
			constexpr						Rect(const float uniformPadding);
			constexpr						Rect(const float left, const float right, const float top, const float bottom);
			constexpr						Rect(const fs::Float2& positionFromLeftTop, const fs::Float2& size);
			constexpr						Rect(const Rect& rhs) = default;
			constexpr						Rect(Rect&& rhs) noexcept = default;

		public:
			Rect&							operator=(const Rect& rhs) = default;
			Rect&							operator=(Rect&& rhs) noexcept = default;
		
		public:
			const bool						operator==(const Rect& rhs) const noexcept;
			const bool						operator!=(const Rect& rhs) const noexcept;

		public:
			constexpr const float			left() const noexcept;
			constexpr const float			right() const noexcept;
			constexpr const float			top() const noexcept;
			constexpr const float			bottom() const noexcept;

			constexpr void					left(const float s) noexcept;
			constexpr void					right(const float s) noexcept;
			constexpr void					top(const float s) noexcept;
			constexpr void					bottom(const float s) noexcept;

			constexpr const fs::Float2		center() const noexcept;
			constexpr const fs::Float2		size() const noexcept;
			constexpr const fs::Float2		position() const noexcept;
			constexpr void					position(const fs::Float2& position) noexcept;

		public:
			const bool						isNan() const noexcept;
			void							setNan() noexcept;

		private:
			fs::Float4						_raw;
		};


		enum class TextAlignmentHorz
		{
			Left,
			Middle,
			Right
		};

		enum class TextAlignmentVert
		{
			Top,
			Center,
			Bottom
		};
	}
}


#include <FsLibrary/Gui/GuiCommon.inl>


#endif // !FS_GUI_COMMON_H

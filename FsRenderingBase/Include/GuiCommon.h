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
			CheckBox,

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
			TopSide,
			BottomSide,

			COUNT
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


#endif // !FS_GUI_COMMON_H

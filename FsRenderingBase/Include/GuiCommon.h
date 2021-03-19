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
			TextBox,
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


#endif // !FS_GUI_COMMON_H

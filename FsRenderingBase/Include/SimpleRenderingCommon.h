#pragma once


#ifndef FS_SIMPLE_RENDERING_COMMON_H
#define FS_SIMPLE_RENDERING_COMMON_H


namespace fs
{
	namespace SimpleRendering
	{
		enum class TextRenderDirectionHorz
		{
			Leftward,
			Centered,
			Rightward
		};

		enum class TextRenderDirectionVert
		{
			Upward,
			Centered,
			Downward
		};

		static constexpr const char* const		kDefaultFont = "Assets/noto_sans_kr_medium";
		static constexpr int32					kDefaultFontSize = 18;
	}
}


#endif // !FS_SIMPLE_RENDERING_COMMON_H

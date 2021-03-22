#pragma once


#ifndef FS_SIMPLE_RENDERING_COMMON_H
#define FS_SIMPLE_RENDERING_COMMON_H


namespace fs
{
	namespace RenderingBase
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

		static constexpr const char* const			kDefaultFont = "FsLibrary/Assets/noto_sans_kr_medium";
		static constexpr int32						kDefaultFontSize = 18;

		using IndexElementType						= uint16;

		// Triangle face
		struct Face
		{
			static constexpr uint8					kIndexCountPerFace = 3;

			fs::RenderingBase::IndexElementType		_indexArray[kIndexCountPerFace];
		};
	}
}


#endif // !FS_SIMPLE_RENDERING_COMMON_H

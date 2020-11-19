#pragma once


#ifndef FS_I_RENDERER_H
#define FS_I_RENDERER_H


#include <Math/Float2.h>
#include <Math/Float3.h>
#include <Math/Float4.h>


namespace fs
{
	class GraphicDevice;


	class IRenderer abstract
	{
	public:
									IRenderer() = default;
									IRenderer(fs::GraphicDevice* const graphicDevice);
		virtual						~IRenderer() = default;

	public:
		FS_INLINE void				setPosition(const fs::Float3& position) { _position = position; }

		void						setColor(const fs::Float4& color);
		void						setColor(const std::vector<fs::Float4>& colorArray);

	public:
		virtual void				drawColored() abstract;
		virtual void				drawTextured(const fs::Float2& texturePosition, const fs::Float2& textureSize) abstract;
		virtual void				drawColoredTextured(const fs::Float2& texturePosition, const fs::Float2& textureSize) abstract;

	protected:
		FS_INLINE const fs::Float4&	getColorInternal(const uint32 index)
		{
			const uint32 colorCount = static_cast<uint32>(_colorArray.size());
			return (colorCount <= index) ? _defaultColor : _colorArray[index];
		}

	protected:
		fs::GraphicDevice*			_graphicDevice;
		fs::Float3					_position;
		std::vector<fs::Float4>		_colorArray;
		fs::Float4					_defaultColor;
	};
}


#endif // !FS_I_RENDERER_H

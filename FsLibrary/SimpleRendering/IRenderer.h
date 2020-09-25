#pragma once


#ifndef FS_IDRAW_H
#define FS_IDRAW_H


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
									IRenderer(fs::GraphicDevice* const graphicDevice) : _graphicDevice{ graphicDevice }
									{
										FS_ASSERT("김장원", nullptr != _graphicDevice, "GraphicDevice 가 nullptr 이면 안 됩니다!");
									}
		virtual						~IRenderer() = default;

	public:
		// Current coordinate system
		// (0.0f, 0.0f, z) = Top Left
		// (1.0f, 1.0f, z) = Bottom Right
		// z value is depth
		FS_INLINE void				setPosition(const fs::Float3& position) { _position = position; }

	public:
		virtual void				drawColored(const fs::Float4& color) abstract;
		virtual void				drawTextured(const fs::Float2& texturePosition, const fs::Float2& textureSize) abstract;
		virtual void				drawColoredTextured(const fs::Float4& color, const fs::Float2& texturePosition, const fs::Float2& textureSize) abstract;

	protected:
		FS_INLINE const fs::Float3	normalizePosition(const fs::Float3& position) 
		{
			return fs::Float3(position.x() * 2.0f - 1.0f, position.y() * -2.0f + 1.0f, position.z()); 
		}

		FS_INLINE const fs::Float2	normalizeSize(const fs::Float2& size)
		{
			return fs::Float2(size.x() * 2.0f, size.y() * -2.0f);
		}

	protected:
		fs::GraphicDevice*	_graphicDevice;
		fs::Float3			_position;
	};
}


#endif // !FS_IDRAW_H

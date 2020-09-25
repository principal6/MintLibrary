#include <stdafx.h>
#include <SimpleRendering/RectangleRenderer.h>

#include <SimpleRendering/GraphicDevice.h>
#include <Math/Float2.hpp>
#include <Math/Float3.hpp>
#include <Math/Float4.hpp>


namespace fs
{
	FS_INLINE fs::Float3 getFinalPosition(const uint32 index, const fs::Float3& normalizedPosition, const fs::Float2& normalizedSize)
	{
		return fs::Float3(normalizedPosition.x() + (index & 1) * normalizedSize.x(), normalizedPosition.y() + ((index & 2) >> 1) * normalizedSize.y(), normalizedPosition.z());
	}

	FS_INLINE fs::Float2 getFinalTexturePosition(const uint32 index, const fs::Float2& normalizedPosition, const fs::Float2& normalizedSize)
	{
		return fs::Float2(normalizedPosition.x() + (index & 1) * normalizedSize.x(), normalizedPosition.y() + ((index & 2) >> 1) * normalizedSize.y());
	}

	FS_INLINE const fs::Float4& getColor(const uint32 index)
	{

	}

	void RectangleRenderer::drawColored()
	{
		const fs::Float3 normalizedPosition	= normalizePosition(_position);
		const fs::Float2 normalizedSize		= normalizeSize(_size);

		_graphicDevice->_triangleVertexArray.emplace_back(fs::VertexData(getFinalPosition(0, normalizedPosition, normalizedSize), getColorInternal(0)));
		_graphicDevice->_triangleVertexArray.emplace_back(fs::VertexData(getFinalPosition(1, normalizedPosition, normalizedSize), getColorInternal(1)));
		_graphicDevice->_triangleVertexArray.emplace_back(fs::VertexData(getFinalPosition(2, normalizedPosition, normalizedSize), getColorInternal(2)));
		_graphicDevice->_triangleVertexArray.emplace_back(fs::VertexData(getFinalPosition(3, normalizedPosition, normalizedSize), getColorInternal(3)));

		prepareIndexArray();
	}

	void RectangleRenderer::drawTextured(const fs::Float2& texturePosition, const fs::Float2& textureSize)
	{
		const fs::Float3 normalizedPosition	= normalizePosition(_position);
		const fs::Float2 normalizedSize		= normalizeSize(_size);
		
		_graphicDevice->_triangleVertexArray.emplace_back(fs::VertexData(getFinalPosition(0, normalizedPosition, normalizedSize), getFinalTexturePosition(0, texturePosition, textureSize)));
		_graphicDevice->_triangleVertexArray.emplace_back(fs::VertexData(getFinalPosition(1, normalizedPosition, normalizedSize), getFinalTexturePosition(1, texturePosition, textureSize)));
		_graphicDevice->_triangleVertexArray.emplace_back(fs::VertexData(getFinalPosition(2, normalizedPosition, normalizedSize), getFinalTexturePosition(2, texturePosition, textureSize)));
		_graphicDevice->_triangleVertexArray.emplace_back(fs::VertexData(getFinalPosition(3, normalizedPosition, normalizedSize), getFinalTexturePosition(3, texturePosition, textureSize)));

		prepareIndexArray();
	}

	void RectangleRenderer::drawColoredTextured(const fs::Float2& texturePosition, const fs::Float2& textureSize)
	{
		const fs::Float3 normalizedPosition = normalizePosition(_position);
		const fs::Float2 normalizedSize = normalizeSize(_size);

		_graphicDevice->_triangleVertexArray.emplace_back(fs::VertexData(getFinalPosition(0, normalizedPosition, normalizedSize), getColorInternal(0), getFinalTexturePosition(0, texturePosition, textureSize)));
		_graphicDevice->_triangleVertexArray.emplace_back(fs::VertexData(getFinalPosition(1, normalizedPosition, normalizedSize), getColorInternal(1), getFinalTexturePosition(1, texturePosition, textureSize)));
		_graphicDevice->_triangleVertexArray.emplace_back(fs::VertexData(getFinalPosition(2, normalizedPosition, normalizedSize), getColorInternal(2), getFinalTexturePosition(2, texturePosition, textureSize)));
		_graphicDevice->_triangleVertexArray.emplace_back(fs::VertexData(getFinalPosition(3, normalizedPosition, normalizedSize), getColorInternal(3), getFinalTexturePosition(3, texturePosition, textureSize)));

		prepareIndexArray();
	}

	void RectangleRenderer::prepareIndexArray()
	{
		const uint32 currentTotalTriangleVertexCount = static_cast<uint32>(_graphicDevice->_triangleVertexArray.size());

		_graphicDevice->_triangleIndexArray.push_back((currentTotalTriangleVertexCount - 4) + 0);
		_graphicDevice->_triangleIndexArray.push_back((currentTotalTriangleVertexCount - 4) + 1);
		_graphicDevice->_triangleIndexArray.push_back((currentTotalTriangleVertexCount - 4) + 2);
		_graphicDevice->_triangleIndexArray.push_back((currentTotalTriangleVertexCount - 4) + 1);
		_graphicDevice->_triangleIndexArray.push_back((currentTotalTriangleVertexCount - 4) + 3);
		_graphicDevice->_triangleIndexArray.push_back((currentTotalTriangleVertexCount - 4) + 2);
	}

}



#include <stdafx.h>
#include <SimpleRendering/RectangleRenderer.h>

#include <SimpleRendering/GraphicDevice.h>
#include <SimpleRendering/TriangleBuffer.hpp>

#include <Math/Float2.hpp>
#include <Math/Float3.hpp>
#include <Math/Float4.hpp>


namespace fs
{
	namespace SimpleRendering
	{
		FS_INLINE fs::Float3 getVertexPosition(const uint32 vertexIndex, const fs::Float3& position, const fs::Float2& size)
		{
			return fs::Float3(position._x + (vertexIndex & 1) * size._x, position._y + ((vertexIndex & 2) >> 1) * size._y, position._z);
		}

		FS_INLINE fs::Float2 getVertexTexturePosition(const uint32 vertexIndex, const fs::Float2& position, const fs::Float2& size)
		{
			return fs::Float2(position._x + (vertexIndex & 1) * size._x, position._y + ((vertexIndex & 2) >> 1) * size._y);
		}

		void RectangleRenderer::drawColored()
		{
			auto& vertexArray = _graphicDevice->_rectangleRendererBuffer.vertexArray();
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(0, _position, _size), getColorInternal(0)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(1, _position, _size), getColorInternal(1)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(2, _position, _size), getColorInternal(2)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(3, _position, _size), getColorInternal(3)));

			prepareIndexArray();
		}

		void RectangleRenderer::drawTextured(const fs::Float2& texturePosition, const fs::Float2& textureSize)
		{
			auto& vertexArray = _graphicDevice->_rectangleRendererBuffer.vertexArray();
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(0, _position, _size), getVertexTexturePosition(0, texturePosition, textureSize)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(1, _position, _size), getVertexTexturePosition(1, texturePosition, textureSize)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(2, _position, _size), getVertexTexturePosition(2, texturePosition, textureSize)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(3, _position, _size), getVertexTexturePosition(3, texturePosition, textureSize)));

			prepareIndexArray();
		}

		void RectangleRenderer::drawColoredTextured(const fs::Float2& texturePosition, const fs::Float2& textureSize)
		{
			auto& vertexArray = _graphicDevice->_rectangleRendererBuffer.vertexArray();
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(0, _position, _size), getColorInternal(0), getVertexTexturePosition(0, texturePosition, textureSize)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(1, _position, _size), getColorInternal(1), getVertexTexturePosition(1, texturePosition, textureSize)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(2, _position, _size), getColorInternal(2), getVertexTexturePosition(2, texturePosition, textureSize)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(3, _position, _size), getColorInternal(3), getVertexTexturePosition(3, texturePosition, textureSize)));

			prepareIndexArray();
		}

		void RectangleRenderer::prepareIndexArray()
		{
			const auto& vertexArray = _graphicDevice->_rectangleRendererBuffer.vertexArray();
			const uint32 currentTotalTriangleVertexCount = static_cast<uint32>(vertexArray.size());

			auto& indexArray = _graphicDevice->_rectangleRendererBuffer.indexArray();
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 0);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 1);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 2);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 1);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 3);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 2);
		}
	}
}

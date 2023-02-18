#pragma once


#include <MintRenderingBase/Include/LowLevelRenderer.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/MeshData.h>

#include <MintLibrary/Include/Algorithm.hpp>


namespace mint
{
	namespace Rendering
	{
		template <typename T>
		inline LowLevelRenderer<T>::LowLevelRenderer(GraphicDevice& graphicDevice)
			: _graphicDevice{ graphicDevice }
			, _vertexStride{ sizeof(T) }
			, _vertexBufferID{}
			, _indexBase{ 0 }
			, _indexBufferID{}
		{
			__noop;
		}

		template <typename T>
		MINT_INLINE Vector<T>& LowLevelRenderer<T>::vertices() noexcept
		{
			return _vertices;
		}

		template <typename T>
		MINT_INLINE Vector<IndexElementType>& LowLevelRenderer<T>::indices() noexcept
		{
			return _indices;
		}

		template<typename T>
		MINT_INLINE uint32 LowLevelRenderer<T>::getVertexCount() const noexcept
		{
			return _vertices.size();
		}

		template<typename T>
		MINT_INLINE uint32 LowLevelRenderer<T>::getIndexCount() const noexcept
		{
			return _indices.size();
		}

		template<typename T>
		MINT_INLINE void LowLevelRenderer<T>::pushMesh(const MeshData& meshData) noexcept
		{
			const uint32 vertexCount = meshData.getVertexCount();
			const uint32 indexCount = meshData.getIndexCount();
			const VS_INPUT* const meshVertices = meshData.getVertices();
			const IndexElementType* const meshIndices = meshData.getIndices();
			for (uint32 vertexIter = 0; vertexIter < vertexCount; ++vertexIter)
			{
				_vertices.push_back(meshVertices[vertexIter]);
			}

			// 여러 메시가 push 될 경우, 추가되는 메시의 vertex index 가
			// 바로 이전 메시의 마지막 vertex index 이후부터 시작되도록 보장한다.
			IndexElementType indexBase = getIndexBaseXXX();
			for (uint32 indexIter = 0; indexIter < indexCount; ++indexIter)
			{
				_indices.push_back(indexBase + meshIndices[indexIter]);
			}
			setIndexBaseXXX(indexBase + vertexCount);
		}

		template<typename T>
		MINT_INLINE void LowLevelRenderer<T>::setIndexBaseXXX(const IndexElementType base) noexcept
		{
			_indexBase = base;
		}

		template<typename T>
		MINT_INLINE IndexElementType LowLevelRenderer<T>::getIndexBaseXXX() const noexcept
		{
			return _indexBase;
		}

		template <typename T>
		MINT_INLINE void LowLevelRenderer<T>::flush() noexcept
		{
			_vertices.clear();
			_indices.clear();
			_indexBase = 0;
		}

		template<typename T>
		MINT_INLINE bool LowLevelRenderer<T>::isRenderable() const noexcept
		{
			return _vertices.empty() == false;
		}

		template <typename T>
		inline void LowLevelRenderer<T>::render(const RenderingPrimitive renderingPrimitive) noexcept
		{
			if (isRenderable() == false)
			{
				return;
			}

			prepareBuffers();

			DxResourcePool& resourcePool = _graphicDevice.getResourcePool();
			DxResource& vertexBuffer = resourcePool.getResource(_vertexBufferID);
			DxResource& indexBuffer = resourcePool.getResource(_indexBufferID);
			vertexBuffer.bindAsInput();
			indexBuffer.bindAsInput();

			const uint32 vertexCount = static_cast<uint32>(_vertices.size());
			const uint32 indexCount = static_cast<uint32>(_indices.size());

			_graphicDevice.getStateManager().setIARenderingPrimitive(renderingPrimitive);

			switch (renderingPrimitive)
			{
			case RenderingPrimitive::LineList:
				_graphicDevice.draw(vertexCount, 0);
				break;
			case RenderingPrimitive::TriangleList:
				_graphicDevice.drawIndexed(indexCount, 0, 0);

				break;
			default:
				break;
			}
		}

		template<typename T>
		MINT_INLINE void LowLevelRenderer<T>::pushRenderCommandIndexed(const RenderingPrimitive primitive, const uint32 vertexOffset, const uint32 indexOffset, const uint32 indexCount, const Rect& clipRect) noexcept
		{
			RenderCommand newRenderCommand;
			newRenderCommand._isOrdinal = _isOrdinalMode;
			newRenderCommand._primitive = primitive;
			newRenderCommand._clipRect = clipRect;
			newRenderCommand._vertexOffset = vertexOffset;
			newRenderCommand._vertexCount = 0;
			newRenderCommand._indexOffset = indexOffset;
			newRenderCommand._indexCount = indexCount;
			if (mergeNewRenderCommand(newRenderCommand) == false)
			{
				_renderCommands.push_back(newRenderCommand);
			}
		}

		template<typename T>
		MINT_INLINE void LowLevelRenderer<T>::beginOrdinalRenderCommands(const uint64 key) noexcept
		{
			_isOrdinalMode = true;
			_isOrdinalRenderCommandGroupsSorted = false;

			OrdinalRenderCommandGroup ordinalRenderCommandGroup;
			ordinalRenderCommandGroup._key = key;
			ordinalRenderCommandGroup._startRenderCommandIndex = _renderCommands.size();
			_ordinalRenderCommandGroups.push_back(ordinalRenderCommandGroup);
		}

		template<typename T>
		MINT_INLINE void LowLevelRenderer<T>::endOrdinalRenderCommands() noexcept
		{
			_isOrdinalMode = false;

			if (_ordinalRenderCommandGroups.empty())
			{
				return;
			}

			OrdinalRenderCommandGroup& last = _ordinalRenderCommandGroups.back();
			if (last._startRenderCommandIndex == _renderCommands.size())
			{
				// 아무런 RenderCommand 도 등록되지 않았다. 무의미한 Group 이므로 제거한다!
				_ordinalRenderCommandGroups.pop_back();
				return;
			}

			last._endRenderCommandIndex = _renderCommands.size() - 1;
		}

		template<typename T>
		inline void LowLevelRenderer<T>::setOrdinalRenderCommandGroupPriority(const uint64 key, const uint32 priority) noexcept
		{
			if (_isOrdinalRenderCommandGroupsSorted == false)
			{
				mint::quickSort(_ordinalRenderCommandGroups, OrdinalRenderCommandGroup::KeyComparator());
				_isOrdinalRenderCommandGroupsSorted = true;
			}

			const int32 index = mint::binarySearch(_ordinalRenderCommandGroups, key, OrdinalRenderCommandGroup::Evaluator());
			if (IsValidIndex(index) == false)
			{
				return;
			}

			_ordinalRenderCommandGroups[index]._priority = priority;
		}

		template<typename T>
		MINT_INLINE void LowLevelRenderer<T>::executeRenderCommands() noexcept
		{
			if (isRenderable() == false)
			{
				return;
			}

			prepareBuffers();

			DxResourcePool& resourcePool = _graphicDevice.getResourcePool();
			DxResource& vertexBuffer = resourcePool.getResource(_vertexBufferID);
			DxResource& indexBuffer = resourcePool.getResource(_indexBufferID);
			vertexBuffer.bindAsInput();
			indexBuffer.bindAsInput();

			const uint32 renderCommandCount = _renderCommands.size();
			for (uint32 renderCommandIndex = 0; renderCommandIndex < renderCommandCount; ++renderCommandIndex)
			{
				const RenderCommand& renderCommand = _renderCommands[renderCommandIndex];
				if (renderCommand._isOrdinal)
				{
					continue;
				}

				executeRenderCommands_draw(renderCommand);
			}

			// Ordinal 그릴 차례.
			{
				// Priority 가 작을 수록 먼저 그려진다.
				// Priority 가 크면 화면 상 제일 위에 와야 하기 때문!
				mint::quickSort(_ordinalRenderCommandGroups, OrdinalRenderCommandGroup::PriorityComparator());

				for (const OrdinalRenderCommandGroup& ordinalRenderCommandGroup : _ordinalRenderCommandGroups)
				{
					const uint32 start = ordinalRenderCommandGroup._startRenderCommandIndex;
					const uint32 end = ordinalRenderCommandGroup._endRenderCommandIndex;
					for (uint32 renderCommandIndex = start; renderCommandIndex <= end; renderCommandIndex++)
					{
						executeRenderCommands_draw(_renderCommands[renderCommandIndex]);
					}
				}
			}

			_ordinalRenderCommandGroups.clear();
			_isOrdinalRenderCommandGroupsSorted = false;

			_renderCommands.clear();
		}

		template<typename T>
		MINT_INLINE bool LowLevelRenderer<T>::mergeNewRenderCommand(const RenderCommand& newRenderCommand) noexcept
		{
			if (_renderCommands.empty())
			{
				return false;
			}

			RenderCommand& mergeDestRenderCommand = _renderCommands.back();
			if (mergeDestRenderCommand._isOrdinal != newRenderCommand._isOrdinal)
			{
				return false;
			}

			if (_isOrdinalMode == true && _renderCommands.size() <= _ordinalRenderCommandGroups.back()._startRenderCommandIndex)
			{
				return false;
			}

			if (newRenderCommand._primitive == mergeDestRenderCommand._primitive && newRenderCommand._clipRect == mergeDestRenderCommand._clipRect)
			{
				switch (mergeDestRenderCommand._primitive)
				{
				case RenderingPrimitive::LineList:
					if (mergeDestRenderCommand._vertexOffset + mergeDestRenderCommand._vertexCount == newRenderCommand._vertexOffset)
					{
						mergeDestRenderCommand._vertexCount += newRenderCommand._vertexCount;
						return true;
					}
					break;
				case RenderingPrimitive::TriangleList:
					if (mergeDestRenderCommand._indexOffset + mergeDestRenderCommand._indexCount == newRenderCommand._indexOffset)
					{
						mergeDestRenderCommand._indexCount += newRenderCommand._indexCount;
						return true;
					}
					break;
				default:
					break;
				}
			}
			return false;
		}

		template <typename T>
		inline void LowLevelRenderer<T>::prepareBuffers() noexcept
		{
			DxResourcePool& resourcePool = _graphicDevice.getResourcePool();

			const uint32 vertexCount = static_cast<uint32>(_vertices.size());
			if (_vertexBufferID.isValid() == false && vertexCount > 0)
			{
				_vertexBufferID = resourcePool.addVertexBuffer(&_vertices[0], _vertexStride, vertexCount);
			}

			if (_vertexBufferID.isValid())
			{
				DxResource& vertexBuffer = resourcePool.getResource(_vertexBufferID);
				vertexBuffer.updateBuffer(&_vertices[0], vertexCount);
			}

			const uint32 indexCount = static_cast<uint32>(_indices.size());
			if (_indexBufferID.isValid() == false && indexCount > 0)
			{
				_indexBufferID = resourcePool.addIndexBuffer(&_indices[0], indexCount);
			}

			if (_indexBufferID.isValid())
			{
				DxResource& indexBuffer = resourcePool.getResource(_indexBufferID);
				indexBuffer.updateBuffer(&_indices[0], indexCount);
			}
		}

		template<typename T>
		inline void LowLevelRenderer<T>::executeRenderCommands_draw(const RenderCommand& renderCommand) const noexcept
		{
			D3D11_RECT scissorRect = rectToD3dRect(renderCommand._clipRect);
			_graphicDevice.getStateManager().setRSScissorRectangle(scissorRect);

			_graphicDevice.getStateManager().setIARenderingPrimitive(renderCommand._primitive);

			switch (renderCommand._primitive)
			{
			case RenderingPrimitive::LineList:
				_graphicDevice.draw(renderCommand._vertexCount, renderCommand._vertexOffset);
				break;
			case RenderingPrimitive::TriangleList:
				_graphicDevice.drawIndexed(renderCommand._indexCount, renderCommand._indexOffset, renderCommand._vertexOffset);
				break;
			default:
				break;
			}
		}
	}
}

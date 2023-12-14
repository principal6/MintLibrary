#pragma once


#include <MintRenderingBase/Include/LowLevelRenderer.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/Algorithm.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/MeshData.h>
#include <MintRenderingBase/Include/ShapeGenerator.h>


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
		MINT_INLINE Vector<T>& LowLevelRenderer<T>::Vertices() noexcept
		{
			return _vertices;
		}

		template <typename T>
		MINT_INLINE Vector<IndexElementType>& LowLevelRenderer<T>::Indices() noexcept
		{
			return _indices;
		}

		template<typename T>
		MINT_INLINE uint32 LowLevelRenderer<T>::GetVertexCount() const noexcept
		{
			return _vertices.Size();
		}

		template<typename T>
		MINT_INLINE uint32 LowLevelRenderer<T>::GetIndexCount() const noexcept
		{
			return _indices.Size();
		}

		template<typename T>
		MINT_INLINE void LowLevelRenderer<T>::PushShape(const Shape& shape) noexcept
		{
			const uint32 vertexCount = shape._vertices.Size();
			const uint32 indexCount = shape._indices.Size();
			for (uint32 i = 0; i < vertexCount; ++i)
			{
				_vertices.PushBack(shape._vertices[i]);
			}

			// 여러 Shape 가 Push 될 경우, 추가되는 Shape 의 vertex index 가
			// 바로 이전 Shape 의 마지막 vertex index 이후부터 시작되도록 보장한다.
			IndexElementType indexBase = GetIndexBaseXXX();
			for (uint32 i = 0; i < indexCount; ++i)
			{
				_indices.PushBack(indexBase + shape._indices[i]);
			}
			SetIndexBaseXXX(indexBase + vertexCount);
		}

		template<typename T>
		MINT_INLINE void LowLevelRenderer<T>::PushMesh(const MeshData& meshData) noexcept
		{
			const uint32 vertexCount = meshData.GetVertexCount();
			const uint32 indexCount = meshData.GetIndexCount();

			const VS_INPUT* const meshVertices = meshData.GetVertices();
			const IndexElementType* const meshIndices = meshData.GetIndices();
			for (uint32 i = 0; i < vertexCount; ++i)
			{
				_vertices.PushBack(meshVertices[i]);
			}

			// 여러 Mesh 가 Push 될 경우, 추가되는 Mesh 의 vertex index 가
			// 바로 이전 Mesh 의 마지막 vertex index 이후부터 시작되도록 보장한다.
			IndexElementType indexBase = GetIndexBaseXXX();
			for (uint32 i = 0; i < indexCount; ++i)
			{
				_indices.PushBack(indexBase + meshIndices[i]);
			}
			SetIndexBaseXXX(indexBase + vertexCount);
		}

		template<typename T>
		MINT_INLINE void LowLevelRenderer<T>::SetIndexBaseXXX(const IndexElementType base) noexcept
		{
			_indexBase = base;
		}

		template<typename T>
		MINT_INLINE IndexElementType LowLevelRenderer<T>::GetIndexBaseXXX() const noexcept
		{
			return _indexBase;
		}

		template <typename T>
		MINT_INLINE void LowLevelRenderer<T>::Flush() noexcept
		{
			_vertices.Clear();
			_indices.Clear();
			_indexBase = 0;
		}

		template<typename T>
		MINT_INLINE bool LowLevelRenderer<T>::IsRenderable() const noexcept
		{
			return _vertices.IsEmpty() == false;
		}

		template <typename T>
		inline void LowLevelRenderer<T>::Render(const RenderingPrimitive renderingPrimitive) noexcept
		{
			if (IsRenderable() == false)
			{
				return;
			}

			PrepareBuffers();

			GraphicResourcePool& resourcePool = _graphicDevice.GetResourcePool();
			GraphicResource& vertexBuffer = resourcePool.GetResource(_vertexBufferID);
			GraphicResource& indexBuffer = resourcePool.GetResource(_indexBufferID);
			vertexBuffer.BindAsInput();
			indexBuffer.BindAsInput();

			const uint32 vertexCount = static_cast<uint32>(_vertices.Size());
			const uint32 indexCount = static_cast<uint32>(_indices.Size());

			_graphicDevice.GetStateManager().SetIARenderingPrimitive(renderingPrimitive);

			switch (renderingPrimitive)
			{
			case RenderingPrimitive::LineList:
				_graphicDevice.Draw(vertexCount, 0);
				break;
			case RenderingPrimitive::TriangleList:
				_graphicDevice.DrawIndexed(indexCount, 0, 0);

				break;
			default:
				break;
			}
		}

		template<typename T>
		MINT_INLINE void LowLevelRenderer<T>::PushRenderCommandIndexed(const RenderingPrimitive primitive, const uint32 vertexOffset, const uint32 indexOffset, const uint32 indexCount, const Rect& clipRect) noexcept
		{
			RenderCommand newRenderCommand;
			newRenderCommand._isOrdinal = _isOrdinalMode;
			newRenderCommand._primitive = primitive;
			newRenderCommand._clipRect = clipRect;
			newRenderCommand._vertexOffset = vertexOffset;
			newRenderCommand._vertexCount = 0;
			newRenderCommand._indexOffset = indexOffset;
			newRenderCommand._indexCount = indexCount;
			if (MergeNewRenderCommand(newRenderCommand) == false)
			{
				_renderCommands.PushBack(newRenderCommand);
			}
		}

		template<typename T>
		MINT_INLINE void LowLevelRenderer<T>::BeginOrdinalRenderCommands(const uint64 key) noexcept
		{
			_isOrdinalMode = true;
			_isOrdinalRenderCommandGroupsSorted = false;

			OrdinalRenderCommandGroup ordinalRenderCommandGroup;
			ordinalRenderCommandGroup._key = key;
			ordinalRenderCommandGroup._startRenderCommandIndex = _renderCommands.Size();
			_ordinalRenderCommandGroups.PushBack(ordinalRenderCommandGroup);
		}

		template<typename T>
		MINT_INLINE void LowLevelRenderer<T>::EndOrdinalRenderCommands() noexcept
		{
			_isOrdinalMode = false;

			if (_ordinalRenderCommandGroups.IsEmpty())
			{
				return;
			}

			OrdinalRenderCommandGroup& last = _ordinalRenderCommandGroups.Back();
			if (last._startRenderCommandIndex == _renderCommands.Size())
			{
				// 아무런 RenderCommand 도 등록되지 않았다. 무의미한 Group 이므로 제거한다!
				_ordinalRenderCommandGroups.PopBack();
				return;
			}

			last._endRenderCommandIndex = _renderCommands.Size() - 1;
		}

		template<typename T>
		inline void LowLevelRenderer<T>::SetOrdinalRenderCommandGroupPriority(const uint64 key, const uint32 priority) noexcept
		{
			if (_isOrdinalRenderCommandGroupsSorted == false)
			{
				mint::QuickSort(_ordinalRenderCommandGroups, OrdinalRenderCommandGroup::KeyComparator());
				_isOrdinalRenderCommandGroupsSorted = true;
			}

			const int32 index = mint::BinarySearch(_ordinalRenderCommandGroups, key, OrdinalRenderCommandGroup::Evaluator());
			if (IsValidIndex(index) == false)
			{
				return;
			}

			_ordinalRenderCommandGroups[index]._priority = priority;
		}

		template<typename T>
		MINT_INLINE void LowLevelRenderer<T>::ExecuteRenderCommands() noexcept
		{
			if (IsRenderable() == false)
			{
				return;
			}

			PrepareBuffers();

			GraphicResourcePool& resourcePool = _graphicDevice.GetResourcePool();
			GraphicResource& vertexBuffer = resourcePool.GetResource(_vertexBufferID);
			GraphicResource& indexBuffer = resourcePool.GetResource(_indexBufferID);
			vertexBuffer.BindAsInput();
			indexBuffer.BindAsInput();

			const uint32 renderCommandCount = _renderCommands.Size();
			for (uint32 renderCommandIndex = 0; renderCommandIndex < renderCommandCount; ++renderCommandIndex)
			{
				const RenderCommand& renderCommand = _renderCommands[renderCommandIndex];
				if (renderCommand._isOrdinal)
				{
					continue;
				}

				ExecuteRenderCommands_Draw(renderCommand);
			}

			// Ordinal 그릴 차례.
			{
				// Priority 가 작을 수록 먼저 그려진다.
				// Priority 가 크면 화면 상 제일 위에 와야 하기 때문!
				mint::QuickSort(_ordinalRenderCommandGroups, OrdinalRenderCommandGroup::PriorityComparator());

				for (const OrdinalRenderCommandGroup& ordinalRenderCommandGroup : _ordinalRenderCommandGroups)
				{
					const uint32 start = ordinalRenderCommandGroup._startRenderCommandIndex;
					const uint32 end = ordinalRenderCommandGroup._endRenderCommandIndex;
					for (uint32 renderCommandIndex = start; renderCommandIndex <= end; renderCommandIndex++)
					{
						ExecuteRenderCommands_Draw(_renderCommands[renderCommandIndex]);
					}
				}
			}

			_ordinalRenderCommandGroups.Clear();
			_isOrdinalRenderCommandGroupsSorted = false;

			_renderCommands.Clear();
		}

		template<typename T>
		MINT_INLINE bool LowLevelRenderer<T>::MergeNewRenderCommand(const RenderCommand& newRenderCommand) noexcept
		{
			if (_renderCommands.IsEmpty())
			{
				return false;
			}

			RenderCommand& mergeDestRenderCommand = _renderCommands.Back();
			if (mergeDestRenderCommand._isOrdinal != newRenderCommand._isOrdinal)
			{
				return false;
			}

			if (_isOrdinalMode == true && _renderCommands.Size() <= _ordinalRenderCommandGroups.Back()._startRenderCommandIndex)
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
		inline void LowLevelRenderer<T>::PrepareBuffers() noexcept
		{
			GraphicResourcePool& resourcePool = _graphicDevice.GetResourcePool();

			const uint32 vertexCount = static_cast<uint32>(_vertices.Size());
			if (_vertexBufferID.IsValid() == false && vertexCount > 0)
			{
				_vertexBufferID = resourcePool.AddVertexBuffer(&_vertices[0], _vertexStride, vertexCount);
			}

			if (_vertexBufferID.IsValid())
			{
				GraphicResource& vertexBuffer = resourcePool.GetResource(_vertexBufferID);
				vertexBuffer.UpdateBuffer(&_vertices[0], vertexCount);
			}

			const uint32 indexCount = static_cast<uint32>(_indices.Size());
			if (_indexBufferID.IsValid() == false && indexCount > 0)
			{
				_indexBufferID = resourcePool.AddIndexBuffer(&_indices[0], indexCount);
			}

			if (_indexBufferID.IsValid())
			{
				GraphicResource& indexBuffer = resourcePool.GetResource(_indexBufferID);
				indexBuffer.UpdateBuffer(&_indices[0], indexCount);
			}
		}

		template<typename T>
		inline void LowLevelRenderer<T>::ExecuteRenderCommands_Draw(const RenderCommand& renderCommand) const noexcept
		{
			D3D11_RECT scissorRect = RectToD3dRect(renderCommand._clipRect);
			_graphicDevice.GetStateManager().SetRSScissorRectangle(scissorRect);

			_graphicDevice.GetStateManager().SetIARenderingPrimitive(renderCommand._primitive);

			switch (renderCommand._primitive)
			{
			case RenderingPrimitive::LineList:
				_graphicDevice.Draw(renderCommand._vertexCount, renderCommand._vertexOffset);
				break;
			case RenderingPrimitive::TriangleList:
				_graphicDevice.DrawIndexed(renderCommand._indexCount, renderCommand._indexOffset, renderCommand._vertexOffset);
				break;
			default:
				break;
			}
		}
	}
}

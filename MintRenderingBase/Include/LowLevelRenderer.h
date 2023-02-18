#pragma once


#ifndef _MINT_RENDERING_BASE_LOW_LEVEL_RENDERER_H_
#define _MINT_RENDERING_BASE_LOW_LEVEL_RENDERER_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>

#include <Assets/Include/CppHlsl/CppHlslStreamData.h>

#include <MintRenderingBase/Include/GraphicObject.h>

#include <MintMath/Include/Rect.h>


namespace mint
{
	namespace Rendering
	{
		class GraphicDevice;
		struct MeshData;


		enum class RenderingPrimitive
		{
			INVALID,
			LineList,
			TriangleList,
		};

		static constexpr uint32 kVertexOffSetZero = 0;


		template <typename T>
		class LowLevelRenderer
		{
			struct RenderCommand
			{
				bool _isOrdinal = true;
				RenderingPrimitive _primitive = RenderingPrimitive::INVALID;
				Rect _clipRect;
				uint32 _vertexOffset = 0;
				uint32 _vertexCount = 0;
				uint32 _indexOffset = 0;
				uint32 _indexCount = 0;
			};

			struct OrdinalRenderCommandGroup
			{
				uint64 _key = 0;
				uint32 _priority = 0;
				uint32 _startRenderCommandIndex = 0;
				uint32 _endRenderCommandIndex = 0;

			public:
				bool operator==(const uint64 key) const noexcept
				{
					return _key == key;
				}

				struct Evaluator
				{
					uint64 operator()(const OrdinalRenderCommandGroup& rhs) const noexcept
					{
						return rhs._key;
					}
				};

				struct KeyComparator
				{
					bool operator()(const OrdinalRenderCommandGroup& lhs, const OrdinalRenderCommandGroup& rhs) const noexcept
					{
						return lhs._key < rhs._key;
					}
				};

				// Priority 가 작을 수록 먼저 그려진다.
				// Priority 가 크면 화면 상 제일 위에 와야 하기 때문!
				struct PriorityComparator
				{
					bool operator()(const OrdinalRenderCommandGroup& lhs, const OrdinalRenderCommandGroup& rhs) const noexcept
					{
						return lhs._priority < rhs._priority;
					}
				};
			};

		public:
			LowLevelRenderer(GraphicDevice& graphicDevice);
			~LowLevelRenderer() = default;

		public:
			Vector<T>& Vertices() noexcept;
			Vector<IndexElementType>& Indices() noexcept;
			uint32 GetVertexCount() const noexcept;
			uint32 GetIndexCount() const noexcept;

		public:
			void PushMesh(const MeshData& meshData) noexcept;

		public:
			void SetIndexBaseXXX(const IndexElementType base) noexcept;
			IndexElementType GetIndexBaseXXX() const noexcept;

		public:
			void Flush() noexcept;
			bool IsRenderable() const noexcept;
			void Render(const RenderingPrimitive renderingPrimitive) noexcept;

		public:
			void PushRenderCommandIndexed(const RenderingPrimitive primitive, const uint32 vertexOffset, const uint32 indexOffset, const uint32 indexCount, const Rect& clipRect) noexcept;
			void BeginOrdinalRenderCommands(const uint64 key) noexcept;
			void EndOrdinalRenderCommands() noexcept;
			void SetOrdinalRenderCommandGroupPriority(const uint64 key, const uint32 priority) noexcept;
			void ExecuteRenderCommands() noexcept;

		private:
			bool MergeNewRenderCommand(const RenderCommand& newRenderCommand) noexcept;
			void PrepareBuffers() noexcept;
			void ExecuteRenderCommands_Draw(const RenderCommand& renderCommand) const noexcept;

		private:
			GraphicDevice& _graphicDevice;
			const uint32 _vertexStride;

		private:
			Vector<T> _vertices;
			GraphicObjectID _vertexBufferID;

		private:
			Vector<IndexElementType> _indices;
			IndexElementType _indexBase;
			GraphicObjectID _indexBufferID;

		private:
			bool _isOrdinalMode = false;
			Vector<RenderCommand> _renderCommands;
			Vector<OrdinalRenderCommandGroup> _ordinalRenderCommandGroups;
			bool _isOrdinalRenderCommandGroupsSorted = false;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_LOW_LEVEL_RENDERER_H_

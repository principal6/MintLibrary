#pragma once


#ifndef _MINT_RENDERING_BASE_MESH_DATA_H_
#define _MINT_RENDERING_BASE_MESH_DATA_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>

#include <MintRendering/Include/RenderingBaseCommon.h>

#include <Assets/Include/CppHlsl/CppHlslStreamData.h>


namespace mint
{
	class Float4;


	namespace Rendering
	{
		struct VS_INPUT;
		struct Face;


		struct MeshData
		{
		public:
			MeshData();

		public:
			bool IsEmpty() const noexcept;
			void Clear() noexcept;
			void ShrinkToFit() noexcept;
			void UpdateVertexFromPositions() noexcept;

		public:
			uint32 GetPositionCount() const noexcept;
			uint32 GetVertexCount() const noexcept;
			uint32 GetFaceCount() const noexcept;
			uint32 GetIndexCount() const noexcept;
			const VS_INPUT* GetVertices() const noexcept;
			const IndexElementType* GetIndices() const noexcept;

		public:
			static const VS_INPUT kNullVertex;
			static const IndexElementType kNullIndex;

		public:
			Vector<Float4> _positionArray;
			Vector<uint32> _vertexToPositionTable;
			Vector<VS_INPUT> _vertexArray;
			Vector<Face> _faceArray;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_MESH_DATA_H_

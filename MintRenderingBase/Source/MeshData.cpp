#include <MintRenderingBase/Include/MeshData.h>

#include <MintContainer/Include/Vector.hpp>


namespace mint
{
	namespace Rendering
	{
		const VS_INPUT          MeshData::kNullVertex = VS_INPUT();
		const IndexElementType  MeshData::kNullIndex = 0;

		MeshData::MeshData()
		{
			__noop;
		}

		bool MeshData::IsEmpty() const noexcept
		{
			return _positionArray.IsEmpty() && _vertexToPositionTable.IsEmpty() && _vertexArray.IsEmpty() && _faceArray.IsEmpty();
		}

		void MeshData::Clear() noexcept
		{
			_positionArray.Clear();
			_vertexToPositionTable.Clear();
			_vertexArray.Clear();
			_faceArray.Clear();
		}

		void MeshData::ShrinkToFit() noexcept
		{
			_positionArray.ShrinkToFit();
			_vertexToPositionTable.ShrinkToFit();
			_vertexArray.ShrinkToFit();
			_faceArray.ShrinkToFit();
		}

		void MeshData::UpdateVertexFromPositions() noexcept
		{
			const uint32 vertexCount = GetVertexCount();
			for (uint32 vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
			{
				const uint32 positionIndex = _vertexToPositionTable[vertexIndex];
				_vertexArray[vertexIndex]._positionU.SetXYZ(_positionArray[positionIndex]);
			}
		}

		uint32 MeshData::GetPositionCount() const noexcept
		{
			return _positionArray.Size();
		}

		uint32 MeshData::GetVertexCount() const noexcept
		{
			return _vertexArray.Size();
		}

		uint32 MeshData::GetFaceCount() const noexcept
		{
			return _faceArray.Size();
		}

		uint32 MeshData::GetIndexCount() const noexcept
		{
			return static_cast<uint32>(_faceArray.Size() * Face::kVertexCountPerFace);
		}

		const VS_INPUT* MeshData::GetVertices() const noexcept
		{
			return (_vertexArray.IsEmpty()) ? &MeshData::kNullVertex : &_vertexArray[0];
		}

		const IndexElementType* MeshData::GetIndices() const noexcept
		{
			return (_faceArray.IsEmpty() == true) ? &MeshData::kNullIndex : &_faceArray[0]._vertexIndexArray[0];
		}
	}
}

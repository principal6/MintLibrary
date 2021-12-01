#include <stdafx.h>
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

        const bool MeshData::isEmpty() const noexcept
        {
            return _positionArray.empty() && _vertexToPositionTable.empty() && _vertexArray.empty() && _faceArray.empty();
        }

        void MeshData::clear() noexcept
        {
            _positionArray.clear();
            _vertexToPositionTable.clear();
            _vertexArray.clear();
            _faceArray.clear();
        }

        void MeshData::shrinkToFit() noexcept
        {
            _positionArray.shrink_to_fit();
            _vertexToPositionTable.shrink_to_fit();
            _vertexArray.shrink_to_fit();
            _faceArray.shrink_to_fit();
        }

        void MeshData::updateVertexFromPositions() noexcept
        {
            const uint32 vertexCount = getVertexCount();
            for (uint32 vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
            {
                const uint32 positionIndex = _vertexToPositionTable[vertexIndex];
                _vertexArray[vertexIndex]._positionU.setXyz(_positionArray[positionIndex]);
            }
        }

        const uint32 MeshData::getPositionCount() const noexcept
        {
            return _positionArray.size();
        }

        const uint32 MeshData::getVertexCount() const noexcept
        {
            return _vertexArray.size();
        }

        const uint32 MeshData::getFaceCount() const noexcept
        {
            return _faceArray.size();
        }

        const uint32 MeshData::getIndexCount() const noexcept
        {
            return static_cast<uint32>(_faceArray.size() * Face::kVertexCountPerFace);
        }

        const VS_INPUT* MeshData::getVertices() const noexcept
        {
            return (_vertexArray.empty()) ? &MeshData::kNullVertex : &_vertexArray[0];
        }

        const IndexElementType* MeshData::getIndices() const noexcept
        {
            return (_faceArray.empty() == true) ? &MeshData::kNullIndex : &_faceArray[0]._vertexIndexArray[0];
        }
    }
}

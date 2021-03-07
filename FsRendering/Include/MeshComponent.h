#pragma once


#ifndef FS_MESH_COMPONENT_H
#define FS_MESH_COMPONENT_H


#include <CommonDefinitions.h>

#include <FsRenderingBase/Include/RenderingBaseCommon.h>

#include <FsRendering/Include/TransformComponent.h>


namespace fs
{
	namespace RenderingBase
	{
		struct VS_INPUT;
		struct Face;
	}
	
	namespace Rendering
	{
		class MeshComponent final : public TransformComponent
		{
		private:
			static fs::RenderingBase::VS_INPUT				kNullVertex;
			static fs::RenderingBase::IndexElementType		kNullIndex;

		public:
			struct MeshData
			{
				void clear()
				{
					_positionArray.clear();
					_vertexToPositionTable.clear();
					_vertexArray.clear();
					_faceArray.clear();
				}

				const uint32 getPositionCount() const noexcept
				{
					return static_cast<uint32>(_positionArray.size());
				}

				const uint32 getVertexCount() const noexcept
				{
					return static_cast<uint32>(_vertexArray.size());
				}

				const uint32 getFaceCount() const noexcept
				{
					return static_cast<uint32>(_faceArray.size());
				}

				std::vector<fs::Float4>						_positionArray;
				std::vector<uint32>							_vertexToPositionTable;
				std::vector<fs::RenderingBase::VS_INPUT>	_vertexArray;
				std::vector<fs::RenderingBase::Face>		_faceArray;
			};

		public:
															MeshComponent();
			virtual											~MeshComponent();

		public:
			const uint32									getVertexCount() const noexcept;
			const fs::RenderingBase::VS_INPUT*				getVertices() const noexcept;
			const uint32									getIndexCount() const noexcept;
			const fs::RenderingBase::IndexElementType*		getIndices() const noexcept;

		private:
			MeshData										_meshData;
		};
	}
}


#endif // !FS_MESH_COMPONENT_H

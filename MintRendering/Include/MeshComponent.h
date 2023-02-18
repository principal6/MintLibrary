#pragma once


#ifndef _MINT_RENDERING_MESH_COMPONENT_H_
#define _MINT_RENDERING_MESH_COMPONENT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintRenderingBase/Include/MeshData.h>

#include <MintRendering/Include/TransformComponent.h>
#include <MintRendering/Include/MeshGenerator.h>


namespace mint
{
	namespace Rendering
	{
		class MeshComponent final : public TransformComponent
		{
		public:
			MeshComponent();
			virtual ~MeshComponent();

		public:
			const MeshData& GetMeshData() const noexcept;
			uint32 GetVertexCount() const noexcept;
			const VS_INPUT* GetVertices() const noexcept;
			uint32 GetIndexCount() const noexcept;
			const IndexElementType* GetIndices() const noexcept;

		public:
			void ShouldDrawNormals(const bool shouldDrawNormals) noexcept;
			bool ShouldDrawNormals() const noexcept;
			void ShouldDrawEdges(const bool shouldDrawEdges) noexcept;
			bool ShouldDrawEdges() const noexcept;

		private:
			MeshData _meshData;
			bool _shouldDrawNormals;
			bool _shouldDrawEdges;
		};
	}
}


#endif // !_MINT_RENDERING_MESH_COMPONENT_H_

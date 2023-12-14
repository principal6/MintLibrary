#pragma once


#ifndef _MINT_GAME_MESH_COMPONENT_H_
#define _MINT_GAME_MESH_COMPONENT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintRenderingBase/Include/MeshData.h>

#include <MintRendering/Include/MeshGenerator.h>

#include <MintGame/Include/TransformComponent.h>


namespace mint
{
	namespace Game
	{
		class MeshComponent final : public ObjectComponent
		{
		public:
			MeshComponent();
			virtual ~MeshComponent();

		public:
			const Rendering::MeshData& GetMeshData() const noexcept;
			uint32 GetVertexCount() const noexcept;
			const Rendering::VS_INPUT* GetVertices() const noexcept;
			uint32 GetIndexCount() const noexcept;
			const Rendering::IndexElementType* GetIndices() const noexcept;

		public:
			void ShouldDrawNormals(const bool shouldDrawNormals) noexcept;
			bool ShouldDrawNormals() const noexcept;
			void ShouldDrawEdges(const bool shouldDrawEdges) noexcept;
			bool ShouldDrawEdges() const noexcept;

		private:
			Rendering::MeshData _meshData;
			bool _shouldDrawNormals;
			bool _shouldDrawEdges;
		};

		class Mesh2DComponent final : public ObjectComponent
		{
		public:
			Mesh2DComponent();
			virtual ~Mesh2DComponent();

		public:
			void SetShape(const Rendering::Shape& shape);
			const Rendering::Shape& GetShape() const noexcept;

		private:
			Rendering::Shape _shape;
		};
	}
}


#endif // !_MINT_GAME_MESH_COMPONENT_H_

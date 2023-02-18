#include <MintRendering/Include/MeshComponent.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/DxResource.h>


namespace mint
{
	namespace Rendering
	{
		MeshComponent::MeshComponent()
			: TransformComponent(ObjectComponentType::MeshComponent)
			, _shouldDrawNormals{ false }
			, _shouldDrawEdges{ false }
		{
			MeshGenerator::GeoSphereParam geosphereParam;
			geosphereParam._radius = 1.0f;
			geosphereParam._subdivisionIteration = 3;
			geosphereParam._smooth = true;
			MeshGenerator::GenerateGeoSphere(geosphereParam, _meshData);
		}

		MeshComponent::~MeshComponent()
		{
			__noop;
		}

		const MeshData& MeshComponent::GetMeshData() const noexcept
		{
			return _meshData;
		}

		uint32 MeshComponent::GetVertexCount() const noexcept
		{
			return _meshData.GetVertexCount();
		}

		const VS_INPUT* MeshComponent::GetVertices() const noexcept
		{
			return (_meshData._vertexArray.IsEmpty()) ? &MeshData::kNullVertex : &_meshData._vertexArray[0];
		}

		uint32 MeshComponent::GetIndexCount() const noexcept
		{
			return _meshData.GetIndexCount();
		}

		const IndexElementType* MeshComponent::GetIndices() const noexcept
		{
			return (_meshData._faceArray.IsEmpty() == true) ? &MeshData::kNullIndex : &_meshData._faceArray[0]._vertexIndexArray[0];
		}

		void MeshComponent::ShouldDrawNormals(const bool shouldDrawNormals) noexcept
		{
			_shouldDrawNormals = shouldDrawNormals;
		}

		bool MeshComponent::ShouldDrawNormals() const noexcept
		{
			return _shouldDrawNormals;
		}

		void MeshComponent::ShouldDrawEdges(const bool shouldDrawEdges) noexcept
		{
			_shouldDrawEdges = shouldDrawEdges;
		}

		bool MeshComponent::ShouldDrawEdges() const noexcept
		{
			return _shouldDrawEdges;
		}
	}
}

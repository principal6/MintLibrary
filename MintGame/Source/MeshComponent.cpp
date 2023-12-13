#include <MintGame/Include/MeshComponent.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/DxResource.h>


namespace mint
{
	namespace Game
	{
#pragma region MeshComponent
		MeshComponent::MeshComponent()
			: ObjectComponent(ObjectComponentType::MeshComponent)
			, _shouldDrawNormals{ false }
			, _shouldDrawEdges{ false }
		{
			Rendering::MeshGenerator::GeoSphereParam geosphereParam;
			geosphereParam._radius = 1.0f;
			geosphereParam._subdivisionIteration = 3;
			geosphereParam._smooth = true;
			Rendering::MeshGenerator::GenerateGeoSphere(geosphereParam, _meshData);
		}

		MeshComponent::~MeshComponent()
		{
			__noop;
		}

		const Rendering::MeshData& MeshComponent::GetMeshData() const noexcept
		{
			return _meshData;
		}

		uint32 MeshComponent::GetVertexCount() const noexcept
		{
			return _meshData.GetVertexCount();
		}

		const Rendering::VS_INPUT* MeshComponent::GetVertices() const noexcept
		{
			return (_meshData._vertexArray.IsEmpty()) ? &Rendering::MeshData::kNullVertex : &_meshData._vertexArray[0];
		}

		uint32 MeshComponent::GetIndexCount() const noexcept
		{
			return _meshData.GetIndexCount();
		}

		const Rendering::IndexElementType* MeshComponent::GetIndices() const noexcept
		{
			return (_meshData._faceArray.IsEmpty() == true) ? &Rendering::MeshData::kNullIndex : &_meshData._faceArray[0]._vertexIndexArray[0];
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
#pragma endregion


#pragma region Mesh2DComponent
		Mesh2DComponent::Mesh2DComponent()
			: ObjectComponent(ObjectComponentType::Mesh2DComponent)
		{
			__noop;
		}

		Mesh2DComponent::~Mesh2DComponent()
		{
			__noop;
		}

		void Mesh2DComponent::SetShape(const Rendering::Shape& shape)
		{
			_shape = shape;
		}

		const Rendering::Shape& Mesh2DComponent::GetShape() const noexcept
		{
			return _shape;
		}
#pragma endregion
	}
}

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
            MeshGenerator::generateGeoSphere(geosphereParam, _meshData);
        }

        MeshComponent::~MeshComponent()
        {
            __noop;
        }

        const MeshData& MeshComponent::getMeshData() const noexcept
        {
            return _meshData;
        }

        uint32 MeshComponent::getVertexCount() const noexcept
        {
            return _meshData.getVertexCount();
        }

        const VS_INPUT* MeshComponent::getVertices() const noexcept
        {
            return (_meshData._vertexArray.empty()) ? &MeshData::kNullVertex : &_meshData._vertexArray[0];
        }

        uint32 MeshComponent::getIndexCount() const noexcept
        {
            return _meshData.getIndexCount();
        }

        const IndexElementType* MeshComponent::getIndices() const noexcept
        {
            return (_meshData._faceArray.empty() == true) ? &MeshData::kNullIndex : &_meshData._faceArray[0]._vertexIndexArray[0];
        }
        
        void MeshComponent::shouldDrawNormals(const bool shouldDrawNormals) noexcept
        {
            _shouldDrawNormals = shouldDrawNormals;
        }
        
        bool MeshComponent::shouldDrawNormals() const noexcept
        {
            return _shouldDrawNormals;
        }
        
        void MeshComponent::shouldDrawEdges(const bool shouldDrawEdges) noexcept
        {
            _shouldDrawEdges = shouldDrawEdges;
        }

        bool MeshComponent::shouldDrawEdges() const noexcept
        {
            return _shouldDrawEdges;
        }
    }
}

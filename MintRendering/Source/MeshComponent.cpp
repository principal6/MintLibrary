#include <stdafx.h>
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
            MeshGenerator::GeoSpherePram geosphereParam;
            geosphereParam._radius = 1.0f;
            geosphereParam._subdivisionIteration = 3;
            geosphereParam._smooth = true;
            MeshGenerator::generateGeoSphere(geosphereParam, _meshData);
        }

        MeshComponent::~MeshComponent()
        {
            __noop;
        }

        const mint::RenderingBase::MeshData& MeshComponent::getMeshData() const noexcept
        {
            return _meshData;
        }

        const uint32 MeshComponent::getVertexCount() const noexcept
        {
            return _meshData.getVertexCount();
        }

        const mint::RenderingBase::VS_INPUT* MeshComponent::getVertices() const noexcept
        {
            return (_meshData._vertexArray.empty()) ? &mint::RenderingBase::MeshData::kNullVertex : &_meshData._vertexArray[0];
        }

        const uint32 MeshComponent::getIndexCount() const noexcept
        {
            return _meshData.getIndexCount();
        }

        const mint::RenderingBase::IndexElementType* MeshComponent::getIndices() const noexcept
        {
            return (_meshData._faceArray.empty() == true) ? &mint::RenderingBase::MeshData::kNullIndex : &_meshData._faceArray[0]._vertexIndexArray[0];
        }
        
        void MeshComponent::shouldDrawNormals(const bool shouldDrawNormals) noexcept
        {
            _shouldDrawNormals = shouldDrawNormals;
        }
        
        const bool MeshComponent::shouldDrawNormals() const noexcept
        {
            return _shouldDrawNormals;
        }
        
        void MeshComponent::shouldDrawEdges(const bool shouldDrawEdges) noexcept
        {
            _shouldDrawEdges = shouldDrawEdges;
        }

        const bool MeshComponent::shouldDrawEdges() const noexcept
        {
            return _shouldDrawEdges;
        }
    }
}

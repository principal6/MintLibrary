#include <stdafx.h>
#include <MintRendering/Include/MeshComponent.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/DxResource.h>

#include <Assets/CppHlsl/CppHlslStreamData.h>


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

        const uint32 MeshComponent::getVertexCount() const noexcept
        {
            return static_cast<uint32>(_meshData._vertexArray.size());
        }

        const mint::RenderingBase::VS_INPUT* MeshComponent::getVertices() const noexcept
        {
            return (_meshData._vertexArray.empty()) ? &MeshData::kNullVertex : &_meshData._vertexArray[0];
        }

        const uint32 MeshComponent::getIndexCount() const noexcept
        {
            return static_cast<uint32>(_meshData._faceArray.size() * mint::RenderingBase::Face::kIndexCountPerFace);
        }

        const mint::RenderingBase::IndexElementType* MeshComponent::getIndices() const noexcept
        {
            return (_meshData._faceArray.empty() == true) ? &MeshData::kNullIndex : &_meshData._faceArray[0]._vertexIndexArray[0];
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

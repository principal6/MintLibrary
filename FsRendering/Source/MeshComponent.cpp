#include <stdafx.h>
#include <FsRendering/Include/MeshComponent.h>

#include <FsContainer/Include/Vector.hpp>

#include <FsRenderingBase/Include/DxResource.h>

#include <Assets/CppHlsl/CppHlslStreamData.h>


namespace fs
{
    namespace Rendering
    {
        MeshComponent::MeshComponent()
            : TransformComponent(ObjectComponentType::MeshComponent)
            , _shouldDrawNormals{ false }
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

        const fs::RenderingBase::VS_INPUT* MeshComponent::getVertices() const noexcept
        {
            return (_meshData._vertexArray.empty()) ? &MeshData::kNullVertex : &_meshData._vertexArray[0];
        }

        const uint32 MeshComponent::getIndexCount() const noexcept
        {
            return static_cast<uint32>(_meshData._faceArray.size() * fs::RenderingBase::Face::kIndexCountPerFace);
        }

        const fs::RenderingBase::IndexElementType* MeshComponent::getIndices() const noexcept
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

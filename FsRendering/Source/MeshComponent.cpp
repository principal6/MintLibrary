#include <stdafx.h>
#include <FsRendering/Include/MeshComponent.h>

#include <FsRenderingBase/Include/DxResource.h>

#include <Assets/CppHlsl/CppHlslStreamData.h>


namespace fs
{
    namespace Rendering
    {
        MeshComponent::MeshComponent()
            : TransformComponent(ObjectComponentType::MeshComponent)
        {
            //MeshGenerator::generateCube(_meshData);

            //MeshGenerator::generateCone(MeshGenerator::ConeParam(), _meshData);
            
            MeshGenerator::generateCylinder(MeshGenerator::CylinderParam(), _meshData);
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
            return (_meshData._faceArray.empty() == true) ? &MeshData::kNullIndex : &_meshData._faceArray[0]._indexArray[0];
        }
    }
}

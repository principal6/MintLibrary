#include <stdafx.h>
#include <FsRendering/Include/MeshRenderer.h>

#include <FsRenderingBase/Include/GraphicDevice.h>
#include <FsRenderingBase/Include/LowLevelRenderer.hpp>

#include <FsRendering/Include/Object.h>
#include <FsRendering/Include/ObjectPool.hpp>
#include <FsRendering/Include/MeshComponent.h>


namespace fs
{
    namespace Rendering
    {
        MeshRenderer::MeshRenderer(fs::RenderingBase::GraphicDevice* const graphicDevice)
            : _graphicDevice{ graphicDevice }
            , _lowLevelRenderer{ graphicDevice }
        {
            __noop;
        }

        MeshRenderer::~MeshRenderer()
        {
            __noop;
        }

        void MeshRenderer::initialize() noexcept
        {
            fs::RenderingBase::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
            const fs::Language::CppHlsl& cppHlsl = _graphicDevice->getCppHlslSteamData();
            const fs::Language::CppHlslTypeInfo& vsInputTypeInfo = cppHlsl.getTypeInfo(typeid(fs::RenderingBase::VS_INPUT));

            _vsDefaultId = shaderPool.pushVertexShader("Assets/Hlsl/", "VsDefault.hlsl", "main", &vsInputTypeInfo, "Assets/HlslBinary/");
            _psDefaultId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "PsDefault.hlsl", "main", fs::RenderingBase::DxShaderType::PixelShader, "Assets/HlslBinary/");
            
            _gsNormalId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "GsNormal.hlsl", "main", fs::RenderingBase::DxShaderType::GeometryShader, "Assets/HlslBinary/");
            _gsTriangleEdgeId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "GsTriangleEdge.hlsl", "main", fs::RenderingBase::DxShaderType::GeometryShader, "Assets/HlslBinary/");
            _psTexCoordAsColorId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "PsTexCoordAsColor.hlsl", "main", fs::RenderingBase::DxShaderType::PixelShader, "Assets/HlslBinary/");
            
            fs::RenderingBase::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
            const fs::Language::CppHlslTypeInfo& cbTransformDataTypeInfo = _graphicDevice->getCppHlslConstantBuffers().getTypeInfo(typeid(_cbTransformData));
            _cbTransformId = resourcePool.pushConstantBuffer(reinterpret_cast<const byte*>(&_cbTransformData), sizeof(_cbTransformData), cbTransformDataTypeInfo.getRegisterIndex());
        }

        void MeshRenderer::render(const fs::Rendering::ObjectPool& objectPool) noexcept
        {
            const std::vector<fs::Rendering::MeshComponent*>& meshComponents = objectPool.getMeshComponents();

            fs::RenderingBase::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
            shaderPool.bindShaderIfNot(fs::RenderingBase::DxShaderType::VertexShader, _vsDefaultId);

            fs::RenderingBase::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
            fs::RenderingBase::DxResource& cbTransform = resourcePool.getResource(_cbTransformId);
            {
                cbTransform.bindToShader(fs::RenderingBase::DxShaderType::VertexShader, cbTransform.getRegisterIndex());
                cbTransform.bindToShader(fs::RenderingBase::DxShaderType::GeometryShader, cbTransform.getRegisterIndex());
            }

            auto& trVertexArray = _lowLevelRenderer.vertexArray();
            auto& trIndexArray = _lowLevelRenderer.indexArray();
            for (auto& meshComponentIter : meshComponents)
            {
                const MeshComponent* const meshComponent = meshComponentIter;
                _cbTransformData._cbWorldMatrix = meshComponent->getOwnerObject()->getObjectTransformMatrix() * meshComponent->_srt.toMatrix();
                cbTransform.updateBuffer(reinterpret_cast<const byte*>(&_cbTransformData), 1);

                _lowLevelRenderer.flush();

                const uint32 vertexCount = meshComponent->getVertexCount();
                const uint32 indexCount = meshComponent->getIndexCount();
                const fs::RenderingBase::VS_INPUT* const vertices = meshComponent->getVertices();
                const fs::RenderingBase::IndexElementType* const indices = meshComponent->getIndices();
                for (uint32 vertexIter = 0; vertexIter < vertexCount; vertexIter++)
                {
                    trVertexArray.emplace_back(vertices[vertexIter]);
                }
                for (uint32 indexIter = 0; indexIter < indexCount; indexIter++)
                {
                    trIndexArray.emplace_back(indices[indexIter]);
                }

                shaderPool.bindShaderIfNot(fs::RenderingBase::DxShaderType::PixelShader, _psDefaultId);
                shaderPool.unbindShader(fs::RenderingBase::DxShaderType::GeometryShader);
                _lowLevelRenderer.render(fs::RenderingBase::RenderingPrimitive::TriangleList);

                if (meshComponent->shouldDrawNormals() == true)
                {
                    shaderPool.bindShaderIfNot(fs::RenderingBase::DxShaderType::GeometryShader, _gsNormalId);
                    shaderPool.bindShaderIfNot(fs::RenderingBase::DxShaderType::PixelShader, _psTexCoordAsColorId);
                    _lowLevelRenderer.render(fs::RenderingBase::RenderingPrimitive::LineList);
                }

                if (meshComponent->shouldDrawEdges() == true)
                {
                    shaderPool.bindShaderIfNot(fs::RenderingBase::DxShaderType::GeometryShader, _gsTriangleEdgeId);
                    shaderPool.bindShaderIfNot(fs::RenderingBase::DxShaderType::PixelShader, _psTexCoordAsColorId);
                    _lowLevelRenderer.render(fs::RenderingBase::RenderingPrimitive::TriangleList);
                }
            }
        }
    }
}

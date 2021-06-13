#include <stdafx.h>
#include <MintRendering/Include/MeshRenderer.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>

#include <MintRendering/Include/Object.h>
#include <MintRendering/Include/ObjectPool.hpp>
#include <MintRendering/Include/MeshComponent.h>


namespace mint
{
    namespace Rendering
    {
        MeshRenderer::MeshRenderer(mint::RenderingBase::GraphicDevice* const graphicDevice)
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
            mint::RenderingBase::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
            const mint::Language::CppHlsl& cppHlsl = _graphicDevice->getCppHlslSteamData();
            const mint::Language::CppHlslTypeInfo& vsInputTypeInfo = cppHlsl.getTypeInfo(typeid(mint::RenderingBase::VS_INPUT));

            _vsDefaultId = shaderPool.pushVertexShader("Assets/Hlsl/", "VsDefault.hlsl", "main", &vsInputTypeInfo, "Assets/HlslBinary/");
            _psDefaultId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "PsDefault.hlsl", "main", mint::RenderingBase::DxShaderType::PixelShader, "Assets/HlslBinary/");
            
            _gsNormalId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "GsNormal.hlsl", "main", mint::RenderingBase::DxShaderType::GeometryShader, "Assets/HlslBinary/");
            _gsTriangleEdgeId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "GsTriangleEdge.hlsl", "main", mint::RenderingBase::DxShaderType::GeometryShader, "Assets/HlslBinary/");
            _psTexCoordAsColorId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "PsTexCoordAsColor.hlsl", "main", mint::RenderingBase::DxShaderType::PixelShader, "Assets/HlslBinary/");
            
            mint::RenderingBase::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
            const mint::Language::CppHlslTypeInfo& cbTransformDataTypeInfo = _graphicDevice->getCppHlslConstantBuffers().getTypeInfo(typeid(_cbTransformData));
            _cbTransformId = resourcePool.pushConstantBuffer(reinterpret_cast<const byte*>(&_cbTransformData), sizeof(_cbTransformData), cbTransformDataTypeInfo.getRegisterIndex());
        }

        void MeshRenderer::render(const mint::Rendering::ObjectPool& objectPool) noexcept
        {
            const mint::Vector<mint::Rendering::MeshComponent*>& meshComponents = objectPool.getMeshComponents();

            mint::RenderingBase::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
            shaderPool.bindShaderIfNot(mint::RenderingBase::DxShaderType::VertexShader, _vsDefaultId);

            mint::RenderingBase::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
            mint::RenderingBase::DxResource& cbTransform = resourcePool.getResource(_cbTransformId);
            {
                cbTransform.bindToShader(mint::RenderingBase::DxShaderType::VertexShader, cbTransform.getRegisterIndex());
                cbTransform.bindToShader(mint::RenderingBase::DxShaderType::GeometryShader, cbTransform.getRegisterIndex());
            }

            auto& trVertexArray = _lowLevelRenderer.vertexArray();
            auto& trIndexArray = _lowLevelRenderer.indexArray();
            const uint32 meshComponentCount = meshComponents.size();
            for (uint32 meshCompnentIndex = 0; meshCompnentIndex < meshComponentCount; ++meshCompnentIndex)
            {
                const MeshComponent* const meshComponent = meshComponents[meshCompnentIndex];
                _cbTransformData._cbWorldMatrix = meshComponent->getOwnerObject()->getObjectTransformMatrix() * meshComponent->_srt.toMatrix();
                cbTransform.updateBuffer(reinterpret_cast<const byte*>(&_cbTransformData), 1);

                _lowLevelRenderer.flush();

                const uint32 vertexCount = meshComponent->getVertexCount();
                const uint32 indexCount = meshComponent->getIndexCount();
                const mint::RenderingBase::VS_INPUT* const vertices = meshComponent->getVertices();
                const mint::RenderingBase::IndexElementType* const indices = meshComponent->getIndices();
                for (uint32 vertexIter = 0; vertexIter < vertexCount; vertexIter++)
                {
                    trVertexArray.push_back(vertices[vertexIter]);
                }
                for (uint32 indexIter = 0; indexIter < indexCount; indexIter++)
                {
                    trIndexArray.push_back(indices[indexIter]);
                }

                shaderPool.bindShaderIfNot(mint::RenderingBase::DxShaderType::PixelShader, _psDefaultId);
                shaderPool.unbindShader(mint::RenderingBase::DxShaderType::GeometryShader);
                _lowLevelRenderer.render(mint::RenderingBase::RenderingPrimitive::TriangleList);

                if (meshComponent->shouldDrawNormals() == true)
                {
                    shaderPool.bindShaderIfNot(mint::RenderingBase::DxShaderType::GeometryShader, _gsNormalId);
                    shaderPool.bindShaderIfNot(mint::RenderingBase::DxShaderType::PixelShader, _psTexCoordAsColorId);
                    _lowLevelRenderer.render(mint::RenderingBase::RenderingPrimitive::LineList);
                }

                if (meshComponent->shouldDrawEdges() == true)
                {
                    shaderPool.bindShaderIfNot(mint::RenderingBase::DxShaderType::GeometryShader, _gsTriangleEdgeId);
                    shaderPool.bindShaderIfNot(mint::RenderingBase::DxShaderType::PixelShader, _psTexCoordAsColorId);
                    _lowLevelRenderer.render(mint::RenderingBase::RenderingPrimitive::TriangleList);
                }
            }
        }
    }
}

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
            const mint::Language::CppHlsl::Interpreter& interpreter = _graphicDevice->getCppHlslSteamData();
            const mint::Language::CppHlsl::TypeMetaData& vsInputTypeMetaData = interpreter.getTypeMetaData(typeid(mint::RenderingBase::VS_INPUT));

            _vsDefaultId = shaderPool.pushVertexShader("Assets/Hlsl/", "VsDefault.hlsl", "main", &vsInputTypeMetaData, "Assets/HlslBinary/");
            _psDefaultId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "PsDefault.hlsl", "main", mint::RenderingBase::DxShaderType::PixelShader, "Assets/HlslBinary/");
            
            _gsNormalId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "GsNormal.hlsl", "main", mint::RenderingBase::DxShaderType::GeometryShader, "Assets/HlslBinary/");
            _gsTriangleEdgeId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "GsTriangleEdge.hlsl", "main", mint::RenderingBase::DxShaderType::GeometryShader, "Assets/HlslBinary/");
            _psTexCoordAsColorId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "PsTexCoordAsColor.hlsl", "main", mint::RenderingBase::DxShaderType::PixelShader, "Assets/HlslBinary/");
        }

        void MeshRenderer::render(const mint::Rendering::ObjectPool& objectPool) noexcept
        {
            const mint::Vector<mint::Rendering::MeshComponent*>& meshComponents = objectPool.getMeshComponents();

            mint::RenderingBase::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
            shaderPool.bindShaderIfNot(mint::RenderingBase::DxShaderType::VertexShader, _vsDefaultId);

            mint::RenderingBase::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
            mint::RenderingBase::DxResource& cbTransform = resourcePool.getResource(_graphicDevice->getCommonCbTransformId());
            {
                cbTransform.bindToShader(mint::RenderingBase::DxShaderType::VertexShader, cbTransform.getRegisterIndex());
                cbTransform.bindToShader(mint::RenderingBase::DxShaderType::GeometryShader, cbTransform.getRegisterIndex());
            }

            mint::RenderingBase::DxResource& sbMaterial = resourcePool.getResource(_graphicDevice->getCommonSbMaterialId());
            {
                sbMaterial.bindToShader(mint::RenderingBase::DxShaderType::PixelShader, sbMaterial.getRegisterIndex());
            }

            mint::RenderingBase::SB_Material sbMaterialData;
            const uint32 meshComponentCount = meshComponents.size();
            for (uint32 meshCompnentIndex = 0; meshCompnentIndex < meshComponentCount; ++meshCompnentIndex)
            {
                const MeshComponent* const meshComponent = meshComponents[meshCompnentIndex];
                _cbTransformData._cbWorldMatrix = meshComponent->getOwnerObject()->getObjectTransformMatrix() * meshComponent->_srt.toMatrix();
                cbTransform.updateBuffer(&_cbTransformData, 1);

                _lowLevelRenderer.flush();
                
                _lowLevelRenderer.pushMesh(meshComponent->getMeshData());

                sbMaterialData._diffuseColor = mint::RenderingBase::Color::kBlue;
                sbMaterial.updateBuffer(&sbMaterialData, 1);
                
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

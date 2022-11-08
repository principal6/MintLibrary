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
        MeshRenderer::MeshRenderer(GraphicDevice& graphicDevice)
            : _graphicDevice{ graphicDevice }
            , _lowLevelRenderer{ graphicDevice }
        {
            initialize();
        }

        MeshRenderer::~MeshRenderer()
        {
            __noop;
        }

        void MeshRenderer::render(const ObjectPool& objectPool) noexcept
        {
            const Vector<MeshComponent*>& meshComponents = objectPool.getMeshComponents();

            DxShaderPool& shaderPool = _graphicDevice.getShaderPool();
            shaderPool.bindShaderIfNot(DxShaderType::VertexShader, _vsDefaultID);

            DxResourcePool& resourcePool = _graphicDevice.getResourcePool();
            DxResource& cbTransform = resourcePool.getResource(_graphicDevice.getCommonCbTransformID());
            {
                cbTransform.bindToShader(DxShaderType::VertexShader, cbTransform.getRegisterIndex());
                cbTransform.bindToShader(DxShaderType::GeometryShader, cbTransform.getRegisterIndex());
            }

            DxResource& sbMaterial = resourcePool.getResource(_graphicDevice.getCommonSBMaterialID());
            {
                sbMaterial.bindToShader(DxShaderType::PixelShader, sbMaterial.getRegisterIndex());
            }

            SB_Material sbMaterialData;
            const uint32 meshComponentCount = meshComponents.size();
            for (uint32 meshCompnentIndex = 0; meshCompnentIndex < meshComponentCount; ++meshCompnentIndex)
            {
                const MeshComponent* const meshComponent = meshComponents[meshCompnentIndex];
                _cbTransformData._cbWorldMatrix = meshComponent->getOwnerObject()->getObjectTransformMatrix() * meshComponent->_transform.toMatrix();
                cbTransform.updateBuffer(&_cbTransformData, 1);

                _lowLevelRenderer.flush();
                
                _lowLevelRenderer.pushMesh(meshComponent->getMeshData());

                sbMaterialData._diffuseColor = Color::kBlue;
                sbMaterial.updateBuffer(&sbMaterialData, 1);
                
                shaderPool.bindShaderIfNot(DxShaderType::PixelShader, _psDefaultID);
                shaderPool.unbindShader(DxShaderType::GeometryShader);
                _lowLevelRenderer.render(RenderingPrimitive::TriangleList);

                if (meshComponent->shouldDrawNormals() == true)
                {
                    shaderPool.bindShaderIfNot(DxShaderType::GeometryShader, _gsNormalID);
                    shaderPool.bindShaderIfNot(DxShaderType::PixelShader, _psTexCoordAsColorID);
                    _lowLevelRenderer.render(RenderingPrimitive::LineList);
                }

                if (meshComponent->shouldDrawEdges() == true)
                {
                    shaderPool.bindShaderIfNot(DxShaderType::GeometryShader, _gsTriangleEdgeID);
                    shaderPool.bindShaderIfNot(DxShaderType::PixelShader, _psTexCoordAsColorID);
                    _lowLevelRenderer.render(RenderingPrimitive::TriangleList);
                }
            }
        }

        void MeshRenderer::initialize() noexcept
        {
            using namespace Language;
            const CppHlsl::Interpreter& interpreter = _graphicDevice.getCppHlslSteamData();
            const TypeMetaData<CppHlsl::TypeCustomData>& vsInputTypeMetaData = interpreter.getTypeMetaData(typeid(VS_INPUT));

            DxShaderPool& shaderPool = _graphicDevice.getShaderPool();
            _vsDefaultID = shaderPool.pushVertexShader(Path::makeIncludeAssetPath("Hlsl/"), "VsDefault.hlsl", "main", &vsInputTypeMetaData, Path::makeIncludeAssetPath("HlslBinary/"));
            _psDefaultID = shaderPool.pushNonVertexShader(Path::makeIncludeAssetPath("Hlsl/"), "PsDefault.hlsl", "main", DxShaderType::PixelShader, Path::makeIncludeAssetPath("HlslBinary/"));

            _gsNormalID = shaderPool.pushNonVertexShader(Path::makeIncludeAssetPath("Hlsl/"), "GsNormal.hlsl", "main", DxShaderType::GeometryShader, Path::makeIncludeAssetPath("HlslBinary/"));
            _gsTriangleEdgeID = shaderPool.pushNonVertexShader(Path::makeIncludeAssetPath("Hlsl/"), "GsTriangleEdge.hlsl", "main", DxShaderType::GeometryShader, Path::makeIncludeAssetPath("HlslBinary/"));
            _psTexCoordAsColorID = shaderPool.pushNonVertexShader(Path::makeIncludeAssetPath("Hlsl/"), "PsTexCoordAsColor.hlsl", "main", DxShaderType::PixelShader, Path::makeIncludeAssetPath("HlslBinary/"));
        }
    }
}

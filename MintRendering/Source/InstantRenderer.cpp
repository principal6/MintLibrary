#include <stdafx.h>
#include <MintRendering/Include/InstantRenderer.h>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>
#include <MintRenderingBase/Include/MeshData.h>

#include <MintRendering/Include/MeshGenerator.h>

#include <Assets/CppHlsl/CppHlslStructuredBuffers.h>


namespace mint
{
    namespace Rendering
    {
        InstantRenderer::InstantRenderer(mint::RenderingBase::GraphicDevice* const graphicDevice)
            : _graphicDevice{ graphicDevice }
            , _lowLevelRendererLine{ graphicDevice }
            , _lowLevelRendererMesh{ graphicDevice }
        {
            __noop;
        }

        InstantRenderer::~InstantRenderer()
        {
            __noop;
        }

        void InstantRenderer::initialize() noexcept
        {
            mint::RenderingBase::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
            const mint::CppHlsl::Interpreter& interpreter = _graphicDevice->getCppHlslSteamData();
            const mint::CppHlsl::TypeMetaData& vsInputTypeMetaData = interpreter.getTypeMetaData(typeid(mint::RenderingBase::VS_INPUT));

            _vsDefaultId = shaderPool.pushVertexShader("Assets/Hlsl/", "VsDefault.hlsl", "main", &vsInputTypeMetaData, "Assets/HlslBinary/");
            _psDefaultId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "PsDefault.hlsl", "main", mint::RenderingBase::DxShaderType::PixelShader, "Assets/HlslBinary/");
            _psColorId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "PsColor.hlsl", "main", mint::RenderingBase::DxShaderType::PixelShader, "Assets/HlslBinary/");
        }

        void InstantRenderer::drawLine(const mint::Float3& a, const mint::Float3& b, const mint::RenderingBase::Color& color) noexcept
        {
            const uint32 materialId = _sbMaterialDatas.size();
            auto& vertices = _lowLevelRendererLine.vertices();
            auto& indices = _lowLevelRendererLine.indices();

            mint::RenderingBase::VS_INPUT vertex;
            vertex._materialId = materialId;

            vertex._positionU.setXyz(a);
            vertices.push_back(vertex);

            vertex._positionU.setXyz(b);
            vertices.push_back(vertex);

            mint::RenderingBase::IndexElementType index = static_cast<mint::RenderingBase::IndexElementType>(indices.size());
            indices.push_back(index + 0);
            indices.push_back(index + 1);

            mint::RenderingBase::SB_Material sbMaterialData;
            sbMaterialData._diffuseColor = color;
            _sbMaterialDatas.push_back(sbMaterialData);
        }

        void InstantRenderer::drawSphere(const mint::Float3& center, const float radius, const uint8 subdivisionIteration, const mint::RenderingBase::Color& color) noexcept
        {

            mint::Rendering::MeshGenerator::GeoSpherePram geosphereParam;
            geosphereParam._radius = radius;
            geosphereParam._subdivisionIteration = subdivisionIteration;
            geosphereParam._smooth = true;
            mint::RenderingBase::MeshData meshData;
            mint::Rendering::MeshGenerator::generateGeoSphere(geosphereParam, meshData);
            
            mint::Float4x4 transformationMatrix;
            transformationMatrix.setTranslation(center);
            mint::Rendering::MeshGenerator::transformMeshData(meshData, transformationMatrix);
            
            const uint32 materialId = _sbMaterialDatas.size();
            mint::Rendering::MeshGenerator::setMaterialId(meshData, materialId);

            _lowLevelRendererMesh.pushMesh(meshData);

            mint::RenderingBase::SB_Material sbMaterialData;
            sbMaterialData._diffuseColor = color;
            _sbMaterialDatas.push_back(sbMaterialData);
        }

        void InstantRenderer::render() noexcept
        {
            mint::RenderingBase::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
            shaderPool.bindShaderIfNot(mint::RenderingBase::DxShaderType::VertexShader, _vsDefaultId);
            shaderPool.unbindShader(mint::RenderingBase::DxShaderType::GeometryShader);

            mint::RenderingBase::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
            mint::RenderingBase::DxResource& cbTransform = resourcePool.getResource(_graphicDevice->getCommonCbTransformId());
            {
                cbTransform.bindToShader(mint::RenderingBase::DxShaderType::VertexShader, cbTransform.getRegisterIndex());
                cbTransform.bindToShader(mint::RenderingBase::DxShaderType::GeometryShader, cbTransform.getRegisterIndex());

                _cbTransformData._cbWorldMatrix.setIdentity();
                cbTransform.updateBuffer(&_cbTransformData);
            }

            mint::RenderingBase::DxResource& sbMaterial = resourcePool.getResource(_graphicDevice->getCommonSbMaterialId());
            {
                sbMaterial.bindToShader(mint::RenderingBase::DxShaderType::PixelShader, sbMaterial.getRegisterIndex());

                sbMaterial.updateBuffer(&_sbMaterialDatas[0], _sbMaterialDatas.size());
            }
            
            shaderPool.bindShaderIfNot(mint::RenderingBase::DxShaderType::PixelShader, _psColorId);
            _lowLevelRendererLine.render(mint::RenderingBase::RenderingPrimitive::LineList);
            _lowLevelRendererLine.flush();

            shaderPool.bindShaderIfNot(mint::RenderingBase::DxShaderType::PixelShader, _psDefaultId);
            _lowLevelRendererMesh.render(mint::RenderingBase::RenderingPrimitive::TriangleList);
            _lowLevelRendererMesh.flush();

            _sbMaterialDatas.clear();
        }
    }
}

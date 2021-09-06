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
        InstantRenderer::InstantRenderer(mint::Rendering::GraphicDevice* const graphicDevice)
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
            mint::Rendering::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
            const Language::CppHlsl::Interpreter& interpreter = _graphicDevice->getCppHlslSteamData();
            const Language::CppHlsl::TypeMetaData& vsInputTypeMetaData = interpreter.getTypeMetaData(typeid(mint::Rendering::VS_INPUT));

            _vsDefaultId = shaderPool.pushVertexShader("Assets/Hlsl/", "VsDefault.hlsl", "main", &vsInputTypeMetaData, "Assets/HlslBinary/");
            _psDefaultId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "PsDefault.hlsl", "main", mint::Rendering::DxShaderType::PixelShader, "Assets/HlslBinary/");
            _psColorId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "PsColor.hlsl", "main", mint::Rendering::DxShaderType::PixelShader, "Assets/HlslBinary/");
        }

        void InstantRenderer::drawLine(const mint::Float3& a, const mint::Float3& b, const mint::Rendering::Color& color) noexcept
        {
            const uint32 materialId = _sbMaterialDatas.size();
            auto& vertices = _lowLevelRendererLine.vertices();
            auto& indices = _lowLevelRendererLine.indices();

            mint::Rendering::VS_INPUT vertex;
            vertex._materialId = materialId;

            vertex._positionU.setXyz(a);
            vertices.push_back(vertex);

            vertex._positionU.setXyz(b);
            vertices.push_back(vertex);

            mint::Rendering::IndexElementType index = static_cast<mint::Rendering::IndexElementType>(indices.size());
            indices.push_back(index + 0);
            indices.push_back(index + 1);

            mint::Rendering::SB_Material sbMaterialData;
            sbMaterialData._diffuseColor = color;
            _sbMaterialDatas.push_back(sbMaterialData);
        }

        void InstantRenderer::drawSphere(const mint::Float3& center, const float radius, const uint8 subdivisionIteration, const mint::Rendering::Color& color) noexcept
        {
            mint::Rendering::MeshGenerator::GeoSpherePram geosphereParam;
            geosphereParam._radius = radius;
            geosphereParam._subdivisionIteration = subdivisionIteration;
            geosphereParam._smooth = true;
            mint::Rendering::MeshData meshData;
            mint::Rendering::MeshGenerator::generateGeoSphere(geosphereParam, meshData);
            
            mint::Float4x4 transformationMatrix;
            transformationMatrix.setTranslation(center);
            mint::Rendering::MeshGenerator::transformMeshData(meshData, transformationMatrix);
            
            const uint32 materialId = _sbMaterialDatas.size();
            mint::Rendering::MeshGenerator::setMaterialId(meshData, materialId);

            _lowLevelRendererMesh.pushMesh(meshData);

            mint::Rendering::SB_Material sbMaterialData;
            sbMaterialData._diffuseColor = color;
            _sbMaterialDatas.push_back(sbMaterialData);
        }

        void InstantRenderer::render() noexcept
        {
            mint::Rendering::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
            shaderPool.bindShaderIfNot(mint::Rendering::DxShaderType::VertexShader, _vsDefaultId);
            shaderPool.unbindShader(mint::Rendering::DxShaderType::GeometryShader);

            mint::Rendering::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
            mint::Rendering::DxResource& cbTransform = resourcePool.getResource(_graphicDevice->getCommonCbTransformId());
            {
                cbTransform.bindToShader(mint::Rendering::DxShaderType::VertexShader, cbTransform.getRegisterIndex());
                cbTransform.bindToShader(mint::Rendering::DxShaderType::GeometryShader, cbTransform.getRegisterIndex());

                _cbTransformData._cbWorldMatrix.setIdentity();
                cbTransform.updateBuffer(&_cbTransformData, 1);
            }

            mint::Rendering::DxResource& sbMaterial = resourcePool.getResource(_graphicDevice->getCommonSbMaterialId());
            {
                sbMaterial.bindToShader(mint::Rendering::DxShaderType::PixelShader, sbMaterial.getRegisterIndex());

                sbMaterial.updateBuffer(&_sbMaterialDatas[0], _sbMaterialDatas.size());
            }
            
            shaderPool.bindShaderIfNot(mint::Rendering::DxShaderType::PixelShader, _psColorId);
            _lowLevelRendererLine.render(mint::Rendering::RenderingPrimitive::LineList);
            _lowLevelRendererLine.flush();

            shaderPool.bindShaderIfNot(mint::Rendering::DxShaderType::PixelShader, _psDefaultId);
            _lowLevelRendererMesh.render(mint::Rendering::RenderingPrimitive::TriangleList);
            _lowLevelRendererMesh.flush();

            _sbMaterialDatas.clear();
        }
    }
}

#include <stdafx.h>
#include <MintRendering/Include/InstantRenderer.h>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>
#include <MintRenderingBase/Include/MeshData.h>

#include <MintRendering/Include/MeshGenerator.h>


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
            const mint::Language::CppHlsl& cppHlsl = _graphicDevice->getCppHlslSteamData();
            const mint::Language::CppHlslTypeInfo& vsInputTypeInfo = cppHlsl.getTypeInfo(typeid(mint::RenderingBase::VS_INPUT));

            _vsDefaultId = shaderPool.pushVertexShader("Assets/Hlsl/", "VsDefault.hlsl", "main", &vsInputTypeInfo, "Assets/HlslBinary/");
            _psDefaultId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "PsDefault.hlsl", "main", mint::RenderingBase::DxShaderType::PixelShader, "Assets/HlslBinary/");

            mint::RenderingBase::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
            const mint::Language::CppHlslTypeInfo& cbTransformDataTypeInfo = _graphicDevice->getCppHlslConstantBuffers().getTypeInfo(typeid(_cbTransformData));
            _cbTransformId = resourcePool.pushConstantBuffer(reinterpret_cast<const byte*>(&_cbTransformData), sizeof(_cbTransformData), cbTransformDataTypeInfo.getRegisterIndex());
        }

        void InstantRenderer::drawLine(const mint::Float3& a, const mint::Float3& b) noexcept
        {
            auto& vertices = _lowLevelRendererLine.vertices();
            auto& indices = _lowLevelRendererLine.indices();

            mint::RenderingBase::VS_INPUT vertex;
            vertex._positionU.setXyz(a);
            vertices.push_back(vertex);

            vertex._positionU.setXyz(b);
            vertices.push_back(vertex);

            mint::RenderingBase::IndexElementType index = static_cast<mint::RenderingBase::IndexElementType>(indices.size());
            indices.push_back(index + 0);
            indices.push_back(index + 1);
        }

        void InstantRenderer::drawSphere(const mint::Float3& center, const float radius, const uint8 subdivisionIteration) noexcept
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

            _lowLevelRendererMesh.pushMesh(meshData);
        }

        void InstantRenderer::render() noexcept
        {
            mint::RenderingBase::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
            shaderPool.bindShaderIfNot(mint::RenderingBase::DxShaderType::VertexShader, _vsDefaultId);
            shaderPool.unbindShader(mint::RenderingBase::DxShaderType::GeometryShader);
            shaderPool.bindShaderIfNot(mint::RenderingBase::DxShaderType::PixelShader, _psDefaultId);

            mint::RenderingBase::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
            mint::RenderingBase::DxResource& cbTransform = resourcePool.getResource(_cbTransformId);
            {
                cbTransform.bindToShader(mint::RenderingBase::DxShaderType::VertexShader, cbTransform.getRegisterIndex());
                cbTransform.bindToShader(mint::RenderingBase::DxShaderType::GeometryShader, cbTransform.getRegisterIndex());
            }

            _cbTransformData._cbWorldMatrix.setIdentity();
            cbTransform.updateBuffer(reinterpret_cast<const byte*>(&_cbTransformData), 1);

            _lowLevelRendererLine.render(mint::RenderingBase::RenderingPrimitive::LineList);
            _lowLevelRendererLine.flush();

            _lowLevelRendererMesh.render(mint::RenderingBase::RenderingPrimitive::TriangleList);
            _lowLevelRendererMesh.flush();
        }
    }
}

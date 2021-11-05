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
        InstantRenderer::InstantRenderer(GraphicDevice* const graphicDevice)
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
            using namespace Language;
            const CppHlsl::Interpreter& interpreter = _graphicDevice->getCppHlslSteamData();
            const TypeMetaData<CppHlsl::TypeCustomData>& vsInputTypeMetaData = interpreter.getTypeMetaData(typeid(VS_INPUT));

            DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
            _vsDefaultId = shaderPool.pushVertexShader("Assets/Hlsl/", "VsDefault.hlsl", "main", &vsInputTypeMetaData, "Assets/HlslBinary/");
            _psDefaultId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "PsDefault.hlsl", "main", DxShaderType::PixelShader, "Assets/HlslBinary/");
            _psColorId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "PsColor.hlsl", "main", DxShaderType::PixelShader, "Assets/HlslBinary/");
        }

        void InstantRenderer::drawLine(const Float3& a, const Float3& b, const Color& color) noexcept
        {
            const uint32 materialId = _sbMaterialDatas.size();
            auto& vertices = _lowLevelRendererLine.vertices();
            auto& indices = _lowLevelRendererLine.indices();

            VS_INPUT vertex;
            vertex._materialId = materialId;

            vertex._positionU.setXyz(a);
            vertices.push_back(vertex);

            vertex._positionU.setXyz(b);
            vertices.push_back(vertex);

            IndexElementType index = static_cast<IndexElementType>(indices.size());
            indices.push_back(index + 0);
            indices.push_back(index + 1);

            SB_Material sbMaterialData;
            sbMaterialData._diffuseColor = color;
            _sbMaterialDatas.push_back(sbMaterialData);
        }

        void InstantRenderer::drawBox(const Srt& worldSrt, const Float3& extents, const Color& color) noexcept
        {
            MeshGenerator::BoxParam boxParam;
            boxParam._width = extents._x;
            boxParam._height = extents._y;
            boxParam._depth = extents._z;
            MeshData meshData;
            MeshGenerator::generateBox(boxParam, meshData);

            Float4x4 transformationMatrix;
            transformationMatrix.srtMatrix(worldSrt._scale, worldSrt._rotation, worldSrt._translation);
            MeshGenerator::transformMeshData(meshData, transformationMatrix);

            pushMeshWithMaterial(meshData, color);
        }

        void InstantRenderer::drawSphere(const Float3& center, const float radius, const uint8 subdivisionIteration, const Color& color) noexcept
        {
            MeshGenerator::GeoSpherePram geosphereParam;
            geosphereParam._radius = radius;
            geosphereParam._subdivisionIteration = subdivisionIteration;
            geosphereParam._smooth = true;
            MeshData meshData;
            MeshGenerator::generateGeoSphere(geosphereParam, meshData);
            
            Float4x4 transformationMatrix;
            transformationMatrix.setTranslation(center);
            MeshGenerator::transformMeshData(meshData, transformationMatrix);
            
            pushMeshWithMaterial(meshData, color);
        }

        void InstantRenderer::pushMeshWithMaterial(MeshData& meshData, const Color& diffuseColor) noexcept
        {
            const uint32 materialId = _sbMaterialDatas.size();
            MeshGenerator::setMaterialId(meshData, materialId);

            _lowLevelRendererMesh.pushMesh(meshData);

            SB_Material sbMaterialData;
            sbMaterialData._diffuseColor = diffuseColor;
            _sbMaterialDatas.push_back(sbMaterialData);
        }

        void InstantRenderer::render() noexcept
        {
            DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
            shaderPool.bindShaderIfNot(DxShaderType::VertexShader, _vsDefaultId);
            shaderPool.unbindShader(DxShaderType::GeometryShader);

            DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
            DxResource& cbTransform = resourcePool.getResource(_graphicDevice->getCommonCbTransformId());
            {
                cbTransform.bindToShader(DxShaderType::VertexShader, cbTransform.getRegisterIndex());
                cbTransform.bindToShader(DxShaderType::GeometryShader, cbTransform.getRegisterIndex());

                _cbTransformData._cbWorldMatrix.setIdentity();
                cbTransform.updateBuffer(&_cbTransformData, 1);
            }

            DxResource& sbMaterial = resourcePool.getResource(_graphicDevice->getCommonSbMaterialId());
            {
                sbMaterial.bindToShader(DxShaderType::PixelShader, sbMaterial.getRegisterIndex());

                sbMaterial.updateBuffer(&_sbMaterialDatas[0], _sbMaterialDatas.size());
            }
            
            shaderPool.bindShaderIfNot(DxShaderType::PixelShader, _psColorId);
            _lowLevelRendererLine.render(RenderingPrimitive::LineList);
            _lowLevelRendererLine.flush();

            shaderPool.bindShaderIfNot(DxShaderType::PixelShader, _psDefaultId);
            _lowLevelRendererMesh.render(RenderingPrimitive::TriangleList);
            _lowLevelRendererMesh.flush();

            _sbMaterialDatas.clear();
        }
    }
}

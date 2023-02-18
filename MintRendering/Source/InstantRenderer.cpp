#include <MintRendering/Include/InstantRenderer.h>

#include <MintMath/Include/Transform.h>
#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>
#include <MintRenderingBase/Include/MeshData.h>
#include <MintRendering/Include/MeshGenerator.h>

#include <Assets/Include/CppHlsl/CppHlslStructuredBuffers.h>


namespace mint
{
	namespace Rendering
	{
		InstantRenderer::InstantRenderer(GraphicDevice& graphicDevice)
			: _graphicDevice{ graphicDevice }
			, _lowLevelRendererLine{ graphicDevice }
			, _lowLevelRendererMesh{ graphicDevice }
		{
			initialize();
		}

		InstantRenderer::~InstantRenderer()
		{
			__noop;
		}

		void InstantRenderer::testDraw(const Float3& worldOffset) noexcept
		{
			drawBox(Transform(worldOffset), Float3(0.5f, 0.5f, 0.5f), Rendering::Color::kGreen);
			drawCone(Transform(worldOffset + Float3(2, 0, 0)), 0.5f, 1.0f, 4, Rendering::Color::kGreen);
			drawCylinder(Transform(worldOffset + Float3(3, 0, 0)), 0.5f, 1.0f, 1, Rendering::Color::kGreen);
			drawGeoSphere(worldOffset + Float3(4, 0, 0), 0.5f, 0, Rendering::Color::kGreen);
			drawSphere(worldOffset + Float3(5, 0, 0), 0.5f, 4, 4, Rendering::Color::kGreen);
			drawCapsule(Transform(worldOffset + Float3(1, 0, 0)), 0.25f, 1.0f, 3, Rendering::Color::kGreen);
		}

		void InstantRenderer::drawLine(const Float3& a, const Float3& b, const Color& color) noexcept
		{
			const uint32 materialID = _sbMaterialDatas.Size();
			auto& vertices = _lowLevelRendererLine.vertices();
			auto& indices = _lowLevelRendererLine.indices();

			VS_INPUT vertex;
			vertex._materialID = materialID;

			vertex._positionU.setXyz(a);
			vertices.PushBack(vertex);

			vertex._positionU.setXyz(b);
			vertices.PushBack(vertex);

			IndexElementType index = static_cast<IndexElementType>(indices.Size());
			indices.PushBack(index + 0);
			indices.PushBack(index + 1);

			SB_Material sbMaterialData;
			sbMaterialData._diffuseColor = color;
			_sbMaterialDatas.PushBack(sbMaterialData);
		}

		void InstantRenderer::drawBox(const Transform& worldTransform, const Float3& extents, const Color& color) noexcept
		{
			MeshGenerator::BoxParam boxParam;
			boxParam._width = extents._x;
			boxParam._height = extents._y;
			boxParam._depth = extents._z;
			MeshData meshData;
			MeshGenerator::generateBox(boxParam, meshData);
			MeshGenerator::transformMeshData(meshData, worldTransform.toMatrix());

			pushMeshWithMaterial(meshData, color);
		}

		void InstantRenderer::drawCone(const Transform& worldTransform, const float radius, const float height, const uint8 detail, const Color& color) noexcept
		{
			MeshGenerator::ConeParam coneParam;
			coneParam._radius = radius;
			coneParam._height = height;
			coneParam._sideCount = detail;
			coneParam._smooth = true;
			MeshData meshData;
			MeshGenerator::generateCone(coneParam, meshData);
			MeshGenerator::transformMeshData(meshData, worldTransform.toMatrix());

			pushMeshWithMaterial(meshData, color);
		}

		void InstantRenderer::drawCylinder(const Transform& worldTransform, const float radius, const float height, const uint8 subdivisionIteration, const Color& color) noexcept
		{
			MeshGenerator::CylinderParam cylinderParam;
			cylinderParam._height = height;
			cylinderParam._radius = radius;
			cylinderParam._sideCount = Math::pow2_ui32(2 + subdivisionIteration);
			cylinderParam._smooth = true;
			MeshData meshData;
			MeshGenerator::generateCylinder(cylinderParam, meshData);
			MeshGenerator::transformMeshData(meshData, worldTransform.toMatrix());

			pushMeshWithMaterial(meshData, color);
		}

		void InstantRenderer::drawSphere(const Float3& center, const float radius, const uint8 polarDetail, const uint8 azimuthalDetail, const Color& color) noexcept
		{
			MeshGenerator::SphereParam sphereParam;
			sphereParam._radius = radius;
			sphereParam._polarDetail = polarDetail;
			sphereParam._azimuthalDetail = azimuthalDetail;
			sphereParam._smooth = true;
			MeshData meshData;
			MeshGenerator::generateSphere(sphereParam, meshData);

			Float4x4 transformationMatrix;
			transformationMatrix.setTranslation(center);
			MeshGenerator::transformMeshData(meshData, transformationMatrix);

			pushMeshWithMaterial(meshData, color);
		}

		void InstantRenderer::drawGeoSphere(const Float3& center, const float radius, const uint8 subdivisionIteration, const Color& color) noexcept
		{
			MeshGenerator::GeoSphereParam geosphereParam;
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

		void InstantRenderer::drawCapsule(const Transform& worldTransform, const float sphereRadius, const float cylinderHeight, const uint8 subdivisionIteration, const Color& color) noexcept
		{
			MeshGenerator::CapsulePram capsulePram;
			capsulePram._sphereRadius = sphereRadius;
			capsulePram._cylinderHeight = cylinderHeight;
			capsulePram._subdivisionIteration = subdivisionIteration;
			capsulePram._smooth = true;
			MeshData meshData;
			MeshGenerator::generateCapsule(capsulePram, meshData);
			MeshGenerator::transformMeshData(meshData, worldTransform.toMatrix());

			pushMeshWithMaterial(meshData, color);
		}

		void InstantRenderer::initialize() noexcept
		{
			using namespace Language;

			DxShaderPool& shaderPool = _graphicDevice.getShaderPool();
			_vsDefaultID = shaderPool.addShader(Path::MakeIncludeAssetPath("Hlsl/"), "VsDefault.hlsl", "main", GraphicShaderType::VertexShader, Path::MakeIncludeAssetPath("HlslBinary/"));

			const CppHlsl::Interpreter& interpreter = _graphicDevice.getCppHlslSteamData();
			const TypeMetaData<CppHlsl::TypeCustomData>& vsInputTypeMetaData = interpreter.GetTypeMetaData(typeid(VS_INPUT));
			_inputLayoutDefaultID = shaderPool.addInputLayout(_vsDefaultID, vsInputTypeMetaData);

			_psDefaultID = shaderPool.addShader(Path::MakeIncludeAssetPath("Hlsl/"), "PsDefault.hlsl", "main", GraphicShaderType::PixelShader, Path::MakeIncludeAssetPath("HlslBinary/"));
			_psColorID = shaderPool.addShader(Path::MakeIncludeAssetPath("Hlsl/"), "PsColor.hlsl", "main", GraphicShaderType::PixelShader, Path::MakeIncludeAssetPath("HlslBinary/"));
		}

		void InstantRenderer::pushMeshWithMaterial(MeshData& meshData, const Color& diffuseColor) noexcept
		{
			const uint32 materialID = _sbMaterialDatas.Size();
			MeshGenerator::setMaterialID(meshData, materialID);

			_lowLevelRendererMesh.pushMesh(meshData);

			SB_Material sbMaterialData;
			sbMaterialData._diffuseColor = diffuseColor;
			_sbMaterialDatas.PushBack(sbMaterialData);
		}

		void InstantRenderer::render() noexcept
		{
			DxShaderPool& shaderPool = _graphicDevice.getShaderPool();
			shaderPool.bindInputLayoutIfNot(_inputLayoutDefaultID);
			shaderPool.bindShaderIfNot(GraphicShaderType::VertexShader, _vsDefaultID);
			shaderPool.unbindShader(GraphicShaderType::GeometryShader);

			DxResourcePool& resourcePool = _graphicDevice.getResourcePool();
			DxResource& cbTransform = resourcePool.getResource(_graphicDevice.getCommonCbTransformID());
			{
				cbTransform.bindToShader(GraphicShaderType::VertexShader, cbTransform.getRegisterIndex());
				cbTransform.bindToShader(GraphicShaderType::GeometryShader, cbTransform.getRegisterIndex());

				_cbTransformData._cbWorldMatrix.setIdentity();
				cbTransform.updateBuffer(&_cbTransformData, 1);
			}

			DxResource& sbMaterial = resourcePool.getResource(_graphicDevice.getCommonSBMaterialID());
			{
				sbMaterial.bindToShader(GraphicShaderType::PixelShader, sbMaterial.getRegisterIndex());

				sbMaterial.updateBuffer(&_sbMaterialDatas[0], _sbMaterialDatas.Size());
			}

			shaderPool.bindShaderIfNot(GraphicShaderType::PixelShader, _psColorID);
			_lowLevelRendererLine.render(RenderingPrimitive::LineList);
			_lowLevelRendererLine.flush();

			shaderPool.bindShaderIfNot(GraphicShaderType::PixelShader, _psDefaultID);
			_lowLevelRendererMesh.render(RenderingPrimitive::TriangleList);
			_lowLevelRendererMesh.flush();

			_sbMaterialDatas.Clear();
		}
	}
}

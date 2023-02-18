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
			Initialize();
		}

		InstantRenderer::~InstantRenderer()
		{
			__noop;
		}

		void InstantRenderer::TestDraw(const Float3& worldOffset) noexcept
		{
			DrawBox(Transform(worldOffset), Float3(0.5f, 0.5f, 0.5f), Rendering::Color::kGreen);
			DrawCone(Transform(worldOffset + Float3(2, 0, 0)), 0.5f, 1.0f, 4, Rendering::Color::kGreen);
			DrawCylinder(Transform(worldOffset + Float3(3, 0, 0)), 0.5f, 1.0f, 1, Rendering::Color::kGreen);
			DrawGeoSphere(worldOffset + Float3(4, 0, 0), 0.5f, 0, Rendering::Color::kGreen);
			DrawSphere(worldOffset + Float3(5, 0, 0), 0.5f, 4, 4, Rendering::Color::kGreen);
			DrawCapsule(Transform(worldOffset + Float3(1, 0, 0)), 0.25f, 1.0f, 3, Rendering::Color::kGreen);
		}

		void InstantRenderer::DrawLine(const Float3& a, const Float3& b, const Color& color) noexcept
		{
			const uint32 materialID = _sbMaterialDatas.Size();
			auto& vertices = _lowLevelRendererLine.Vertices();
			auto& indices = _lowLevelRendererLine.Indices();

			VS_INPUT vertex;
			vertex._materialID = materialID;

			vertex._positionU.SetXYZ(a);
			vertices.PushBack(vertex);

			vertex._positionU.SetXYZ(b);
			vertices.PushBack(vertex);

			IndexElementType index = static_cast<IndexElementType>(indices.Size());
			indices.PushBack(index + 0);
			indices.PushBack(index + 1);

			SB_Material sbMaterialData;
			sbMaterialData._diffuseColor = color;
			_sbMaterialDatas.PushBack(sbMaterialData);
		}

		void InstantRenderer::DrawBox(const Transform& worldTransform, const Float3& extents, const Color& color) noexcept
		{
			MeshGenerator::BoxParam boxParam;
			boxParam._width = extents._x;
			boxParam._height = extents._y;
			boxParam._depth = extents._z;
			MeshData meshData;
			MeshGenerator::GenerateBox(boxParam, meshData);
			MeshGenerator::TransformMeshData(meshData, worldTransform.ToMatrix());

			PushMeshWithMaterial(meshData, color);
		}

		void InstantRenderer::DrawCone(const Transform& worldTransform, const float radius, const float height, const uint8 detail, const Color& color) noexcept
		{
			MeshGenerator::ConeParam coneParam;
			coneParam._radius = radius;
			coneParam._height = height;
			coneParam._sideCount = detail;
			coneParam._smooth = true;
			MeshData meshData;
			MeshGenerator::GenerateCone(coneParam, meshData);
			MeshGenerator::TransformMeshData(meshData, worldTransform.ToMatrix());

			PushMeshWithMaterial(meshData, color);
		}

		void InstantRenderer::DrawCylinder(const Transform& worldTransform, const float radius, const float height, const uint8 subdivisionIteration, const Color& color) noexcept
		{
			MeshGenerator::CylinderParam cylinderParam;
			cylinderParam._height = height;
			cylinderParam._radius = radius;
			cylinderParam._sideCount = Math::Pow2_Uint32(2 + subdivisionIteration);
			cylinderParam._smooth = true;
			MeshData meshData;
			MeshGenerator::GenerateCylinder(cylinderParam, meshData);
			MeshGenerator::TransformMeshData(meshData, worldTransform.ToMatrix());

			PushMeshWithMaterial(meshData, color);
		}

		void InstantRenderer::DrawSphere(const Float3& center, const float radius, const uint8 polarDetail, const uint8 azimuthalDetail, const Color& color) noexcept
		{
			MeshGenerator::SphereParam sphereParam;
			sphereParam._radius = radius;
			sphereParam._polarDetail = polarDetail;
			sphereParam._azimuthalDetail = azimuthalDetail;
			sphereParam._smooth = true;
			MeshData meshData;
			MeshGenerator::GenerateSphere(sphereParam, meshData);

			Float4x4 transformationMatrix;
			transformationMatrix.SetTranslation(center);
			MeshGenerator::TransformMeshData(meshData, transformationMatrix);

			PushMeshWithMaterial(meshData, color);
		}

		void InstantRenderer::DrawGeoSphere(const Float3& center, const float radius, const uint8 subdivisionIteration, const Color& color) noexcept
		{
			MeshGenerator::GeoSphereParam geosphereParam;
			geosphereParam._radius = radius;
			geosphereParam._subdivisionIteration = subdivisionIteration;
			geosphereParam._smooth = true;
			MeshData meshData;
			MeshGenerator::GenerateGeoSphere(geosphereParam, meshData);

			Float4x4 transformationMatrix;
			transformationMatrix.SetTranslation(center);
			MeshGenerator::TransformMeshData(meshData, transformationMatrix);

			PushMeshWithMaterial(meshData, color);
		}

		void InstantRenderer::DrawCapsule(const Transform& worldTransform, const float sphereRadius, const float cylinderHeight, const uint8 subdivisionIteration, const Color& color) noexcept
		{
			MeshGenerator::CapsulePram capsulePram;
			capsulePram._sphereRadius = sphereRadius;
			capsulePram._cylinderHeight = cylinderHeight;
			capsulePram._subdivisionIteration = subdivisionIteration;
			capsulePram._smooth = true;
			MeshData meshData;
			MeshGenerator::GenerateCapsule(capsulePram, meshData);
			MeshGenerator::TransformMeshData(meshData, worldTransform.ToMatrix());

			PushMeshWithMaterial(meshData, color);
		}

		void InstantRenderer::Initialize() noexcept
		{
			using namespace Language;

			DxShaderPool& shaderPool = _graphicDevice.GetShaderPool();
			_vsDefaultID = shaderPool.AddShader(Path::MakeIncludeAssetPath("Hlsl/"), "VsDefault.hlsl", "main", GraphicShaderType::VertexShader, Path::MakeIncludeAssetPath("HlslBinary/"));

			const CppHlsl::Interpreter& interpreter = _graphicDevice.GetCppHlslSteamData();
			const TypeMetaData<CppHlsl::TypeCustomData>& vsInputTypeMetaData = interpreter.GetTypeMetaData(typeid(VS_INPUT));
			_inputLayoutDefaultID = shaderPool.AddInputLayout(_vsDefaultID, vsInputTypeMetaData);

			_psDefaultID = shaderPool.AddShader(Path::MakeIncludeAssetPath("Hlsl/"), "PsDefault.hlsl", "main", GraphicShaderType::PixelShader, Path::MakeIncludeAssetPath("HlslBinary/"));
			_psColorID = shaderPool.AddShader(Path::MakeIncludeAssetPath("Hlsl/"), "PsColor.hlsl", "main", GraphicShaderType::PixelShader, Path::MakeIncludeAssetPath("HlslBinary/"));
		}

		void InstantRenderer::PushMeshWithMaterial(MeshData& meshData, const Color& diffuseColor) noexcept
		{
			const uint32 materialID = _sbMaterialDatas.Size();
			MeshGenerator::SetMaterialID(meshData, materialID);

			_lowLevelRendererMesh.PushMesh(meshData);

			SB_Material sbMaterialData;
			sbMaterialData._diffuseColor = diffuseColor;
			_sbMaterialDatas.PushBack(sbMaterialData);
		}

		void InstantRenderer::Render() noexcept
		{
			DxShaderPool& shaderPool = _graphicDevice.GetShaderPool();
			shaderPool.BindInputLayoutIfNot(_inputLayoutDefaultID);
			shaderPool.BindShaderIfNot(GraphicShaderType::VertexShader, _vsDefaultID);
			shaderPool.UnbindShader(GraphicShaderType::GeometryShader);

			DxResourcePool& resourcePool = _graphicDevice.GetResourcePool();
			DxResource& cbTransform = resourcePool.GetResource(_graphicDevice.GetCommonCBTransformID());
			{
				cbTransform.BindToShader(GraphicShaderType::VertexShader, cbTransform.GetRegisterIndex());
				cbTransform.BindToShader(GraphicShaderType::GeometryShader, cbTransform.GetRegisterIndex());

				_cbTransformData._cbWorldMatrix.SetIdentity();
				cbTransform.UpdateBuffer(&_cbTransformData, 1);
			}

			DxResource& sbMaterial = resourcePool.GetResource(_graphicDevice.GetCommonSBMaterialID());
			{
				sbMaterial.BindToShader(GraphicShaderType::PixelShader, sbMaterial.GetRegisterIndex());

				sbMaterial.UpdateBuffer(&_sbMaterialDatas[0], _sbMaterialDatas.Size());
			}

			shaderPool.BindShaderIfNot(GraphicShaderType::PixelShader, _psColorID);
			_lowLevelRendererLine.Render(RenderingPrimitive::LineList);
			_lowLevelRendererLine.Flush();

			shaderPool.BindShaderIfNot(GraphicShaderType::PixelShader, _psDefaultID);
			_lowLevelRendererMesh.Render(RenderingPrimitive::TriangleList);
			_lowLevelRendererMesh.Flush();

			_sbMaterialDatas.Clear();
		}
	}
}

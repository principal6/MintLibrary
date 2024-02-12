#include <MintApp/Include/ObjectRenderer.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>

#include <MintApp/Include/Object.h>
#include <MintApp/Include/ObjectPool.hpp>
#include <MintApp/Include/MeshComponent.h>


namespace mint
{
	ObjectRenderer::ObjectRenderer(Rendering::GraphicDevice& graphicDevice)
		: _graphicDevice{ graphicDevice }
		, _lowLevelRenderer{ graphicDevice }
	{
		Initialize();
	}

	ObjectRenderer::~ObjectRenderer()
	{
		__noop;
	}

	void ObjectRenderer::Render(const ObjectPool& objectPool) noexcept
	{
		using namespace Rendering;

		ShaderPool& shaderPool = _graphicDevice.GetShaderPool();
		GraphicResourcePool& resourcePool = _graphicDevice.GetResourcePool();
		const Vector<ObjectComponent*>& meshComponents = objectPool.GetMeshComponents();
		if (meshComponents.IsEmpty() == false)
		{
			shaderPool.BindInputLayoutIfNot(_inputLayoutDefaultID);
			shaderPool.BindShaderIfNot(GraphicShaderType::VertexShader, _vsDefaultID);

			GraphicResource& cbTransform = resourcePool.GetResource(_graphicDevice.GetCommonCBTransformID());
			cbTransform.BindToShader(GraphicShaderType::VertexShader, cbTransform.GetRegisterIndex());
			cbTransform.BindToShader(GraphicShaderType::GeometryShader, cbTransform.GetRegisterIndex());

			GraphicResource& sbMaterial = resourcePool.GetResource(_graphicDevice.GetCommonSBMaterialID());
			sbMaterial.BindToShader(GraphicShaderType::PixelShader, sbMaterial.GetRegisterIndex());

			SB_Material sbMaterialData;
			const uint32 meshComponentCount = meshComponents.Size();
			for (uint32 i = 0; i < meshComponentCount; ++i)
			{
				const MeshComponent* const meshComponent = static_cast<MeshComponent*>(meshComponents[i]);

				_cbTransformData._cbWorldMatrix = meshComponent->GetOwnerObject()->GetObjectTransformMatrix();
				cbTransform.UpdateBuffer(&_cbTransformData, 1);

				_lowLevelRenderer.PushMesh(meshComponent->GetMeshData());

				sbMaterialData._diffuseColor = Color::kBlue;
				sbMaterial.UpdateBuffer(&sbMaterialData, 1);

				shaderPool.BindShaderIfNot(GraphicShaderType::PixelShader, _psDefaultID);
				shaderPool.UnbindShader(GraphicShaderType::GeometryShader);
				_lowLevelRenderer.Render(RenderingPrimitive::TriangleList);

				if (meshComponent->ShouldDrawNormals() == true)
				{
					shaderPool.BindShaderIfNot(GraphicShaderType::GeometryShader, _gsNormalID);
					shaderPool.BindShaderIfNot(GraphicShaderType::PixelShader, _psTexCoordAsColorID);
					_lowLevelRenderer.Render(RenderingPrimitive::LineList);
				}

				if (meshComponent->ShouldDrawEdges() == true)
				{
					shaderPool.BindShaderIfNot(GraphicShaderType::GeometryShader, _gsTriangleEdgeID);
					shaderPool.BindShaderIfNot(GraphicShaderType::PixelShader, _psTexCoordAsColorID);
					_lowLevelRenderer.Render(RenderingPrimitive::TriangleList);
				}

				_lowLevelRenderer.Flush();
			}

			shaderPool.UnbindShader(GraphicShaderType::VertexShader);
			shaderPool.UnbindShader(GraphicShaderType::GeometryShader);
			shaderPool.UnbindShader(GraphicShaderType::PixelShader);

			cbTransform.UnbindFromShader();
			sbMaterial.UnbindFromShader();
		}

		const Vector<ObjectComponent*>& mesh2DComponents = objectPool.GetMesh2DComponents();
		if (mesh2DComponents.IsEmpty() == false)
		{
			ShapeRendererContext& shapeRendererContext = _graphicDevice.GetShapeRendererContext();
			const uint32 mesh2DComponentCount = mesh2DComponents.Size();
			for (uint32 i = 0; i < mesh2DComponentCount; ++i)
			{
				const Mesh2DComponent* const mesh2DComponent = static_cast<Mesh2DComponent*>(mesh2DComponents[i]);
				Object* const object = mesh2DComponent->GetOwnerObject();
				shapeRendererContext.SetPosition(Float4(object->GetObjectTransform()._translation));
				shapeRendererContext.AddShape(mesh2DComponent->GetShape());
			}
			shapeRendererContext.Render();
		}
	}

	void ObjectRenderer::Initialize() noexcept
	{
		using namespace Rendering;
		using namespace Language;

		ShaderPool& shaderPool = _graphicDevice.GetShaderPool();
		_vsDefaultID = shaderPool.AddShader(Path::MakeIncludeAssetPath("Hlsl/"), "VsDefault.hlsl", "main", GraphicShaderType::VertexShader, Path::MakeIncludeAssetPath("HlslBinary/"));

		const CppHlsl::Interpreter& interpreter = _graphicDevice.GetCppHlslSteamData();
		const TypeMetaData<CppHlsl::TypeCustomData>& vsInputTypeMetaData = interpreter.GetTypeMetaData(typeid(VS_INPUT));
		_inputLayoutDefaultID = shaderPool.AddInputLayout(_vsDefaultID, vsInputTypeMetaData);

		_psDefaultID = shaderPool.AddShader(Path::MakeIncludeAssetPath("Hlsl/"), "PsDefault.hlsl", "main", GraphicShaderType::PixelShader, Path::MakeIncludeAssetPath("HlslBinary/"));

		_gsNormalID = shaderPool.AddShader(Path::MakeIncludeAssetPath("Hlsl/"), "GsNormal.hlsl", "main", GraphicShaderType::GeometryShader, Path::MakeIncludeAssetPath("HlslBinary/"));
		_gsTriangleEdgeID = shaderPool.AddShader(Path::MakeIncludeAssetPath("Hlsl/"), "GsTriangleEdge.hlsl", "main", GraphicShaderType::GeometryShader, Path::MakeIncludeAssetPath("HlslBinary/"));
		_psTexCoordAsColorID = shaderPool.AddShader(Path::MakeIncludeAssetPath("Hlsl/"), "PsTexCoordAsColor.hlsl", "main", GraphicShaderType::PixelShader, Path::MakeIncludeAssetPath("HlslBinary/"));
	}
}

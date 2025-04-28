#include <MintApp/Include/SceneObjectRenderer.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicsDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>

#include <MintApp/Include/SceneObject.h>
#include <MintApp/Include/SceneObjectPool.hpp>
#include <MintApp/Include/MeshComponent.h>


namespace mint
{
	SceneObjectRenderer::SceneObjectRenderer(Rendering::GraphicsDevice& graphicsDevice)
		: _graphicsDevice{ graphicsDevice }
		, _lowLevelRenderer{ graphicsDevice }
	{
		Initialize();
	}

	SceneObjectRenderer::~SceneObjectRenderer()
	{
		__noop;
	}

	void SceneObjectRenderer::Render(const SceneObjectPool& sceneObjectPool) noexcept
	{
		using namespace Rendering;

		ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
		GraphicsResourcePool& resourcePool = _graphicsDevice.GetResourcePool();
		const Vector<SceneObjectComponent*>& meshComponents = sceneObjectPool.GetMeshComponents();
		if (meshComponents.IsEmpty() == false)
		{
			shaderPool.BindInputLayoutIfNot(_inputLayoutDefaultID);
			shaderPool.BindShaderIfNot(GraphicsShaderType::VertexShader, _vsDefaultID);

			GraphicsResource& cbTransform = resourcePool.GetResource(_graphicsDevice.GetCommonCBTransformID());
			cbTransform.BindToShader(GraphicsShaderType::VertexShader, cbTransform.GetRegisterIndex());
			cbTransform.BindToShader(GraphicsShaderType::GeometryShader, cbTransform.GetRegisterIndex());

			GraphicsResource& sbMaterial = resourcePool.GetResource(_graphicsDevice.GetCommonSBMaterialID());
			sbMaterial.BindToShader(GraphicsShaderType::PixelShader, sbMaterial.GetRegisterIndex());

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

				shaderPool.BindShaderIfNot(GraphicsShaderType::PixelShader, _psDefaultID);
				shaderPool.UnbindShader(GraphicsShaderType::GeometryShader);
				_lowLevelRenderer.Render(RenderingPrimitive::TriangleList);

				if (meshComponent->ShouldDrawNormals() == true)
				{
					shaderPool.BindShaderIfNot(GraphicsShaderType::GeometryShader, _gsNormalID);
					shaderPool.BindShaderIfNot(GraphicsShaderType::PixelShader, _psTexCoordAsColorID);
					_lowLevelRenderer.Render(RenderingPrimitive::LineList);
				}

				if (meshComponent->ShouldDrawEdges() == true)
				{
					shaderPool.BindShaderIfNot(GraphicsShaderType::GeometryShader, _gsTriangleEdgeID);
					shaderPool.BindShaderIfNot(GraphicsShaderType::PixelShader, _psTexCoordAsColorID);
					_lowLevelRenderer.Render(RenderingPrimitive::TriangleList);
				}

				_lowLevelRenderer.Flush();
			}

			shaderPool.UnbindShader(GraphicsShaderType::VertexShader);
			shaderPool.UnbindShader(GraphicsShaderType::GeometryShader);
			shaderPool.UnbindShader(GraphicsShaderType::PixelShader);

			cbTransform.UnbindFromShader();
			sbMaterial.UnbindFromShader();
		}

		const Vector<SceneObjectComponent*>& mesh2DComponents = sceneObjectPool.GetMesh2DComponents();
		if (mesh2DComponents.IsEmpty() == false)
		{
			ShapeRenderer& shapeRenderer = _graphicsDevice.GetShapeRenderer();
			const uint32 mesh2DComponentCount = mesh2DComponents.Size();
			for (uint32 i = 0; i < mesh2DComponentCount; ++i)
			{
				const Mesh2DComponent* const mesh2DComponent = static_cast<Mesh2DComponent*>(mesh2DComponents[i]);
				SceneObject* const sceneObject = mesh2DComponent->GetOwnerObject();
				shapeRenderer.AddShape(mesh2DComponent->GetShape(), sceneObject->GetObjectTransform());
			}
			shapeRenderer.Render();
		}
	}

	void SceneObjectRenderer::Initialize() noexcept
	{
		using namespace Rendering;
		using namespace Language;

		ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
		_vsDefaultID = shaderPool.AddShader(Path::MakeIncludeAssetPath("Hlsl/"), "VsDefault.hlsl", "main", GraphicsShaderType::VertexShader, Path::MakeIncludeAssetPath("HlslBinary/"));

		const CppHlsl::Interpreter& interpreter = _graphicsDevice.GetCppHlslSteamData();
		const TypeMetaData<CppHlsl::TypeCustomData>& vsInputTypeMetaData = interpreter.GetTypeMetaData(typeid(VS_INPUT));
		_inputLayoutDefaultID = shaderPool.AddInputLayout(_vsDefaultID, vsInputTypeMetaData);

		_psDefaultID = shaderPool.AddShader(Path::MakeIncludeAssetPath("Hlsl/"), "PsDefault.hlsl", "main", GraphicsShaderType::PixelShader, Path::MakeIncludeAssetPath("HlslBinary/"));

		_gsNormalID = shaderPool.AddShader(Path::MakeIncludeAssetPath("Hlsl/"), "GsNormal.hlsl", "main", GraphicsShaderType::GeometryShader, Path::MakeIncludeAssetPath("HlslBinary/"));
		_gsTriangleEdgeID = shaderPool.AddShader(Path::MakeIncludeAssetPath("Hlsl/"), "GsTriangleEdge.hlsl", "main", GraphicsShaderType::GeometryShader, Path::MakeIncludeAssetPath("HlslBinary/"));
		_psTexCoordAsColorID = shaderPool.AddShader(Path::MakeIncludeAssetPath("Hlsl/"), "PsTexCoordAsColor.hlsl", "main", GraphicsShaderType::PixelShader, Path::MakeIncludeAssetPath("HlslBinary/"));
	}
}

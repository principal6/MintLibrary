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
		ShaderPipelinePool& shaderPipelinePool = _graphicsDevice.GetShaderPipelinePool();
		GraphicsResourcePool& resourcePool = _graphicsDevice.GetResourcePool();

		SceneObjectComponentPool<TransformComponent>& transformComponentPool = SceneObjectComponentPool<TransformComponent>::GetInstance();
		SceneObjectComponentPool<MeshComponent>& meshComponentPool = SceneObjectComponentPool<MeshComponent>::GetInstance();
		ContiguousHashMap<SceneObject, MeshComponent>& meshComponentMap = meshComponentPool.GetComponentMap();
		if (meshComponentMap.IsEmpty() == false)
		{
			ShaderPipeline& shaderPipelineDefault = shaderPipelinePool.AccessShaderPipeline(_shaderPipelineDefaultID);
			ShaderPipeline& shaderPipelineDrawEdges = shaderPipelinePool.AccessShaderPipeline(_shaderPipelineDrawEdgesID);
			ShaderPipeline& shaderPipelineDrawNormals = shaderPipelinePool.AccessShaderPipeline(_shaderPipelineDrawNormalsID);

			GraphicsResource& cbTransform = resourcePool.GetResource(_graphicsDevice.GetCommonCBTransformID());
			cbTransform.BindToShader(GraphicsShaderType::VertexShader, cbTransform.GetRegisterIndex());
			cbTransform.BindToShader(GraphicsShaderType::GeometryShader, cbTransform.GetRegisterIndex());

			GraphicsResource& sbMaterial = resourcePool.GetResource(_graphicsDevice.GetCommonSBMaterialID());
			sbMaterial.BindToShader(GraphicsShaderType::PixelShader, sbMaterial.GetRegisterIndex());

			SB_Material sbMaterialData;
			for (auto iter = meshComponentMap.begin(); iter != meshComponentMap.end(); ++iter)
			{
				const SceneObject& sceneObject = iter.GetKey();
				MeshComponent& meshComponent = iter.GetValue();
				TransformComponent* transformComponent = transformComponentPool.GetComponent(sceneObject);
				_cbTransformData._cbWorldMatrix = transformComponent->_transform.ToMatrix();
				cbTransform.UpdateBuffer(&_cbTransformData, 1);

				_lowLevelRenderer.PushMesh(meshComponent._meshData);

				sbMaterialData._diffuseColor = Color::kBlue;
				sbMaterial.UpdateBuffer(&sbMaterialData, 1);

				shaderPipelineDefault.BindShaderPipeline();
				_lowLevelRenderer.Render(_graphicsDevice, RenderingPrimitive::TriangleList);

				if (meshComponent._shouldDrawNormals == true)
				{
					shaderPipelineDrawNormals.BindShaderPipeline();
					_lowLevelRenderer.Render(_graphicsDevice, RenderingPrimitive::LineList);
				}
				
				if (meshComponent._shouldDrawEdges == true)
				{
					shaderPipelineDrawEdges.BindShaderPipeline();
					_lowLevelRenderer.Render(_graphicsDevice, RenderingPrimitive::TriangleList);
				}

				_lowLevelRenderer.Flush();
			}

			shaderPool.UnbindShader(GraphicsShaderType::VertexShader);
			shaderPool.UnbindShader(GraphicsShaderType::GeometryShader);
			shaderPool.UnbindShader(GraphicsShaderType::PixelShader);

			cbTransform.UnbindFromShader();
			sbMaterial.UnbindFromShader();
		}

		SceneObjectComponentPool<Mesh2DComponent>& mesh2DComponentPool = SceneObjectComponentPool<Mesh2DComponent>::GetInstance();
		ContiguousHashMap<SceneObject, Mesh2DComponent>& mesh2DComponentMap = mesh2DComponentPool.GetComponentMap();
		if (mesh2DComponentMap.IsEmpty() == false)
		{
			ShapeRenderer& shapeRenderer = _graphicsDevice.GetShapeRenderer();
			for (auto iter = mesh2DComponentMap.begin(); iter != mesh2DComponentMap.end(); ++iter)
			{
				const SceneObject& sceneObject = iter.GetKey();
				Mesh2DComponent& mesh2DComponent = iter.GetValue();
				TransformComponent* transformComponent = transformComponentPool.GetComponent(sceneObject);
				shapeRenderer.AddShape(mesh2DComponent._shape, transformComponent->_transform);
			}
			shapeRenderer.Render();
		}
	}

	void SceneObjectRenderer::Initialize() noexcept
	{
		using namespace Rendering;
		using namespace Language;

		ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
		Rendering::GraphicsObjectID inputLayoutDefaultID;
		Rendering::GraphicsObjectID vsDefaultID;
		Rendering::GraphicsObjectID gsNormalID;
		Rendering::GraphicsObjectID gsTriangleEdgeID;
		Rendering::GraphicsObjectID psDefaultID;
		Rendering::GraphicsObjectID psTexCoordAsColorID;
		vsDefaultID = shaderPool.AddShader(Path::MakeIncludeAssetPath("Hlsl/"), "VsDefault.hlsl", "main", GraphicsShaderType::VertexShader, Path::MakeIncludeAssetPath("HlslBinary/"));

		const CppHlsl::Interpreter& interpreter = _graphicsDevice.GetCppHlslSteamData();
		const TypeMetaData<CppHlsl::TypeCustomData>& vsInputTypeMetaData = interpreter.GetTypeMetaData(typeid(VS_INPUT));
		inputLayoutDefaultID = shaderPool.AddInputLayout(vsDefaultID, vsInputTypeMetaData);

		psDefaultID = shaderPool.AddShader(Path::MakeIncludeAssetPath("Hlsl/"), "PsDefault.hlsl", "main", GraphicsShaderType::PixelShader, Path::MakeIncludeAssetPath("HlslBinary/"));

		gsNormalID = shaderPool.AddShader(Path::MakeIncludeAssetPath("Hlsl/"), "GsNormal.hlsl", "main", GraphicsShaderType::GeometryShader, Path::MakeIncludeAssetPath("HlslBinary/"));
		gsTriangleEdgeID = shaderPool.AddShader(Path::MakeIncludeAssetPath("Hlsl/"), "GsTriangleEdge.hlsl", "main", GraphicsShaderType::GeometryShader, Path::MakeIncludeAssetPath("HlslBinary/"));
		psTexCoordAsColorID = shaderPool.AddShader(Path::MakeIncludeAssetPath("Hlsl/"), "PsTexCoordAsColor.hlsl", "main", GraphicsShaderType::PixelShader, Path::MakeIncludeAssetPath("HlslBinary/"));

		ShaderPipelinePool& shaderPipelinePool = _graphicsDevice.GetShaderPipelinePool();
		_shaderPipelineDefaultID = shaderPipelinePool.CreateShaderPipeline(_graphicsDevice);
		_shaderPipelineDrawNormalsID = shaderPipelinePool.CreateShaderPipeline(_graphicsDevice);
		_shaderPipelineDrawEdgesID = shaderPipelinePool.CreateShaderPipeline(_graphicsDevice);
		{
			ShaderPipeline& shaderPipelineDefault = shaderPipelinePool.AccessShaderPipeline(_shaderPipelineDefaultID);
			shaderPipelineDefault.SetInputLayout(inputLayoutDefaultID);
			shaderPipelineDefault.SetVertexShader(vsDefaultID);
			shaderPipelineDefault.SetPixelShader(psDefaultID);
		}
		{
			ShaderPipeline& shaderPipelineDrawNormals = shaderPipelinePool.AccessShaderPipeline(_shaderPipelineDrawNormalsID);
			shaderPipelineDrawNormals.SetInputLayout(inputLayoutDefaultID);
			shaderPipelineDrawNormals.SetVertexShader(vsDefaultID);
			shaderPipelineDrawNormals.SetGeometryShader(gsNormalID);
			shaderPipelineDrawNormals.SetPixelShader(psTexCoordAsColorID);
		}
		{
			ShaderPipeline& shaderPipelineDrawEdges = shaderPipelinePool.AccessShaderPipeline(_shaderPipelineDrawEdgesID);
			shaderPipelineDrawEdges.SetInputLayout(inputLayoutDefaultID);
			shaderPipelineDrawEdges.SetVertexShader(vsDefaultID);
			shaderPipelineDrawEdges.SetGeometryShader(gsTriangleEdgeID);
			shaderPipelineDrawEdges.SetPixelShader(psTexCoordAsColorID);
		}
	}
}

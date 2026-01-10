#include <MintApp/Include/SceneObjectRenderer.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRendering/Include/GraphicsDevice.h>
#include <MintRendering/Include/LowLevelRenderer.hpp>
#include <MintRendering/Include/ShapeRenderer.h>

#include <MintApp/Include/SceneObject.h>
#include <MintApp/Include/SceneObjectRegistry.hpp>
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
		Terminate();
	}

	void SceneObjectRenderer::Render(const SceneObjectRegistry& sceneObjectRegistry) noexcept
	{
		using namespace Rendering;

		ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
		ShaderPipelinePool& shaderPipelinePool = _graphicsDevice.GetShaderPipelinePool();
		GraphicsResourcePool& resourcePool = _graphicsDevice.GetResourcePool();

		const SceneObjectComponentPool<TransformComponent>& transformComponentPool = sceneObjectRegistry.GetComponentPool<TransformComponent>();
		const SceneObjectComponentPool<MeshComponent>& meshComponentPool = sceneObjectRegistry.GetComponentPool<MeshComponent>();
		const ContiguousHashMap<SceneObject, MeshComponent>& meshComponentMap = meshComponentPool.GetComponentMap();
		if (meshComponentMap.IsEmpty() == false)
		{
			const ShaderPipeline& shaderPipelineDefault = shaderPipelinePool.GetShaderPipeline(_shaderPipelineDefaultID);
			const ShaderPipeline& shaderPipelineDrawEdges = shaderPipelinePool.GetShaderPipeline(_shaderPipelineDrawEdgesID);
			const ShaderPipeline& shaderPipelineDrawNormals = shaderPipelinePool.GetShaderPipeline(_shaderPipelineDrawNormalsID);

			GraphicsResource& cbTransform = resourcePool.GetResource(_graphicsDevice.GetCommonCBTransformID());
			cbTransform.BindToShader(GraphicsShaderType::VertexShader, cbTransform.GetRegisterIndex());
			cbTransform.BindToShader(GraphicsShaderType::GeometryShader, cbTransform.GetRegisterIndex());

			GraphicsResource& cbMaterial = resourcePool.GetResource(_graphicsDevice.GetCommonCBMaterialID());
			cbMaterial.BindToShader(GraphicsShaderType::PixelShader, cbMaterial.GetRegisterIndex());

			CB_Material cbMaterialData;
			for (auto iter = meshComponentMap.begin(); iter != meshComponentMap.end(); ++iter)
			{
				const SceneObject& sceneObject = iter.GetKey();
				const MeshComponent& meshComponent = iter.GetValue();
				const TransformComponent* transformComponent = transformComponentPool.GetComponent(sceneObject);
				_cbTransformData._cbWorldMatrix = transformComponent->_transform.ToMatrix();
				cbTransform.UpdateBuffer(&_cbTransformData, 1);

				_lowLevelRenderer.PushMesh(meshComponent._meshData);

				cbMaterialData._cbDiffuseColor = Color::kBlue;
				cbMaterial.UpdateBuffer(&cbMaterialData, 1);

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

			cbTransform.UnbindFromShader();
			cbMaterial.UnbindFromShader();
		}

		const SceneObjectComponentPool<Mesh2DComponent>& mesh2DComponentPool = sceneObjectRegistry.GetComponentPool<Mesh2DComponent>();
		const ContiguousHashMap<SceneObject, Mesh2DComponent>& mesh2DComponentMap = mesh2DComponentPool.GetComponentMap();
		if (mesh2DComponentMap.IsEmpty() == false)
		{
			ShapeRenderer& shapeRenderer = _graphicsDevice.GetShapeRenderer();
			for (auto iter = mesh2DComponentMap.begin(); iter != mesh2DComponentMap.end(); ++iter)
			{
				const SceneObject& sceneObject = iter.GetKey();
				const Mesh2DComponent& mesh2DComponent = iter.GetValue();
				const TransformComponent* transformComponent = transformComponentPool.GetComponent(sceneObject);
				shapeRenderer.AddShape(mesh2DComponent._shape, transformComponent->_transform);
			}
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
		vsDefaultID = shaderPool.CreateShader(Path::MakeIncludeAssetPath("Hlsl/"), "VsDefault.hlsl", "main", GraphicsShaderType::VertexShader, Path::MakeIncludeAssetPath("HlslBinary/"));

		const CppHlsl::Interpreter& interpreter = _graphicsDevice.GetCppHlslSteamData();
		const TypeMetaData<CppHlsl::TypeCustomData>& vsInputTypeMetaData = interpreter.GetTypeMetaData(typeid(VS_INPUT));
		inputLayoutDefaultID = shaderPool.CreateInputLayout(vsDefaultID, vsInputTypeMetaData);

		gsNormalID = shaderPool.CreateShader(Path::MakeIncludeAssetPath("Hlsl/"), "GsNormal.hlsl", "main", GraphicsShaderType::GeometryShader, Path::MakeIncludeAssetPath("HlslBinary/"));
		gsTriangleEdgeID = shaderPool.CreateShader(Path::MakeIncludeAssetPath("Hlsl/"), "GsTriangleEdge.hlsl", "main", GraphicsShaderType::GeometryShader, Path::MakeIncludeAssetPath("HlslBinary/"));
		
		psDefaultID = shaderPool.CreateShader(Path::MakeIncludeAssetPath("Hlsl/"), "PsDefault.hlsl", "main", GraphicsShaderType::PixelShader, Path::MakeIncludeAssetPath("HlslBinary/"));
		psTexCoordAsColorID = shaderPool.CreateShader(Path::MakeIncludeAssetPath("Hlsl/"), "PsTexCoordAsColor.hlsl", "main", GraphicsShaderType::PixelShader, Path::MakeIncludeAssetPath("HlslBinary/"));

		ShaderPipelinePool& shaderPipelinePool = _graphicsDevice.GetShaderPipelinePool();
		{
			ShaderPipelineDesc shaderPipelineDesc;
			shaderPipelineDesc._inputLayoutID = inputLayoutDefaultID;
			shaderPipelineDesc._vertexShaderID = vsDefaultID;
			shaderPipelineDesc._pixelShaderID = psDefaultID;
			_shaderPipelineDefaultID = shaderPipelinePool.CreateShaderPipeline(shaderPipelineDesc);
		}
		{
			ShaderPipelineDesc shaderPipelineDesc;
			shaderPipelineDesc._inputLayoutID = inputLayoutDefaultID;
			shaderPipelineDesc._vertexShaderID = vsDefaultID;
			shaderPipelineDesc._geometryShaderID = gsNormalID;
			shaderPipelineDesc._pixelShaderID = psTexCoordAsColorID;
			_shaderPipelineDrawNormalsID = shaderPipelinePool.CreateShaderPipeline(shaderPipelineDesc);
		}
		{
			ShaderPipelineDesc shaderPipelineDesc;
			shaderPipelineDesc._inputLayoutID = inputLayoutDefaultID;
			shaderPipelineDesc._vertexShaderID = vsDefaultID;
			shaderPipelineDesc._geometryShaderID = gsTriangleEdgeID;
			shaderPipelineDesc._pixelShaderID = psTexCoordAsColorID;
			_shaderPipelineDrawEdgesID = shaderPipelinePool.CreateShaderPipeline(shaderPipelineDesc);
		}
	}

	void SceneObjectRenderer::Terminate() noexcept
	{
		using namespace Rendering;

		ShaderPipelinePool& shaderPipelinePool = _graphicsDevice.GetShaderPipelinePool();
		shaderPipelinePool.DestroyShaderPipeline(_shaderPipelineDefaultID);
		shaderPipelinePool.DestroyShaderPipeline(_shaderPipelineDrawNormalsID);
		shaderPipelinePool.DestroyShaderPipeline(_shaderPipelineDrawEdgesID);
	}
}

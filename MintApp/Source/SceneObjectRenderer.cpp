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

		SceneObjectComponentPool<TransformComponent>& transformComponentPool = SceneObjectComponentPool<TransformComponent>::GetInstance();
		SceneObjectComponentPool<MeshComponent>& meshComponentPool = SceneObjectComponentPool<MeshComponent>::GetInstance();
		ContiguousHashMap<SceneObject, MeshComponent>& meshComponentMap = meshComponentPool.GetComponentMap();
		if (meshComponentMap.IsEmpty() == false)
		{
			shaderPool.BindInputLayoutIfNot(_inputLayoutDefaultID);
			shaderPool.BindShaderIfNot(GraphicsShaderType::VertexShader, _vsDefaultID);

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
			
				shaderPool.BindShaderIfNot(GraphicsShaderType::PixelShader, _psDefaultID);
				shaderPool.UnbindShader(GraphicsShaderType::GeometryShader);
				_lowLevelRenderer.Render(RenderingPrimitive::TriangleList);
			
				if (meshComponent._shouldDrawNormals == true)
				{
					shaderPool.BindShaderIfNot(GraphicsShaderType::GeometryShader, _gsNormalID);
					shaderPool.BindShaderIfNot(GraphicsShaderType::PixelShader, _psTexCoordAsColorID);
					_lowLevelRenderer.Render(RenderingPrimitive::LineList);
				}
			
				if (meshComponent._shouldDrawEdges == true)
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

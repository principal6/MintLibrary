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
		MaterialPool& materialPool = _graphicsDevice.GetMaterialPool();

		const SceneObjectComponentPool<MeshComponent>& meshComponentPool = sceneObjectRegistry.GetComponentPool<MeshComponent>();
		const ContiguousHashMap<SceneObject, MeshComponent>& meshComponentMap = meshComponentPool.GetComponentMap();
		if (meshComponentMap.IsEmpty() == false)
		{
			const ShaderPipeline& shaderPipelineTriangleDrawEdges = shaderPipelinePool.GetShaderPipeline(_graphicsDevice.GetShaderPipelineTriangleDrawEdgesID());
			const ShaderPipeline& shaderPipelineTriangleDrawNormals = shaderPipelinePool.GetShaderPipeline(_graphicsDevice.GetShaderPipelineTriangleDrawNormalsID());

			GraphicsResource& cbTransform = resourcePool.GetResource(_graphicsDevice.GetCommonCBTransformID());
			cbTransform.BindToShader(GraphicsShaderType::VertexShader, cbTransform.GetRegisterIndex());
			cbTransform.BindToShader(GraphicsShaderType::GeometryShader, cbTransform.GetRegisterIndex());

			GraphicsResource& cbMaterial = resourcePool.GetResource(_graphicsDevice.GetCommonCBMaterialID());
			cbMaterial.BindToShader(GraphicsShaderType::PixelShader, cbMaterial.GetRegisterIndex());

			const Material& defaultMaterial = *materialPool.GetMaterial(_defaultMaterialID);
			for (auto iter = meshComponentMap.begin(); iter != meshComponentMap.end(); ++iter)
			{
				const SceneObject& sceneObject = iter.GetKey();
				const MeshComponent& meshComponent = iter.GetValue();
				const TransformComponent& transformComponent = sceneObjectRegistry.GetComponentMust<TransformComponent>(sceneObject);
				_cbTransformData._cbWorldMatrix = transformComponent._transform.ToMatrix();
				cbTransform.UpdateBuffer(&_cbTransformData, 1);

				_lowLevelRenderer.PushMesh(meshComponent._meshData);

				GraphicsObjectID meshMaterialID = meshComponent._materialID;
				if (meshMaterialID.IsValid() == false)
				{
					meshMaterialID = _defaultMaterialID;
				}

				const Material* const material = materialPool.GetMaterial(meshMaterialID);
				MINT_ASSERT(material != nullptr, "Material must not be null");
				CB_MaterialData cbMaterialData;
				cbMaterialData._cbBaseColor = material->GetBaseColor();
				cbMaterial.UpdateBuffer(&cbMaterialData, 1);

				const ShaderPipeline& materialShaderPipeline = shaderPipelinePool.GetShaderPipeline(material->GetShaderPipelineID());
				materialShaderPipeline.BindShaderPipeline();
				_lowLevelRenderer.Render(_graphicsDevice, RenderingPrimitive::TriangleList);

				if (meshComponent._shouldDrawNormals == true)
				{
					shaderPipelineTriangleDrawNormals.BindShaderPipeline();
					_lowLevelRenderer.Render(_graphicsDevice, RenderingPrimitive::LineList);
				}
				
				if (meshComponent._shouldDrawEdges == true)
				{
					shaderPipelineTriangleDrawEdges.BindShaderPipeline();
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
				const TransformComponent& transformComponent = sceneObjectRegistry.GetComponentMust<TransformComponent>(sceneObject);
				shapeRenderer.AddShape(mesh2DComponent._shape, transformComponent._transform);
			}
		}
	}

	void SceneObjectRenderer::Initialize() noexcept
	{
		using namespace Rendering;
		MaterialPool& materialPool = _graphicsDevice.GetMaterialPool();
		{
			MaterialDesc materialDesc;
			materialDesc._materialName = "SceneObjectRendererDefault";
			materialDesc._baseColor = Color(255, 64, 32);
			_defaultMaterialID = materialPool.CreateMaterial(materialDesc);
		}
	}

	void SceneObjectRenderer::Terminate() noexcept
	{
		__noop;
	}
}

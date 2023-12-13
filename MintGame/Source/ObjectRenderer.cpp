#include <MintGame/Include/ObjectRenderer.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>

#include <MintGame/Include/Object.h>
#include <MintGame/Include/ObjectPool.hpp>
#include <MintGame/Include/MeshComponent.h>


namespace mint
{
	namespace Game
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

			DxShaderPool& shaderPool = _graphicDevice.GetShaderPool();
			DxResourcePool& resourcePool = _graphicDevice.GetResourcePool();
			const Vector<MeshComponent*>& meshComponents = objectPool.GetMeshComponents();
			if (meshComponents.IsEmpty() == false)
			{
				shaderPool.BindInputLayoutIfNot(_inputLayoutDefaultID);
				shaderPool.BindShaderIfNot(GraphicShaderType::VertexShader, _vsDefaultID);

				DxResource& cbTransform = resourcePool.GetResource(_graphicDevice.GetCommonCBTransformID());
				cbTransform.BindToShader(GraphicShaderType::VertexShader, cbTransform.GetRegisterIndex());
				cbTransform.BindToShader(GraphicShaderType::GeometryShader, cbTransform.GetRegisterIndex());

				DxResource& sbMaterial = resourcePool.GetResource(_graphicDevice.GetCommonSBMaterialID());
				sbMaterial.BindToShader(GraphicShaderType::PixelShader, sbMaterial.GetRegisterIndex());

				SB_Material sbMaterialData;
				const uint32 meshComponentCount = meshComponents.Size();
				for (uint32 i = 0; i < meshComponentCount; ++i)
				{
					const MeshComponent* const meshComponent = meshComponents[i];

					_cbTransformData._cbWorldMatrix = meshComponent->getOwnerObject()->GetObjectTransformMatrix() * meshComponent->_transform.ToMatrix();
					cbTransform.UpdateBuffer(&_cbTransformData, 1);

					_lowLevelRenderer.Flush();

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
				}
			}

			const Vector<Mesh2DComponent*>& mesh2DComponents = objectPool.GetMesh2DComponents();
			if (mesh2DComponents.IsEmpty() == false)
			{
				ShapeRendererContext& shapeRendererContext = _graphicDevice.GetShapeRendererContext();
				shapeRendererContext.Flush();
				const uint32 mesh2DComponentCount = mesh2DComponents.Size();
				for (uint32 i = 0; i < mesh2DComponentCount; ++i)
				{
					const Mesh2DComponent* const mesh2DComponent = mesh2DComponents[i];

					shapeRendererContext.SetPosition(Float4(mesh2DComponent->_position));
					shapeRendererContext.AddShape(mesh2DComponent->GetShape());
				}
				shapeRendererContext.Render();
			}
		}

		void ObjectRenderer::Initialize() noexcept
		{
			using namespace Rendering;
			using namespace Language;

			DxShaderPool& shaderPool = _graphicDevice.GetShaderPool();
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
}

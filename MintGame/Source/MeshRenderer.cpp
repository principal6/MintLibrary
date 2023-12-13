#include <MintGame/Include/MeshRenderer.h>

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
		MeshRenderer::MeshRenderer(Rendering::GraphicDevice& graphicDevice)
			: _graphicDevice{ graphicDevice }
			, _lowLevelRenderer{ graphicDevice }
		{
			Initialize();
		}

		MeshRenderer::~MeshRenderer()
		{
			__noop;
		}

		void MeshRenderer::Render(const ObjectPool& objectPool) noexcept
		{
			using namespace Rendering;

			const Vector<MeshComponent*>& meshComponents = objectPool.GetMeshComponents();

			DxShaderPool& shaderPool = _graphicDevice.GetShaderPool();
			shaderPool.BindInputLayoutIfNot(_inputLayoutDefaultID);
			shaderPool.BindShaderIfNot(GraphicShaderType::VertexShader, _vsDefaultID);

			DxResourcePool& resourcePool = _graphicDevice.GetResourcePool();
			DxResource& cbTransform = resourcePool.GetResource(_graphicDevice.GetCommonCBTransformID());
			{
				cbTransform.BindToShader(GraphicShaderType::VertexShader, cbTransform.GetRegisterIndex());
				cbTransform.BindToShader(GraphicShaderType::GeometryShader, cbTransform.GetRegisterIndex());
			}

			DxResource& sbMaterial = resourcePool.GetResource(_graphicDevice.GetCommonSBMaterialID());
			{
				sbMaterial.BindToShader(GraphicShaderType::PixelShader, sbMaterial.GetRegisterIndex());
			}

			SB_Material sbMaterialData;
			const uint32 meshComponentCount = meshComponents.Size();
			for (uint32 meshCompnentIndex = 0; meshCompnentIndex < meshComponentCount; ++meshCompnentIndex)
			{
				const MeshComponent* const meshComponent = meshComponents[meshCompnentIndex];
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

		void MeshRenderer::Initialize() noexcept
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

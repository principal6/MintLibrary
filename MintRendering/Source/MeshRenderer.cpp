#include <MintRendering/Include/MeshRenderer.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>

#include <MintRendering/Include/Object.h>
#include <MintRendering/Include/ObjectPool.hpp>
#include <MintRendering/Include/MeshComponent.h>


namespace mint
{
	namespace Rendering
	{
		MeshRenderer::MeshRenderer(GraphicDevice& graphicDevice)
			: _graphicDevice{ graphicDevice }
			, _lowLevelRenderer{ graphicDevice }
		{
			initialize();
		}

		MeshRenderer::~MeshRenderer()
		{
			__noop;
		}

		void MeshRenderer::render(const ObjectPool& objectPool) noexcept
		{
			const Vector<MeshComponent*>& meshComponents = objectPool.getMeshComponents();

			DxShaderPool& shaderPool = _graphicDevice.getShaderPool();
			shaderPool.bindInputLayoutIfNot(_inputLayoutDefaultID);
			shaderPool.bindShaderIfNot(GraphicShaderType::VertexShader, _vsDefaultID);

			DxResourcePool& resourcePool = _graphicDevice.getResourcePool();
			DxResource& cbTransform = resourcePool.getResource(_graphicDevice.getCommonCbTransformID());
			{
				cbTransform.bindToShader(GraphicShaderType::VertexShader, cbTransform.getRegisterIndex());
				cbTransform.bindToShader(GraphicShaderType::GeometryShader, cbTransform.getRegisterIndex());
			}

			DxResource& sbMaterial = resourcePool.getResource(_graphicDevice.getCommonSBMaterialID());
			{
				sbMaterial.bindToShader(GraphicShaderType::PixelShader, sbMaterial.getRegisterIndex());
			}

			SB_Material sbMaterialData;
			const uint32 meshComponentCount = meshComponents.size();
			for (uint32 meshCompnentIndex = 0; meshCompnentIndex < meshComponentCount; ++meshCompnentIndex)
			{
				const MeshComponent* const meshComponent = meshComponents[meshCompnentIndex];
				_cbTransformData._cbWorldMatrix = meshComponent->getOwnerObject()->getObjectTransformMatrix() * meshComponent->_transform.toMatrix();
				cbTransform.updateBuffer(&_cbTransformData, 1);

				_lowLevelRenderer.flush();

				_lowLevelRenderer.pushMesh(meshComponent->getMeshData());

				sbMaterialData._diffuseColor = Color::kBlue;
				sbMaterial.updateBuffer(&sbMaterialData, 1);

				shaderPool.bindShaderIfNot(GraphicShaderType::PixelShader, _psDefaultID);
				shaderPool.unbindShader(GraphicShaderType::GeometryShader);
				_lowLevelRenderer.render(RenderingPrimitive::TriangleList);

				if (meshComponent->shouldDrawNormals() == true)
				{
					shaderPool.bindShaderIfNot(GraphicShaderType::GeometryShader, _gsNormalID);
					shaderPool.bindShaderIfNot(GraphicShaderType::PixelShader, _psTexCoordAsColorID);
					_lowLevelRenderer.render(RenderingPrimitive::LineList);
				}

				if (meshComponent->shouldDrawEdges() == true)
				{
					shaderPool.bindShaderIfNot(GraphicShaderType::GeometryShader, _gsTriangleEdgeID);
					shaderPool.bindShaderIfNot(GraphicShaderType::PixelShader, _psTexCoordAsColorID);
					_lowLevelRenderer.render(RenderingPrimitive::TriangleList);
				}
			}
		}

		void MeshRenderer::initialize() noexcept
		{
			using namespace Language;

			DxShaderPool& shaderPool = _graphicDevice.getShaderPool();
			_vsDefaultID = shaderPool.addShader(Path::makeIncludeAssetPath("Hlsl/"), "VsDefault.hlsl", "main", GraphicShaderType::VertexShader, Path::makeIncludeAssetPath("HlslBinary/"));

			const CppHlsl::Interpreter& interpreter = _graphicDevice.getCppHlslSteamData();
			const TypeMetaData<CppHlsl::TypeCustomData>& vsInputTypeMetaData = interpreter.getTypeMetaData(typeid(VS_INPUT));
			_inputLayoutDefaultID = shaderPool.addInputLayout(_vsDefaultID, vsInputTypeMetaData);

			_psDefaultID = shaderPool.addShader(Path::makeIncludeAssetPath("Hlsl/"), "PsDefault.hlsl", "main", GraphicShaderType::PixelShader, Path::makeIncludeAssetPath("HlslBinary/"));

			_gsNormalID = shaderPool.addShader(Path::makeIncludeAssetPath("Hlsl/"), "GsNormal.hlsl", "main", GraphicShaderType::GeometryShader, Path::makeIncludeAssetPath("HlslBinary/"));
			_gsTriangleEdgeID = shaderPool.addShader(Path::makeIncludeAssetPath("Hlsl/"), "GsTriangleEdge.hlsl", "main", GraphicShaderType::GeometryShader, Path::makeIncludeAssetPath("HlslBinary/"));
			_psTexCoordAsColorID = shaderPool.addShader(Path::makeIncludeAssetPath("Hlsl/"), "PsTexCoordAsColor.hlsl", "main", GraphicShaderType::PixelShader, Path::makeIncludeAssetPath("HlslBinary/"));
		}
	}
}

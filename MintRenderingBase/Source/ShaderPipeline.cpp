#include <MintRenderingBase/Include/ShaderPipeline.h>
#include <MintRenderingBase/Include/Shader.h>


namespace mint
{
	namespace Rendering
	{
		ShaderPipeline::ShaderPipeline(GraphicsDevice& graphicsDevice, ShaderPool& shaderPool)
			: GraphicsObject{ graphicsDevice, GraphicsObjectType::ShaderPipeline }
			, _shaderPool{ shaderPool }
		{
			__noop;
		}

		void ShaderPipeline::SetVertexShader(const GraphicsObjectID& vertexShaderID) noexcept
		{
			MINT_ASSERT(_shaderPool.CheckShaderType(vertexShaderID, GraphicsShaderType::VertexShader) == true, "Invalid parameter - check ShaderType");
			_vertexShaderID = vertexShaderID;
		}

		void ShaderPipeline::SetGeometryShader(const GraphicsObjectID& geometryShaderID) noexcept
		{
			MINT_ASSERT(_shaderPool.CheckShaderType(geometryShaderID, GraphicsShaderType::GeometryShader) == true, "Invalid parameter - check ShaderType");
			_geometryShaderID = geometryShaderID;
		}

		void ShaderPipeline::SetPixelShader(const GraphicsObjectID& pixelShaderID) noexcept
		{
			MINT_ASSERT(_shaderPool.CheckShaderType(pixelShaderID, GraphicsShaderType::PixelShader) == true, "Invalid parameter - check ShaderType");
			_pixelShaderID = pixelShaderID;
		}
	}
}

#pragma once


#ifndef _MINT_RENDERING_BASE_SHADER_PIPELINE_H_
#define _MINT_RENDERING_BASE_SHADER_PIPELINE_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/GraphicsObject.h>


namespace mint
{
	namespace Rendering
	{
		class ShaderPool;
	}
}


namespace mint
{
	namespace Rendering
	{
		class ShaderPipeline final : public GraphicsObject
		{
		public:
			ShaderPipeline(GraphicsDevice& graphicsDevice, ShaderPool& shaderPool);
			~ShaderPipeline() = default;
		
		public:
			void SetVertexShader(const GraphicsObjectID& vertexShaderID) noexcept;
			void SetGeometryShader(const GraphicsObjectID& geometryShaderID) noexcept;
			void SetPixelShader(const GraphicsObjectID& pixelShaderID) noexcept;

		public:
			MINT_INLINE GraphicsObjectID GetVertexShaderID() const noexcept { return _vertexShaderID; }
			MINT_INLINE GraphicsObjectID GetGeometryShaderID() const noexcept { return _geometryShaderID; }
			MINT_INLINE GraphicsObjectID GetPixelShaderID() const noexcept { return _pixelShaderID; }

		private:
			ShaderPool& _shaderPool;
			GraphicsObjectID _vertexShaderID;
			GraphicsObjectID _geometryShaderID;
			GraphicsObjectID _pixelShaderID;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_SHADER_PIPELINE_H_

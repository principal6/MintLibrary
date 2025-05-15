#pragma once


#ifndef _MINT_RENDERING_BASE_SHADER_PIPELINE_H_
#define _MINT_RENDERING_BASE_SHADER_PIPELINE_H_


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintContainer/Include/RefCounted.h>
#include <MintRendering/Include/GraphicsObject.h>


namespace mint
{
	namespace Rendering
	{
		class ShaderPool;
		class ShaderPipelinePool;
	}
}


namespace mint
{
	namespace Rendering
	{
		struct ShaderPipelineDesc
		{
			GraphicsObjectID _inputLayoutID;
			GraphicsObjectID _vertexShaderID;
			GraphicsObjectID _geometryShaderID;
			GraphicsObjectID _pixelShaderID;
		};

		class ShaderPipeline final : public GraphicsObject
		{
			friend ShaderPipelinePool;

		public:
			virtual ~ShaderPipeline();
		
		public:
			void BindShaderPipeline() const noexcept;

		private:
			ShaderPipeline(GraphicsDevice& graphicsDevice, const ShaderPipelineDesc& shaderPipelineDesc);
			ShaderPipeline(const ShaderPipeline& rhs) = delete;

		private:
			void SetInputLayout(const GraphicsObjectID& inputLayoutID) noexcept;
			void SetVertexShader(const GraphicsObjectID& vertexShaderID) noexcept;
			void SetGeometryShader(const GraphicsObjectID& geometryShaderID) noexcept;
			void SetPixelShader(const GraphicsObjectID& pixelShaderID) noexcept;

		private:
			GraphicsObjectID _inputLayoutID;
			GraphicsObjectID _vertexShaderID;
			GraphicsObjectID _geometryShaderID;
			GraphicsObjectID _pixelShaderID;
		};


		class ShaderPipelinePool final
		{
			friend GraphicsDevice;

		public:
			~ShaderPipelinePool();

		public:
			GraphicsObjectID CreateShaderPipeline(const ShaderPipelineDesc& shaderPipelineDesc);
			void DestroyShaderPipeline(const GraphicsObjectID& shaderPipelineID);
			const ShaderPipeline& GetShaderPipeline(const GraphicsObjectID& shaderPipelineID) const;

		private:
			ShaderPipelinePool(GraphicsDevice& graphicsDevice);
			ShaderPipelinePool(const ShaderPipelinePool& rhs) = delete;

		private:
			GraphicsDevice& _graphicsDevice;
			Vector<RefCounted<ShaderPipeline>> _shaderPipelines;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_SHADER_PIPELINE_H_

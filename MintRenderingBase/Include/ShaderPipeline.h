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
		class ShaderPipelinePool;
	}
}


namespace mint
{
	namespace Rendering
	{
		class ShaderPipeline final : public GraphicsObject
		{
			friend ShaderPipelinePool;

		public:
			ShaderPipeline(GraphicsDevice& graphicsDevice);
			virtual ~ShaderPipeline();
		
		public:
			void SetInputLayout(const GraphicsObjectID& inputLayoutID) noexcept;
			void SetVertexShader(const GraphicsObjectID& vertexShaderID) noexcept;
			void SetGeometryShader(const GraphicsObjectID& geometryShaderID) noexcept;
			void SetPixelShader(const GraphicsObjectID& pixelShaderID) noexcept;

		public:
			void BindShaderPipeline() const noexcept;
			//void UnbindShaders() const noexcept;

		public:
			MINT_INLINE GraphicsObjectID GetInputLayoutID() const noexcept { return _inputLayoutID; }
			MINT_INLINE GraphicsObjectID GetVertexShaderID() const noexcept { return _vertexShaderID; }
			MINT_INLINE GraphicsObjectID GetGeometryShaderID() const noexcept { return _geometryShaderID; }
			MINT_INLINE GraphicsObjectID GetPixelShaderID() const noexcept { return _pixelShaderID; }

		private:
			GraphicsObjectID _inputLayoutID;
			GraphicsObjectID _vertexShaderID;
			GraphicsObjectID _geometryShaderID;
			GraphicsObjectID _pixelShaderID;
		};


		class ShaderPipelinePool final
		{
		public:
			ShaderPipelinePool(GraphicsDevice& graphicsDevice);
			ShaderPipelinePool(const ShaderPipelinePool& rhs) = delete;
			~ShaderPipelinePool() = default;

		public:
			GraphicsObjectID CreateShaderPipeline();
			void DestroyShaderPipeline(const GraphicsObjectID& shaderPipelineID);
			ShaderPipeline& AccessShaderPipeline(const GraphicsObjectID& shaderPipelineID);
			const ShaderPipeline& GetShaderPipeline(const GraphicsObjectID& shaderPipelineID) const;

		private:
			GraphicsDevice& _graphicsDevice;
			Vector<OwnPtr<GraphicsObject>> _shaderPipelines;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_SHADER_PIPELINE_H_

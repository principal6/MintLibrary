#include <MintRenderingBase/Include/ShaderPipeline.h>
#include <MintRenderingBase/Include/Shader.h>


namespace mint
{
	namespace Rendering
	{
#pragma region ShaderPipeline
		ShaderPipeline::ShaderPipeline(GraphicsDevice& graphicsDevice)
			: GraphicsObject{ graphicsDevice, GraphicsObjectType::ShaderPipeline }
		{
			__noop;
		}

		void ShaderPipeline::SetInputLayout(const GraphicsObjectID& inputLayoutID) noexcept
		{
			const ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
			MINT_ASSERT(shaderPool.ExistsInputLayout(inputLayoutID) == true, "Invalid parameter - check InputLayoutID");
			_inputLayoutID = inputLayoutID;
		}

		void ShaderPipeline::SetVertexShader(const GraphicsObjectID& vertexShaderID) noexcept
		{
			const ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
			MINT_ASSERT(shaderPool.ExistsShader(vertexShaderID, GraphicsShaderType::VertexShader) == true, "Invalid parameter - check ShaderType");
			_vertexShaderID = vertexShaderID;
		}

		void ShaderPipeline::SetGeometryShader(const GraphicsObjectID& geometryShaderID) noexcept
		{
			const ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
			MINT_ASSERT(shaderPool.ExistsShader(geometryShaderID, GraphicsShaderType::GeometryShader) == true, "Invalid parameter - check ShaderType");
			_geometryShaderID = geometryShaderID;
		}

		void ShaderPipeline::SetPixelShader(const GraphicsObjectID& pixelShaderID) noexcept
		{
			const ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
			MINT_ASSERT(shaderPool.ExistsShader(pixelShaderID, GraphicsShaderType::PixelShader) == true, "Invalid parameter - check ShaderType");
			_pixelShaderID = pixelShaderID;
		}

		void ShaderPipeline::BindShaderPipeline() const noexcept
		{
			MINT_ASSERT(_inputLayoutID.IsValid() == true, "You must set the InputLayoutID!");
			MINT_ASSERT(_vertexShaderID.IsValid() == true, "You must set the VertexShaderID!");
			MINT_ASSERT(_pixelShaderID.IsValid() == true, "You must set the PixelShaderID!");

			ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
			shaderPool.BindInputLayoutIfNot(_inputLayoutID);
			shaderPool.BindShaderIfNot(GraphicsShaderType::VertexShader, _vertexShaderID);
			shaderPool.BindShaderIfNot(GraphicsShaderType::PixelShader, _pixelShaderID);

			if (_geometryShaderID.IsValid())
			{
				shaderPool.BindShaderIfNot(GraphicsShaderType::GeometryShader, _geometryShaderID);
			}
			else
			{
				shaderPool.UnbindShader(GraphicsShaderType::GeometryShader);
			}
		}

		//void ShaderPipeline::UnbindShaders() const noexcept
		//{
		//	ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
		//	//shaderPool.UnbindInputLayout(_inputLayoutID);
		//	shaderPool.UnbindShader(GraphicsShaderType::VertexShader);
		//	shaderPool.UnbindShader(GraphicsShaderType::PixelShader);
		//	
		//	if (_geometryShaderID.IsValid())
		//	{
		//		shaderPool.UnbindShader(GraphicsShaderType::GeometryShader);
		//	}
		//}
#pragma endregion


#pragma region ShaderPipelinePool
		ShaderPipelinePool::ShaderPipelinePool(GraphicsDevice& graphicsDevice)
			: _graphicsDevice{ graphicsDevice }
		{
			__noop;
		}

		GraphicsObjectID ShaderPipelinePool::CreateShaderPipeline()
		{
			OwnPtr<GraphicsObject> shaderPipeline{ MINT_NEW(ShaderPipeline, _graphicsDevice) };
			static_cast<ShaderPipeline*>(shaderPipeline.Get())->AssignIDXXX();
			_shaderPipelines.PushBack(std::move(shaderPipeline));
			return _shaderPipelines.Back()->GetID();
		}

		ShaderPipeline& ShaderPipelinePool::AccessShaderPipeline(const GraphicsObjectID& shaderPipelineID)
		{
			return const_cast<ShaderPipeline&>(GetShaderPipeline(shaderPipelineID));
		}

		const ShaderPipeline& ShaderPipelinePool::GetShaderPipeline(const GraphicsObjectID& shaderPipelineID) const
		{
			const uint32 index = BinarySearch(_shaderPipelines, shaderPipelineID, GraphicsObject::Evaluator());
			if (IsValidIndex(index) == false)
			{
				MINT_ASSERT(false, "ShaderPipeline 를 찾지 못했습니다!!!");
				static ShaderPipeline s_invalidInstance{ GraphicsDevice::GetInvalidInstance() };
				return s_invalidInstance;
			}
			return static_cast<const ShaderPipeline&>(*_shaderPipelines[index].Get());
		}
#pragma endregion
	}
}

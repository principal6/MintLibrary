#include <MintRenderingBase/Include/ShaderPipeline.h>
#include <MintContainer/Include/RefCounted.hpp>
#include <MintRenderingBase/Include/Shader.h>


namespace mint
{
	namespace Rendering
	{
#pragma region ShaderPipeline
		ShaderPipeline::ShaderPipeline(GraphicsDevice& graphicsDevice, const ShaderPipelineDesc& shaderPipelineDesc)
			: GraphicsObject{ graphicsDevice, GraphicsObjectType::ShaderPipeline }
		{
			SetInputLayout(shaderPipelineDesc._inputLayoutID);
			SetVertexShader(shaderPipelineDesc._vertexShaderID);
			SetPixelShader(shaderPipelineDesc._pixelShaderID);
			
			if (shaderPipelineDesc._geometryShaderID.IsValid())
			{
				SetGeometryShader(shaderPipelineDesc._geometryShaderID);
			}
		}

		ShaderPipeline::~ShaderPipeline()
		{
			ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
			if (_inputLayoutID.IsValid())
			{
				shaderPool.DecreaseInputLayoutRefCount(_inputLayoutID);
			}

			if (_vertexShaderID.IsValid())
			{
				shaderPool.DecreaseShaderRefCount(_vertexShaderID);
			}

			if (_geometryShaderID.IsValid())
			{
				shaderPool.DecreaseShaderRefCount(_geometryShaderID);
			}

			if (_pixelShaderID.IsValid())
			{
				shaderPool.DecreaseShaderRefCount(_pixelShaderID);
			}
		}

		void ShaderPipeline::SetInputLayout(const GraphicsObjectID& inputLayoutID) noexcept
		{
			ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
			MINT_ASSERT(shaderPool.ExistsInputLayout(inputLayoutID) == true, "Invalid parameter - check InputLayoutID");
			_inputLayoutID = inputLayoutID;
			shaderPool.IncreaseInputLayoutRefCount(_inputLayoutID);
		}

		void ShaderPipeline::SetVertexShader(const GraphicsObjectID& vertexShaderID) noexcept
		{
			ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
			MINT_ASSERT(shaderPool.ExistsShader(vertexShaderID, GraphicsShaderType::VertexShader) == true, "Invalid parameter - check ShaderType");
			_vertexShaderID = vertexShaderID;
			shaderPool.IncreaseShaderRefCount(_vertexShaderID);
		}

		void ShaderPipeline::SetGeometryShader(const GraphicsObjectID& geometryShaderID) noexcept
		{
			ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
			MINT_ASSERT(shaderPool.ExistsShader(geometryShaderID, GraphicsShaderType::GeometryShader) == true, "Invalid parameter - check ShaderType");
			_geometryShaderID = geometryShaderID;
			shaderPool.IncreaseShaderRefCount(_geometryShaderID);
		}

		void ShaderPipeline::SetPixelShader(const GraphicsObjectID& pixelShaderID) noexcept
		{
			ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
			MINT_ASSERT(shaderPool.ExistsShader(pixelShaderID, GraphicsShaderType::PixelShader) == true, "Invalid parameter - check ShaderType");
			_pixelShaderID = pixelShaderID;
			shaderPool.IncreaseShaderRefCount(_pixelShaderID);
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
#pragma endregion


#pragma region ShaderPipelinePool
		ShaderPipelinePool::ShaderPipelinePool(GraphicsDevice& graphicsDevice)
			: _graphicsDevice{ graphicsDevice }
		{
			__noop;
		}

		ShaderPipelinePool::~ShaderPipelinePool()
		{
			MINT_ASSERT(_shaderPipelines.IsEmpty() == true, "All ShaderPipelines must be destroyed before destroying ShaderPipelinePool!");
		}

		GraphicsObjectID ShaderPipelinePool::CreateShaderPipeline(const ShaderPipelineDesc& shaderPipelineDesc)
		{
			for (RefCounted<ShaderPipeline>& shaderPipeline : _shaderPipelines)
			{
				if (shaderPipeline->_inputLayoutID == shaderPipelineDesc._inputLayoutID &&
					shaderPipeline->_vertexShaderID == shaderPipelineDesc._vertexShaderID &&
					shaderPipeline->_geometryShaderID == shaderPipelineDesc._geometryShaderID &&
					shaderPipeline->_pixelShaderID == shaderPipelineDesc._pixelShaderID)
				{
					shaderPipeline.IncreaseRefCount();
					return shaderPipeline->GetID();
				}
			}

			RefCounted<ShaderPipeline> shaderPipeline{ MINT_NEW(ShaderPipeline, _graphicsDevice, shaderPipelineDesc) };
			shaderPipeline->AssignIDXXX();
			shaderPipeline.IncreaseRefCount();
			const GraphicsObjectID graphicsObjectID = shaderPipeline->GetID();
			_shaderPipelines.PushBack(std::move(shaderPipeline));
			QuickSort(_shaderPipelines, GraphicsObject::AscendingComparator());
			return graphicsObjectID;
		}

		void ShaderPipelinePool::DestroyShaderPipeline(const GraphicsObjectID& shaderPipelineID)
		{
			const uint32 index = BinarySearch(_shaderPipelines, shaderPipelineID, GraphicsObject::Evaluator());
			if (IsValidIndex(index) == false)
			{
				MINT_ASSERT(false, "Could not find the ShaderPipeline!");
				return;
			}

			_shaderPipelines[index].DecreaseRefCount();

			if (_shaderPipelines[index].IsValid() == false)
			{
				_shaderPipelines.Erase(index);
			}
		}

		const ShaderPipeline& ShaderPipelinePool::GetShaderPipeline(const GraphicsObjectID& shaderPipelineID) const
		{
			const uint32 index = BinarySearch(_shaderPipelines, shaderPipelineID, GraphicsObject::Evaluator());
			if (IsValidIndex(index) == false)
			{
				MINT_ASSERT(false, "ShaderPipeline 를 찾지 못했습니다!!!");
				static ShaderPipeline s_invalidInstance{ GraphicsDevice::GetInvalidInstance(), ShaderPipelineDesc() };
				return s_invalidInstance;
			}
			return static_cast<const ShaderPipeline&>(*_shaderPipelines[index].Get());
		}
#pragma endregion
	}
}

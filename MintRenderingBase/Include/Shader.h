#pragma once


#ifndef _MINT_RENDERING_BASE_DX_SHADER_H_
#define _MINT_RENDERING_BASE_DX_SHADER_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/GraphicsObject.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/String.h>


namespace mint
{
	namespace Language
	{
		template <typename CustomDataType>
		class TypeMetaData;

		namespace CppHlsl
		{
			class TypeCustomData;
		}
	}

	namespace Rendering
	{
		class DxShaderHeaderMemory;
		class GraphicsDevice;
		class ShaderPool;
		using Microsoft::WRL::ComPtr;
	}
}


namespace mint
{
	namespace Rendering
	{
		struct DxInputElementSet
		{
			Vector<StringA> _semanticNameArray;
			Vector<D3D11_INPUT_ELEMENT_DESC> _inputElementDescriptorArray;
		};

		enum class ShaderVersion
		{
			v_4_0,
			v_5_0,
		};


		class GraphicsInputLayout final : public GraphicsObject
		{
			friend ShaderPool;

		public:
			~GraphicsInputLayout() = default;
		
		public:
			GraphicsInputLayout& operator=(GraphicsInputLayout&& rhs) noexcept = default;

		public:
			void Bind() const;
			void Unbind() const;

		private:
			GraphicsInputLayout(GraphicsDevice& graphicsDevice);
			GraphicsInputLayout(GraphicsInputLayout&& rhs) noexcept = default;

		private:
			DxInputElementSet _inputElementSet;
			ComPtr<ID3D11InputLayout> _inputLayout;
		};

		class Shader final : public GraphicsObject
		{
			friend ShaderPool;

		public:
			virtual ~Shader() = default;

		public:
			Shader& operator=(Shader&& rhs) noexcept = default;

		public:
			void Bind() const noexcept;
			void Unbind() const noexcept;

		private:
			Shader(GraphicsDevice& graphicsDevice, const GraphicsShaderType shaderType);
			Shader(Shader&& rhs) noexcept = default;

		private:
			ComPtr<ID3D10Blob> _shaderBlob;
			ComPtr<ID3D11DeviceChild> _shader;
			GraphicsShaderType _shaderType;
			StringA _hlslFileName;
			StringA _hlslBinaryFileName;
			StringA _entryPoint;
		};


		struct ShaderCompileParam
		{
			const char* _inputFileName = nullptr;
			const char* _outputFileName = nullptr;
			const char* _shaderIdentifier = nullptr;
			const char* _shaderTextContent = nullptr;
		};

		class ShaderPool
		{
			friend GraphicsDevice;
			template <typename CustomDataType>
			using TypeMetaData = Language::TypeMetaData<CustomDataType>;
			using TypeCustomData = Language::CppHlsl::TypeCustomData;

			static constexpr const char* const kCompiledShaderFileExtension = ".hlslbin";

		public:
			~ShaderPool();

		public:
			GraphicsObjectID CreateShaderFromMemory(const char* const shaderIdentifier, const char* const textContent, const char* const entryPoint, const GraphicsShaderType shaderType);
			GraphicsObjectID CreateShader(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const GraphicsShaderType shaderType, const char* const outputDirectory = nullptr);
			GraphicsObjectID CreateInputLayout(const GraphicsObjectID& vertexShaderID, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData);

		public:
			void IncreaseShaderRefCount(const GraphicsObjectID& shaderID);
			void DecreaseShaderRefCount(const GraphicsObjectID& shaderID);
			void IncreaseInputLayoutRefCount(const GraphicsObjectID& inputLayoutID);
			void DecreaseInputLayoutRefCount(const GraphicsObjectID& inputLayoutID);

		public:
			void RecompileAllShaders();

		public:
			void BindShaderIfNot(const GraphicsShaderType shaderType, const GraphicsObjectID& objectID);
			void BindInputLayoutIfNot(const GraphicsObjectID& objectID);
			void UnbindShader(const GraphicsShaderType shaderType);
			bool ExistsShader(const GraphicsObjectID& shaderID, const GraphicsShaderType shaderType) const;
			bool ExistsInputLayout(const GraphicsObjectID& inputLayoutID) const;

		private:
			ShaderPool(GraphicsDevice& graphicsDevice, DxShaderHeaderMemory* const shaderHeaderMemory, const ShaderVersion shaderVersion);
			ShaderPool(const ShaderPool& rhs) = delete;

		private:
			GraphicsObjectID CreateShaderInternal(const GraphicsShaderType shaderType, Shader& shader);
			GraphicsObjectID CreateInputLayoutInternal(const Shader& vertexShader, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData);

		private:
			bool CreateLowLevelShader(const GraphicsShaderType shaderType, Shader& shader);
			bool CreateLowLevelInputLayout(const Shader& vertexShader, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData, GraphicsInputLayout& outInputLayout);
			void PushInputElement(DxInputElementSet& inputElementSet, const TypeMetaData<TypeCustomData>& outerDataTypeMetaData, const TypeMetaData<TypeCustomData>& memberTypeMetaData);

		private:
			bool CompileShaderFromFile(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const char* const outputDirectory, const GraphicsShaderType shaderType, const bool forceCompilation, Shader& inoutShader);
			bool CompileShaderFromFile(const char* const inputShaderFilePath, const char* const entryPoint, const char* const outputShaderFilePath, const GraphicsShaderType shaderType, const bool forceCompilation, Shader& inoutShader);
			bool CompileShaderInternalXXX(const GraphicsShaderType shaderType, const ShaderCompileParam& compileParam, const char* const entryPoint, ID3D10Blob** outBlob);

		private:
			void ReportCompileError();

		private:
			uint32 GetShaderIndex(const GraphicsShaderType shaderType, const GraphicsObjectID& objectID) const;
			uint32 GetInputLayoutIndex(const GraphicsObjectID& objectID) const;
			uint32 GetShaderCount(const GraphicsShaderType shaderType) const;
			GraphicsObjectID& AccessBoundShaderID(const GraphicsShaderType shaderType);
			const Vector<RefCounted<Shader>>& GetShaders(const GraphicsShaderType shaderType) const;
			Vector<RefCounted<Shader>>& AccessShaders(const GraphicsShaderType shaderType);

		private:
			GraphicsDevice& _graphicsDevice;
			ComPtr<ID3DBlob> _errorMessageBlob;

		private:
			DxShaderHeaderMemory* _shaderHeaderMemory;

		private:
			ShaderVersion _shaderVersion;

		private:
			Vector<RefCounted<GraphicsInputLayout>> _inputLayouts;
			Vector<RefCounted<Shader>> _shadersPerType[static_cast<uint32>(GraphicsShaderType::COUNT)];

		private:
			GraphicsObjectID _boundInputLayoutID;
			GraphicsObjectID _boundShaderIDPerType[static_cast<uint32>(GraphicsShaderType::COUNT)];
		};
	}
}


#endif // !_MINT_RENDERING_BASE_DX_SHADER_H_

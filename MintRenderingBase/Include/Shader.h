#pragma once


#ifndef _MINT_RENDERING_BASE_DX_SHADER_H_
#define _MINT_RENDERING_BASE_DX_SHADER_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/GraphicObject.h>

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
		class GraphicDevice;
		class ShaderPool;


		using Microsoft::WRL::ComPtr;


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


		class GraphicInputLayout final : public GraphicObject
		{
			friend ShaderPool;

		public:
			static const GraphicInputLayout kNullInstance;

		private:
			GraphicInputLayout(GraphicDevice& graphicDevice);

		public:
			GraphicInputLayout(GraphicInputLayout&& rhs) noexcept = default;
		
		public:
			GraphicInputLayout& operator=(GraphicInputLayout&& rhs) noexcept = default;

		public:
			void Bind() const;
			void Unbind() const;

		private:
			DxInputElementSet _inputElementSet;
			ComPtr<ID3D11InputLayout> _inputLayout;
		};

		class Shader final : public GraphicObject
		{
			friend ShaderPool;

		private:
			Shader(GraphicDevice& graphicDevice, const GraphicShaderType shaderType);

		public:
			Shader(Shader&& rhs) noexcept = default;
			virtual ~Shader() = default;

		public:
			Shader& operator=(Shader&& rhs) noexcept = default;

		public:
			void Bind() const noexcept;
			void Unbind() const noexcept;

		private:
			ComPtr<ID3D10Blob> _shaderBlob;
			ComPtr<ID3D11DeviceChild> _shader;
			GraphicShaderType _shaderType;
			StringA _hlslFileName;
			StringA _hlslBinaryFileName;
			StringA _entryPoint;

		public:
			static const Shader kNullInstance;
		};


		struct ShaderCompileParam
		{
			const char* _inputFileName = nullptr;
			const char* _outputFileName = nullptr;
			const char* _shaderIdentifier = nullptr;
			const char* _shaderTextContent = nullptr;
		};

		class ShaderPool final : public GraphicObject
		{
			template <typename CustomDataType>
			using TypeMetaData = Language::TypeMetaData<CustomDataType>;
			using TypeCustomData = Language::CppHlsl::TypeCustomData;

			static constexpr const char* const kCompiledShaderFileExtension = ".hlslbin";

		public:
			ShaderPool(GraphicDevice& graphicDevice, DxShaderHeaderMemory* const shaderHeaderMemory, const ShaderVersion shaderVersion);
			ShaderPool(const ShaderPool& rhs) = delete;
			virtual ~ShaderPool() = default;

		public:
			GraphicObjectID AddShaderFromMemory(const char* const shaderIdentifier, const char* const textContent, const char* const entryPoint, const GraphicShaderType shaderType);
			GraphicObjectID AddShader(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const GraphicShaderType shaderType, const char* const outputDirectory = nullptr);
			GraphicObjectID AddInputLayout(const GraphicObjectID& vertexShaderID, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData);
			void RemoveShader(const GraphicObjectID& shaderID);
			void RemoveInputLayout(const GraphicObjectID& shaderID);

		private:
			GraphicObjectID AddShaderInternal(const GraphicShaderType shaderType, Shader& shader);
			GraphicObjectID AddInputLayoutInternal(const Shader& vertexShader, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData);

		private:
			bool CreateShaderInternal(const GraphicShaderType shaderType, Shader& shader);
			bool CreateInputLayoutInternal(const Shader& vertexShader, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData, GraphicInputLayout& outInputLayout);
			void PushInputElement(DxInputElementSet& inputElementSet, const TypeMetaData<TypeCustomData>& outerDataTypeMetaData, const TypeMetaData<TypeCustomData>& memberTypeMetaData);

		private:
			bool CompileShaderFromFile(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const char* const outputDirectory, const GraphicShaderType shaderType, const bool forceCompilation, Shader& inoutShader);
			bool CompileShaderFromFile(const char* const inputShaderFilePath, const char* const entryPoint, const char* const outputShaderFilePath, const GraphicShaderType shaderType, const bool forceCompilation, Shader& inoutShader);
			bool CompileShaderInternalXXX(const GraphicShaderType shaderType, const ShaderCompileParam& compileParam, const char* const entryPoint, ID3D10Blob** outBlob);

		public:
			void RecompileAllShaders();

		private:
			void ReportCompileError();

		public:
			void BindShaderIfNot(const GraphicShaderType shaderType, const GraphicObjectID& objectID);
			void BindInputLayoutIfNot(const GraphicObjectID& objectID);
			void UnbindShader(const GraphicShaderType shaderType);

		private:
			int32 GetShaderIndex(const GraphicShaderType shaderType, const GraphicObjectID& objectID) const;
			int32 GetInputLayoutIndex(const GraphicObjectID& objectID) const;
			uint32 GetShaderCount(const GraphicShaderType shaderType) const;
			GraphicObjectID& AccessBoundShaderID(const GraphicShaderType shaderType);
			const Vector<Shader>& GetShaders(const GraphicShaderType shaderType) const;
			Vector<Shader>& AccessShaders(const GraphicShaderType shaderType);

		private:
			ComPtr<ID3DBlob> _errorMessageBlob;

		private:
			DxShaderHeaderMemory* _shaderHeaderMemory;

		private:
			ShaderVersion _shaderVersion;

		private:
			Vector<GraphicInputLayout> _inputLayouts;
			Vector<Shader> _shadersPerType[static_cast<uint32>(GraphicShaderType::COUNT)];

		private:
			GraphicObjectID	_boundInputLayoutID;
			GraphicObjectID _boundShaderIDPerType[static_cast<uint32>(GraphicShaderType::COUNT)];
		};
	}
}


#endif // !_MINT_RENDERING_BASE_DX_SHADER_H_

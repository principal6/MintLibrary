﻿#pragma once


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
			static const GraphicsInputLayout kNullInstance;

		private:
			GraphicsInputLayout(GraphicsDevice& graphicsDevice);

		public:
			GraphicsInputLayout(GraphicsInputLayout&& rhs) noexcept = default;
		
		public:
			GraphicsInputLayout& operator=(GraphicsInputLayout&& rhs) noexcept = default;

		public:
			void Bind() const;
			void Unbind() const;

		private:
			DxInputElementSet _inputElementSet;
			ComPtr<ID3D11InputLayout> _inputLayout;
		};

		class Shader final : public GraphicsObject
		{
			friend ShaderPool;

		private:
			Shader(GraphicsDevice& graphicsDevice, const GraphicsShaderType shaderType);

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
			GraphicsShaderType _shaderType;
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

		class ShaderPool final : public GraphicsObject
		{
			template <typename CustomDataType>
			using TypeMetaData = Language::TypeMetaData<CustomDataType>;
			using TypeCustomData = Language::CppHlsl::TypeCustomData;

			static constexpr const char* const kCompiledShaderFileExtension = ".hlslbin";

		public:
			ShaderPool(GraphicsDevice& graphicsDevice, DxShaderHeaderMemory* const shaderHeaderMemory, const ShaderVersion shaderVersion);
			ShaderPool(const ShaderPool& rhs) = delete;
			virtual ~ShaderPool() = default;

		public:
			GraphicsObjectID AddShaderFromMemory(const char* const shaderIdentifier, const char* const textContent, const char* const entryPoint, const GraphicsShaderType shaderType);
			GraphicsObjectID AddShader(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const GraphicsShaderType shaderType, const char* const outputDirectory = nullptr);
			GraphicsObjectID AddInputLayout(const GraphicsObjectID& vertexShaderID, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData);
			void RemoveShader(const GraphicsObjectID& shaderID);
			void RemoveInputLayout(const GraphicsObjectID& shaderID);

		private:
			GraphicsObjectID AddShaderInternal(const GraphicsShaderType shaderType, Shader& shader);
			GraphicsObjectID AddInputLayoutInternal(const Shader& vertexShader, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData);

		private:
			bool CreateShaderInternal(const GraphicsShaderType shaderType, Shader& shader);
			bool CreateInputLayoutInternal(const Shader& vertexShader, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData, GraphicsInputLayout& outInputLayout);
			void PushInputElement(DxInputElementSet& inputElementSet, const TypeMetaData<TypeCustomData>& outerDataTypeMetaData, const TypeMetaData<TypeCustomData>& memberTypeMetaData);

		private:
			bool CompileShaderFromFile(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const char* const outputDirectory, const GraphicsShaderType shaderType, const bool forceCompilation, Shader& inoutShader);
			bool CompileShaderFromFile(const char* const inputShaderFilePath, const char* const entryPoint, const char* const outputShaderFilePath, const GraphicsShaderType shaderType, const bool forceCompilation, Shader& inoutShader);
			bool CompileShaderInternalXXX(const GraphicsShaderType shaderType, const ShaderCompileParam& compileParam, const char* const entryPoint, ID3D10Blob** outBlob);

		public:
			void RecompileAllShaders();

		private:
			void ReportCompileError();

		public:
			void BindShaderIfNot(const GraphicsShaderType shaderType, const GraphicsObjectID& objectID);
			void BindInputLayoutIfNot(const GraphicsObjectID& objectID);
			void UnbindShader(const GraphicsShaderType shaderType);

		private:
			int32 GetShaderIndex(const GraphicsShaderType shaderType, const GraphicsObjectID& objectID) const;
			int32 GetInputLayoutIndex(const GraphicsObjectID& objectID) const;
			uint32 GetShaderCount(const GraphicsShaderType shaderType) const;
			GraphicsObjectID& AccessBoundShaderID(const GraphicsShaderType shaderType);
			const Vector<Shader>& GetShaders(const GraphicsShaderType shaderType) const;
			Vector<Shader>& AccessShaders(const GraphicsShaderType shaderType);

		private:
			ComPtr<ID3DBlob> _errorMessageBlob;

		private:
			DxShaderHeaderMemory* _shaderHeaderMemory;

		private:
			ShaderVersion _shaderVersion;

		private:
			Vector<GraphicsInputLayout> _inputLayouts;
			Vector<Shader> _shadersPerType[static_cast<uint32>(GraphicsShaderType::COUNT)];

		private:
			GraphicsObjectID	_boundInputLayoutID;
			GraphicsObjectID _boundShaderIDPerType[static_cast<uint32>(GraphicsShaderType::COUNT)];
		};
	}
}


#endif // !_MINT_RENDERING_BASE_DX_SHADER_H_

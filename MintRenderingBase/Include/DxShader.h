#pragma once


#ifndef _MINT_RENDERING_BASE_DX_SHADER_H_
#define _MINT_RENDERING_BASE_DX_SHADER_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/GraphicObject.h>

#include <MintContainer/Include/Vector.h>


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
		class DxShaderPool;


		using Microsoft::WRL::ComPtr;


		struct DxInputElementSet
		{
			Vector<std::string> _semanticNameArray;
			Vector<D3D11_INPUT_ELEMENT_DESC> _inputElementDescriptorArray;
		};

		enum class DxShaderVersion
		{
			v_4_0,
			v_5_0,
		};


		class GraphicInputLayout final : public GraphicObject
		{
			friend DxShaderPool;

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

		class DxShader final : public GraphicObject
		{
			friend DxShaderPool;

		private:
			DxShader(GraphicDevice& graphicDevice, const GraphicShaderType shaderType);

		public:
			DxShader(DxShader&& rhs) noexcept = default;
			virtual ~DxShader() = default;

		public:
			DxShader& operator=(DxShader&& rhs) noexcept = default;

		public:
			void Bind() const noexcept;
			void Unbind() const noexcept;

		private:
			ComPtr<ID3D10Blob> _shaderBlob;
			ComPtr<ID3D11DeviceChild> _shader;
			GraphicShaderType _shaderType;
			std::string _hlslFileName;
			std::string _hlslBinaryFileName;
			std::string _entryPoint;

		public:
			static const DxShader kNullInstance;
		};


		struct DxShaderCompileParam
		{
			const char* _inputFileName = nullptr;
			const char* _outputFileName = nullptr;
			const char* _shaderIdentifier = nullptr;
			const char* _shaderTextContent = nullptr;
		};

		class DxShaderPool final : public GraphicObject
		{
			template <typename CustomDataType>
			using TypeMetaData = Language::TypeMetaData<CustomDataType>;
			using TypeCustomData = Language::CppHlsl::TypeCustomData;


			static constexpr const char* const kCompiledShaderFileExtension = ".hlslbin";

		public:
			DxShaderPool(GraphicDevice& graphicDevice, DxShaderHeaderMemory* const shaderHeaderMemory, const DxShaderVersion shaderVersion);
			DxShaderPool(const DxShaderPool& rhs) = delete;
			virtual ~DxShaderPool() = default;

		public:
			GraphicObjectID AddShaderFromMemory(const char* const shaderIdentifier, const char* const textContent, const char* const entryPoint, const GraphicShaderType shaderType);
			GraphicObjectID AddShader(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const GraphicShaderType shaderType, const char* const outputDirectory = nullptr);
			GraphicObjectID AddInputLayout(const GraphicObjectID& vertexShaderID, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData);
			void RemoveShader(const GraphicObjectID& shaderID);
			void RemoveInputLayout(const GraphicObjectID& shaderID);

		private:
			GraphicObjectID AddShaderInternal(const GraphicShaderType shaderType, DxShader& shader);
			GraphicObjectID AddInputLayoutInternal(const DxShader& vertexShader, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData);

		private:
			bool CreateShaderInternal(const GraphicShaderType shaderType, DxShader& shader);
			bool CreateInputLayoutInternal(const DxShader& vertexShader, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData, GraphicInputLayout& outInputLayout);
			void PushInputElement(DxInputElementSet& inputElementSet, const TypeMetaData<TypeCustomData>& outerDataTypeMetaData, const TypeMetaData<TypeCustomData>& memberTypeMetaData);

		private:
			bool CompileShaderFromFile(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const char* const outputDirectory, const GraphicShaderType shaderType, const bool forceCompilation, DxShader& inoutShader);
			bool CompileShaderFromFile(const char* const inputShaderFilePath, const char* const entryPoint, const char* const outputShaderFilePath, const GraphicShaderType shaderType, const bool forceCompilation, DxShader& inoutShader);
			bool CompileShaderInternalXXX(const GraphicShaderType shaderType, const DxShaderCompileParam& compileParam, const char* const entryPoint, ID3D10Blob** outBlob);

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
			const Vector<DxShader>& GetShaders(const GraphicShaderType shaderType) const;
			Vector<DxShader>& AccessShaders(const GraphicShaderType shaderType);

		private:
			ComPtr<ID3DBlob> _errorMessageBlob;

		private:
			DxShaderHeaderMemory* _shaderHeaderMemory;

		private:
			DxShaderVersion _shaderVersion;

		private:
			Vector<GraphicInputLayout> _inputLayouts;
			Vector<DxShader> _shadersPerType[static_cast<uint32>(GraphicShaderType::COUNT)];

		private:
			GraphicObjectID	_boundInputLayoutID;
			GraphicObjectID _boundShaderIDPerType[static_cast<uint32>(GraphicShaderType::COUNT)];
		};
	}
}


#endif // !_MINT_RENDERING_BASE_DX_SHADER_H_

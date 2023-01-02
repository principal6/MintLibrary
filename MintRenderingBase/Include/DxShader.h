#pragma once


#ifndef _MINT_RENDERING_BASE_DX_SHADER_H_
#define _MINT_RENDERING_BASE_DX_SHADER_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/IGraphicObject.h>

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


		class GraphicInputLayout final : public IGraphicObject
		{
			friend DxShaderPool;

		public:
			static const GraphicInputLayout kNullInstance;

		public:
			void bind() const;
			void unbind() const;

		private:
			GraphicInputLayout(GraphicDevice& graphicDevice);

		private:
			DxInputElementSet _inputElementSet;
			ComPtr<ID3D11InputLayout> _inputLayout;
		};

		class DxShader final : public IGraphicObject
		{
			friend DxShaderPool;

		private:
			DxShader(GraphicDevice& graphicDevice, const GraphicShaderType shaderType);

		public:
			virtual ~DxShader() = default;

		public:
			void bind() const noexcept;
			void unbind() const noexcept;

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

		class DxShaderPool final : public IGraphicObject
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
			GraphicObjectID pushShaderFromMemory(const char* const shaderIdentifier, const char* const textContent, const char* const entryPoint, const GraphicShaderType shaderType);
			GraphicObjectID pushShader(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const GraphicShaderType shaderType, const char* const outputDirectory = nullptr);
			GraphicObjectID pushInputLayout(const GraphicObjectID& vertexShaderID, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData);

		private:
			GraphicObjectID pushShaderInternal(const GraphicShaderType shaderType, DxShader& shader);
			GraphicObjectID pushInputLayoutInternal(const DxShader& vertexShader, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData);

		private:
			bool createShaderInternal(const GraphicShaderType shaderType, DxShader& shader);
			bool createInputLayoutInternal(const DxShader& vertexShader, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData, GraphicInputLayout& outInputLayout);
			void pushInputElement(DxInputElementSet& inputElementSet, const TypeMetaData<TypeCustomData>& outerDataTypeMetaData, const TypeMetaData<TypeCustomData>& memberTypeMetaData);

		private:
			bool compileShaderFromFile(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const char* const outputDirectory, const GraphicShaderType shaderType, const bool forceCompilation, DxShader& inoutShader);
			bool compileShaderFromFile(const char* const inputShaderFilePath, const char* const entryPoint, const char* const outputShaderFilePath, const GraphicShaderType shaderType, const bool forceCompilation, DxShader& inoutShader);
			bool compileShaderInternalXXX(const GraphicShaderType shaderType, const DxShaderCompileParam& compileParam, const char* const entryPoint, ID3D10Blob** outBlob);

		public:
			void recompileAllShaders();

		private:
			void reportCompileError();

		public:
			void bindShaderIfNot(const GraphicShaderType shaderType, const GraphicObjectID& objectID);
			void bindInputLayoutIfNot(const GraphicObjectID& objectID);
			void unbindShader(const GraphicShaderType shaderType);

		private:
			int32 getShaderIndex(const GraphicShaderType shaderType, const GraphicObjectID& objectID) const;
			int32 getInputLayoutIndex(const GraphicObjectID& objectID) const;
			uint32 getShaderCount(const GraphicShaderType shaderType) const;
			DxShader& accessShader(const GraphicShaderType shaderType, const int32 shaderIndex);
			GraphicObjectID& accessBoundShaderID(const GraphicShaderType shaderType);

		private:
			ComPtr<ID3DBlob> _errorMessageBlob;

		private:
			DxShaderHeaderMemory* _shaderHeaderMemory;

		private:
			DxShaderVersion _shaderVersion;

		private:
			Vector<GraphicInputLayout> _inputLayouts;
			Vector<DxShader> _shaders[static_cast<uint32>(GraphicShaderType::COUNT)];

		private:
			GraphicObjectID	_boundInputLayoutID;
			GraphicObjectID _boundShaderIDs[static_cast<uint32>(GraphicShaderType::COUNT)];
		};
	}
}


#endif // !_MINT_RENDERING_BASE_DX_SHADER_H_

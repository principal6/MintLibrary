#pragma once


#ifndef FS_DX_SHADER_H
#define FS_DX_SHADER_H


#include <CommonDefinitions.h>

#include <FsRenderingBase/Include/IDxObject.h>


namespace fs
{
	namespace Language
	{
		class CppHlslTypeInfo;
	}

	namespace RenderingBase
	{
		class DxShaderHeaderMemory;
		class GraphicDevice;
		class DxShaderPool;


		using Microsoft::WRL::ComPtr;


		struct DxInputElementSet
		{
			std::vector<std::string>				_semanticNameArray;
			std::vector<D3D11_INPUT_ELEMENT_DESC>	_inputElementDescriptorArray;
		};

		enum class DxShaderVersion
		{
			v_4_0,
			v_5_0,
		};


		class DxShader final : public IDxObject
		{
			friend DxShaderPool;

		private:
										DxShader(GraphicDevice* const graphicDevice, const DxShaderType shaderType);

		public:
			virtual						~DxShader() = default;

		public:
			void						bind() const noexcept;

		private:
			DxInputElementSet			_inputElementSet;
			ComPtr<ID3D11InputLayout>	_inputLayout;
			ComPtr<ID3D10Blob>			_shaderBlob;
			ComPtr<ID3D11DeviceChild>	_shader;
			DxShaderType				_shaderType;
			std::string					_identifier;

		public:
			static const DxShader		kNullInstance;
		};

		
		struct DxShaderCompileParam
		{
			const char*		_inputFileName = nullptr;
			const char*		_outputFileName = nullptr;
			const char*		_shaderIdentifier = nullptr;
			const char*		_shaderTextContent = nullptr;
		};

		class DxShaderPool final : public IDxObject
		{
			static constexpr const char* const	kCompiledShaderFileExtension = ".hlslbin";

		public:
										DxShaderPool(GraphicDevice* const graphicDevice, DxShaderHeaderMemory* const shaderHeaderMemory, const DxShaderVersion shaderVersion);
			virtual						~DxShaderPool() = default;

		public:
			const DxObjectId&			pushVertexShaderFromMemory(const char* const shaderIdentifier, const char* const textContent, const char* const entryPoint, const fs::Language::CppHlslTypeInfo* const inputElementTypeInfo);
			const DxObjectId&			pushNonVertexShaderFromMemory(const char* const shaderIdentifier, const char* const textContent, const char* const entryPoint, const DxShaderType shaderType);

		public:
			const DxObjectId&			pushVertexShader(const char* const inputShaderFileName, const char* const entryPoint, const fs::Language::CppHlslTypeInfo* const inputElementTypeInfo, const char* const outputDirectory = nullptr);
			const DxObjectId&			pushNonVertexShader(const char* const inputShaderFileName, const char* const entryPoint, const DxShaderType shaderType, const char* const outputDirectory = nullptr);

		private:
			const DxObjectId&			createVertexShaderInternal(DxShader& shader, const fs::Language::CppHlslTypeInfo* const inputElementTypeInfo);
			const DxObjectId&			createNonVertexShaderInternal(DxShader& shader, const DxShaderType shaderType);

		private:
			const bool					compileShaderFromFile(const char* const inputShaderFileName, const char* const entryPoint, const char* const outputDirectory, const DxShaderType shaderType, DxShader& inoutShader);
			const bool					compileShaderInternalXXX(const DxShaderType shaderType, const DxShaderCompileParam& compileParam, const char* const entryPoint, ID3D10Blob** outBlob);

		private:
			void						reportCompileError();

		public:
			void						bindShader(const DxShaderType shaderType, const DxObjectId& objectId);
			void						unbindShader(const DxShaderType shaderType);
			
		private:
			const DxShader&				getShader(const DxShaderType shaderType, const DxObjectId& objectId);

		private:
			ComPtr<ID3DBlob>			_errorMessageBlob;
		
		private:
			DxShaderHeaderMemory*		_shaderHeaderMemory;

		private:
			DxShaderVersion				_shaderVersion;

		private:
			std::vector<DxShader>		_vertexShaderArray;
			std::vector<DxShader>		_geometryShaderArray;
			std::vector<DxShader>		_pixelShaderArray;

		private:
			DxObjectId					_boundShaderIdArray[static_cast<uint32>(DxShaderType::COUNT)];
		};
	}
}


#endif // !FS_DX_SHADER_H

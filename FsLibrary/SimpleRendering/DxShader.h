#pragma once


#ifndef FS_DX_SHADER_H
#define FS_DX_SHADER_H


#include <FsLibrary/CommonDefinitions.h>

#include <FsLibrary/SimpleRendering/IDxObject.h>


namespace fs
{
	namespace Language
	{
		class CppHlslTypeInfo;
	}

	namespace SimpleRendering
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

		public:
			static const DxShader		kNullInstance;
		};

	
		class DxShaderPool final : public IDxObject
		{
		public:
										DxShaderPool(GraphicDevice* const graphicDevice, DxShaderHeaderMemory* const shaderHeaderMemory);
			virtual						~DxShaderPool() = default;

		public:
			const DxObjectId&			pushVertexShader(const char* const shaderIdentifier, const char* const content, const char* const entryPoint, const DxShaderVersion shaderVersion, const fs::Language::CppHlslTypeInfo* const inputElementTypeInfo);
			const DxObjectId&			pushNonVertexShader(const char* const shaderIdentifier, const char* const content, const char* const entryPoint, const DxShaderVersion shaderVersion, const DxShaderType shaderType);
			
		private:
			const DxObjectId&			reportCompileError();

		public:
			void						bindShader(const DxShaderType shaderType, const DxObjectId& objectId);
			
		private:
			const DxShader&				getShader(const DxShaderType shaderType, const DxObjectId& objectId);

		private:
			ComPtr<ID3DBlob>			_errorMessageBlob;
		
		private:
			DxShaderHeaderMemory*		_shaderHeaderMemory;

		private:
			std::vector<DxShader>		_vertexShaderArray;
			std::vector<DxShader>		_pixelShaderArray;

		private:
			DxObjectId					_boundShaderIdArray[static_cast<uint32>(DxShaderType::COUNT)];
		};
	}
}


#endif // !FS_DX_SHADER_H

#pragma once


#ifndef FS_DX_SHADER_H
#define FS_DX_SHADER_H


#include <CommonDefinitions.h>

#include <SimpleRendering/IDxObject.h>


namespace fs
{
	using Microsoft::WRL::ComPtr;


	class DxShaderHeaderMemory;
	class GraphicDevice;
	class DxShaderPool;

	namespace Language
	{
		class CppHlslTypeInfo;
	}


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
		const DxObjectId&			pushVertexShader(const char* const content, const char* const entryPoint, const DxShaderVersion shaderVersion, const fs::Language::CppHlslTypeInfo* const inputElementTypeInfo);
		const DxObjectId&			pushNonVertexShader(const char* const content, const char* const entryPoint, const DxShaderVersion shaderVersion, const DxShaderType shaderType);

	public:
		const DxShader&				getShader(const DxShaderType shaderType, const DxObjectId& objectId);

	private:
		DxShaderHeaderMemory*		_shaderHeaderMemory;

	public:
		std::vector<DxShader>		_vertexShaderArray;
		std::vector<DxShader>		_pixelShaderArray;
	};
}


#endif // !FS_DX_SHADER_H

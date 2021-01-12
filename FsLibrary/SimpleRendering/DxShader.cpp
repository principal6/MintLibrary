#include <stdafx.h>
#include <FsLibrary/SimpleRendering/DxShader.h>

#include <d3dcompiler.h>

#include <Algorithm.hpp>

#include <FsLibrary/Container/ScopeString.hpp>

#include <FsLibrary/SimpleRendering/GraphicDevice.h>
#include <FsLibrary/SimpleRendering/DxShaderHeaderMemory.h>

#include <FsLibrary/Language/CppHlslParser.h>


#pragma comment(lib, "d3dcompiler.lib")


namespace fs
{
	namespace SimpleRendering
	{
#pragma region Static function definitions
		template<uint32 BufferSize>
		static void makeShaderVersion(fs::ScopeStringA<BufferSize>& out, const DxShaderType shaderType, const DxShaderVersion shaderVersion)
		{
			out.clear();

			if (shaderType == DxShaderType::VertexShader)
			{
				out.append("vs_");
			}
			else if (shaderType == DxShaderType::PixelShader)
			{
				out.append("ps_");
			}

			if (shaderVersion == DxShaderVersion::v_4_0)
			{
				out.append("4_0");
			}
		}
#pragma endregion


		const DxShader DxShader::kNullInstance(nullptr, DxShaderType::VertexShader);
		DxShader::DxShader(GraphicDevice* const graphicDevice, const DxShaderType shaderType)
			: IDxObject(graphicDevice, DxObjectType::Shader), _shaderType{ shaderType }
		{
			__noop;
		}

		void DxShader::bind() const noexcept
		{
			if (_shaderType == DxShaderType::VertexShader)
			{
				_graphicDevice->getDxDeviceContext()->VSSetShader(static_cast<ID3D11VertexShader*>(_shader.Get()), nullptr, 0);
				_graphicDevice->getDxDeviceContext()->IASetInputLayout(_inputLayout.Get());
			}
			else if (_shaderType == DxShaderType::PixelShader)
			{
				_graphicDevice->getDxDeviceContext()->PSSetShader(static_cast<ID3D11PixelShader*>(_shader.Get()), nullptr, 0);
			}
		}


		DxShaderPool::DxShaderPool(GraphicDevice* const graphicDevice, DxShaderHeaderMemory* const shaderHeaderMemory)
			: IDxObject(graphicDevice, DxObjectType::Pool), _shaderHeaderMemory{ shaderHeaderMemory }
		{
			__noop;
		}

		const DxObjectId& DxShaderPool::pushVertexShader(const char* const shaderIdentifier, const char* const content, const char* const entryPoint, const DxShaderVersion shaderVersion, const fs::Language::CppHlslTypeInfo* const inputElementTypeInfo)
		{
			DxShader shader(_graphicDevice, DxShaderType::VertexShader);

			fs::ScopeStringA<20> version;
			makeShaderVersion(version, DxShaderType::VertexShader, shaderVersion);
			if (FAILED(D3DCompile(content, strlen(content), shaderIdentifier, nullptr, _shaderHeaderMemory, entryPoint, version.c_str(), 0, 0, shader._shaderBlob.ReleaseAndGetAddressOf(), _errorMessageBlob.ReleaseAndGetAddressOf())))
			{
				return reportCompileError();
			}

			if (FAILED(_graphicDevice->getDxDevice()->CreateVertexShader(shader._shaderBlob->GetBufferPointer(), shader._shaderBlob->GetBufferSize(), NULL, reinterpret_cast<ID3D11VertexShader**>(shader._shader.ReleaseAndGetAddressOf()))))
			{
				return DxObjectId::kInvalidObjectId;
			}

			// Input Layer
			const uint32 memberCount = inputElementTypeInfo->getMemberCount();
			shader._inputElementSet._semanticNameArray.reserve(memberCount);
			shader._inputElementSet._inputElementDescriptorArray.reserve(memberCount);
			for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
			{
				const Language::CppHlslTypeInfo& memberType = inputElementTypeInfo->getMember(memberIndex);
				shader._inputElementSet._semanticNameArray.emplace_back(Language::CppHlslParser::convertDeclarationNameToHlslSemanticName(memberType.getDeclName()));

				D3D11_INPUT_ELEMENT_DESC inputElementDescriptor;
				inputElementDescriptor.SemanticName = shader._inputElementSet._semanticNameArray[memberIndex].c_str();
				inputElementDescriptor.SemanticIndex = 0;
				inputElementDescriptor.Format = Language::CppHlslParser::convertCppHlslTypeToDxgiFormat(memberType);
				inputElementDescriptor.InputSlot = 0;
				inputElementDescriptor.AlignedByteOffset = memberType.getByteOffset();
				inputElementDescriptor.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
				inputElementDescriptor.InstanceDataStepRate = 0;
				shader._inputElementSet._inputElementDescriptorArray.emplace_back(inputElementDescriptor);
			}
			if (FAILED(_graphicDevice->getDxDevice()->CreateInputLayout(&shader._inputElementSet._inputElementDescriptorArray[0], static_cast<UINT>(shader._inputElementSet._inputElementDescriptorArray.size()),
				shader._shaderBlob->GetBufferPointer(), shader._shaderBlob->GetBufferSize(), shader._inputLayout.ReleaseAndGetAddressOf())))
			{
				return DxObjectId::kInvalidObjectId;
			}

			shader.assignIdXXX();
			_vertexShaderArray.emplace_back(std::move(shader));
			return _vertexShaderArray.back().getId();
		}

		const DxObjectId& DxShaderPool::pushNonVertexShader(const char* const shaderIdentifier, const char* const content, const char* const entryPoint, const DxShaderVersion shaderVersion, const DxShaderType shaderType)
		{
			DxShader shader(_graphicDevice, shaderType);

			fs::ScopeStringA<20> version;
			makeShaderVersion(version, shaderType, shaderVersion);
			if (FAILED(D3DCompile(content, strlen(content), shaderIdentifier, nullptr, _shaderHeaderMemory, entryPoint, version.c_str(), 0, 0, shader._shaderBlob.ReleaseAndGetAddressOf(), _errorMessageBlob.ReleaseAndGetAddressOf())))
			{
				return reportCompileError();
			}
		
			if (shaderType == DxShaderType::PixelShader)
			{
				if (FAILED(_graphicDevice->getDxDevice()->CreatePixelShader(shader._shaderBlob->GetBufferPointer(), shader._shaderBlob->GetBufferSize(), NULL, reinterpret_cast<ID3D11PixelShader**>(shader._shader.ReleaseAndGetAddressOf()))))
				{
					return DxObjectId::kInvalidObjectId;
				}

				shader.assignIdXXX();
				_pixelShaderArray.emplace_back(std::move(shader));
				return _pixelShaderArray.back().getId();
			}

			return DxObjectId::kInvalidObjectId;
		}

		const DxObjectId& DxShaderPool::reportCompileError()
		{
			std::string errorMessages(reinterpret_cast<char*>(_errorMessageBlob->GetBufferPointer()));
			
			const size_t firstNewLinePos = errorMessages.find('\n');
			const size_t secondNewLinePos = errorMessages.find('\n', firstNewLinePos + 1);
			errorMessages = errorMessages.substr(0, secondNewLinePos);
			FS_LOG_ERROR("±èÀå¿ø", "Shader Compile Error\n\n%s", errorMessages.c_str());

			return DxObjectId::kInvalidObjectId;
		}

		void DxShaderPool::bindShader(const DxShaderType shaderType, const DxObjectId& objectId)
		{
			const uint32 shaderTypeIndex = static_cast<uint32>(shaderType);
			if (_boundShaderIdArray[shaderTypeIndex] != objectId)
			{
				_boundShaderIdArray[shaderTypeIndex] = objectId;

				getShader(shaderType, objectId).bind();
			}
		}

		const DxShader& DxShaderPool::getShader(const DxShaderType shaderType, const DxObjectId& objectId)
		{
			FS_ASSERT("±èÀå¿ø", objectId.isObjectType(DxObjectType::Shader) == true, "Invalid parameter - ObjectType !!");

			if (shaderType == DxShaderType::VertexShader)
			{
				const int32 index = fs::binarySearch(_vertexShaderArray, objectId);
				if (0 <= index)
				{
					return _vertexShaderArray[index];
				}
			}
			else if (shaderType == DxShaderType::PixelShader)
			{
				const int32 index = fs::binarySearch(_pixelShaderArray, objectId);
				if (0 <= index)
				{
					return _pixelShaderArray[index];
				}
			}
			return DxShader::kNullInstance;
		}
	}
}

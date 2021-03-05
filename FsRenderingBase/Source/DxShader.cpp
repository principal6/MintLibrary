#include <stdafx.h>
#include <FsRenderingBase/Include/DxShader.h>

#include <d3dcompiler.h>

#include <FsLibrary/Algorithm.hpp>

#include <FsContainer/Include/ScopeString.hpp>
#include <FsContainer/Include/StringUtil.hpp>

#include <FsPlatform/Include/TextFile.h>
#include <FsPlatform/Include/FileUtil.hpp>

#include <FsRenderingBase/Include/GraphicDevice.h>
#include <FsRenderingBase/Include/DxShaderHeaderMemory.h>

#include <FsRenderingBase/Include/Language/CppHlslParser.h>


#pragma comment(lib, "d3dcompiler.lib")


namespace fs
{
	namespace RenderingBase
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
			else if (shaderType == DxShaderType::GeometryShader)
			{
				out.append("gs_");
			}
			else if (shaderType == DxShaderType::PixelShader)
			{
				out.append("ps_");
			}

			if (shaderVersion == DxShaderVersion::v_4_0)
			{
				out.append("4_0");
			}
			else if (shaderVersion == DxShaderVersion::v_5_0)
			{
				out.append("5_0");
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
			else if (_shaderType == DxShaderType::GeometryShader)
			{
				_graphicDevice->getDxDeviceContext()->GSSetShader(static_cast<ID3D11GeometryShader*>(_shader.Get()), nullptr, 0);
			}
			else if (_shaderType == DxShaderType::PixelShader)
			{
				_graphicDevice->getDxDeviceContext()->PSSetShader(static_cast<ID3D11PixelShader*>(_shader.Get()), nullptr, 0);
			}
		}


		DxShaderPool::DxShaderPool(GraphicDevice* const graphicDevice, DxShaderHeaderMemory* const shaderHeaderMemory, const DxShaderVersion shaderVersion)
			: IDxObject(graphicDevice, DxObjectType::Pool)
			, _shaderHeaderMemory{ shaderHeaderMemory }
			, _shaderVersion{ shaderVersion }
		{
			__noop;
		}

		const DxObjectId& DxShaderPool::pushVertexShaderFromMemory(const char* const shaderIdentifier, const char* const textContent, const char* const entryPoint, const fs::Language::CppHlslTypeInfo* const inputElementTypeInfo)
		{
			DxShader shader(_graphicDevice, DxShaderType::VertexShader);

			DxShaderCompileParam compileParam;
			compileParam._shaderIdentifier = shaderIdentifier;
			compileParam._shaderTextContent = textContent;
			if (compileShaderInternalXXX(DxShaderType::VertexShader, compileParam, entryPoint, shader._shaderBlob.ReleaseAndGetAddressOf()) == false)
			{
				return DxObjectId::kInvalidObjectId;
			}
			shader._identifier = shaderIdentifier;

			return createVertexShaderInternal(shader, inputElementTypeInfo);
		}

		const DxObjectId& DxShaderPool::pushNonVertexShaderFromMemory(const char* const shaderIdentifier, const char* const textContent, const char* const entryPoint, const DxShaderType shaderType)
		{
			DxShader shader(_graphicDevice, shaderType);

			DxShaderCompileParam compileParam;
			compileParam._shaderIdentifier = shaderIdentifier;
			compileParam._shaderTextContent = textContent;
			if (compileShaderInternalXXX(shaderType, compileParam, entryPoint, shader._shaderBlob.ReleaseAndGetAddressOf()) == false)
			{
				return DxObjectId::kInvalidObjectId;
			}
			shader._identifier = shaderIdentifier;

			return createNonVertexShaderInternal(shader, shaderType);
		}

		const DxObjectId& DxShaderPool::pushVertexShader(const char* const inputShaderFileName, const char* const entryPoint, const fs::Language::CppHlslTypeInfo* const inputElementTypeInfo, const char* const outputDirectory)
		{
			DxShader shader(_graphicDevice, DxShaderType::VertexShader);
			if (compileShaderFromFile(inputShaderFileName, entryPoint, outputDirectory, DxShaderType::VertexShader, shader) == false)
			{
				return DxObjectId::kInvalidObjectId;
			}
			return createVertexShaderInternal(shader, inputElementTypeInfo);
		}

		const DxObjectId& DxShaderPool::pushNonVertexShader(const char* const inputShaderFileName, const char* const entryPoint, const DxShaderType shaderType, const char* const outputDirectory)
		{
			DxShader shader(_graphicDevice, shaderType);
			if (compileShaderFromFile(inputShaderFileName, entryPoint, outputDirectory, shaderType, shader) == false)
			{
				return DxObjectId::kInvalidObjectId;
			}
			return createNonVertexShaderInternal(shader, shaderType);
		}

		const DxObjectId& DxShaderPool::createVertexShaderInternal(DxShader& shader, const fs::Language::CppHlslTypeInfo* const inputElementTypeInfo)
		{
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
				FS_LOG_ERROR("김장원", "VertexShader [[%s]] 의 InputLayout 생성에 실패했습니다. Input 자료형 으로 [[%s]] 을 쓰는게 맞는지 확인해 주세요.", shader._identifier.c_str(), inputElementTypeInfo->getTypeName().c_str());
				return DxObjectId::kInvalidObjectId;
			}

			shader.assignIdXXX();
			_vertexShaderArray.emplace_back(std::move(shader));
			return _vertexShaderArray.back().getId();
		}

		const DxObjectId& DxShaderPool::createNonVertexShaderInternal(DxShader& shader, const DxShaderType shaderType)
		{
			if (shaderType == DxShaderType::GeometryShader)
			{
				if (FAILED(_graphicDevice->getDxDevice()->CreateGeometryShader(shader._shaderBlob->GetBufferPointer(), shader._shaderBlob->GetBufferSize(), NULL, reinterpret_cast<ID3D11GeometryShader**>(shader._shader.ReleaseAndGetAddressOf()))))
				{
					return DxObjectId::kInvalidObjectId;
				}

				shader.assignIdXXX();
				_geometryShaderArray.emplace_back(std::move(shader));
				return _geometryShaderArray.back().getId();
			}
			else if (shaderType == DxShaderType::PixelShader)
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

		const bool DxShaderPool::compileShaderFromFile(const char* const inputShaderFileName, const char* const entryPoint, const char* const outputDirectory, const DxShaderType shaderType, DxShader& inoutShader)
		{
			std::string compiledShaderFileName{ inputShaderFileName };
			fs::StringUtil::excludeExtension(compiledShaderFileName);

			if (fs::FileUtil::exists(inputShaderFileName) == false)
			{
				FS_LOG_ERROR("김장원", "Input shader file not found : %s", inputShaderFileName);
				return false;
			}

			if (outputDirectory != nullptr)
			{
				compiledShaderFileName = outputDirectory + compiledShaderFileName;
			}
			compiledShaderFileName.append(kCompiledShaderFileExtension);

			if (fs::FileUtil::exists(compiledShaderFileName.c_str()) == false)
			{
				DxShaderCompileParam compileParam;
				compileParam._inputFileName = inputShaderFileName;
				compileParam._outputFileName = compiledShaderFileName.c_str();
				if (compileShaderInternalXXX(shaderType, compileParam, entryPoint, inoutShader._shaderBlob.ReleaseAndGetAddressOf()) == false)
				{
					return false;
				}

				inoutShader._identifier = inputShaderFileName;
			}
			else
			{
				std::wstring compiledShaderFileNameWide;
				fs::StringUtil::convertStringToWideString(compiledShaderFileName, compiledShaderFileNameWide);

				if (FAILED(D3DReadFileToBlob(compiledShaderFileNameWide.c_str(), inoutShader._shaderBlob.ReleaseAndGetAddressOf())))
				{
					return false;
				}

				inoutShader._identifier = compiledShaderFileName.c_str();
			}

			return true;
		}

		const bool DxShaderPool::compileShaderInternalXXX(const DxShaderType shaderType, const DxShaderCompileParam& compileParam, const char* const entryPoint, ID3D10Blob** outBlob)
		{
			fs::ScopeStringA<20> version;
			makeShaderVersion(version, shaderType, _shaderVersion);

			const char* content{};
			const char* identifier{};
			uint32 contentLength{};
			fs::TextFileReader textFileReader;
			if (compileParam._inputFileName != nullptr)
			{
				if (textFileReader.open(compileParam._inputFileName) == false)
				{
					FS_LOG_ERROR("김장원", "Input file not found : %s", compileParam._inputFileName);
					return false;
				}

				content = textFileReader.get();
				contentLength = textFileReader.getFileSize();
				identifier = compileParam._inputFileName;
			}
			else
			{
				content = compileParam._shaderTextContent;
				contentLength = fs::StringUtil::strlen(compileParam._shaderTextContent);
				identifier = compileParam._shaderIdentifier;
			}

			if (FAILED(D3DCompile(content, contentLength, identifier, nullptr, _shaderHeaderMemory, entryPoint, version.c_str(), 0, 0, outBlob, _errorMessageBlob.ReleaseAndGetAddressOf())))
			{
				reportCompileError();
				return false;
			}

			if (compileParam._outputFileName != nullptr)
			{
				std::wstring outputFileNameWideString;
				fs::StringUtil::convertStringToWideString(compileParam._outputFileName, outputFileNameWideString);
				if (FAILED(D3DWriteBlobToFile(*outBlob, outputFileNameWideString.c_str(), TRUE)))
				{
					return false;
				}
			}

			return true;
		}

		void DxShaderPool::reportCompileError()
		{
			std::string errorMessages(reinterpret_cast<char*>(_errorMessageBlob->GetBufferPointer()));
			
			const size_t firstNewLinePos = errorMessages.find('\n');
			const size_t secondNewLinePos = errorMessages.find('\n', firstNewLinePos + 1);
			errorMessages = errorMessages.substr(0, secondNewLinePos);

			FS_LOG_ERROR("김장원", "Shader Compile Error\n\n%s", errorMessages.c_str());
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

		void DxShaderPool::unbindShader(const DxShaderType shaderType)
		{
			switch (shaderType)
			{
			case fs::RenderingBase::DxShaderType::VertexShader:
				_graphicDevice->getDxDeviceContext()->VSSetShader(nullptr, nullptr, 0);
				_graphicDevice->getDxDeviceContext()->IASetInputLayout(nullptr);
				break;
			case fs::RenderingBase::DxShaderType::GeometryShader:
				_graphicDevice->getDxDeviceContext()->GSSetShader(nullptr, nullptr, 0);
				break;
			case fs::RenderingBase::DxShaderType::PixelShader:
				_graphicDevice->getDxDeviceContext()->PSSetShader(nullptr, nullptr, 0);
				break;
			case fs::RenderingBase::DxShaderType::COUNT:
				break;
			default:
				break;
			}

			const uint32 shaderTypeIndex = static_cast<uint32>(shaderType);
			_boundShaderIdArray[shaderTypeIndex] = DxObjectId();
		}

		const DxShader& DxShaderPool::getShader(const DxShaderType shaderType, const DxObjectId& objectId)
		{
			FS_ASSERT("김장원", objectId.isObjectType(DxObjectType::Shader) == true, "Invalid parameter - ObjectType !!");

			if (shaderType == DxShaderType::VertexShader)
			{
				const int32 index = fs::binarySearch(_vertexShaderArray, objectId);
				if (0 <= index)
				{
					return _vertexShaderArray[index];
				}
			}
			else if (shaderType == DxShaderType::GeometryShader)
			{
				const int32 index = fs::binarySearch(_geometryShaderArray, objectId);
				if (0 <= index)
				{
					return _geometryShaderArray[index];
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

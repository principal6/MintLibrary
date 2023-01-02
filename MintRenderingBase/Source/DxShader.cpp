#include <MintRenderingBase/Include/DxShader.h>

#include <d3dcompiler.h>

#include <MintLibrary/Include/Algorithm.hpp>

#include <MintContainer/Include/StackString.hpp>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/Vector.hpp>

#include <MintPlatform/Include/TextFile.h>
#include <MintPlatform/Include/FileUtil.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/DxShaderHeaderMemory.h>

#include <MintRenderingBase/Include/CppHlsl/Parser.h>


#pragma comment(lib, "d3dcompiler.lib")


namespace mint
{
	namespace Rendering
	{
#pragma region Static function definitions
		template<uint32 BufferSize>
		static void makeShaderVersion(StackStringA<BufferSize>& out, const GraphicShaderType shaderType, const DxShaderVersion shaderVersion)
		{
			out.clear();

			if (shaderType == GraphicShaderType::VertexShader)
			{
				out.append("vs_");
			}
			else if (shaderType == GraphicShaderType::GeometryShader)
			{
				out.append("gs_");
			}
			else if (shaderType == GraphicShaderType::PixelShader)
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


#pragma region GraphicInputLayout
		const GraphicInputLayout GraphicInputLayout::kNullInstance{ GraphicDevice::getInvalidInstance() };
		GraphicInputLayout::GraphicInputLayout(GraphicDevice& graphicDevice)
			: GraphicObject(graphicDevice, GraphicObjectType::InputLayout)
		{
			__noop;
		}

		void GraphicInputLayout::bind() const
		{
			_graphicDevice.getStateManager().setIAInputLayout(_inputLayout.Get());
		}

		void GraphicInputLayout::unbind() const
		{
			_graphicDevice.getStateManager().setIAInputLayout(nullptr);
		}
#pragma endregion


#pragma region DxShader
		const DxShader DxShader::kNullInstance{ GraphicDevice::getInvalidInstance(), GraphicShaderType::VertexShader };
		DxShader::DxShader(GraphicDevice& graphicDevice, const GraphicShaderType shaderType)
			: GraphicObject(graphicDevice, GraphicObjectType::Shader), _shaderType{ shaderType }
		{
			__noop;
		}

		void DxShader::bind() const noexcept
		{
			if (_shaderType == GraphicShaderType::VertexShader)
			{
				_graphicDevice.getStateManager().setVSShader(static_cast<ID3D11VertexShader*>(_shader.Get()));
			}
			else if (_shaderType == GraphicShaderType::GeometryShader)
			{
				_graphicDevice.getStateManager().setGSShader(static_cast<ID3D11GeometryShader*>(_shader.Get()));
			}
			else if (_shaderType == GraphicShaderType::PixelShader)
			{
				_graphicDevice.getStateManager().setPSShader(static_cast<ID3D11PixelShader*>(_shader.Get()));
			}
		}

		void DxShader::unbind() const noexcept
		{
			if (_shaderType == GraphicShaderType::VertexShader)
			{
				_graphicDevice.getStateManager().setVSShader(nullptr);
			}
			else if (_shaderType == GraphicShaderType::GeometryShader)
			{
				_graphicDevice.getStateManager().setGSShader(nullptr);
			}
			else if (_shaderType == GraphicShaderType::PixelShader)
			{
				_graphicDevice.getStateManager().setPSShader(nullptr);
			}
		}
#pragma endregion


		DxShaderPool::DxShaderPool(GraphicDevice& graphicDevice, DxShaderHeaderMemory* const shaderHeaderMemory, const DxShaderVersion shaderVersion)
			: GraphicObject(graphicDevice, GraphicObjectType::Pool)
			, _shaderHeaderMemory{ shaderHeaderMemory }
			, _shaderVersion{ shaderVersion }
		{
			__noop;
		}

		GraphicObjectID DxShaderPool::pushShaderFromMemory(const char* const shaderIdentifier, const char* const textContent, const char* const entryPoint, const GraphicShaderType shaderType)
		{
			DxShader shader(_graphicDevice, shaderType);

			DxShaderCompileParam compileParam;
			compileParam._shaderIdentifier = shaderIdentifier;
			compileParam._shaderTextContent = textContent;
			if (compileShaderInternalXXX(shaderType, compileParam, entryPoint, shader._shaderBlob.ReleaseAndGetAddressOf()) == false)
			{
				return GraphicObjectID::kInvalidGraphicObjectID;
			}
			shader._hlslFileName = shaderIdentifier;

			return pushShaderInternal(shaderType, shader);
		}

		GraphicObjectID DxShaderPool::pushShader(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const GraphicShaderType shaderType, const char* const outputDirectory)
		{
			DxShader shader(_graphicDevice, shaderType);
			if (compileShaderFromFile(inputDirectory, inputShaderFileName, entryPoint, outputDirectory, shaderType, false, shader) == false)
			{
				return GraphicObjectID::kInvalidGraphicObjectID;
			}
			return pushShaderInternal(shaderType, shader);
		}

		GraphicObjectID DxShaderPool::pushInputLayout(const GraphicObjectID& vertexShaderID, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData)
		{
			const int32 vertexShaderIndex = getShaderIndex(GraphicShaderType::VertexShader, vertexShaderID);
			if (isValidIndex(vertexShaderIndex) == false)
			{
				return GraphicObjectID::kInvalidGraphicObjectID;
			}

			const DxShader& vertexShader = accessShaders(GraphicShaderType::VertexShader)[vertexShaderIndex];
			return pushInputLayoutInternal(vertexShader, inputElementTypeMetaData);
		}

		GraphicObjectID DxShaderPool::pushShaderInternal(const GraphicShaderType shaderType, DxShader& shader)
		{
			if (createShaderInternal(shaderType, shader) == false)
			{
				return GraphicObjectID::kInvalidGraphicObjectID;
			}

			shader.assignIDXXX();
			const GraphicObjectID graphicObjectID = shader.getID();
			accessShaders(shaderType).push_back(std::move(shader));
			quickSort(accessShaders(shaderType), GraphicObject::AscendingComparator());
			return graphicObjectID;
		}

		GraphicObjectID DxShaderPool::pushInputLayoutInternal(const DxShader& vertexShader, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData)
		{
			GraphicInputLayout inputLayout(_graphicDevice);
			if (createInputLayoutInternal(vertexShader, inputElementTypeMetaData, inputLayout) == false)
			{
				return GraphicObjectID::kInvalidGraphicObjectID;
			}

			inputLayout.assignIDXXX();
			const GraphicObjectID graphicObjectID = inputLayout.getID();
			_inputLayouts.push_back(std::move(inputLayout));
			quickSort(_inputLayouts, GraphicObject::AscendingComparator());
			return graphicObjectID;
		}

		bool DxShaderPool::createShaderInternal(const GraphicShaderType shaderType, DxShader& shader)
		{
			switch (shaderType)
			{
			case GraphicShaderType::VertexShader:
			{
				if (FAILED(_graphicDevice.getDxDevice()->CreateVertexShader(shader._shaderBlob->GetBufferPointer(), shader._shaderBlob->GetBufferSize(), NULL, reinterpret_cast<ID3D11VertexShader**>(shader._shader.ReleaseAndGetAddressOf()))))
				{
					return false;
				}
				return true;
			}
			case GraphicShaderType::GeometryShader:
			{
				if (FAILED(_graphicDevice.getDxDevice()->CreateGeometryShader(shader._shaderBlob->GetBufferPointer(), shader._shaderBlob->GetBufferSize(), NULL, reinterpret_cast<ID3D11GeometryShader**>(shader._shader.ReleaseAndGetAddressOf()))))
				{
					return false;
				}
				return true;
			}
			case GraphicShaderType::PixelShader:
			{
				if (FAILED(_graphicDevice.getDxDevice()->CreatePixelShader(shader._shaderBlob->GetBufferPointer(), shader._shaderBlob->GetBufferSize(), NULL, reinterpret_cast<ID3D11PixelShader**>(shader._shader.ReleaseAndGetAddressOf()))))
				{
					return false;
				}
				return true;
			}
			case GraphicShaderType::COUNT:
				break;
			default:
				break;
			}
			return false;
		}

		bool DxShaderPool::createInputLayoutInternal(const DxShader& vertexShader, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData, GraphicInputLayout& outInputLayout)
		{
			outInputLayout._inputElementSet._semanticNameArray.clear();
			outInputLayout._inputElementSet._inputElementDescriptorArray.clear();

			{
				const uint32 memberCount = inputElementTypeMetaData.getMemberCount();
				for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
				{
					const TypeMetaData<TypeCustomData>& memberTypeMetaData = inputElementTypeMetaData.getMember(memberIndex);
					pushInputElement(outInputLayout._inputElementSet, inputElementTypeMetaData, memberTypeMetaData);
				}
			}

			// Input slot 처리
			const uint32 slottedStreamDataCount = inputElementTypeMetaData._customData.getSlottedStreamDataCount();
			for (uint32 slottedStreamDataIndex = 0; slottedStreamDataIndex < slottedStreamDataCount; ++slottedStreamDataIndex)
			{
				const TypeMetaData<TypeCustomData>& slottedStreamData = inputElementTypeMetaData._customData.getSlottedStreamData(slottedStreamDataIndex);
				const uint32 memberCount = slottedStreamData.getMemberCount();
				for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
				{
					pushInputElement(outInputLayout._inputElementSet, slottedStreamData, slottedStreamData.getMember(memberIndex));
				}
			}

			if (FAILED(_graphicDevice.getDxDevice()->CreateInputLayout(outInputLayout._inputElementSet._inputElementDescriptorArray.data(), static_cast<UINT>(outInputLayout._inputElementSet._inputElementDescriptorArray.size()),
				vertexShader._shaderBlob->GetBufferPointer(), vertexShader._shaderBlob->GetBufferSize(), outInputLayout._inputLayout.ReleaseAndGetAddressOf())))
			{
				MINT_LOG_ERROR("VertexShader [[%s]] 의 InputLayout 생성에 실패했습니다. Input 자료형 으로 [[%s]] 을 쓰는게 맞는지 확인해 주세요.", vertexShader._hlslFileName.c_str(), inputElementTypeMetaData.getTypeName().c_str());
				return false;
			}
			return true;
		}

		void DxShaderPool::pushInputElement(DxInputElementSet& inputElementSet, const TypeMetaData<TypeCustomData>& outerDataTypeMetaData, const TypeMetaData<TypeCustomData>& memberTypeMetaData)
		{
			inputElementSet._semanticNameArray.push_back(Language::CppHlsl::Parser::convertDeclarationNameToHlslSemanticName(memberTypeMetaData.getDeclName()));

			D3D11_INPUT_ELEMENT_DESC inputElementDescriptor;
			inputElementDescriptor.SemanticName = inputElementSet._semanticNameArray.back().c_str();
			inputElementDescriptor.SemanticIndex = 0;
			inputElementDescriptor.Format = Language::CppHlsl::Parser::convertCppHlslTypeToDxgiFormat(memberTypeMetaData);
			inputElementDescriptor.InputSlot = memberTypeMetaData._customData.getInputSlot();
			inputElementDescriptor.AlignedByteOffset = memberTypeMetaData.getByteOffset();
			inputElementDescriptor.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
			inputElementDescriptor.InstanceDataStepRate = outerDataTypeMetaData._customData.getInstanceDataStepRate();
			if (inputElementDescriptor.InstanceDataStepRate > 0)
			{
				inputElementDescriptor.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
			}

			inputElementSet._inputElementDescriptorArray.push_back(inputElementDescriptor);
		}

		bool DxShaderPool::compileShaderFromFile(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const char* const outputDirectory, const GraphicShaderType shaderType, const bool forceCompilation, DxShader& inoutShader)
		{
			StringA inputShaderFilePath{ inputDirectory };
			inputShaderFilePath += inputShaderFileName;
			if (FileUtil::exists(inputShaderFilePath.c_str()) == false)
			{
				MINT_LOG_ERROR("Input shader file not found : %s", inputShaderFilePath.c_str());
				return false;
			}

			std::string outputShaderFilePath{ inputShaderFileName };
			StringUtil::excludeExtension(outputShaderFilePath);
			if (outputDirectory != nullptr)
			{
				if (FileUtil::exists(outputDirectory) == false)
				{
					MINT_ASSERT(FileUtil::createDirectory(outputDirectory) == true, "경로 생성에 실패했습니다!");
				}

				outputShaderFilePath = outputDirectory + outputShaderFilePath;
			}
			else
			{
				outputShaderFilePath = inputDirectory + outputShaderFilePath;
			}
			outputShaderFilePath.append(kCompiledShaderFileExtension);

			return compileShaderFromFile(inputShaderFilePath.c_str(), entryPoint, outputShaderFilePath.c_str(), shaderType, forceCompilation, inoutShader);
		}

		bool DxShaderPool::compileShaderFromFile(const char* const inputShaderFilePath, const char* const entryPoint, const char* const outputShaderFilePath, const GraphicShaderType shaderType, const bool forceCompilation, DxShader& inoutShader)
		{
			if (StringUtil::contains(inputShaderFilePath, ".hlsl") == false)
			{
				return false;
			}

			if (FileUtil::exists(outputShaderFilePath) == false || forceCompilation == true)
			{
				DxShaderCompileParam compileParam;
				compileParam._inputFileName = inputShaderFilePath;
				compileParam._outputFileName = outputShaderFilePath;
				if (compileShaderInternalXXX(shaderType, compileParam, entryPoint, inoutShader._shaderBlob.ReleaseAndGetAddressOf()) == false)
				{
					return false;
				}
			}
			else
			{
				StringW compiledShaderFileNameW;
				StringUtil::convertStringAToStringW(outputShaderFilePath, compiledShaderFileNameW);
				if (FAILED(D3DReadFileToBlob(compiledShaderFileNameW.c_str(), inoutShader._shaderBlob.ReleaseAndGetAddressOf())))
				{
					return false;
				}
			}

			inoutShader._entryPoint = entryPoint;
			inoutShader._hlslFileName = inputShaderFilePath;
			inoutShader._hlslBinaryFileName = outputShaderFilePath;
			return true;
		}

		bool DxShaderPool::compileShaderInternalXXX(const GraphicShaderType shaderType, const DxShaderCompileParam& compileParam, const char* const entryPoint, ID3D10Blob** outBlob)
		{
			StackStringA<20> version;
			makeShaderVersion(version, shaderType, _shaderVersion);

			const char* content{};
			const char* identifier{};
			uint32 contentLength{};
			if (compileParam._inputFileName == nullptr)
			{
				content = compileParam._shaderTextContent;
				contentLength = StringUtil::length(compileParam._shaderTextContent);
				identifier = compileParam._shaderIdentifier;
			}
			else
			{
				TextFileReader textFileReader;
				if (textFileReader.open(compileParam._inputFileName) == false)
				{
					MINT_LOG_ERROR("Input file not found : %s", compileParam._inputFileName);
					return false;
				}

				content = textFileReader.get();
				contentLength = textFileReader.getFileSize();
				identifier = compileParam._inputFileName;
			}

			const UINT debugFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
			if (FAILED(D3DCompile(content, contentLength, identifier, nullptr, _shaderHeaderMemory, entryPoint, version.c_str(), debugFlag, 0, outBlob, _errorMessageBlob.ReleaseAndGetAddressOf())))
			{
				reportCompileError();
				return false;
			}

			if (compileParam._outputFileName != nullptr)
			{
				std::wstring outputFileNameWideString;
				StringUtil::convertStringToWideString(compileParam._outputFileName, outputFileNameWideString);
				if (FAILED(D3DWriteBlobToFile(*outBlob, outputFileNameWideString.c_str(), TRUE)))
				{
					return false;
				}
			}
			return true;
		}

		void DxShaderPool::recompileAllShaders()
		{
			const uint32 shaderTypeCount = static_cast<uint32>(GraphicShaderType::COUNT);
			for (uint32 shaderTypeIndex = 0; shaderTypeIndex < shaderTypeCount; ++shaderTypeIndex)
			{
				const GraphicShaderType shaderType = static_cast<GraphicShaderType>(shaderTypeIndex);
				const GraphicObjectID& boundShaderID = accessBoundShaderID(shaderType);
				if (boundShaderID.isValid())
				{
					const int32 boundShaderIndex = getShaderIndex(shaderType, boundShaderID);
					accessShaders(shaderType)[boundShaderIndex].unbind();
				}

				const uint32 shaderCount = getShaderCount(shaderType);
				for (uint32 shaderIndex = 0; shaderIndex < shaderCount; ++shaderIndex)
				{
					DxShader& shader = accessShaders(shaderType)[shaderIndex];
					compileShaderFromFile(shader._hlslFileName.c_str(), shader._entryPoint.c_str(), shader._hlslBinaryFileName.c_str(), shader._shaderType, true, shader);
					createShaderInternal(shaderType, shader);
				}
			}

			for (uint32 shaderTypeIndex = 0; shaderTypeIndex < shaderTypeCount; ++shaderTypeIndex)
			{
				const GraphicShaderType shaderType = static_cast<GraphicShaderType>(shaderTypeIndex);
				const GraphicObjectID& boundShaderID = accessBoundShaderID(shaderType);
				if (boundShaderID.isValid())
				{
					const int32 boundShaderIndex = getShaderIndex(shaderType, boundShaderID);
					accessShaders(shaderType)[boundShaderIndex].bind();
				}
			}
		}

		void DxShaderPool::reportCompileError()
		{
			std::string errorMessages(reinterpret_cast<char*>(_errorMessageBlob->GetBufferPointer()));

			const size_t firstNewLinePos = errorMessages.find('\n');
			const size_t secondNewLinePos = errorMessages.find('\n', firstNewLinePos + 1);
			errorMessages = errorMessages.substr(0, secondNewLinePos);

			MINT_LOG_ERROR("Shader Compile Error\n\n%s", errorMessages.c_str());
		}

		void DxShaderPool::bindShaderIfNot(const GraphicShaderType shaderType, const GraphicObjectID& objectID)
		{
			GraphicObjectID& boundShaderID = accessBoundShaderID(shaderType);
			if (boundShaderID == objectID)
			{
				return;
			}

			const int32 shaderIndex = getShaderIndex(shaderType, objectID);
			if (isValidIndex(shaderIndex) == false)
			{
				return;
			}

			boundShaderID = objectID;
			accessShaders(shaderType)[shaderIndex].bind();
		}

		void DxShaderPool::bindInputLayoutIfNot(const GraphicObjectID& objectID)
		{
			if (_boundInputLayoutID == objectID)
			{
				return;
			}

			const int32 inputLayoutIndex = getInputLayoutIndex(objectID);
			if (inputLayoutIndex < 0)
			{
				return;
			}

			_inputLayouts[inputLayoutIndex].bind();
			_boundInputLayoutID = objectID;
		}

		void DxShaderPool::unbindShader(const GraphicShaderType shaderType)
		{
			if (shaderType == GraphicShaderType::COUNT)
			{
				return;
			}

			GraphicObjectID& boundShaderID = accessBoundShaderID(shaderType);
			if (boundShaderID.isValid() == false)
			{
				return;
			}

			const int32 shaderIndex = getShaderIndex(shaderType, boundShaderID);
			if (isValidIndex(shaderIndex) == false)
			{
				MINT_NEVER;
				return;
			}

			boundShaderID.invalidate();
			accessShaders(shaderType)[shaderIndex].unbind();
		}

		int32 DxShaderPool::getShaderIndex(const GraphicShaderType shaderType, const GraphicObjectID& objectID) const
		{
			MINT_ASSERT(shaderType != GraphicShaderType::COUNT, "Invalid parameter - check ShaderType");
			MINT_ASSERT(objectID.isObjectType(GraphicObjectType::Shader) == true, "Invalid parameter - check ObjectType");
			return binarySearch(getShaders(shaderType), objectID, GraphicObject::Evaluator());
		}

		int32 DxShaderPool::getInputLayoutIndex(const GraphicObjectID& objectID) const
		{
			MINT_ASSERT(objectID.isObjectType(GraphicObjectType::InputLayout) == true, "Invalid parameter - check ObjectType");
			return binarySearch(_inputLayouts, objectID, GraphicObject::Evaluator());
		}

		uint32 DxShaderPool::getShaderCount(const GraphicShaderType shaderType) const
		{
			MINT_ASSERT(shaderType != GraphicShaderType::COUNT, "Invalid parameter - check ShaderType");
			return getShaders(shaderType).size();
		}

		GraphicObjectID& DxShaderPool::accessBoundShaderID(const GraphicShaderType shaderType)
		{
			MINT_ASSERT(shaderType != GraphicShaderType::COUNT, "Invalid parameter - check ShaderType");
			const uint32 shaderTypeIndex = static_cast<uint32>(shaderType);
			return _boundShaderIDPerType[shaderTypeIndex];
		}

		const Vector<DxShader>& DxShaderPool::getShaders(const GraphicShaderType shaderType) const
		{
			MINT_ASSERT(shaderType != GraphicShaderType::COUNT, "Invalid parameter - check ShaderType");
			const uint32 shaderTypeIndex = static_cast<uint32>(shaderType);
			return _shadersPerType[shaderTypeIndex];
		}

		Vector<DxShader>& DxShaderPool::accessShaders(const GraphicShaderType shaderType)
		{
			return const_cast<Vector<DxShader>&>(getShaders(shaderType));
		}
	}
}

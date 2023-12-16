#include <MintRenderingBase/Include/Shader.h>

#include <d3dcompiler.h>

#include <MintContainer/Include/StackString.hpp>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/Algorithm.hpp>

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
		static void makeShaderVersion(StackStringA<BufferSize>& out, const GraphicShaderType shaderType, const ShaderVersion shaderVersion)
		{
			out.Clear();

			if (shaderType == GraphicShaderType::VertexShader)
			{
				out.Append("vs_");
			}
			else if (shaderType == GraphicShaderType::GeometryShader)
			{
				out.Append("gs_");
			}
			else if (shaderType == GraphicShaderType::PixelShader)
			{
				out.Append("ps_");
			}

			if (shaderVersion == ShaderVersion::v_4_0)
			{
				out.Append("4_0");
			}
			else if (shaderVersion == ShaderVersion::v_5_0)
			{
				out.Append("5_0");
			}
		}
#pragma endregion


#pragma region GraphicInputLayout
		const GraphicInputLayout GraphicInputLayout::kNullInstance{ GraphicDevice::GetInvalidInstance() };
		GraphicInputLayout::GraphicInputLayout(GraphicDevice& graphicDevice)
			: GraphicObject(graphicDevice, GraphicObjectType::InputLayout)
		{
			__noop;
		}

		void GraphicInputLayout::Bind() const
		{
			_graphicDevice.GetStateManager().SetIAInputLayout(_inputLayout.Get());
		}

		void GraphicInputLayout::Unbind() const
		{
			_graphicDevice.GetStateManager().SetIAInputLayout(nullptr);
		}
#pragma endregion


#pragma region Shader
		const Shader Shader::kNullInstance{ GraphicDevice::GetInvalidInstance(), GraphicShaderType::VertexShader };
		Shader::Shader(GraphicDevice& graphicDevice, const GraphicShaderType shaderType)
			: GraphicObject(graphicDevice, GraphicObjectType::Shader), _shaderType{ shaderType }
		{
			__noop;
		}

		void Shader::Bind() const noexcept
		{
			if (_shaderType == GraphicShaderType::VertexShader)
			{
				_graphicDevice.GetStateManager().SetVSShader(static_cast<ID3D11VertexShader*>(_shader.Get()));
			}
			else if (_shaderType == GraphicShaderType::GeometryShader)
			{
				_graphicDevice.GetStateManager().SetGSShader(static_cast<ID3D11GeometryShader*>(_shader.Get()));
			}
			else if (_shaderType == GraphicShaderType::PixelShader)
			{
				_graphicDevice.GetStateManager().SetPSShader(static_cast<ID3D11PixelShader*>(_shader.Get()));
			}
		}

		void Shader::Unbind() const noexcept
		{
			if (_shaderType == GraphicShaderType::VertexShader)
			{
				_graphicDevice.GetStateManager().SetVSShader(nullptr);
			}
			else if (_shaderType == GraphicShaderType::GeometryShader)
			{
				_graphicDevice.GetStateManager().SetGSShader(nullptr);
			}
			else if (_shaderType == GraphicShaderType::PixelShader)
			{
				_graphicDevice.GetStateManager().SetPSShader(nullptr);
			}
		}
#pragma endregion


		ShaderPool::ShaderPool(GraphicDevice& graphicDevice, DxShaderHeaderMemory* const shaderHeaderMemory, const ShaderVersion shaderVersion)
			: GraphicObject(graphicDevice, GraphicObjectType::Pool)
			, _shaderHeaderMemory{ shaderHeaderMemory }
			, _shaderVersion{ shaderVersion }
		{
			__noop;
		}

		GraphicObjectID ShaderPool::AddShaderFromMemory(const char* const shaderIdentifier, const char* const textContent, const char* const entryPoint, const GraphicShaderType shaderType)
		{
			Shader shader(_graphicDevice, shaderType);
			ShaderCompileParam compileParam;
			compileParam._shaderIdentifier = shaderIdentifier;
			compileParam._shaderTextContent = textContent;
			if (CompileShaderInternalXXX(shaderType, compileParam, entryPoint, shader._shaderBlob.ReleaseAndGetAddressOf()) == false)
			{
				return GraphicObjectID::kInvalidGraphicObjectID;
			}
			shader._hlslFileName = shaderIdentifier;

			return AddShaderInternal(shaderType, shader);
		}

		GraphicObjectID ShaderPool::AddShader(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const GraphicShaderType shaderType, const char* const outputDirectory)
		{
			Shader shader(_graphicDevice, shaderType);
			if (CompileShaderFromFile(inputDirectory, inputShaderFileName, entryPoint, outputDirectory, shaderType, false, shader) == false)
			{
				return GraphicObjectID::kInvalidGraphicObjectID;
			}
			return AddShaderInternal(shaderType, shader);
		}

		GraphicObjectID ShaderPool::AddInputLayout(const GraphicObjectID& vertexShaderID, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData)
		{
			const int32 vertexShaderIndex = GetShaderIndex(GraphicShaderType::VertexShader, vertexShaderID);
			if (IsValidIndex(vertexShaderIndex) == false)
			{
				return GraphicObjectID::kInvalidGraphicObjectID;
			}

			const Shader& vertexShader = AccessShaders(GraphicShaderType::VertexShader)[vertexShaderIndex];
			return AddInputLayoutInternal(vertexShader, inputElementTypeMetaData);
		}
		
		void ShaderPool::RemoveShader(const GraphicObjectID& shaderID)
		{
			if (shaderID.IsValid() == false)
			{
				MINT_NEVER;
				return;
			}

			for (auto& shaders : _shadersPerType)
			{
				const uint32 shaderCount = shaders.Size();
				for (uint32 shaderIndex = 0; shaderIndex < shaderCount; ++shaderIndex)
				{
					if (shaders[shaderIndex] == shaderID)
					{
						shaders.Erase(shaderIndex);
						break;
					}
				}
			}
		}
		
		void ShaderPool::RemoveInputLayout(const GraphicObjectID& shaderID)
		{
			if (shaderID.IsValid() == false)
			{
				MINT_NEVER;
				return;
			}

			const uint32 inputLayoutCount = _inputLayouts.Size();
			for (uint32 inputLayoutIndex = 0; inputLayoutIndex < inputLayoutCount; ++inputLayoutIndex)
			{
				if (_inputLayouts[inputLayoutIndex] == shaderID)
				{
					_inputLayouts.Erase(inputLayoutIndex);
					break;
				}
			}
		}

		GraphicObjectID ShaderPool::AddShaderInternal(const GraphicShaderType shaderType, Shader& shader)
		{
			if (CreateShaderInternal(shaderType, shader) == false)
			{
				return GraphicObjectID::kInvalidGraphicObjectID;
			}

			shader.AssignIDXXX();
			const GraphicObjectID graphicObjectID = shader.GetID();
			AccessShaders(shaderType).PushBack(std::move(shader));
			QuickSort(AccessShaders(shaderType), GraphicObject::AscendingComparator());
			return graphicObjectID;
		}

		GraphicObjectID ShaderPool::AddInputLayoutInternal(const Shader& vertexShader, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData)
		{
			GraphicInputLayout inputLayout(_graphicDevice);
			if (CreateInputLayoutInternal(vertexShader, inputElementTypeMetaData, inputLayout) == false)
			{
				return GraphicObjectID::kInvalidGraphicObjectID;
			}

			inputLayout.AssignIDXXX();
			const GraphicObjectID graphicObjectID = inputLayout.GetID();
			_inputLayouts.PushBack(std::move(inputLayout));
			QuickSort(_inputLayouts, GraphicObject::AscendingComparator());
			return graphicObjectID;
		}

		bool ShaderPool::CreateShaderInternal(const GraphicShaderType shaderType, Shader& shader)
		{
			switch (shaderType)
			{
			case GraphicShaderType::VertexShader:
			{
				if (FAILED(_graphicDevice.GetDxDevice()->CreateVertexShader(shader._shaderBlob->GetBufferPointer(), shader._shaderBlob->GetBufferSize(), nullptr, reinterpret_cast<ID3D11VertexShader**>(shader._shader.ReleaseAndGetAddressOf()))))
				{
					return false;
				}
				return true;
			}
			case GraphicShaderType::GeometryShader:
			{
				if (FAILED(_graphicDevice.GetDxDevice()->CreateGeometryShader(shader._shaderBlob->GetBufferPointer(), shader._shaderBlob->GetBufferSize(), nullptr, reinterpret_cast<ID3D11GeometryShader**>(shader._shader.ReleaseAndGetAddressOf()))))
				{
					return false;
				}
				return true;
			}
			case GraphicShaderType::PixelShader:
			{
				if (FAILED(_graphicDevice.GetDxDevice()->CreatePixelShader(shader._shaderBlob->GetBufferPointer(), shader._shaderBlob->GetBufferSize(), nullptr, reinterpret_cast<ID3D11PixelShader**>(shader._shader.ReleaseAndGetAddressOf()))))
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

		bool ShaderPool::CreateInputLayoutInternal(const Shader& vertexShader, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData, GraphicInputLayout& outInputLayout)
		{
			outInputLayout._inputElementSet._semanticNameArray.Clear();
			outInputLayout._inputElementSet._inputElementDescriptorArray.Clear();

			{
				const uint32 memberCount = inputElementTypeMetaData.GetMemberCount();
				for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
				{
					const TypeMetaData<TypeCustomData>& memberTypeMetaData = inputElementTypeMetaData.GetMember(memberIndex);
					PushInputElement(outInputLayout._inputElementSet, inputElementTypeMetaData, memberTypeMetaData);
				}
			}

			// Input slot 처리
			const uint32 slottedStreamDataCount = inputElementTypeMetaData._customData.GetSlottedStreamDataCount();
			for (uint32 slottedStreamDataIndex = 0; slottedStreamDataIndex < slottedStreamDataCount; ++slottedStreamDataIndex)
			{
				const TypeMetaData<TypeCustomData>& slottedStreamData = inputElementTypeMetaData._customData.GetSlottedStreamData(slottedStreamDataIndex);
				const uint32 memberCount = slottedStreamData.GetMemberCount();
				for (uint32 memberIndex = 0; memberIndex < memberCount; ++memberIndex)
				{
					PushInputElement(outInputLayout._inputElementSet, slottedStreamData, slottedStreamData.GetMember(memberIndex));
				}
			}

			if (FAILED(_graphicDevice.GetDxDevice()->CreateInputLayout(outInputLayout._inputElementSet._inputElementDescriptorArray.Data(), static_cast<UINT>(outInputLayout._inputElementSet._inputElementDescriptorArray.Size()),
				vertexShader._shaderBlob->GetBufferPointer(), vertexShader._shaderBlob->GetBufferSize(), outInputLayout._inputLayout.ReleaseAndGetAddressOf())))
			{
				MINT_LOG_ERROR("VertexShader [[%s]] 의 InputLayout 생성에 실패했습니다. Input 자료형 으로 [[%s]] 을 쓰는게 맞는지 확인해 주세요.", vertexShader._hlslFileName.CString(), inputElementTypeMetaData.GetTypeName().CString());
				return false;
			}
			return true;
		}

		void ShaderPool::PushInputElement(DxInputElementSet& inputElementSet, const TypeMetaData<TypeCustomData>& outerDataTypeMetaData, const TypeMetaData<TypeCustomData>& memberTypeMetaData)
		{
			inputElementSet._semanticNameArray.PushBack(Language::CppHlsl::Parser::ConvertDeclarationNameToHlslSemanticName(memberTypeMetaData.GetDeclName()));

			D3D11_INPUT_ELEMENT_DESC inputElementDescriptor;
			inputElementDescriptor.SemanticName = inputElementSet._semanticNameArray.Back().CString();
			inputElementDescriptor.SemanticIndex = 0;
			inputElementDescriptor.Format = Language::CppHlsl::Parser::ConvertCppHlslTypeToDxgiFormat(memberTypeMetaData);
			inputElementDescriptor.InputSlot = memberTypeMetaData._customData.GetInputSlot();
			inputElementDescriptor.AlignedByteOffset = memberTypeMetaData.GetByteOffset();
			inputElementDescriptor.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
			inputElementDescriptor.InstanceDataStepRate = outerDataTypeMetaData._customData.GetInstanceDataStepRate();
			if (inputElementDescriptor.InstanceDataStepRate > 0)
			{
				inputElementDescriptor.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
			}

			inputElementSet._inputElementDescriptorArray.PushBack(inputElementDescriptor);
		}

		bool ShaderPool::CompileShaderFromFile(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const char* const outputDirectory, const GraphicShaderType shaderType, const bool forceCompilation, Shader& inoutShader)
		{
			StringA inputShaderFilePath{ inputDirectory };
			inputShaderFilePath += inputShaderFileName;
			if (FileUtil::Exists(inputShaderFilePath.CString()) == false)
			{
				MINT_LOG_ERROR("Input shader file not found : %s", inputShaderFilePath.CString());
				return false;
			}

			StringA outputShaderFilePath{ inputShaderFileName };
			StringUtil::ExcludeExtension(outputShaderFilePath);
			if (outputDirectory != nullptr)
			{
				if (FileUtil::Exists(outputDirectory) == false)
				{
					MINT_ASSERT(FileUtil::CreateDirectory_(outputDirectory) == true, "경로 생성에 실패했습니다!");
				}

				outputShaderFilePath = outputDirectory + outputShaderFilePath;
			}
			else
			{
				outputShaderFilePath = inputDirectory + outputShaderFilePath;
			}
			outputShaderFilePath.Append(kCompiledShaderFileExtension);

			return CompileShaderFromFile(inputShaderFilePath.CString(), entryPoint, outputShaderFilePath.CString(), shaderType, forceCompilation, inoutShader);
		}

		bool ShaderPool::CompileShaderFromFile(const char* const inputShaderFilePath, const char* const entryPoint, const char* const outputShaderFilePath, const GraphicShaderType shaderType, const bool forceCompilation, Shader& inoutShader)
		{
			if (StringUtil::Contains(inputShaderFilePath, ".hlsl") == false)
			{
				return false;
			}

			if (FileUtil::Exists(outputShaderFilePath) == false || forceCompilation == true)
			{
				ShaderCompileParam compileParam;
				compileParam._inputFileName = inputShaderFilePath;
				compileParam._outputFileName = outputShaderFilePath;
				if (CompileShaderInternalXXX(shaderType, compileParam, entryPoint, inoutShader._shaderBlob.ReleaseAndGetAddressOf()) == false)
				{
					return false;
				}
			}
			else
			{
				StringW compiledShaderFileNameW;
				StringUtil::ConvertStringAToStringW(outputShaderFilePath, compiledShaderFileNameW);
				if (FAILED(D3DReadFileToBlob(compiledShaderFileNameW.CString(), inoutShader._shaderBlob.ReleaseAndGetAddressOf())))
				{
					return false;
				}
			}

			inoutShader._entryPoint = entryPoint;
			inoutShader._hlslFileName = inputShaderFilePath;
			inoutShader._hlslBinaryFileName = outputShaderFilePath;
			return true;
		}

		bool ShaderPool::CompileShaderInternalXXX(const GraphicShaderType shaderType, const ShaderCompileParam& compileParam, const char* const entryPoint, ID3D10Blob** outBlob)
		{
			StackStringA<20> version;
			makeShaderVersion(version, shaderType, _shaderVersion);

			TextFileReader textFileReader;
			const char* content{};
			const char* identifier{};
			uint32 contentLength{};
			if (compileParam._inputFileName == nullptr)
			{
				content = compileParam._shaderTextContent;
				contentLength = StringUtil::Length(compileParam._shaderTextContent);
				identifier = compileParam._shaderIdentifier;
			}
			else
			{
				if (textFileReader.Open(compileParam._inputFileName) == false)
				{
					MINT_LOG_ERROR("Input file not found : %s", compileParam._inputFileName);
					return false;
				}

				content = textFileReader.Get();
				contentLength = textFileReader.GetFileSize();
				identifier = compileParam._inputFileName;
			}

			const UINT debugFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
			if (FAILED(D3DCompile(content, contentLength, identifier, nullptr, _shaderHeaderMemory, entryPoint, version.CString(), debugFlag, 0, outBlob, _errorMessageBlob.ReleaseAndGetAddressOf())))
			{
				ReportCompileError();
				return false;
			}

			if (compileParam._outputFileName != nullptr)
			{
				StringW outputFileNameWideString;
				StringUtil::ConvertStringAToStringW(compileParam._outputFileName, outputFileNameWideString);
				if (FAILED(D3DWriteBlobToFile(*outBlob, outputFileNameWideString.CString(), TRUE)))
				{
					return false;
				}
			}
			return true;
		}

		void ShaderPool::RecompileAllShaders()
		{
			const uint32 shaderTypeCount = static_cast<uint32>(GraphicShaderType::COUNT);
			for (uint32 shaderTypeIndex = 0; shaderTypeIndex < shaderTypeCount; ++shaderTypeIndex)
			{
				const GraphicShaderType shaderType = static_cast<GraphicShaderType>(shaderTypeIndex);
				const GraphicObjectID& boundShaderID = AccessBoundShaderID(shaderType);
				if (boundShaderID.IsValid())
				{
					const int32 boundShaderIndex = GetShaderIndex(shaderType, boundShaderID);
					AccessShaders(shaderType)[boundShaderIndex].Unbind();
				}

				const uint32 shaderCount = GetShaderCount(shaderType);
				for (uint32 shaderIndex = 0; shaderIndex < shaderCount; ++shaderIndex)
				{
					Shader& shader = AccessShaders(shaderType)[shaderIndex];
					CompileShaderFromFile(shader._hlslFileName.CString(), shader._entryPoint.CString(), shader._hlslBinaryFileName.CString(), shader._shaderType, true, shader);
					CreateShaderInternal(shaderType, shader);
				}
			}

			for (uint32 shaderTypeIndex = 0; shaderTypeIndex < shaderTypeCount; ++shaderTypeIndex)
			{
				const GraphicShaderType shaderType = static_cast<GraphicShaderType>(shaderTypeIndex);
				const GraphicObjectID& boundShaderID = AccessBoundShaderID(shaderType);
				if (boundShaderID.IsValid())
				{
					const int32 boundShaderIndex = GetShaderIndex(shaderType, boundShaderID);
					AccessShaders(shaderType)[boundShaderIndex].Bind();
				}
			}
		}

		void ShaderPool::ReportCompileError()
		{
			StringA errorMessages(reinterpret_cast<char*>(_errorMessageBlob->GetBufferPointer()));

			const uint32 firstNewLinePos = errorMessages.Find('\n');
			const uint32 secondNewLinePos = errorMessages.Find('\n', firstNewLinePos + 1);
			errorMessages = errorMessages.Substring(0, secondNewLinePos);

			MINT_LOG_ERROR("Shader Compile Error\n\n%s", errorMessages.CString());
		}

		void ShaderPool::BindShaderIfNot(const GraphicShaderType shaderType, const GraphicObjectID& objectID)
		{
			GraphicObjectID& boundShaderID = AccessBoundShaderID(shaderType);
			if (boundShaderID == objectID)
			{
				return;
			}

			const int32 shaderIndex = GetShaderIndex(shaderType, objectID);
			if (IsValidIndex(shaderIndex) == false)
			{
				return;
			}

			boundShaderID = objectID;
			AccessShaders(shaderType)[shaderIndex].Bind();
		}

		void ShaderPool::BindInputLayoutIfNot(const GraphicObjectID& objectID)
		{
			if (_boundInputLayoutID == objectID)
			{
				return;
			}

			const int32 inputLayoutIndex = GetInputLayoutIndex(objectID);
			if (inputLayoutIndex < 0)
			{
				return;
			}

			_inputLayouts[inputLayoutIndex].Bind();
			_boundInputLayoutID = objectID;
		}

		void ShaderPool::UnbindShader(const GraphicShaderType shaderType)
		{
			if (shaderType == GraphicShaderType::COUNT)
			{
				return;
			}

			GraphicObjectID& boundShaderID = AccessBoundShaderID(shaderType);
			if (boundShaderID.IsValid() == false)
			{
				return;
			}

			const int32 shaderIndex = GetShaderIndex(shaderType, boundShaderID);
			if (IsValidIndex(shaderIndex) == false)
			{
				MINT_NEVER;
				return;
			}

			boundShaderID.Invalidate();
			AccessShaders(shaderType)[shaderIndex].Unbind();
		}

		int32 ShaderPool::GetShaderIndex(const GraphicShaderType shaderType, const GraphicObjectID& objectID) const
		{
			MINT_ASSERT(shaderType != GraphicShaderType::COUNT, "Invalid parameter - check ShaderType");
			MINT_ASSERT(objectID.IsObjectType(GraphicObjectType::Shader) == true, "Invalid parameter - check ObjectType");
			return BinarySearch(GetShaders(shaderType), objectID, GraphicObject::Evaluator());
		}

		int32 ShaderPool::GetInputLayoutIndex(const GraphicObjectID& objectID) const
		{
			MINT_ASSERT(objectID.IsObjectType(GraphicObjectType::InputLayout) == true, "Invalid parameter - check ObjectType");
			return BinarySearch(_inputLayouts, objectID, GraphicObject::Evaluator());
		}

		uint32 ShaderPool::GetShaderCount(const GraphicShaderType shaderType) const
		{
			MINT_ASSERT(shaderType != GraphicShaderType::COUNT, "Invalid parameter - check ShaderType");
			return GetShaders(shaderType).Size();
		}

		GraphicObjectID& ShaderPool::AccessBoundShaderID(const GraphicShaderType shaderType)
		{
			MINT_ASSERT(shaderType != GraphicShaderType::COUNT, "Invalid parameter - check ShaderType");
			const uint32 shaderTypeIndex = static_cast<uint32>(shaderType);
			return _boundShaderIDPerType[shaderTypeIndex];
		}

		const Vector<Shader>& ShaderPool::GetShaders(const GraphicShaderType shaderType) const
		{
			MINT_ASSERT(shaderType != GraphicShaderType::COUNT, "Invalid parameter - check ShaderType");
			const uint32 shaderTypeIndex = static_cast<uint32>(shaderType);
			return _shadersPerType[shaderTypeIndex];
		}

		Vector<Shader>& ShaderPool::AccessShaders(const GraphicShaderType shaderType)
		{
			return const_cast<Vector<Shader>&>(GetShaders(shaderType));
		}
	}
}

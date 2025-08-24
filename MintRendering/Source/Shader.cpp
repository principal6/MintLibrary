#include <MintRendering/Include/Shader.h>

#include <d3dcompiler.h>

#include <MintContainer/Include/RefCounted.hpp>
#include <MintContainer/Include/StackString.hpp>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/Algorithm.hpp>

#include <MintPlatform/Include/TextFile.h>
#include <MintPlatform/Include/FileUtil.hpp>

#include <MintRendering/Include/GraphicsDevice.h>
#include <MintRendering/Include/DxShaderHeaderMemory.h>

#include <MintRendering/Include/CppHlsl/CppHlslParser.h>


#pragma comment(lib, "d3dcompiler.lib")


namespace mint
{
	namespace Rendering
	{
#pragma region Static function definitions
		template<uint32 BufferSize>
		static void makeShaderVersion(StackStringA<BufferSize>& out, const GraphicsShaderType shaderType, const ShaderVersion shaderVersion)
		{
			out.Clear();

			if (shaderType == GraphicsShaderType::VertexShader)
			{
				out.Append("vs_");
			}
			else if (shaderType == GraphicsShaderType::GeometryShader)
			{
				out.Append("gs_");
			}
			else if (shaderType == GraphicsShaderType::PixelShader)
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


#pragma region GraphicsInputLayout
		GraphicsInputLayout::GraphicsInputLayout(GraphicsDevice& graphicsDevice)
			: GraphicsObject(graphicsDevice, GraphicsObjectType::InputLayout)
		{
			__noop;
		}

		void GraphicsInputLayout::Bind() const
		{
			_graphicsDevice.GetStateManager().SetIAInputLayout(_inputLayout.Get());
		}

		void GraphicsInputLayout::Unbind() const
		{
			_graphicsDevice.GetStateManager().SetIAInputLayout(nullptr);
		}
#pragma endregion


#pragma region Shader
		Shader::Shader(GraphicsDevice& graphicsDevice, const GraphicsShaderType shaderType)
			: GraphicsObject(graphicsDevice, GraphicsObjectType::Shader)
			, _shaderHashKey{ 0 }
			, _shaderType{ shaderType }
		{
			__noop;
		}

		void Shader::Bind() const noexcept
		{
			if (_shaderType == GraphicsShaderType::VertexShader)
			{
				_graphicsDevice.GetStateManager().SetVSShader(static_cast<ID3D11VertexShader*>(_shader.Get()));
			}
			else if (_shaderType == GraphicsShaderType::GeometryShader)
			{
				_graphicsDevice.GetStateManager().SetGSShader(static_cast<ID3D11GeometryShader*>(_shader.Get()));
			}
			else if (_shaderType == GraphicsShaderType::PixelShader)
			{
				_graphicsDevice.GetStateManager().SetPSShader(static_cast<ID3D11PixelShader*>(_shader.Get()));
			}
		}

		void Shader::Unbind() const noexcept
		{
			if (_shaderType == GraphicsShaderType::VertexShader)
			{
				_graphicsDevice.GetStateManager().SetVSShader(nullptr);
			}
			else if (_shaderType == GraphicsShaderType::GeometryShader)
			{
				_graphicsDevice.GetStateManager().SetGSShader(nullptr);
			}
			else if (_shaderType == GraphicsShaderType::PixelShader)
			{
				_graphicsDevice.GetStateManager().SetPSShader(nullptr);
			}
		}
#pragma endregion


		ShaderPool::ShaderPool(GraphicsDevice& graphicsDevice, DxShaderHeaderMemory* const shaderHeaderMemory, const ShaderVersion shaderVersion)
			: _graphicsDevice{ graphicsDevice }
			, _shaderHeaderMemory{ shaderHeaderMemory }
			, _shaderVersion{ shaderVersion }
		{
			__noop;
		}

		ShaderPool::~ShaderPool()
		{
			__noop;
		}

		GraphicsObjectID ShaderPool::CreateShaderFromMemory(const char* const shaderIdentifier, const char* const textContent, const char* const entryPoint, const GraphicsShaderType shaderType)
		{
			const char* const shaderName = shaderIdentifier;
			const uint64 shaderHashKey = ComputeHash(textContent);
			const GraphicsObjectID existingShaderID = GetExistingShader(shaderType, shaderName, shaderHashKey, entryPoint);
			if (existingShaderID.IsValid() == true)
			{
				return existingShaderID;
			}

			Shader shader(_graphicsDevice, shaderType);
			ShaderCompileParam compileParam;
			compileParam._shaderIdentifier = shaderIdentifier;
			compileParam._shaderTextContent = textContent;
			if (CompileShaderInternalXXX(shaderType, compileParam, entryPoint, shader._shaderBlob.ReleaseAndGetAddressOf()) == false)
			{
				return GraphicsObjectID::kInvalidGraphicsObjectID;
			}
			shader._shaderName = shaderName;
			shader._shaderHashKey = shaderHashKey;
			shader._entryPoint = entryPoint;

			return CreateShaderInternal(shaderType, shader);
		}

		GraphicsObjectID ShaderPool::CreateShader(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const GraphicsShaderType shaderType, const char* const outputDirectory)
		{
			StringA shaderName{ inputDirectory };
			shaderName += inputShaderFileName;
			const uint64 shaderHashKey = ComputeHash(outputDirectory);
			const GraphicsObjectID existingShaderID = GetExistingShader(shaderType, shaderName.CString(), shaderHashKey, entryPoint);
			if (existingShaderID.IsValid() == true)
			{
				return existingShaderID;
			}

			Shader shader(_graphicsDevice, shaderType);
			if (CompileShaderFromFile(inputDirectory, inputShaderFileName, entryPoint, outputDirectory, shaderType, false, shader) == false)
			{
				return GraphicsObjectID::kInvalidGraphicsObjectID;
			}
			shader._shaderName = shaderName.CString();
			shader._shaderHashKey = ComputeHash(outputDirectory);
			shader._entryPoint = entryPoint;

			return CreateShaderInternal(shaderType, shader);
		}

		GraphicsObjectID ShaderPool::CreateInputLayout(const GraphicsObjectID& vertexShaderID, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData)
		{
			for (const RefCounted<GraphicsInputLayout>& inputLayout : _inputLayouts)
			{
				if (inputLayout->_vertexShaderID == vertexShaderID && inputLayout->_inputLayoutName == inputElementTypeMetaData.GetTypeName())
				{
					return inputLayout->GetID();
				}
			}

			const uint32 vertexShaderIndex = GetShaderIndex(GraphicsShaderType::VertexShader, vertexShaderID);
			if (IsValidIndex(vertexShaderIndex) == false)
			{
				return GraphicsObjectID::kInvalidGraphicsObjectID;
			}

			const Shader& vertexShader = *AccessShaders(GraphicsShaderType::VertexShader)[vertexShaderIndex];
			return CreateInputLayoutInternal(vertexShader, inputElementTypeMetaData);
		}

		void ShaderPool::IncreaseShaderRefCount(const GraphicsObjectID& shaderID)
		{
			for (uint8 shaderTypeRaw = 0; shaderTypeRaw < static_cast<uint8>(GraphicsShaderType::COUNT); ++shaderTypeRaw)
			{
				const GraphicsShaderType shaderType = static_cast<GraphicsShaderType>(shaderTypeRaw);
				const uint32 shaderIndex = GetShaderIndex(shaderType, shaderID);
				if (IsValidIndex(shaderIndex) == false)
				{
					continue;
				}

				AccessShaders(shaderType)[shaderIndex].IncreaseRefCount();
			}
		}

		void ShaderPool::DecreaseShaderRefCount(const GraphicsObjectID& shaderID)
		{
			for (uint8 shaderTypeRaw = 0; shaderTypeRaw < static_cast<uint8>(GraphicsShaderType::COUNT); ++shaderTypeRaw)
			{
				const GraphicsShaderType shaderType = static_cast<GraphicsShaderType>(shaderTypeRaw);
				const uint32 shaderIndex = GetShaderIndex(shaderType, shaderID);
				if (IsValidIndex(shaderIndex) == false)
				{
					continue;
				}

				Vector<RefCounted<Shader>>& shaders = AccessShaders(shaderType);
				shaders[shaderIndex].DecreaseRefCount();

				if (shaders[shaderIndex].IsValid() == false)
				{
					shaders.Erase(shaderIndex);
				}
			}
		}

		void ShaderPool::IncreaseInputLayoutRefCount(const GraphicsObjectID& inputLayoutID)
		{
			const uint32 inputLayoutIndex = GetInputLayoutIndex(inputLayoutID);
			if (IsValidIndex(inputLayoutIndex) == false)
			{
				return;
			}

			_inputLayouts[inputLayoutIndex].IncreaseRefCount();
		}

		void ShaderPool::DecreaseInputLayoutRefCount(const GraphicsObjectID& inputLayoutID)
		{
			const uint32 inputLayoutIndex = GetInputLayoutIndex(inputLayoutID);
			if (IsValidIndex(inputLayoutIndex) == false)
			{
				return;
			}

			_inputLayouts[inputLayoutIndex].DecreaseRefCount();

			if (_inputLayouts[inputLayoutIndex].IsValid() == false)
			{
				_inputLayouts.Erase(inputLayoutIndex);
			}
		}

		GraphicsObjectID ShaderPool::GetExistingShader(const GraphicsShaderType shaderType, const char* const shaderName, const uint64 shaderHashKey, const char* const entryPoint) const
		{
			const Vector<RefCounted<Shader>>& shaders = GetShaders(shaderType);
			for (const RefCounted<Shader>& shader : shaders)
			{
				if (shader->_shaderName.Equals(shaderName))
				{
					MINT_ASSERT(shader->_entryPoint == entryPoint, "Shader with same name has different entry point!");
					MINT_ASSERT(shader->_shaderHashKey == shaderHashKey, "Shader with same name has different shader hash key!");
					return shader->GetID();
				}
			}
			return GraphicsObjectID::kInvalidGraphicsObjectID;
		}

		GraphicsObjectID ShaderPool::CreateShaderInternal(const GraphicsShaderType shaderType, Shader& shader)
		{
			if (CreateLowLevelShader(shaderType, shader) == false)
			{
				return GraphicsObjectID::kInvalidGraphicsObjectID;
			}

			shader.AssignIDXXX();
			const GraphicsObjectID graphicsObjectID = shader.GetID();
			AccessShaders(shaderType).PushBack(RefCounted<Shader>(MINT_NEW(Shader, std::move(shader))));
			QuickSort(AccessShaders(shaderType), GraphicsObject::AscendingComparator());
			return graphicsObjectID;
		}

		GraphicsObjectID ShaderPool::CreateInputLayoutInternal(const Shader& vertexShader, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData)
		{
			GraphicsInputLayout inputLayout(_graphicsDevice);
			if (CreateLowLevelInputLayout(vertexShader, inputElementTypeMetaData, inputLayout) == false)
			{
				return GraphicsObjectID::kInvalidGraphicsObjectID;
			}

			inputLayout.AssignIDXXX();
			inputLayout._vertexShaderID = vertexShader.GetID();
			inputLayout._inputLayoutName = inputElementTypeMetaData.GetTypeName();
			const GraphicsObjectID graphicsObjectID = inputLayout.GetID();
			_inputLayouts.PushBack(RefCounted<GraphicsInputLayout>(MINT_NEW(GraphicsInputLayout, std::move(inputLayout))));
			QuickSort(_inputLayouts, GraphicsObject::AscendingComparator());
			return graphicsObjectID;
		}

		bool ShaderPool::CreateLowLevelShader(const GraphicsShaderType shaderType, Shader& shader)
		{
			switch (shaderType)
			{
			case GraphicsShaderType::VertexShader:
			{
				if (FAILED(_graphicsDevice.GetDxDevice()->CreateVertexShader(shader._shaderBlob->GetBufferPointer(), shader._shaderBlob->GetBufferSize(), nullptr, reinterpret_cast<ID3D11VertexShader**>(shader._shader.ReleaseAndGetAddressOf()))))
				{
					return false;
				}
				return true;
			}
			case GraphicsShaderType::GeometryShader:
			{
				if (FAILED(_graphicsDevice.GetDxDevice()->CreateGeometryShader(shader._shaderBlob->GetBufferPointer(), shader._shaderBlob->GetBufferSize(), nullptr, reinterpret_cast<ID3D11GeometryShader**>(shader._shader.ReleaseAndGetAddressOf()))))
				{
					return false;
				}
				return true;
			}
			case GraphicsShaderType::PixelShader:
			{
				if (FAILED(_graphicsDevice.GetDxDevice()->CreatePixelShader(shader._shaderBlob->GetBufferPointer(), shader._shaderBlob->GetBufferSize(), nullptr, reinterpret_cast<ID3D11PixelShader**>(shader._shader.ReleaseAndGetAddressOf()))))
				{
					return false;
				}
				return true;
			}
			case GraphicsShaderType::COUNT:
				break;
			default:
				break;
			}
			return false;
		}

		bool ShaderPool::CreateLowLevelInputLayout(const Shader& vertexShader, const TypeMetaData<TypeCustomData>& inputElementTypeMetaData, GraphicsInputLayout& outInputLayout)
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

			if (FAILED(_graphicsDevice.GetDxDevice()->CreateInputLayout(outInputLayout._inputElementSet._inputElementDescriptorArray.Data(), static_cast<UINT>(outInputLayout._inputElementSet._inputElementDescriptorArray.Size()),
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

		bool ShaderPool::CompileShaderFromFile(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const char* const outputDirectory, const GraphicsShaderType shaderType, const bool forceCompilation, Shader& inoutShader)
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

			return CompileShaderFromFileInternal(inputShaderFilePath.CString(), entryPoint, outputShaderFilePath.CString(), shaderType, forceCompilation, inoutShader);
		}

		bool ShaderPool::CompileShaderFromFileInternal(const char* const inputShaderFilePath, const char* const entryPoint, const char* const outputShaderFilePath, const GraphicsShaderType shaderType, const bool forceCompilation, Shader& inoutShader)
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

		bool ShaderPool::CompileShaderInternalXXX(const GraphicsShaderType shaderType, const ShaderCompileParam& compileParam, const char* const entryPoint, ID3D10Blob** outBlob)
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
			const uint32 shaderTypeCount = static_cast<uint32>(GraphicsShaderType::COUNT);
			for (uint32 shaderTypeIndex = 0; shaderTypeIndex < shaderTypeCount; ++shaderTypeIndex)
			{
				const GraphicsShaderType shaderType = static_cast<GraphicsShaderType>(shaderTypeIndex);
				const GraphicsObjectID& boundShaderID = AccessBoundShaderID(shaderType);
				if (boundShaderID.IsValid())
				{
					const int32 boundShaderIndex = GetShaderIndex(shaderType, boundShaderID);
					AccessShaders(shaderType)[boundShaderIndex]->Unbind();
				}

				const uint32 shaderCount = GetShaderCount(shaderType);
				for (uint32 shaderIndex = 0; shaderIndex < shaderCount; ++shaderIndex)
				{
					Shader& shader = *AccessShaders(shaderType)[shaderIndex];
					CompileShaderFromFileInternal(shader._hlslFileName.CString(), shader._entryPoint.CString(), shader._hlslBinaryFileName.CString(), shader._shaderType, true, shader);
					CreateLowLevelShader(shaderType, shader);
				}
			}

			for (uint32 shaderTypeIndex = 0; shaderTypeIndex < shaderTypeCount; ++shaderTypeIndex)
			{
				const GraphicsShaderType shaderType = static_cast<GraphicsShaderType>(shaderTypeIndex);
				const GraphicsObjectID& boundShaderID = AccessBoundShaderID(shaderType);
				if (boundShaderID.IsValid())
				{
					const int32 boundShaderIndex = GetShaderIndex(shaderType, boundShaderID);
					AccessShaders(shaderType)[boundShaderIndex]->Bind();
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

		void ShaderPool::BindShaderIfNot(const GraphicsShaderType shaderType, const GraphicsObjectID& objectID)
		{
			GraphicsObjectID& boundShaderID = AccessBoundShaderID(shaderType);
			if (boundShaderID == objectID)
			{
				return;
			}

			const uint32 shaderIndex = GetShaderIndex(shaderType, objectID);
			if (IsValidIndex(shaderIndex) == false)
			{
				return;
			}

			boundShaderID = objectID;
			AccessShaders(shaderType)[shaderIndex]->Bind();
		}

		void ShaderPool::BindInputLayoutIfNot(const GraphicsObjectID& objectID)
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

			_inputLayouts[inputLayoutIndex]->Bind();
			_boundInputLayoutID = objectID;
		}

		void ShaderPool::UnbindShader(const GraphicsShaderType shaderType)
		{
			if (shaderType == GraphicsShaderType::COUNT)
			{
				return;
			}

			GraphicsObjectID& boundShaderID = AccessBoundShaderID(shaderType);
			if (boundShaderID.IsValid() == false)
			{
				return;
			}

			const uint32 shaderIndex = GetShaderIndex(shaderType, boundShaderID);
			if (IsValidIndex(shaderIndex) == false)
			{
				MINT_NEVER;
				return;
			}

			boundShaderID.Invalidate();
			AccessShaders(shaderType)[shaderIndex]->Unbind();
		}

		bool ShaderPool::ExistsShader(const GraphicsObjectID& shaderID, const GraphicsShaderType shaderType) const
		{
			const uint32 shaderIndex = GetShaderIndex(shaderType, shaderID);
			return IsValidIndex(shaderIndex);
		}

		bool ShaderPool::ExistsInputLayout(const GraphicsObjectID& inputLayoutID) const
		{
			const uint32 inputLayoutIndex = GetInputLayoutIndex(inputLayoutID);
			return IsValidIndex(inputLayoutIndex);
		}

		uint32 ShaderPool::GetShaderIndex(const GraphicsShaderType shaderType, const GraphicsObjectID& objectID) const
		{
			MINT_ASSERT(shaderType != GraphicsShaderType::COUNT, "Invalid parameter - check ShaderType");
			MINT_ASSERT(objectID.IsObjectType(GraphicsObjectType::Shader) == true, "Invalid parameter - check ObjectType");
			return BinarySearch(GetShaders(shaderType), objectID, GraphicsObject::Evaluator());
		}

		uint32 ShaderPool::GetInputLayoutIndex(const GraphicsObjectID& objectID) const
		{
			MINT_ASSERT(objectID.IsObjectType(GraphicsObjectType::InputLayout) == true, "Invalid parameter - check ObjectType");
			return BinarySearch(_inputLayouts, objectID, GraphicsObject::Evaluator());
		}

		uint32 ShaderPool::GetShaderCount(const GraphicsShaderType shaderType) const
		{
			MINT_ASSERT(shaderType != GraphicsShaderType::COUNT, "Invalid parameter - check ShaderType");
			return GetShaders(shaderType).Size();
		}

		GraphicsObjectID& ShaderPool::AccessBoundShaderID(const GraphicsShaderType shaderType)
		{
			MINT_ASSERT(shaderType != GraphicsShaderType::COUNT, "Invalid parameter - check ShaderType");
			const uint32 shaderTypeIndex = static_cast<uint32>(shaderType);
			return _boundShaderIDPerType[shaderTypeIndex];
		}

		const Vector<RefCounted<Shader>>& ShaderPool::GetShaders(const GraphicsShaderType shaderType) const
		{
			MINT_ASSERT(shaderType != GraphicsShaderType::COUNT, "Invalid parameter - check ShaderType");
			const uint32 shaderTypeIndex = static_cast<uint32>(shaderType);
			return _shadersPerType[shaderTypeIndex];
		}

		Vector<RefCounted<Shader>>& ShaderPool::AccessShaders(const GraphicsShaderType shaderType)
		{
			return const_cast<Vector<RefCounted<Shader>>&>(GetShaders(shaderType));
		}
	}
}

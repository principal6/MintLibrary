#include <stdafx.h>
#include <FsRenderingBase/Include/DxShader.h>

#include <d3dcompiler.h>

#include <FsLibrary/Include/Algorithm.hpp>

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

        void DxShader::unbind() const noexcept
        {
            if (_shaderType == DxShaderType::VertexShader)
            {
                _graphicDevice->getDxDeviceContext()->VSSetShader(nullptr, nullptr, 0);
                _graphicDevice->getDxDeviceContext()->IASetInputLayout(nullptr);
            }
            else if (_shaderType == DxShaderType::GeometryShader)
            {
                _graphicDevice->getDxDeviceContext()->GSSetShader(nullptr, nullptr, 0);
            }
            else if (_shaderType == DxShaderType::PixelShader)
            {
                _graphicDevice->getDxDeviceContext()->PSSetShader(nullptr, nullptr, 0);
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
            shader._hlslFileName = shaderIdentifier;

            return pushVertexShaderInternal(shader, inputElementTypeInfo);
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
            shader._hlslFileName = shaderIdentifier;

            return pushNonVertexShaderInternal(shader, shaderType);
        }

        const DxObjectId& DxShaderPool::pushVertexShader(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const fs::Language::CppHlslTypeInfo* const inputElementTypeInfo, const char* const outputDirectory)
        {
            DxShader shader(_graphicDevice, DxShaderType::VertexShader);
            if (compileShaderFromFile(inputDirectory, inputShaderFileName, entryPoint, outputDirectory, DxShaderType::VertexShader, false, shader) == false)
            {
                return DxObjectId::kInvalidObjectId;
            }
            return pushVertexShaderInternal(shader, inputElementTypeInfo);
        }

        const DxObjectId& DxShaderPool::pushNonVertexShader(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const DxShaderType shaderType, const char* const outputDirectory)
        {
            DxShader shader(_graphicDevice, shaderType);
            if (compileShaderFromFile(inputDirectory, inputShaderFileName, entryPoint, outputDirectory, shaderType, false, shader) == false)
            {
                return DxObjectId::kInvalidObjectId;
            }
            return pushNonVertexShaderInternal(shader, shaderType);
        }

        const DxObjectId& DxShaderPool::pushVertexShaderInternal(DxShader& shader, const fs::Language::CppHlslTypeInfo* const inputElementTypeInfo)
        {
            if (createVertexShaderInternal(shader, inputElementTypeInfo) == true)
            {
                shader.assignIdXXX();
                _vertexShaderArray.emplace_back(std::move(shader));
                return _vertexShaderArray.back().getId();
            }
            return DxObjectId::kInvalidObjectId;
        }

        const DxObjectId& DxShaderPool::pushNonVertexShaderInternal(DxShader& shader, const DxShaderType shaderType)
        {
            if (createNonVertexShaderInternal(shader, shaderType) == true)
            {
                if (shaderType == DxShaderType::GeometryShader)
                {
                    shader.assignIdXXX();
                    _geometryShaderArray.emplace_back(std::move(shader));
                    return _geometryShaderArray.back().getId();
                }
                else if (shaderType == DxShaderType::PixelShader)
                {
                    shader.assignIdXXX();
                    _pixelShaderArray.emplace_back(std::move(shader));
                    return _pixelShaderArray.back().getId();
                }
            }
            return DxObjectId::kInvalidObjectId;
        }

        const bool DxShaderPool::createVertexShaderInternal(DxShader& shader, const fs::Language::CppHlslTypeInfo* const inputElementTypeInfo)
        {
            if (FAILED(_graphicDevice->getDxDevice()->CreateVertexShader(shader._shaderBlob->GetBufferPointer(), shader._shaderBlob->GetBufferSize(), NULL, reinterpret_cast<ID3D11VertexShader**>(shader._shader.ReleaseAndGetAddressOf()))))
            {
                return false;
            }

            // Input Layer
            if (inputElementTypeInfo != nullptr)
            {
                const uint32 memberCount = inputElementTypeInfo->getMemberCount();
                shader._inputElementSet._semanticNameArray.clear();
                shader._inputElementSet._semanticNameArray.reserve(memberCount);
                shader._inputElementSet._inputElementDescriptorArray.clear();
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
                    FS_LOG_ERROR("김장원", "VertexShader [[%s]] 의 InputLayout 생성에 실패했습니다. Input 자료형 으로 [[%s]] 을 쓰는게 맞는지 확인해 주세요.", shader._hlslFileName.c_str(), inputElementTypeInfo->getTypeName().c_str());
                    return false;
                }
            }

            return true;
        }

        const bool DxShaderPool::createNonVertexShaderInternal(DxShader& shader, const DxShaderType shaderType)
        {
            if (shaderType == DxShaderType::GeometryShader)
            {
                if (FAILED(_graphicDevice->getDxDevice()->CreateGeometryShader(shader._shaderBlob->GetBufferPointer(), shader._shaderBlob->GetBufferSize(), NULL, reinterpret_cast<ID3D11GeometryShader**>(shader._shader.ReleaseAndGetAddressOf()))))
                {
                    return false;
                }
            }
            else if (shaderType == DxShaderType::PixelShader)
            {
                if (FAILED(_graphicDevice->getDxDevice()->CreatePixelShader(shader._shaderBlob->GetBufferPointer(), shader._shaderBlob->GetBufferSize(), NULL, reinterpret_cast<ID3D11PixelShader**>(shader._shader.ReleaseAndGetAddressOf()))))
                {
                    return false;
                }
            }
            else
            {
                return false;
            }

            return true;
        }

        const bool DxShaderPool::compileShaderFromFile(const char* const inputDirectory, const char* const inputShaderFileName, const char* const entryPoint, const char* const outputDirectory, const DxShaderType shaderType, const bool forceCompilation, DxShader& inoutShader)
        {
            std::string inputShaderFilePath{ inputDirectory };
            inputShaderFilePath += inputShaderFileName;
            if (fs::FileUtil::exists(inputShaderFilePath.c_str()) == false)
            {
                FS_LOG_ERROR("김장원", "Input shader file not found : %s", inputShaderFilePath.c_str());
                return false;
            }

            std::string outputShaderFilePath{ inputShaderFileName };
            fs::StringUtil::excludeExtension(outputShaderFilePath);
            if (outputDirectory != nullptr)
            {
                outputShaderFilePath = outputDirectory + outputShaderFilePath;
            }
            else
            {
                outputShaderFilePath = inputDirectory + outputShaderFilePath;
            }
            outputShaderFilePath.append(kCompiledShaderFileExtension);

            return compileShaderFromFile(inputShaderFilePath.c_str(), entryPoint, outputShaderFilePath.c_str(), shaderType, forceCompilation, inoutShader);
        }

        const bool DxShaderPool::compileShaderFromFile(const char* const inputShaderFilePath, const char* const entryPoint, const char* const outputShaderFilePath, const DxShaderType shaderType, const bool forceCompilation, DxShader& inoutShader)
        {
            if (kStringNPos == fs::StringUtil::find(inputShaderFilePath, ".hlsl"))
            {
                return false;
            }

            if (fs::FileUtil::exists(outputShaderFilePath) == false || forceCompilation == true)
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
                std::wstring compiledShaderFileNameWide;
                fs::StringUtil::convertStringToWideString(outputShaderFilePath, compiledShaderFileNameWide);
                if (FAILED(D3DReadFileToBlob(compiledShaderFileNameWide.c_str(), inoutShader._shaderBlob.ReleaseAndGetAddressOf())))
                {
                    return false;
                }
            }

            inoutShader._entryPoint = entryPoint;
            inoutShader._hlslFileName = inputShaderFilePath;
            inoutShader._hlslBinaryFileName = outputShaderFilePath;

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

            const UINT debugFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
            if (FAILED(D3DCompile(content, contentLength, identifier, nullptr, _shaderHeaderMemory, entryPoint, version.c_str(), debugFlag, 0, outBlob, _errorMessageBlob.ReleaseAndGetAddressOf())))
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

        void DxShaderPool::recompileAllShaders()
        {
            const uint32 shaderTypeCount = static_cast<uint32>(fs::RenderingBase::DxShaderType::COUNT);
            for (uint32 shaderTypeIndex = 0; shaderTypeIndex < shaderTypeCount; shaderTypeIndex++)
            {
                const fs::RenderingBase::DxShaderType shaderType = static_cast<fs::RenderingBase::DxShaderType>(shaderTypeIndex);
                const DxObjectId objectId = _boundShaderIdArray[shaderTypeIndex];
                if (objectId.isValid() == true)
                {
                    getShader(shaderType, _boundShaderIdArray[shaderTypeIndex]).unbind();
                }
            }

            for (auto& shader : _vertexShaderArray)
            {
                compileShaderFromFile(shader._hlslFileName.c_str(), shader._entryPoint.c_str(), shader._hlslBinaryFileName.c_str(), shader._shaderType, true, shader);
                createVertexShaderInternal(shader, nullptr);
            }

            for (auto& shader : _geometryShaderArray)
            {
                compileShaderFromFile(shader._hlslFileName.c_str(), shader._entryPoint.c_str(), shader._hlslBinaryFileName.c_str(), shader._shaderType, true, shader);
                createNonVertexShaderInternal(shader, fs::RenderingBase::DxShaderType::GeometryShader);
            }

            for (auto& shader : _pixelShaderArray)
            {
                compileShaderFromFile(shader._hlslFileName.c_str(), shader._entryPoint.c_str(), shader._hlslBinaryFileName.c_str(), shader._shaderType, true, shader);
                createNonVertexShaderInternal(shader, fs::RenderingBase::DxShaderType::PixelShader);
            }

            for (uint32 shaderTypeIndex = 0; shaderTypeIndex < shaderTypeCount; shaderTypeIndex++)
            {
                const fs::RenderingBase::DxShaderType shaderType = static_cast<fs::RenderingBase::DxShaderType>(shaderTypeIndex);
                const DxObjectId objectId = _boundShaderIdArray[shaderTypeIndex];
                if (objectId.isValid() == true)
                {
                    getShader(shaderType, _boundShaderIdArray[shaderTypeIndex]).bind();
                }
            }
        }

        void DxShaderPool::reportCompileError()
        {
            std::string errorMessages(reinterpret_cast<char*>(_errorMessageBlob->GetBufferPointer()));
            
            const size_t firstNewLinePos = errorMessages.find('\n');
            const size_t secondNewLinePos = errorMessages.find('\n', firstNewLinePos + 1);
            errorMessages = errorMessages.substr(0, secondNewLinePos);

            FS_LOG_ERROR("김장원", "Shader Compile Error\n\n%s", errorMessages.c_str());
        }

        void DxShaderPool::bindShaderIfNot(const DxShaderType shaderType, const DxObjectId& objectId)
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
            if (shaderType == DxShaderType::COUNT)
            {
                return;
            }

            const uint32 shaderTypeIndex = static_cast<uint32>(shaderType);
            if (false == _boundShaderIdArray[shaderTypeIndex].isValid())
            {
                return;
            }

            getShader(shaderType, _boundShaderIdArray[shaderTypeIndex]).unbind();
            _boundShaderIdArray[shaderTypeIndex] = DxObjectId::kInvalidObjectId;
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

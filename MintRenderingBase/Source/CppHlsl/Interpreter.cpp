#include <stdafx.h>
#include <MintRenderingBase/Include/CppHlsl/Interpreter.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/HashMap.hpp>

#include <MintPlatform/Include/TextFile.h>

#include <typeindex>


namespace mint
{
    namespace CppHlsl
    {
        Interpreter::Interpreter()
            : _parser{ _lexer }
            , _fileType{}
        {
            __noop;
        }

        void Interpreter::parseCppHlslFile(const char* const fileName)
        {
            TextFileReader textFileReader;
            if (textFileReader.open(fileName) == false)
            {
                MINT_LOG_ERROR("김장원", "CppHlsl 파일[%s] 을 여는 데 실패했습니다!!", fileName);
                return;
            }

            _lexer.setSource(textFileReader.get());
            _lexer.execute();

            _parser.execute();
        }

        void Interpreter::generateHlslString(const CppHlslFileType fileType)
        {
            _fileType = fileType;

            _hlslString.clear();

            const uint32 typeMetaDataCount = _parser.getTypeMetaDataCount();
            if (_fileType == CppHlslFileType::StructuredBuffers)
            {
                for (uint32 typeMetaDataIndex = 0; typeMetaDataIndex < typeMetaDataCount; ++typeMetaDataIndex)
                {
                    const TypeMetaData& typeMetaData = _parser.getTypeMetaData(typeMetaDataIndex);
                    if (typeMetaData.isBuiltIn() == true)
                    {
                        continue;
                    }

                    _hlslString.append(CppHlsl::Parser::serializeCppHlslTypeToHlslStructuredBufferDefinition(typeMetaData));
                }
            }

            uint32 bufferIndex = 0;
            uint32 structuredBufferIndex = 0;
            for (uint32 typeMetaDataIndex = 0; typeMetaDataIndex < typeMetaDataCount; ++typeMetaDataIndex)
            {
                const TypeMetaData& typeMetaData = _parser.getTypeMetaData(typeMetaDataIndex);
                if (typeMetaData.isBuiltIn() == true)
                {
                    continue;
                }

                if (_fileType == CppHlslFileType::StreamData)
                {
                    _hlslString.append(CppHlsl::Parser::serializeCppHlslTypeToHlslStreamDatum(typeMetaData));
                }
                else if (_fileType == CppHlslFileType::ConstantBuffers)
                {
                    _hlslString.append(CppHlsl::Parser::serializeCppHlslTypeToHlslConstantBuffer(typeMetaData, bufferIndex));

                    ++bufferIndex;
                }
            }

            if (_hlslString.empty() == true)
            {
                MINT_LOG_ERROR("김장원", "Hlsl String 을 Generate 하지 못했습니다!! typeMetaDataCount: %d", typeMetaDataCount);
            }
        }

        const char* Interpreter::getHlslString() const noexcept
        {
            return _hlslString.c_str();
        }

        const uint32 Interpreter::getTypeMetaDataCount() const noexcept
        {
            return _parser.getTypeMetaDataCount();
        }

        const TypeMetaData& Interpreter::getTypeMetaData(const uint32 typeIndex) const noexcept
        {
            return _parser.getTypeMetaData(typeIndex);
        }

        const TypeMetaData& Interpreter::getTypeMetaData(const std::string& typeName) const noexcept
        {
            return _parser.getTypeMetaData(typeName);
        }

        const TypeMetaData& Interpreter::getTypeMetaData(const std::type_info& stdTypeInfo) const noexcept
        {
            std::string stdTypeName = stdTypeInfo.name();
            const uint64 firstSpacePosition = stdTypeName.find(' ');
            if (firstSpacePosition != std::string::npos)
            {
                stdTypeName = stdTypeName.substr(firstSpacePosition + 1);
            }
            return getTypeMetaData(stdTypeName);
        }

    }
}

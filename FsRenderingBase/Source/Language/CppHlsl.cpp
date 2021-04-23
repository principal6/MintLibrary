#include <stdafx.h>
#include <FsRenderingBase/Include/Language/CppHlsl.h>

#include <FsContainer/Include/Vector.hpp>
#include <FsContainer/Include/HashMap.hpp>

#include <FsPlatform/Include/TextFile.h>

#include <typeindex>


namespace fs
{
    namespace Language
    {
        CppHlsl::CppHlsl()
            : _parser{ _lexer }
        {
            __noop;
        }

        void CppHlsl::parseCppHlslFile(const char* const fileName)
        {
            TextFileReader textFileReader;
            if (textFileReader.open(fileName) == false)
            {
                FS_LOG_ERROR("김장원", "CppHlsl 파일[%s] 을 여는 데 실패했습니다!!", fileName);
                return;
            }

            _lexer.setSource(textFileReader.get());
            _lexer.execute();

            _parser.execute();
        }

        void CppHlsl::generateHlslString(const CppHlslFileType fileType)
        {
            _fileType = fileType;

            _hlslString.clear();

            const uint32 typeInfoCount = _parser.getTypeInfoCount();
            if (_fileType == CppHlslFileType::StructuredBuffers)
            {
                for (uint32 typeInfoIndex = 0; typeInfoIndex < typeInfoCount; ++typeInfoIndex)
                {
                    _hlslString.append(Language::CppHlslParser::serializeCppHlslTypeToHlslStructuredBufferDefinition(_parser.getTypeInfo(typeInfoIndex)));
                }
            }

            uint32 bufferIndex = 0;
            uint32 structuredBufferIndex = 0;
            for (uint32 typeInfoIndex = 0; typeInfoIndex < typeInfoCount; ++typeInfoIndex)
            {
                if (_fileType == CppHlslFileType::StreamData)
                {
                    _hlslString.append(Language::CppHlslParser::serializeCppHlslTypeToHlslStreamDatum(_parser.getTypeInfo(typeInfoIndex)));
                }
                else if (_fileType == CppHlslFileType::ConstantBuffers)
                {
                    _hlslString.append(Language::CppHlslParser::serializeCppHlslTypeToHlslConstantBuffer(_parser.getTypeInfo(typeInfoIndex), bufferIndex));

                    ++bufferIndex;
                }
            }

            if (_hlslString.empty() == true)
            {
                FS_LOG_ERROR("김장원", "Hlsl String 을 Generate 하지 못했습니다!! typeInfoCount: %d", typeInfoCount);
            }
        }

        const char* CppHlsl::getHlslString() const noexcept
        {
            return _hlslString.c_str();
        }

        const uint32 CppHlsl::getTypeInfoCount() const noexcept
        {
            return _parser.getTypeInfoCount();
        }

        const CppHlslTypeInfo& CppHlsl::getTypeInfo(const uint32 typeIndex) const noexcept
        {
            return _parser.getTypeInfo(typeIndex);
        }

        const CppHlslTypeInfo& CppHlsl::getTypeInfo(const std::string& typeName) const noexcept
        {
            return _parser.getTypeInfo(typeName);
        }

        const CppHlslTypeInfo& CppHlsl::getTypeInfo(const std::type_info& stdTypeInfo) const noexcept
        {
            std::string stdTypeName = stdTypeInfo.name();
            const uint64 firstSpacePosition = stdTypeName.find(' ');
            if (firstSpacePosition != std::string::npos)
            {
                stdTypeName = stdTypeName.substr(firstSpacePosition + 1);
            }
            return getTypeInfo(stdTypeName);
        }

    }
}

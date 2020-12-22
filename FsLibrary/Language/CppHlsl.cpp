#include <stdafx.h>
#include <Language/CppHlsl.h>

#include <File/TextFile.h>

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
			textFileReader.open(fileName);

			_lexer.setSource(textFileReader.get());
			_lexer.execute();

			_parser.execute();
		}

		void CppHlsl::generateHlslString(const CppHlslFileType fileType)
		{
			_fileType = fileType;

			_hlslString.clear();

			uint32 cbufferIndex = 0;
			const uint32 typeInfoCount = _parser.getTypeInfoCount();
			for (uint32 typeInfoIndex = 0; typeInfoIndex < typeInfoCount; ++typeInfoIndex)
			{
				if (_fileType == CppHlslFileType::Structs)
				{
					_hlslString.append(Language::CppHlslParser::serializeCppHlslTypeToHlslStruct(_parser.getTypeInfo(typeInfoIndex)));
				}
				else if (_fileType == CppHlslFileType::Cbuffers)
				{
					_hlslString.append(Language::CppHlslParser::serializeCppHlslTypeToHlslCbuffer(_parser.getTypeInfo(typeInfoIndex), cbufferIndex));

					++cbufferIndex;
				}
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

		const CppHlslTypeInfo& CppHlsl::getTypeInfo(const uint64 typeIndex) const noexcept
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
			const uint64 lastColonPosition = stdTypeName.find_last_of(':');
			if (lastColonPosition != std::string::npos)
			{
				stdTypeName = stdTypeName.substr(lastColonPosition + 1);
			}
			return getTypeInfo(stdTypeName);
		}

	}
}

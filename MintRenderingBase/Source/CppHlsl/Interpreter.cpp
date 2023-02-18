#include <MintRenderingBase/Include/CppHlsl/Interpreter.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/HashMap.hpp>

#include <MintPlatform/Include/TextFile.h>


namespace mint
{
	namespace Language
	{
		namespace CppHlsl
		{
			Interpreter::Interpreter()
				: _parser{ _lexer }
				, _fileType{}
			{
				__noop;
			}

			void Interpreter::ParseCppHlslFile(const char* const fileName)
			{
				TextFileReader textFileReader;
				if (textFileReader.Open(fileName) == false)
				{
					MINT_LOG_ERROR("CppHlsl 파일[%s] 을 여는 데 실패했습니다!!", fileName);
					return;
				}

				_lexer.SetSource(textFileReader.Get());
				_lexer.Execute();

				_parser.Execute();
			}

			void Interpreter::GenerateHlslString(const CppHlslFileType fileType)
			{
				_fileType = fileType;

				_hlslString.clear();

				const uint32 typeMetaDataCount = _parser.GetTypeMetaDataCount();
				if (_fileType == CppHlslFileType::StructuredBuffers)
				{
					for (uint32 typeMetaDataIndex = 0; typeMetaDataIndex < typeMetaDataCount; ++typeMetaDataIndex)
					{
						const TypeMetaData<TypeCustomData>& typeMetaData = _parser.GetTypeMetaData(typeMetaDataIndex);
						if (typeMetaData.IsBuiltIn() == true)
						{
							continue;
						}

						_hlslString.append(_parser.SerializeCppHlslTypeToHlslStructuredBufferDefinition(typeMetaData));
					}
				}

				uint32 bufferIndex = 0;
				uint32 structuredBufferIndex = 0;
				for (uint32 typeMetaDataIndex = 0; typeMetaDataIndex < typeMetaDataCount; ++typeMetaDataIndex)
				{
					const TypeMetaData<TypeCustomData>& typeMetaData = _parser.GetTypeMetaData(typeMetaDataIndex);
					if (typeMetaData.IsBuiltIn() == true)
					{
						continue;
					}

					if (_fileType == CppHlslFileType::StreamData)
					{
						_hlslString.append(_parser.SerializeCppHlslTypeToHlslStreamDatum(typeMetaData));
					}
					else if (_fileType == CppHlslFileType::ConstantBuffers)
					{
						_hlslString.append(_parser.SerializeCppHlslTypeToHlslConstantBuffer(typeMetaData, bufferIndex));

						++bufferIndex;
					}
				}

				if (_hlslString.empty() == true)
				{
					MINT_LOG_ERROR("Hlsl String 을 Generate 하지 못했습니다!! typeMetaDataCount: %d", typeMetaDataCount);
				}
			}

			const char* Interpreter::GetHlslString() const noexcept
			{
				return _hlslString.c_str();
			}

			uint32 Interpreter::GetTypeMetaDataCount() const noexcept
			{
				return _parser.GetTypeMetaDataCount();
			}

			const TypeMetaData<TypeCustomData>& Interpreter::GetTypeMetaData(const uint32 typeIndex) const noexcept
			{
				return _parser.GetTypeMetaData(typeIndex);
			}

			const TypeMetaData<TypeCustomData>& Interpreter::GetTypeMetaData(const std::string& typeName) const noexcept
			{
				return _parser.GetTypeMetaData(typeName);
			}

			const TypeMetaData<TypeCustomData>& Interpreter::GetTypeMetaData(const std::type_info& stdTypeInfo) const noexcept
			{
				std::string stdTypeName = stdTypeInfo.name();
				const uint64 firstSpacePosition = stdTypeName.find(' ');
				if (firstSpacePosition != std::string::npos)
				{
					stdTypeName = stdTypeName.substr(firstSpacePosition + 1);
				}
				return GetTypeMetaData(stdTypeName);
			}
		}
	}
}

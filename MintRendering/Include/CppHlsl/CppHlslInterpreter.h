#pragma once


#ifndef _MINT_RENDERING_BASE_CPP_HLSL_INTERPRETER_H_
#define _MINT_RENDERING_BASE_CPP_HLSL_INTERPRETER_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRendering/Include/CppHlsl/CppHlslLexer.h>
#include <MintRendering/Include/CppHlsl/CppHlslParser.h>


namespace mint
{
	namespace Language
	{
		namespace CppHlsl
		{
			enum class CppHlslFileType
			{
				StreamData,
				ConstantBuffers,
				StructuredBuffers,
			};

			class Interpreter
			{
			public:
				Interpreter();
				~Interpreter() = default;

			public:
				void ParseCppHlslFile(const char* const fileName);

			public:
				void GenerateHlslString(const CppHlslFileType fileType);

			public:
				const char* GetHlslString() const noexcept;

			public:
				uint32 GetTypeMetaDataCount() const noexcept;
				const TypeMetaData<TypeCustomData>& GetTypeMetaData(const uint32 typeIndex) const noexcept;
				const TypeMetaData<TypeCustomData>& GetTypeMetaData(const StringA& typeName) const noexcept;
				const TypeMetaData<TypeCustomData>& GetTypeMetaData(const std::type_info& stdTypeInfo) const noexcept;

			private:
				Lexer _lexer;
				Parser _parser;

			private:
				CppHlslFileType _fileType;
				StringA _hlslString;
			};
		}
	}
}


#endif // !_MINT_RENDERING_BASE_CPP_HLSL_INTERPRETER_H_

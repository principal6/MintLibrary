#pragma once


#ifndef FS_CPP_HLSL_H
#define FS_CPP_HLSL_H


#include <CommonDefinitions.h>

#include <Language/CppHlslLexer.h>
#include <Language/CppHlslParser.h>


namespace fs
{
	namespace Language
	{
		enum class CppHlslFileType
		{
			Structs,
			Cbuffers
		};

		class CppHlsl
		{
		public:
										CppHlsl();
										~CppHlsl() = default;

		public:
			void						parseCppFile(const char* const fileName);
		
		public:
			void						generateHlslStringDefault(const CppHlslFileType fileType);
		
		public:
			const char*					getHlslString() const noexcept;

		public:
			const uint32				getTypeInfoCount() const noexcept;
			const CppHlslTypeInfo&		getTypeInfo(const uint64 typeIndex) const noexcept;
			const CppHlslTypeInfo&		getTypeInfo(const std::string& typeName) const noexcept;
		
		private:
			CppHlslLexer				_lexer;
			CppHlslParser				_parser;

		private:
			CppHlslFileType				_fileType;
			std::string					_hlslString;
		};
	}
}


#endif // !FS_CPP_HLSL_H

#pragma once


#ifndef FS_CPP_HLSL_H
#define FS_CPP_HLSL_H


#include <CommonDefinitions.h>

#include <FsLibrary/Language/CppHlslLexer.h>
#include <FsLibrary/Language/CppHlslParser.h>


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
			void						parseCppHlslFile(const char* const fileName);
		
		public:
			void						generateHlslString(const CppHlslFileType fileType);
		
		public:
			const char*					getHlslString() const noexcept;

		public:
			const uint32				getTypeInfoCount() const noexcept;
			const CppHlslTypeInfo&		getTypeInfo(const uint64 typeIndex) const noexcept;
			const CppHlslTypeInfo&		getTypeInfo(const std::string& typeName) const noexcept;
			const CppHlslTypeInfo&		getTypeInfo(const std::type_info& stdTypeInfo) const noexcept;
		
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

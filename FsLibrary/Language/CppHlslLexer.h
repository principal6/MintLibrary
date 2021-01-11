#pragma once


#ifndef FS_CPP_LEXER_H
#define FS_CPP_LEXER_H


#include <FsLibrary/CommonDefinitions.h>

#include <FsLibrary/Language/ILexer.h>


namespace fs
{
	namespace Language
	{
		class CppHlslLexer final : public ILexer
		{
		public:
													CppHlslLexer();
													CppHlslLexer(const std::string& source);
			virtual									~CppHlslLexer() = default;

		public:
			virtual const bool						execute() override final;
		};
	}
}


#endif // !FS_CPP_LEXER_H

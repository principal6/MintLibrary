#pragma once


#ifndef FS_CPP_LEXER_H
#define FS_CPP_LEXER_H


#include <CommonDefinitions.h>

#include <Language/ILexer.h>


namespace fs
{
	namespace Language
	{
		class CppHlslLexer final : public ILexer
		{
		public:
													CppHlslLexer(const std::string& source);
			virtual									~CppHlslLexer() = default;

		public:
			virtual const bool						execute() override final;
		};
	}
}


#endif // !FS_CPP_LEXER_H

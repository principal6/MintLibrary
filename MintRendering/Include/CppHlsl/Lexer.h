#pragma once


#ifndef _MINT_RENDERING_BASE_CPP_HLSL_LEXER_H_
#define _MINT_RENDERING_BASE_CPP_HLSL_LEXER_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintLanguage/Include/ILexer.h>


namespace mint
{
	namespace Language
	{
		namespace CppHlsl
		{
			class Lexer final : public ILexer
			{
			public:
				Lexer();
				Lexer(const StringA& source);
				virtual ~Lexer() = default;

			public:
				virtual bool Execute() noexcept override final;
			};
		}
	}
}


#endif // !_MINT_RENDERING_BASE_CPP_HLSL_LEXER_H_
